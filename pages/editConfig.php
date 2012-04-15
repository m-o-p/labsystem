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
* Edit page for the configs.
*/
require( "../include/init.inc" );

$file2edit        = $currentConfig;
$pge->visibleFor  = IS_CONFIG_EDITOR;
$matchingMenu     = $lng->get( "MnuEntryEditConfig" ); /* must be the same as in the menu file fo rhighlighting! */
$filePrefix       = "config_".$configPrefix; // for the IS_CONFIG_EDITOR in the useradmin configuration.
                                             // Only files with this prefix will be editable.
                                             // $configPrefix comes from configuration.inc

if ( $usr->isOfKind( $pge->visibleFor ) ){
// The pge is inheriting from Element which is always
// visible for IS_CONTENT_EDITOR.
// as we do not want this we exclude it here.
// -> not IS_CONFIG_EDITOR -> blank
  require( "pgeStdFileEdit.inc" );
  // the following could be the currently open configuration...
  // this depends on the host we are currently on...
  // one could improve this with the host2config in the configuration.inc
  // by loading the host respectively...
  $currentlyOpenConfiguration = substr( $file2edit,
                                       ($tempStart=strpos( $file2edit, $filePrefix )+
                                                   strlen( $filePrefix )
                                        ), strrpos( $file2edit, '.' )-$tempStart );
  $pge->put( "<div class=\"labsys_mop_note\">\n".
             $lng->get( 'setupLinkNote' ).
             ' <a href="../setup?config='. $currentlyOpenConfiguration.'">[setup&gt;&gt;]</a>'.
             "</div>\n" );
}

require( $cfg->get("SystemPageLayoutFile") );
?>
