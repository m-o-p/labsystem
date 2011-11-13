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
* Called by ../pages/uaMyPW.php to save the user's new password.
*
* @module     ../php/uaMyPWSave.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $_POST['REDIRECTTO']   The address to redirect to after saving.
* @param $_POST['SESSION_ID']   To verify that the user is the user that set the call and is logged in.
* @param $_POST['USERNAME']
* @param $_POST['NAME']
* @param $_POST['FORENAME']
* @param $_POST['EMAIL']
* @param $_POST['LABSYS_MOP_?'] CUSTOM FIELDS...
*/

require( "../include/init.inc" );

if ( !isset( $_POST['REDIRECTTO'] ) ||
     !isset( $_POST['USERNAME'] ) ||
     !isset( $_POST['NAME'] ) ||
     !isset( $_POST['FORENAME'] ) ||
     !isset( $_POST['EMAIL'] )
   ){
      trigger_error( $lng->get( 'NotAllNecValPosted' ), E_USER_ERROR );
      exit;
     }

if (  (substr( $url->get('config'), -9 ) != 'useradmin') || // only in this configuration you are allowed to make that call!
     !( isset($_POST['SESSION_ID']) && 
      ($_POST['SESSION_ID'] != "") && 
      ($_POST['SESSION_ID'] == session_id()) ) /* valid call? */   
   ){
      trigger_error( $lng->get( 'NotAllowedToMkCall' ), E_USER_ERROR );
      exit;
     }

// length of username
if ( strlen( $_POST['USERNAME'] ) <= $cfg->get('uaMinUsrNameLength') )
    // alert
    $url->put( 'sysalert='.$lng->get('uaUsrNameTooShort').' ( !<='.$cfg->get('uaMinUsrNameLength').' )' );
// eMail must contain @ and have a dot behind
elseif ( (strpos( $_POST['EMAIL'], '@' ) === FALSE) ||
         (strrpos( $_POST['EMAIL'], '@' ) > strrpos( $_POST['EMAIL'], '.' ) ) )
    // alert
    $url->put( 'sysalert='.$lng->get('uaMailInvalid') );
elseif( ( $_POST['NAME'] == '' ) || ( $_POST['FORENAME'] == '' ) )
    // alert
    $url->put( 'sysalert='.$lng->get('uaSurNameEmpty') );
else{
    // new Interface to the userDB
    $userDBC = new DBConnection($cfg->get('UserDatabaseHost'), 
                                $cfg->get('UserDatabaseUserName'), 
                                $cfg->get('UserDatabasePassWord'), 
                                $cfg->get('UserDatabaseName'));

    // check if the username exists:
    $result = $userDBC->mkSelect( $cfg->get('UserDBField_name').', '.
                                  $cfg->get('UserDBField_forename').', '.
                                  $cfg->get('UserDBField_uid'), 
                                  $cfg->get('UserDatabaseTable'), 
                                  $cfg->get('UserDBField_username')."='".$_POST['USERNAME']."' && ".
                                  $cfg->get('UserDBField_uid')."!='".( $usr->isOfKind( IS_DB_USER_ADMIN ) && $usr->isSeeingSomeonesData() ? $usr->theSeeingUid()  : $usr->uid  )."'"
                                 );
    $data = mysql_fetch_assoc( $result );
    if ( mysql_num_rows( $result ) != 0){
      // alert
      $url->put( 'sysalert='.$_POST['USERNAME'].' '.$lng->get('uaAsUsrNmeIsUsedBy').' '.$data[ $cfg->get('UserDBField_forename') ].' '.$data[  $cfg->get('UserDBField_name') ] );
    }
    else{ // save data
      
      // process the custom fields:
      $customFields = '';
      // The following fields and those starting with "_" (course id)  will not be processed:
      $doNotListFromUser = Array( $cfg->get('UserDBField_username'), 
                                  $cfg->get('UserDBField_name'), 
                                  $cfg->get('UserDBField_forename'),
                                  $cfg->get('UserDBField_email'),
                                  $cfg->get('UserDBField_uid'),
                                  $cfg->get('UserDBField_password'),
                                  'labsys_mop_last_change'
                                 );
      foreach ( $_POST as $key => $value )
        if( substr( $key, 0, 11 ) == 'LABSYS_MOP_' ){ // all start with that prefix
          $key = substr( $key, 11 );
          if ( in_array( $key, $doNotListFromUser ) || ( $key[0] == '_' ) ) /* do nothing */;
          else $customFields .= $key."='".$value."'".', ';
        }
  
      // update the values
      $userDBC->mkUpdate( $customFields. // coming on top they will not override system fields below.
                          $cfg->get('UserDBField_username')."='".$_POST['USERNAME']."', ".
                          $cfg->get('UserDBField_name')."='".$_POST['NAME']."', ".
                          $cfg->get('UserDBField_forename')."='".$_POST['FORENAME']."', ".
                          $cfg->get('UserDBField_email')."='".$_POST['EMAIL']."'", 
                          $cfg->get('UserDatabaseTable'), 
                          $cfg->get('UserDBField_uid')."='".( $usr->isOfKind( IS_DB_USER_ADMIN ) && $usr->isSeeingSomeonesData() ?  $usr->theSeeingUid()  : $usr->uid  )."'"
                         );
      // note
      $url->put( "sysinfo=".$lng->get("DataHasBeenSaved") );
      makeLogEntry( 'useradmin', 'saved userdata of '.$_POST['USERNAME'] );
    }
}

// redirect
  header( "Location: ".$url->rawLink2( urldecode($_POST['REDIRECTTO']) ) );
?>