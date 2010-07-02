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
* History page for the element $lastFinal.
*
* @module     ../pages/editMenu.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $_GET['address'] The address of the element the history should be shown of.
* @param $_GET['history'] For additional history (p.e. user stats not saved with the element).
*/
require( "../include/init.inc" );
require( "../php/getFirstLastFinal.inc" ); $id = $lastFinal{0}; $num = substr( $lastFinal, 1);
require( "../php/getDBIbyID.inc" ); /* -> $DBI */

if ( !$element = $DBI->getData2idx( $num ) ){
                                              trigger_error( $lng->get(strtolower( $id )."Number").$num." ".$lng->get("doesNotExist"), E_USER_ERROR );
                                              exit;
                                             }

$pge->title        = $lng->get("historyOf")." ".$id.$num.": ".$element->title;
$pge->matchingMenu = $element->getMatchingMenu();

$pge->put('<div class="labsys_mop_h2"><span class="labsys_mop_grayed">'.$lng->get("historyOf")." ".$id.$num.": </span>".$element->title.'</div>'."\n");
$pge->put('<div class="labsys_mop_history" style="display: block;">'.nl2br( $element->history ).'</div>' );

// history= & title= set in url? -> Additional history (like user answer stuff) -> doesn't get loaded but displayed
if ( isset( $_GET['history'] ) ){
  $pge->put('<div class="labsys_mop_h2"><span class="labsys_mop_grayed">'.$lng->get('addHistory').'</span></div>'."\n");
  $pge->put('<div class="labsys_mop_additional_history">'.stripslashes( $url->get('history') ).'</div>' );
}

// back button
$pge->put( "<div class=\"labsys_mop_button_fullwidth\">\n".
           "<a href=\"javascript: history.back();\">".
           $lng->get("back").
           "</a>".
           "</div>\n"
          );
  
require( $cfg->get("SystemPageLayoutFile") );
?>
