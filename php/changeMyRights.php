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
* Called by ../pages/myRights.php to save the modified rights in $_SESSION["userRights"].
*
* @module     ../php/changeMyRights.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $_POST[ "UR_".$i]  The user right checkbox return value
*/
require( "../include/init.inc" );

if ( !( (isset($_POST['SESSION_ID']) &&
         ($_POST['SESSION_ID'] != "") &&
         ($_POST['SESSION_ID'] == session_id())
        ) || ( $url->available('newrights') &&
               is_numeric( $url->get('newrights') ) &&
               $usr->isOfKind( IS_USER ) ) ) /* valid call? */
       ){
          trigger_error( $lng->get("notAllowed"), E_USER_ERROR );
          exit;
         }

// compute desired rights:
  $newRights=IS_USER;
 // walk over rules and check if the checkbox was set
  if ( isset($_POST['SESSION_ID']) ){
    for ($i=2; $i<=MAX_USER_ROLE; $i=$i<<1) if ( isset( $_POST[ "UR_".$i] ) ) $newRights += $_POST["UR_".$i];
  } else {
    $newRights = $url->get('newrights');
  }

// check against user's full rights (can set only less)
  require_once( INCLUDE_DIR."/classes/DBInterfaceUserRights.inc" );
  $urDBI = new DBInterfaceUserRights();
  $data = $urDBI->getData4( $_SESSION["uid"] );

  if ( $usr->isOfKind( $newRights, $data['rights'] ) ) $_SESSION["userRights"] = $newRights;
  else $_SESSION["userRights"] = IS_USER; // user gave himself rights he doesn't have (SU)...

  makeLogEntry( 'system', 'user rights changed' );
// redirect
  header( "Location: ".$url->rawLink2( (isset($_POST['REDIRECTTO']) ? $_POST['REDIRECTTO'] : ($url->available('redirectto') ? $url->get('redirectto') : '../pages/myRights.php') ) ) );
?>