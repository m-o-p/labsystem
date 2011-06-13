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
* Called by ../pages/uaCreateUsers.php to create new users.
*
* @module     ../php/uaCreateUsers.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $_POST['REDIRECTTO']     The address to redirect to after saving.
* @param $_POST['SESSION_ID']     To verify that the user is the user that set the call and is logged in.
* @param $_POST['MAILADDRESSES']  Mailaddresses of the users to be created.
*/

require( "../include/init.inc" );

if ( !isset( $_POST['REDIRECTTO'] ) ||
     !isset( $_POST['MAILADDRESSES'] )
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

// new Interface to the userDB
  $userDBC = new DBConnection($cfg->get('UserDatabaseHost'), 
                              $cfg->get('UserDatabaseUserName'), 
                              $cfg->get('UserDatabasePassWord'), 
                              $cfg->get('UserDatabaseName'));

// 1) The users subscriptions:
// which courses exist?
  // ask for the couseID fields starting with _                         
  // list all columns
  $result = $userDBC->query( 'SHOW COLUMNS FROM '.$cfg->get('UserDatabaseTable') );
  $courseArray = Array();
  while( $data = mysql_fetch_array( $result ) )
    if ( substr( $data[0], 0, 1 ) == '_' ) array_push( $courseArray, $data[0] );
    
  // create update string
  $updateString = "";
  for( $i=0; $i<count( $courseArray ); $i++ ){
    $key = $courseArray[ $i ];
    $postedValue = ( isset( $_POST[ $data[ $cfg->get('UserDBField_uid') ].$key ] ) ? 1 : 0 );
    $updateString .= ', '.$key."='".$postedValue."'";
  }
  // $updatestring starts with ", "!

// 2) Create the users:
  $newUsers = explode( "\n", $_POST['MAILADDRESSES'] );
  
  foreach( $newUsers as $value ){
    // Each line is $name, $prename, $email, ...
    $value = str_replace( "\r", '', $value ); // remove anyother newline signs!
    if ( $value == '' ) continue; // jump over empty ones
    
    $entries = explode( ',', $value );
    foreach($entries as $key=>$toDo){
      $entries[$key] = trim($toDo); // remove spaces
      $entries[$key] = preg_replace( array('/"/',"/'/"), '', $toDo ); // remove quotes
    }
    //echo( 'name: -'.$entries[0]."-<br>\r\n" );
    //echo( 'prename: -'.$entries[1]."-<br>\r\n" );
    //echo( 'mail: -'.$entries[2]."-<br><hr>\r\n" );

// generate new Password:
    srand((double)microtime()*1000000);
    $newPW = substr( md5( uniqid( rand() ) ), 13, 8 );
    
    $userDBC->mkInsert( $cfg->get('UserDBField_username')."='".$entries[2]."', ".
                        $cfg->get('UserDBField_name')."='".$entries[0]."', ".
                        $cfg->get('UserDBField_forename')."='".$entries[1]."', ".
                        $cfg->get('UserDBField_password')."='".sha1( $newPW )."', ".
                        $cfg->get('UserDBField_email')."='".$entries[2]."', ".
                        $cfg->get('UserDBField_uid')."='".md5( uniqid( rand() ) )."'".
                        $updateString,
                        $cfg->get('UserDatabaseTable') );

  }
  
// note
  $url->put( "sysinfo=".urlencode( $lng->get("DataHasBeenSaved") ) );
  
  makeLogEntry( 'useradmin', 'new users created' );

// redirect
  header( "Location: ".$url->rawLink2( urldecode($_POST['REDIRECTTO']) ) );
?>