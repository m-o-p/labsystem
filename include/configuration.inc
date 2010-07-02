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
* This module loads the configuration file (contains global variables like database, title, ...).
* Multiple configurations are possible thru the parameter $_GET["config"].
*
* @module     ../include/configuration.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $_GET["config"]  If present "../ini/config_".$_GET["config"].".ini" is loaded.
*                         If not present it is assumed that the config file is "../ini/config_demo.ini".
*/
if ( isset($cfg) ) return;

if ( !isset($configPrefix) ) $configPrefix='';

require_once( INCLUDE_DIR."/classes/Rom.inc" );

if ( isset($_GET["config"]) )
  $configFile = "../ini/config_".$configPrefix.$_GET["config"].".ini";
else
  $configFile = "../ini/config_demo.ini";
  
if ( !file_exists( $configFile ) ){
                                    trigger_error( $configFile.' missing! (config-file)' , E_USER_ERROR );
                                    exit;
                                   }
$cfg = new Rom( parse_ini_file( $configFile ) );
?>