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
* Called by ../pages/manageUsers.php to save the modified rights of all users.
*
* @module     ../php/saveUserRights.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $_POST['REDIRECTTO']   The address to redirect to after saving.
* @param $_POST['SESSION_ID']   To verify that the user is the user that set the call and is logged in.
* @param $_POST[$userData["uid"]."_".$i]  The rights checkboxes
*/

require( "../include/init.inc" );

if ( !isset($_POST['REDIRECTTO'])     
   ) trigger_error( $lng->get( 'NotAllNecValPosted' ), E_USER_ERROR );

if ( !( isset($_POST['SESSION_ID']) && 
      ($_POST['SESSION_ID'] != "") && 
      ($_POST['SESSION_ID'] == session_id()) ) /* valid call? */   
   ) trigger_error( $lng->get( 'NotAllowedToMkCall' ), E_USER_ERROR );

// check for all user ids if data are there and save the changes if there are changes.
  require_once( INCLUDE_DIR."/classes/DBInterfaceUser.inc" );
  require_once( INCLUDE_DIR."/classes/DBInterfaceUserRights.inc" );
  
  $uDBi = new DBInterfaceUser();
  $uDBi->getAllData();
  while( $userData = $uDBi->getNextData() ){
    // only take present users!
    if ( !isset( $_POST[ $userData["uid"].'_present' ] ) ) continue;
    
    $userRightsNew=IS_USER; // set back to minimum rights

   /* Wouldn't it make sense to be able to remove the IS_USER bit?
    *   Not really. If you want to remove a user do that in your authentication source.
    */
    for ($i=2; $i<=MAX_USER_ROLE; $i=$i<<1)
      if ( isset( $_POST['ID'.$userData["uid"]."_".$i] ) ) $userRightsNew += $_POST['ID'.$userData["uid"]."_".$i];

    $currentTeam = "";
    if ( isset( $_POST[$userData["uid"]."_team"] ) ) $currentTeam = $_POST[$userData["uid"]."_team"];
    
    $urDBI = new DBInterfaceUserRights();
    $ur = $urDBI->getData4( $userData["uid"] );
    if ( ( $userRightsNew != $ur['rights'] ) || ( $currentTeam != $ur['currentTeam'] ) ) // changes?
      $urDBI->setData4( $userData["uid"], $userRightsNew, $currentTeam );
  }

// note
  $url->put( "sysinfo=".urlencode( $lng->get("DataHasBeenSaved") ) );
  
  makeLogEntry( 'system', 'user rights saved' );

// redirect
  header( "Location: ".$url->rawLink2( urldecode($_POST['REDIRECTTO']) ) );
?>