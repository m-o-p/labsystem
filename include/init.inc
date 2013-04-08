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
* This module contains the standard page includes.
*
* @module     ../include/init.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/

// time measurement
$mtime = microtime();
$mtime = explode(" ",$mtime);
$mtime = $mtime[1] + $mtime[0];
$pagecreation_starttime = $mtime;

ini_set( "display_errors", "On" ); // uncomment to force php to display errors (php.ini value gets temporarily overwritten).
//error_reporting(E_ALL ^ (E_NOTICE & E_STRICT) ); // remove warnings php5
ini_set( "register_globals", "Off" ); // a) security b) problems may arise with duplicate variable names

/* The order of the includes should be like it is since the later
 * includes require the presence of the previous ones. */

define( "INCLUDE_DIR", "../include" );

require( INCLUDE_DIR."/classes/Url.inc" );      // Include url handling and rewriting stuff. => Object $url.
                                                // needed to get parameters from the url ($url->get, ->available)

require( INCLUDE_DIR."/customErrHandle.inc" );  // The custom Error handler.
require( INCLUDE_DIR."/helper.inc" );           // Some helper functions that are used.

require( INCLUDE_DIR."/configuration.inc" );    // Include the global configuration. => Object $cfg.
require( $cfg->get("SystemResourcePath").$cfg->get("SystemUserRoles") );  // Include the user role constants.
require( INCLUDE_DIR."/language.inc" );         // Include the localized Strings. => Object $lng.

$url->preserve("nomenu");                       //  keep in url if present (menu off)
$url->preserve("config");                       //  keep in url if present (config file)
$url->preserve("lng");                          //  keep in url if present (language file)
$url->preserve("tinyMCE");                      //  keep in url if present (tinyMCE integration (200k js))
$url->preserve("hideAnswers");                  //  keep in url if present (hideAnswers if in seeing mode)
$url->preserve('hideExampleSolutionInThisTab');

require( INCLUDE_DIR."/classes/ElementUser.inc" );                // user
require( INCLUDE_DIR."/classes/Usr.inc" );      /* Creates a current user with default values for accessLevel, name, ...
                                                   => Object $usr.
                                                 */
require( INCLUDE_DIR."/session.inc" );          /* Include session management (db based) and, if available,
                                                   set the information ($usr data) from the session.
                                                 */
require( INCLUDE_DIR."/logger.inc" );          /* Include to enable logging. */
                                               // activated when config contains lagDir = "someDirectory"

require_once( INCLUDE_DIR."/classes/elements/LpDBInterface.inc" ); // page
require_once( INCLUDE_DIR."/classes/elements/LmDBInterface.inc" ); // multiple choice
require_once( INCLUDE_DIR."/classes/elements/LiDBInterface.inc" ); // input
require_once( INCLUDE_DIR."/classes/elements/LsDBInterface.inc" ); // schedule
require_once( INCLUDE_DIR."/classes/elements/LcDBInterface.inc" ); // collection
require_once( INCLUDE_DIR."/classes/elements/BCDBInterface.inc" ); // Collection
require_once( INCLUDE_DIR."/classes/elements/LlDBInterface.inc" ); // lab

require( INCLUDE_DIR."/classes/Pge.inc" );      /* Extends the page element and will hold the values for the currently display-
                                                   ing html page (p.e. accessLevel, title, ...). => Object $pge.
                                                 */
require( INCLUDE_DIR."/classes/ElementButtons.inc" ); // Standard buttons.
require( INCLUDE_DIR."/classes/ElementMenues.inc" );  // Standard menues.
?>