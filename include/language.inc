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
* @module     ../include/language.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $_GET["lng"]    If present "../ini/".$_GET["lng"].".lng" is loaded.
*                         If not present the language file given in the ini-file is loaded.
*/

require_once( INCLUDE_DIR."/configuration.inc" );
require_once( INCLUDE_DIR."/classes/Rom.inc" );

if ( isset($_GET["lng"]) )
  $languageFile = $cfg->get("SystemResourcePath").$_GET["lng"].'.lng';
else
  $languageFile = $cfg->get("SystemResourcePath").$cfg->get("SystemDefaultLanguage");
  
if ( !file_exists( $languageFile ) ) trigger_error( $languageFile.' missing! (language-file)' , E_USER_ERROR );
$lng = new Rom( parse_ini_file( $languageFile ) );
?>