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
* This module loads the language file (contains localized system strings).
* Multiple languages can easily be integrated:
*  - You use different config files
*  - You parse for a tag like $_GET["lng"] and include the appropriate language file.
*    ( see ../include/configuration.inc for such an example ).
*
*
* In the given language file you can include another (default) language file by using the "include = " syntax.
* This including can only happen once not recursively.
* The included file must also be in the $cfg->get("SystemResourcePath") directory.
*
* @module     ../include/language.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.1
*
* @param $_GET["lng"]    If present "../ini/".$_GET["lng"].".lng" is loaded.
*                         If not present the language file given in the ini-file is loaded.
*/

require_once( INCLUDE_DIR."/configuration.inc" );
require_once( INCLUDE_DIR."/classes/Rom.inc" );

if ( $GLOBALS['url']->available("lng") )
  $languageFile = $cfg->get("SystemResourcePath").$GLOBALS['url']->get("lng").'.lng';
else
  $languageFile = $cfg->get("SystemResourcePath").$cfg->get("SystemDefaultLanguage");

if ( !file_exists( $languageFile ) ){
                                      trigger_error( $languageFile.' missing! (language-file)' , E_USER_ERROR );
                                      exit;
                                     }

$runningSystemLanguage = $GLOBALS['url']->available("lng") ? $GLOBALS['url']->get("lng") : substr( $cfg->get("SystemDefaultLanguage"), 0, strpos( $cfg->get("SystemDefaultLanguage"), '.' ) ); // used by /include/page_layout.inc to link to the language selector
                                     
$languageData = parse_ini_file( $languageFile );

if (isset( $languageData['include'] )){ // we have an include for default values...
  $languageFile2 = $cfg->get("SystemResourcePath").$languageData['include'].'.lng';
  if ( !file_exists( $languageFile2 ) ){
                                      trigger_error( $languageData['include'].' included in '.$languageFile.' missing! (language-file)' , E_USER_ERROR );
                                      exit;
                                     }
  else{
    $languageData = array_merge( parse_ini_file( $languageFile2 ), $languageData ); // latter one overwrites non-numerical key/ value pairs of former one
  }
}

$lng = new Rom( $languageData );
?>