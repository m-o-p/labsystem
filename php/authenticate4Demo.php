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
* NOT NECESSARY FOR NORMAL USE!!!
*
* Called to do the authentication for the demo page.
*
* @module     ../php/authenticate4Demo.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2009
* @version    1.0
*
* @param $_GET['userrole'] user, corrector, admin
*/
require( "../include/init.inc" );

$allowed = Array( 'demo1',
                  'demo2',
                  'demo3',
                  'demo4',
                  'demo5'
                 );
if ( !in_array( $GLOBALS['url']->get('config'), $allowed ) ){
                                                trigger_error( 'login not allowed with this config! '.$GLOBALS['url']->get('config'), E_USER_ERROR );
                                                exit;
                                              }
if ( !$GLOBALS['url']->available('userrole') ){
                                  trigger_error( 'userrole not provided', E_USER_ERROR );
                                  exit;
                                 }

require_once( INCLUDE_DIR."/classes/DBInterfaceUser.inc" );

$uDBI = new DBInterfaceUser();

// preserve the current url since we will probably link back (p.e. to give an error) or add something to the url.
$url->clearQueryString(); $url->put( $url->get("oldQueryString") );

/* demo login issues */
                      $authUserData["uid"]      = "demoUser";
                      $authUserData["userName"] = "DemoUser";
                      $authUserData["foreName"] = "Marc-Oliver";
                      $authUserData["name"]     = "Pahl";
                      $authUserData["eMail"]    = "pelase_donate@labsystem.m-o-p.de";
/* /demo login issues */

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

  $_SESSION["currentTeam"] = 1234;
if ( $GLOBALS['url']->get('userrole') == 'all' ){
  $_SESSION["userRights"]  = (MAX_USER_ROLE<<1)-1; // all
  $link2 = $cfg->get("AfterLogInPage");
}
elseif ( $GLOBALS['url']->get('userrole') == 'corrector' ){
  $_SESSION["userRights"]  = IS_USER+IS_MAIL_SUPPORTER+IS_ALL_MAILER+IS_SCHEDULER+IS_CORRECTOR+IS_EX_SOLUTION_VIEWER; // corrector
  $_SESSION['seeingUID']   = 'participant'; // correct this guy
  $_SESSION['seeingDESCR'] = 'Patrice Participant (patrice)';
  $link2 = '../pages/view.php?address=l2.allLabQ';
}
else{
  $_SESSION["userRights"]  = IS_USER; // user
  $link2 = '../pages/view.php?address=l2.C6.c2';
}

// Link to the after login page from the config file or to 
  header( "Location: ".$url->rawLink2( $link2/* .'&tinyMCE' */ ) );
?>
