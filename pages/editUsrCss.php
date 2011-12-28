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
* Edit page for the user stylesheet.
*
* To be editable it must be writable by php...
*
* @module     ../pages/editMenu.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2011
* @version    1.1
*/
require( "../include/init.inc" );

$file2edit        = $cfg->get("UserStyleSheet");
$userRestriction  = IS_CONTENT_EDITOR;
$matchingMenu     = $lng->get( "MnuEntryStyleSheet" ); /* must be the same as in the menu file fo rhighlighting! */
$filePrefix       = "" ; // for the IS_CONFIG_EDITOR in the useradmin configuration.
                         // Only files with this prefix will be editable.

require( "pgeStdFileEdit.inc" );

require( $cfg->get("SystemPageLayoutFile") );
?>
