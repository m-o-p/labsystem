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
* @param $_POST['NEWPW']        The new password.
* @param $_POST['NEWPWRETYPE']  It's retype.
*/

require( "../include/init.inc" );

if ( !isset( $_POST['REDIRECTTO'] ) ||
     !isset( $_POST['NEWPW'] ) ||
     !isset( $_POST['NEWPWRETYPE'] )
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

if ( $_POST['NEWPW'] != $_POST['NEWPWRETYPE'] )
    // alert
    $url->put( 'sysalert='.$lng->get('uaPwRetypeMismath') );
elseif ( strlen( $_POST['NEWPW'] ) < $cfg->get('uaMinPassLength') )
    // alert
    $url->put( 'sysalert='.$lng->get('uaPwTooShort').' ( '.$cfg->get('uaMinPassLength').' )' );
else{ // save new PW
    // new Interface to the userDB
    $userDBC = new DBConnection($cfg->get('UserDatabaseHost'), 
                                $cfg->get('UserDatabaseUserName'), 
                                $cfg->get('UserDatabasePassWord'), 
                                $cfg->get('UserDatabaseName'));
    $accordingUID = ( $usr->isOfKind( IS_DB_USER_ADMIN ) && $usr->isSeeingSomeonesData() ? $usr->theSeeingUid()  : $usr->uid  );                
    $userDBC->mkUpdate( $cfg->get('UserDBField_password')."='".crypt( $_POST['NEWPW'], $accordingUID )."'", // the UID is used as salt
                        $cfg->get('UserDatabaseTable'), 
                        $cfg->get('UserDBField_uid')."='".$accordingUID."'"
                       );
    // note
    $url->put( "sysinfo=".$lng->get("DataHasBeenSaved") );
    makeLogEntry( 'useradmin', 'saved password of '.$accordingUID );
}

// redirect
  header( "Location: ".$url->rawLink2( urldecode($_POST['REDIRECTTO']) ) );
?>