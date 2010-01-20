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
* This module parses the menu ini and creates the menu using ../include/classes/Menu.inc.
* IT gets called by ../include/page_layout.inc if $_GET["nomenu"] is not present.
*
* @module     ../include/menu.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/

/* The size of the table is affected by the presence of the "nomenu" in the url.
 * If you only change it in the stylesheet you are safe...
 * page_layout.inc also sets the size! 
 */
require_once( INCLUDE_DIR."/classes/Menu.inc" );
 
echo("<div class=\"labsys_mop_menu\"><ul>\n");

$myMenu = new Menu( "labsys_mop_menuEntry", "on", "off", "hover", "title", "spacer" ); // So you could easy create menus based on other stylesheet entries.

$mainMenuIni = parse_ini_file( $cfg->get("SystemResourcePath").$cfg->get("SystemMenuFile"), TRUE ); // The menu resource file gets loaded.

foreach ( $mainMenuIni as $key => $value ){ // Iteration over the menu entries.
  $hasTitle = false; // -> $this::doTitle();
  $possibleMenu = "";
  
  // "[sys_login]" starts with the login/ -ou stuff.
  if ( $key == "sys_login" ){
                              $key = $usr->userName;
                              if ( $usr->isLoggedIn() ) 
                                $possibleMenu .= $myMenu->entry( $lng->get( "MnuEntryLogOut" ), "../php/logout.php" );
                              else
                                $possibleMenu .= $myMenu->entry( $lng->get( "MnuEntryLogIn" ), retIfTrue( $cfg->doesExist( 'SSLLogin' ) && ( $cfg->get( 'SSLLogin' ) == 1 ), 'https://'.$_SERVER[ 'SERVER_NAME' ].'/pages/login.php', '../pages/login.php') );
                             }

  // Parse the submenu:
  foreach ( $value as $key_ => $value_ ){ // Parse the section and make the menu entries
    if ( ( ( $posBang = strpos( $value_, "!" ) ) !== FALSE ) ){ // We have to test if restriction is fulfilled.
      if ( ($posBang == 0) && is_string( $value_{1} ) && !is_numeric( $value_{1} ) && is_numeric( substr( $value_, 2) ) ){
          // is syntactically a shortcut (p.e. "!p2")
          $id = $value_{1};
          require( "../php/getDBIbyID.inc" ); /* -> $DBI */
          $element = $DBI->getMenuData2idx( substr( $value_, 2) );
          if ( $element && $element->isVisible() ) $possibleMenu .=  $element->getMenu( substr( $value_, 1 ), 
                                                                                        $myMenu, "", 
                                                                                        isset( $GLOBALS["firstFinal"] ) && 
                                                                                        ( $GLOBALS["firstFinal"] == substr( $value_, 1 ) ) ); /* I am the current top element viewed and so I should unfold */
          continue; // no more processing here -> continue with next
      }
      if ( !$usr->isOfKind( substr($value_, $posBang+1) ) ) continue; // the entry is restricted and the user has not the necessary rights
      $value_ = substr( $value_, 0, $posBang ); // remove all beginning from "!"
    }
        
    if ( $value_ == "" ){
      if ( $possibleMenu != "" ) // show spacer only if there is at least one entry above
        $possibleMenu .= $myMenu->spacer();
    }else $possibleMenu .= $myMenu->entry( $key_, $value_ ); // show the menu entry
  }
  
  if ( $possibleMenu != "" ) echo( $myMenu->title( $key ).$possibleMenu ); // Only show the title if there is something inside the section.
}
echo("\t</ul></div>\n");
?>