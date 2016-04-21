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

if ( $GLOBALS['url']->available("config") ) session_name ( $url->get("config") );
/* This allows users to log on for different configurations with different cookies.
   Otherwise the one session cookie would be used for all configurations and that could cause trouble
   because some settings (p.e. seeingUID for correcting) might not exist in the other config's DBs...
 */

// enable HTTP only to avoid JS cookie stealing
$cookieInfo = session_get_cookie_params();
session_set_cookie_params(
    $cookieInfo['lifetime'],
    $cookieInfo['path'],
    $cookieInfo['domain'],
    $cookieInfo['secure'],
    TRUE
);
unset( $cookieInfo );

if ( isset( $NEWSESSION ) ) {
   session_regenerate_id(true);
}

session_start();
print_r($_SESSION);
require_once( INCLUDE_DIR."/classes/Url.inc");
if ( strip_tags(SID) != "" ) $url->put( strip_tags(SID) );

if ( isset($_SESSION['uid']) &&/* if one of the fields is available all must be */
     ( isset($_SESSION['config']) && !empty($_SESSION['config']) && ( $GLOBALS['url']->available('config') && ($_SESSION['config'] == $url->get('config') ) ) )
     /* This second check (you view data for your configuration) is important because otherwise
        one could change to another configuration and keep his rights even though he did never log
        in for that configuration!
      */
    ) {
  $usr = new Usr($_SESSION['uid']);
}else{
	echo('Session is not found or not valid for this instance: '.$_SESSION['config'].' != '.$url->get('config'));
}
?>
