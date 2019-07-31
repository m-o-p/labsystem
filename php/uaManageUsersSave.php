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
*/

require( "../include/init.inc" );

if ( !isset( $_POST['REDIRECTTO'] )
   ){
      trigger_error( $lng->get( 'NotAllNecValPosted' ), E_USER_ERROR );
      exit;
     }

if (  (substr( $url->get('config'), -9 ) != 'useradmin') || // only in this configuration you are allowed to make that call!
     !(defined('IS_DB_USER_ADMIN') && $usr->isOfKind(IS_DB_USER_ADMIN)) /* valid call? */
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
  while( $data = $result->fetch_array() )
    if ( substr( $data[0], 0, 1 ) == '_' ) $courseArray[$data[0]] = $data[0] . '=0';

// get affected UIDs
if (array_key_exists('uids', $_POST))
  $uids = $_POST['uids'];
else
  $uids = Array();

foreach ($uids as $uid => $courseData) {
  $courses = $courseArray;
  foreach (array_keys($courseData) as $course)
    if (array_key_exists($course, $courses))
      $courses[$course] = $course . '=1';
  $userDBC->mkUpdate( implode(', ', $courses),
                      $cfg->get('UserDatabaseTable'),
                      $cfg->get('UserDBField_uid')."='" . $userDBC->escapeString($uid) . "'"
                     );
}

// note
  $url->put( "sysinfo", $lng->get("DataHasBeenSaved") );

// redirect
  header( "Location: ".urldecode($_POST['REDIRECTTO']) );

?>
