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
* Called by ../pages/login.php to do the authentication.
*
* As this is the only place where authentication happens you can insert
* any authentication mechanism you want to use here like kerberos, LDAP
* etc.
* If you plan to do so make sure you have consistent user keys as well as
* an adopted user listing function in the '../classes/DBInterfaceUser.inc'
*
* @module     ../php/authenticate.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $_POST['USERNAME']
* @param $_POST['PASSWORD']
* @param $_POST['REDIRECTTO'] If given the user is redirected there after login.
*/
require( "../include/init.inc" );

if ( !isset($_POST['USERNAME']) || !isset($_POST['PASSWORD']) ){
                                                                 trigger_error( $lng->get( 'NotAllNecValPosted' ), E_USER_ERROR );
                                                                 exit;
                                                                }

require_once( INCLUDE_DIR."/classes/DBInterfaceUser.inc" );

$uDBI = new DBInterfaceUser();

// preserve the current url since we will probably link back (p.e. to give an error) or add something to the url.
$url->clearQueryString(); $url->put( $url->get("oldQueryString") );

if ( !($authUserData = $uDBI->authenticate($_POST['USERNAME'], $_POST['PASSWORD']) ) ){
 // not authenticated
  $url->put( "sysalert=".$lng->get("AlertWrongUsrPw") );
  header( "Location: ".$url->rawLink2("../pages/login.php") );
  exit;
  }

// authenticated
  $NEWSESSION = true;
  require( INCLUDE_DIR."/session.inc" );

  $_SESSION["uid"]          = $authUserData["uid"];
  $_SESSION["userName"]     = $authUserData["userName"];
  $_SESSION["foreName"]     = $authUserData["foreName"];
  $_SESSION["surName"]      = $authUserData["name"];
  $_SESSION["mailAddress"]  = $authUserData["eMail"];

  $_SESSION["userRights"]   = $cfg->get("DefaultAuthUserRights");
  $_SESSION["currentTeam"]  = $cfg->get("DefaultAuthUserTeam");

// The following field is for security reasons:
// If the configuration would not be checked you could change the field in the url and would be logged on with your
// current rights for the different configuration...
  $_SESSION["config"]  = $url->get('config');

 // get the user rights from the DB
  require_once( INCLUDE_DIR."/classes/DBInterfaceUserRights.inc" );
  $urDBI = new DBInterfaceUserRights();
  $data = $urDBI->getData4( $_SESSION["uid"] );
  $_SESSION["userRights"]  = $data['rights'];
  $_SESSION["currentTeam"] = $data['currentTeam'];

// This special user gets additionally the IS_USER_RIGHTS_EDITOR
  if ( $_POST['USERNAME'] == $cfg->get("RightsAdminUsername") )
    $_SESSION["userRights"]  = (intval($_SESSION["userRights"]) | IS_USER_RIGHTS_EDITOR);

  makeLogEntry( 'system', 'login' );
  makeLogEntry( 'system', 'loginLog');
  $GLOBALS['Logger']->logToDatabase('system', logActions::login);
// Link to the after login page from the config file or to
  if ( isset( $_POST['REDIRECTTO'] ) ) header( "Location: ".$url->rawLink2( urldecode($_POST['REDIRECTTO']) ) );
                                  else header( "Location: ".$url->rawLink2( $cfg->get("AfterLogInPage") ) );
?>
