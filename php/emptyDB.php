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

/*
 *
 * FOR NORMAL USE YOU DO NOT NEED THIS FILE!
 *
 *
 * This is used for the demo installation only. As it empties databases
 * it is restricted to certain configurations only.
 *
 * The according config files must exist as well as the according tables.
 *
 * Consider /ini/#demosetup.txt for more information.
 */
define( "INCLUDE_DIR", "../include" );

require( INCLUDE_DIR."/classes/Url.inc" );      // Include url handling and rewriting stuff. => Object $url.
                                                // needed to get parameters from the url ($url->get, ->available)

require( INCLUDE_DIR."/customErrHandle.inc" );  // The custom Error handler.

$allowed = Array( 'demo1',
                  'demo2',
                  'demo3',
                  'demo4',
                  'demo5'
                 );
if ( !in_array( $GLOBALS['url']->get('config'), $allowed ) ){
                                                trigger_error( 'db.emptying not allowed with this config! '.$GLOBALS['url']->get('config'), E_USER_ERROR );
                                                exit;
                                              }

require_once( INCLUDE_DIR."/configuration.inc" );

// does NOT work in SAFE_MODE!!!!
exec( 'mysql --debug-info -u'.$cfg->get("WorkingDatabaseUserName").' -p'.$cfg->get("WorkingDatabasePassWord").' '.$cfg->get("WorkingDatabaseName").' < forTest.sql' );

sleep ( 2 ); // wait for db to settle...
header( 'Location: ../php/authenticate4Demo.php?config='.$GLOBALS['url']->get('config').'&sysinfo='.urlencode( 'db.emtied' ).'&userrole='.$GLOBALS['url']->get('userrole') );

?>
