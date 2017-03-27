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

/* This allows users to log on for different configurations with different cookies.
   Otherwise the one session cookie would be used for all configurations and that could cause trouble
   because some settings (p.e. seeingUID for correcting) might not exist in the other config's DBs...
 */

require_once(INCLUDE_DIR."/classes/Url.inc");
if ($GLOBALS['url']->available('config')) {
    session_name($url->get("config"));
    if (isset($_COOKIE[session_name()]) || isset($NEWSESSION)) {
        // enable HTTP only to avoid JS cookie stealing
        // also set the secure flag
        $cookieInfo = session_get_cookie_params();
        session_set_cookie_params(
            $cookieInfo['lifetime'],
            $cookieInfo['path'],
            $cookieInfo['domain'],
            TRUE, // secure: If TRUE cookie will only be sent over secure connections.
            TRUE // httponly: If set to TRUE then PHP will attempt to send the httponly flag when setting the session cookie.
        );
        unset($cookieInfo);
        session_start();

        if (isset($NEWSESSION)) {
            // the session is already authenticated, fill the required info,
            // regenerate session id just in case
            session_regenerate_id(true);
            $_SESSION['uid'] = $NEWSESSION;
            $_SESSION['config'] = $url->get('config');
        }

        if (isset($_SESSION['uid']) && isset($_SESSION['config']) &&
                $_SESSION['config'] == $url->get('config')) {
            // new or resumed session, load data
            $usr = new Usr($_SESSION['uid']);
        } else {
            // not a new session and no valid existing session found, destroy
            // session and cookie
            setcookie(session_name(), NULL, 1, '/');
            session_unset();
            session_destroy();
        }
        // TODO: ideally we would stop using the session all together at this point and can close it (
        // (this if we do this, accessing _SESSION[..] is no longer possible
        // session_write_close();
    }
}

?>
