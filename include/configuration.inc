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
* The active configuration is selected the parameter $_GET["config"].
*
* In the given config file you can include other config files by using the 
* include = "../ini/configA.ini, ../ini/configB.ini, "
* field.
* As you can see many files can be included.
* Inclusion can happen recursively, so confA.ini can include confB.ini which can include confC.ini, etc.
*
* This can be useful to share parts of configurations like databases between configurations.
*
* @module     ../include/configuration.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2011
* @version    1.3
*
* @param $_GET["config"]  If present "../ini/config_".$_GET["config"].".ini" is loaded.
*                         If not present it is assumed that the config file is "../ini/config_demo.ini".
*/
if ( isset($cfg) ) return;

require_once( INCLUDE_DIR."/classes/Rom.inc" );

// use the hostname prefix as base for the config?
  $configPrefix='';
  /* An interesting feature is to use name based hosts 
   * to select the proper configuration files.
   *
   * In the following script the $_GET['config'] gets
   * rewritten according to the hostname.
   *
   * As configuration for /include/configuration.inc the following 
   * file is used. 
   * 
   * Note the big letters of the hostnames.
   * Its content is:
   * HOSTNAME     =  "configprefix"
   * ILAB         =  "ilab_"
   * ILAB2        = "ilab2_"
   */
  $h2cFile = "../ini/host2config.ini";
  if ( !file_exists( $h2cFile ) ) return; // no configuration there
  $h2c = new Rom( parse_ini_file( $h2cFile ) );
  $subHost = strtoupper( substr($_SERVER['SERVER_NAME'], 0, strpos($_SERVER['SERVER_NAME'], '.') ) );
  if ($h2c->doesExist( $subHost )) $configPrefix=$h2c->get( $subHost );
  // to always add the infix of the hostname the following code may help:
  // $configPrefix=strtolower(substr($_SERVER['SERVER_NAME'], 0, strpos($_SERVER['SERVER_NAME'], '.'))).'_';
// /use the hostname prefix as base for the config?


if ( $GLOBALS['url']->available("config") )
  $configFile = "../ini/config_".$configPrefix.$GLOBALS['url']->get("config").".ini";
else
  $configFile = "../ini/config_demo.ini";

$configData = array('include' => $configFile);
while ( $configData['include'] != '' ){
  $nextConfigs2Include = explode(',', $configData['include']);
  $configData['include'] = ''; // clear field, might be refilled in the parsing process
  
  foreach ( $nextConfigs2Include as $configFile ){
    $configFile = trim( $configFile ); // remove white spaces at borders
    if ( !file_exists( $configFile ) ) // File there?
      { trigger_error( $configFile.' missing! (config-file)' , E_USER_ERROR ); exit; }
    $newConfigData = parse_ini_file( $configFile );
    $configData = array_merge( $newConfigData, $configData ); // existing data overwrites included data (overwrites non-numerical key/ value pairs)
    if ( isset($newConfigData['include']) ) $configData['include'] = $newConfigData['include']; // only the includes must be followed further...
  }
}

$cfg = new Rom( $configData );
?>