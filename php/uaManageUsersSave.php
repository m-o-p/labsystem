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
* Called by ../pages/uaManageUsers.php to save the user's subscriptions to the courses.
*
* @module     ../php/uaManageUsersSave.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $_POST['REDIRECTTO']   The address to redirect to after saving.
* @param $_POST['SESSION_ID']   To verify that the user is the user that set the call and is logged in.
*/

require( "../include/init.inc" );

if ( !isset( $_POST['REDIRECTTO'] )
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

// which courses exist?
  // ask for the couseID fields starting with _                         
  // list all columns
  $result = $userDBC->query( 'SHOW COLUMNS FROM '.$cfg->get('UserDatabaseTable') );
  $courseArray = Array();
  while( $data = mysql_fetch_array( $result ) )
    if ( substr( $data[0], 0, 1 ) == '_' ) array_push( $courseArray, $data[0] );
      
// query for all datasets for iterating over the uids
$result = $userDBC->mkSelect( '*', 
                              $cfg->get('UserDatabaseTable'), 
                              ''
                             );

$noCourses = count( $courseArray );
while ( $data = mysql_fetch_assoc( $result ) ){
  // only take present users! (selection)
  if ( !isset( $_POST[ $data[ $cfg->get('UserDBField_uid') ] ] ) ) continue;

  $changes = false; // update of the record necessary?
  $updateString = "";
  for( $i=0; $i<$noCourses; $i++ ){
    $key = $courseArray[ $i ];
    $postedValue = ( isset( $_POST[ $data[ $cfg->get('UserDBField_uid') ].$key ] ) ? 1: 0 );
    $updateString .= ', '.$key."='".$postedValue."'";
    $changes |= ( $postedValue != $data[ $key ] );
  }

  if ( $changes ) 
    // update the values
    $userDBC->mkUpdate( substr( $updateString, 2 ),
                        $cfg->get('UserDatabaseTable'), 
                        $cfg->get('UserDBField_uid')."='".$data[ $cfg->get('UserDBField_uid') ]."'"
                       );
}

// note
  $url->put( "sysinfo=".urlencode( $lng->get("DataHasBeenSaved") ) );

// redirect
  header( "Location: ".urldecode($_POST['REDIRECTTO']) );

?>