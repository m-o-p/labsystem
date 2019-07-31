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
* Called by ../pages/uaManageUsers.php to
*  - enable the seeing mode
*  - delete a user
*
* @module     ../php/uaManageUsersExecute.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $_GET['function']    Function's name.
* @param $_GET['param']       Function's parameters.
* @param $_GET['redirectto']  The redirect after executing the code url. Gets processed by the function.
*/

require( "../include/init.inc" );

if ( !$url->available('function') ||
     !$url->available('param') ||
     !$url->available('redirectTo')
   ){
      trigger_error( $lng->get( 'NotAllNecValPosted' ), E_USER_ERROR );
      exit;
     }

if (  (substr( $url->get('config'), -9 ) != 'useradmin') || // only in this configuration you are allowed to make that call!
      !$usr->isOfKind( IS_DB_USER_ADMIN )
   ){
      trigger_error( $lng->get( 'NotAllowedToMkCall' ), E_USER_ERROR );
      exit;
     }

// enable seeing mode
if ( $url->get( 'function' ) == 'see' )
  $usr->seesDataOf( stripslashes( $url->get( 'param' ) ) );

elseif( $url->get( 'function' ) == 'del' ){
  if ( !$url->available("isConfirmed") ){ // not confirmed via script -> do it via page
    header("Location: ".$url->rawLink2( '../pages/confirm.php', Array('text' => $lng->get("confirmDelete"), 'redirectTo' => $_SERVER["REQUEST_URI"]) ) );
    exit;
  }
// new Interface to the userDB
  $userDBC = new DBConnection($cfg->get('UserDatabaseHost'),
                              $cfg->get('UserDatabaseUserName'),
                              $cfg->get('UserDatabasePassWord'),
                              $cfg->get('UserDatabaseName'));

  makeLogEntry( "UserAdmin", "delete user", $url->get( 'param' ) );

  $ret = $userDBC->mkDelete( $cfg->get('UserDatabaseTable'), $cfg->get('UserDBField_uid')."='".$userDBC->escapeString($url->get( 'param' ))."'" );
  if ($ret === false) $text = 'Mysql Error: ' . $userDBC->link->error;
  else $text = $url->get( 'param' ).": ".$lng->get( "deleted" );
  $url->put( 'sysalert', $text );
}
else /* alert */ $url->put( 'sysalert', $lng->get('NotAllowedToMkCall') );

// redirect
  header( "Location: ".$url->rewriteExistingUrl( $url->get( 'redirectTo' ) ) );
?>
