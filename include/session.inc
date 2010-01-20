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
* This module restores the session stuff.
*
* @module     ../include/session.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/

if ( !isset( $_GET["inside"] ) &&    // if user is not logged in, we don't need that
     !isset( $NEWSESSION ) ) return; // but if we just authenticated and want to start!

if ( isset( $_GET["config"] ) ) session_name ( $url->get("config") );
/* This allows users to log on for different configurations with different cookies.
   Otherwise the one session cookie would be used for all configurations and that could cause trouble
   because some settings (p.e. seeingUID for correcting) might not exist in the other config's DBs...
 */

require( INCLUDE_DIR."/php_session_management.inc"); /** store the session variables in the table instead of a cookie or url */

if ( isset( $NEWSESSION ) ) {
  $url->put("inside=true");
  session_regenerate_id();
}else
  $url->preserve("inside"); /* The presence of this tag in the url has two effects:
                             *  - not all that stuff got to be loaded (but could be)
                             *  - if one doesn't log ou you are not automatically in if cookie is set
                             *    because as soon as you log in, you override the cookie.
                             *    (Of course one can add ?inside=true to the url and will still be in).
                             */

session_start();

require_once( INCLUDE_DIR."/classes/Url.inc");
if ( strip_tags(SID) != "" ) $url->put( strip_tags(SID) );

if ( isset($_SESSION["uid"]) &&/* if one of the fields is available all must be */
     ( ($_SESSION["config"] == "") || ( isset( $_GET['config'] ) && ($_SESSION["config"] == $url->get('config') ) ) )
     /* This second check (you view data for your configuration) is important because otherwise
        one could change to another configuration and keep his rights even thoug he did never log
        in for that configuration!
      */
    ) { 
  $usr->uid         = $_SESSION["uid"];
  $usr->userName    = $_SESSION["userName"];
  $usr->foreName    = $_SESSION["foreName"];
  $usr->surName     = $_SESSION["surName"];
  $usr->currentTeam = $_SESSION["currentTeam"];
  $usr->userRights  = $_SESSION["userRights"];
  $usr->mailAddress = $_SESSION["mailAddress"];
}
?>