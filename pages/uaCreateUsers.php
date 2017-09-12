<?php
/**
 *  labsystem.m-o-p.de -
 *                  the web based eLearning tool for practical exercises
 *  Copyright (C) 2010  Marc-Oliver Pahl
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
* This page is to create userDB users.
*
* @module     ../pages/uaCreateUsers.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2014
* @version    2.0
*/
require( "../include/init.inc" );
require_once( INCLUDE_DIR."/classes/DBInterfaceUser.inc" );

$pge->title        = $lng->get("titleUaCreateUsr");
$pge->matchingMenu = $lng->get("MnuEntryUaCreateUsr");
$pge->visibleFor   = IS_DB_USER_ADMIN;

if ( !$pge->isVisible() ){ // directly show warning and close.
  require( $cfg->get("SystemPageLayoutFile") );
  exit;
}

  if ( substr( $url->get('config'), -9 ) != 'useradmin' ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("TxtNotConfigUA")."\n</div>" );
  else{ // showing
  // title
     $pge->put( "<div class=\"labsys_mop_h2\">__PAGETITLE__</div>\n" );

  // note
     if ( $lng->get("uaCreateUsrNote") != "" ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("uaCreateUsrNote")."</div>\n" );

  // new Interface to the userDB
    $userDBC = new DBConnection($cfg->get('UserDatabaseHost'),
                                $cfg->get('UserDatabaseUserName'),
                                $cfg->get('UserDatabasePassWord'),
                                $cfg->get('UserDatabaseName'));

  // collect all columns from the user table...
    // The following fields and those starting with "_" (course id)  will not be listed:
    $doNotListFromUser = Array( $cfg->get('UserDBField_username'),
        $cfg->get('UserDBField_uid'),
        $cfg->get('UserDBField_password'),
        //'last_registered',
        'labsys_mop_last_change',
        'registerFor',
        'history'
    );

  // which courses exist?
    // ask for the couseID fields starting with _
    // list all columns
    $result = $userDBC->query( 'SHOW COLUMNS FROM '.$cfg->get('UserDatabaseTable') );
    $courseArray = Array();
    $otherFieldsArray = Array();
    while( $data = $result->fetch_array() ){
      if ( substr( $data[0], 0, 1 ) == '_' ){
        array_push( $courseArray, $data[0] );
      } elseif (!in_array( $data[0], $doNotListFromUser )){
        // Those may be of interest...
        array_push( $otherFieldsArray, $data[0] );
      }
    }

// form
    $pge->put( '<FORM class="labsys_mop_std_form"
                 NAME="createUser"
                 METHOD="POST"
                 ACTION="#">
                 <input type="hidden" name="REDIRECTTO" value="../pages/uaManageUsers.php">
               ' );

    $readOnlyInputs = !empty($_POST['NEWUSERS']) && !empty($_POST['courseSubscribe_multi']);
    $pge->put( '<div class="labsys_mop_u_row">'."\n" );

    $pge->put('<label for="courseSubscribe_multi[]" class="labsys_mop_input_field_label">'.$lng->get( 'uaNewUsrsSubscript' ).'</label>');
    $pge->put('<select '.($readOnlyInputs ? 'readonly="readonly" ':'' ).'style="float:left;margin-right:2em;width:100%" size="10" multiple="multiple" tabindex="'.$pge->nextTab++.'" name="courseSubscribe_multi[]">');
    $courseList = '';
    $unselectedCoursesHTML='';
    foreach ( $courseArray as $value ){
       $pge->put( '<option value="'.$value.'" onchange="isDirty=true"'.
                  ( empty($_POST['courseSubscribe_multi']) || !in_array( $value, $_POST['courseSubscribe_multi'] ) ? '' : ' selected="selected"' ).
                  '>'.
                  $value.
                  "</option>\n" );
    }
    $pge->put('</select>');

    $pge->put( '<label for="newUsers" class="labsys_mop_input_field_label_top">'.$lng->get( 'mailaddressesNewUsr' ).'</label>'.
               "<textarea ".($readOnlyInputs ? 'readonly="readonly" ':'')."tabindex=\"".$pge->nextTab++."\" id=\"newUsers\" name=\"NEWUSERS\" class=\"labsys_mop_textarea\" rows=\"".$cfg->get("uaCreateUsrsRows")."\" onchange='isDirty=true'>".
               (empty($_POST['NEWUSERS']) ? implode( ', ',$otherFieldsArray ) : $_POST['NEWUSERS']).
               "</textarea>\n".
               "</div>\n" );
    if ((!empty($_POST['preview']) || !empty($_POST['save'])) && !empty($_POST['courseSubscribe_multi'])) {
      // data there... preview
      // parse data
      $subscribedCourses = $_POST['courseSubscribe_multi'];
      $ignorePrefix = 'ignore_foo';
      $lines = explode("\n", $_POST['NEWUSERS']);
      $headerFields = Array();
      $data = Array();
      foreach ( $lines AS $line ) {
      	if (empty(trim($line))){
      		continue; // jump over empty lines...
      	}
        if (empty($headerFields)){
          $counter=0;
          foreach (str_getcsv($line) AS $token){
            $token = trim($token);
            array_push( $headerFields,
                        (!in_array( $token, $otherFieldsArray ) ? $ignorePrefix.$counter++ : $token ) );
          }
        } else {
          // data line
          $values = str_getcsv($line);
          foreach($values as &$value){
            $value=trim($value);
          }
          unset($value);
          $datarow = array_combine( $headerFields, $values );
          array_push( $data, $datarow );
        }
      }
      // /parse data
      if ( !empty($_POST['preview']) ){
        $pge->put( '<table class="createUserimportPreview">' . "\n" . '<tr>' );
        $pge->put('<th>#</th>');
        foreach ( $data[0] AS $key => $val ) {
          if (strpos($key, $ignorePrefix) !== 0){
            $pge->put('<th>' . $key . '</th>');
          }
        }
        $pge->put('<th></th>');
        $pge->put('</tr>' . "\n");
        $participantNumber=1;
        foreach ( $data AS $row ) {
          $pge->put('<tr>');
          $pge->put('<td>'.$participantNumber++.'</td>');
          foreach ( $row AS $key => $val ) {
            if (strpos($key, $ignorePrefix) !== 0){
              $pge->put('<td><pre>'.($key!='last_registered' ? $val : date('r',strtotime($val)) ).'</pre></td>');
            }
          }
          $pge->put('<td>'.implode(', ', $subscribedCourses).'</td>');
          $pge->put("</tr>\n");
        }
        $pge->put('</table>' . "\n");
      } elseif ( !empty($_POST['save']) ) {
        // preview checked... CREATE users!
        foreach ( $data AS $row ) {
          // Use the email as user name by default
          $username = $row[$cfg->get('UserDBField_email')];
          $insertString = '';
          $foundExisting = false;
          // if an extraid was configured by the admin, check if a user already exists based on that id (and update $username as appropriate)
          if ( $cfg->doesExist('UserDBField_extraid') && isset($row[$cfg->get('UserDBField_extraid')]) ) {
            $result=$userDBC->mkSelect($cfg->get('UserDBField_username'), $cfg->get('UserDatabaseTable'), $cfg->get('UserDBField_extraid').'="'.$userDBC->escapeString($row[$cfg->get('UserDBField_extraid')]).'"');
            $existingData=$result->fetch_array();
            if (!empty($existingData)) {
              $username = $existingData[$cfg->get('UserDBField_username')];
              $foundExisting = true;
            }
          }
          // check if a user with the same username already exists (if the mapping based on extraid was not performed)
          // NOTE: this is somewhat dangerous: if a user intentionally sets his username to an e-mail address of someone who will join the course, they will get the access rights to the instances instead of creating a new user
          // However, the missing access rights for participants should be discovered before the course starts and then the problem can be corrected by hand, so this is not considered to be a critical problem
          if ( !$foundExisting ) {
            $result=$userDBC->mkSelect($cfg->get('UserDBField_uid'), $cfg->get('UserDatabaseTable'),$cfg->get('UserDBField_username').'="'.$userDBC->escapeString($username).'"');
            $existingData=$result->fetch_array();
            if (!empty($existingData)){
              $foundExisting = true;
            }
          }
          // Set userdata only for new users (do not update it)
          if ( !$foundExisting ) {
            foreach ( $row AS $key => $val ) {
              if (strpos($key, $ignorePrefix) !== 0){
                $insertString .= (empty($insertString)?'':',').$key.'='.($key!='last_registered' ? '"'.$userDBC->escapeString($val).'"' : 'FROM_UNIXTIME('.strtotime($val).')');
              }
            }
            srand((double)microtime()*1000000);
            $UID         = md5( $row[$cfg->get('UserDBField_email')].uniqid( rand() ) );
            $insertString .= ','.$cfg->get('UserDBField_uid').'="'.$userDBC->escapeString($UID).'"';
          }
          // add newly subscribed courses for all users (also existing ones)
          foreach ($subscribedCourses as $value){
            $insertString .= (empty($insertString) ? '' : ',') . $value . '=1';
          }
          $userDBC->mkUpdIns($insertString, $cfg->get('UserDatabaseTable'), $cfg->get('UserDBField_username').'="'.$userDBC->escapeString($username).'"' );
          makeLogEntry( 'useradmin', 'user '.$userDBC->escapeString($username).' created or updated' );
        }

        // note
        $url->put( "sysinfo", $lng->get("DataHasBeenSaved") );

        // redirect
        header( "Location: ".$url->rewriteExistingUrl( $_POST['REDIRECTTO'] ) );
      }
    }
// /form
    $pge->put( "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" name=\"".( empty($_POST['preview']) ? 'preview' : 'save' )."\" value=\"".$lng->get('titleUaCreateUsr')."\" onclick='isDirty=false'>\n".
               "</FORM>"
             );
} // /showing
// show!
  require( $cfg->get("SystemPageLayoutFile") );
?>
