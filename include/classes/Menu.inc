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
* Contains the Menu class.
*/

/**
* This Class encapsulates the menu functionality.
* This class only creates list entries! The list (<ul>) around must be present.
*
* The menu will get something like:
* <ul>
*  <li>0 grade</li>
*  <li>0 grade
*   <ul>
*    <li>1st grade</li>
*    <li>1st grade
*     <ul>
*      <li>2nd grade</li>
*     </ul>
*    </li>
*   </ul>
*  </li>
* </ul>
*
* @module     ../include/classes/Menu.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
require_once( INCLUDE_DIR."/classes/Url.inc" );

class Menu{
  var $cssRowClass;
  var $cssCellClassOn;
  var $cssCellClassOff;
  var $cssCellClassHover;
  var $cssCellClassTitle;
  var $cssCellClassSpacer;
  var $subMenuIndenter; // used to indent submenu entries...
  var $maxCharsPerMenuEntry; // The menu entry will be cut after so many character (the title (hover) will stay the full title)
  var $gotOneMenuEntry; // gets true if a menu entry got highlighted
  
  /**
  * The menu is based on stylesheet entries. You can have totally different looking menus.
  * Just create a new instance of this object with other underlying stylesheet classes.
  * @param $cssRowClass         Used to format the table row.
  * @param $cssCellClassOn      Used to format a table cell that is on.
  * @param $cssCellClassOff     Used to format a table cell that is off.
  * @param $cssCellClassHover   Used to format a table cell that gets hovered.
  * @param $cssCellClassTitle   Used to format a table cell that contains a title.
  * @param $cssCellClassSpacer  Used to format a table cell that contains a spacer (empty).
  */
  function Menu( $cssRowClass, $cssCellClassOn, $cssCellClassOff, $cssCellClassHover, $cssCellClassTitle, $cssCellClassSpacer ){
    global $cfg;
    $this->cssRowClass = $cssRowClass;
    $this->cssCellClassOn = $cssCellClassOn;
    $this->cssCellClassOff = $cssCellClassOff;
    $this->cssCellClassHover = $cssCellClassHover;
    $this->cssCellClassTitle = $cssCellClassTitle;
    $this->cssCellClassSpacer = $cssCellClassSpacer;
    $this->maxCharsPerMenuEntry = $cfg->get("maxCharsPerMenuEntry");
    $this->subMenuIndenter = "";
    $this->gotOneMenuEntry = false;
  }
  
  /**
  * This is added in front of the title to indent submenus. 
  * Stylesheet features could also be used for that but they would have to surround the entries
  * and even more important the calculation in cutTitle would have to deal with the number of
  * surrounding stylesheet tags -> too much unnecessary work.
  */
  /*private*/ function forSubmenuAdd(){
    return "&nbsp; &nbsp;";
  }
  
  /**
  * Starts a submenu with given stuff as first entry.
  * To non-absolute urls ( characterized by their beginning with "http://" ) the current environment is added (like config=...).
  * @param $strText   The menu entries title.
  * @param $link      The link the menu entry should link to.
  * @param $isOn      Is the menu active? This gets set if the ../menu.inc determines that this entry is the one that is currently shown (by address=).
  *
  * @return           HTML-code.
  */
  function startSubmenu( $strText, $link, $isOn=false ){
    $val = $this->entry( $strText, $link, $isOn, true )."<ul>\n";
    $this->subMenuIndenter .= $this->forSubmenuAdd();
    return $val;
  }
  
  /**
  * This function gets called to mark the end of a submenu.
  */
  function stopSubmenu(){
    $this->subMenuIndenter = substr( $this->subMenuIndenter, 0, -1*strlen( $this->forSubmenuAdd() ) );
    return "</ul>\n\t</li>\n";
  }

  /**
  * Creates a menu entry.
  * To non-absolute urls ( characterized by their beginning with "http://" ) the current environment is added (like config=...).
  * @param $strText   The menu entries title.
  * @param $link      The link the menu entry should link to.
  * @param $isOn      Is the menu active? This gets set if the ../menu.inc determines that this entry is the one that is currently shown (by address=).
  * @param $dontClose Doesn't close the <li> section. Only used inside this class to start a Submenu.
  *
  * @return           HTML-code.
  */
  function entry( $strText, $link, $isOn=false, $dontClose=false ){
    $this->gotOneMenuEntry |= $isOn;
    // The second possibility of setting a menu active is by its title:
    global $pge; if ( !$isOn && 
                      !$this->gotOneMenuEntry &&
                      ( ( $GLOBALS['url']->available('address') && ( "../pages/view.php?address=".$GLOBALS['url']->get('address') == $link ) ) 
                        || // this link
                        ( !$GLOBALS['url']->available('address') && ( $strText == $pge->matchingMenu ) )
                       )
                     ) $isOn = true;

    // If not absolute link add url stuff.
    if ( !(strtoupper( substr( $link, 0, 4 ) ) == 'HTTP' && 
           (strpos( $link, $_SERVER[ 'SERVER_NAME' ].'/pages/login.php' ) === false) // pages within the system should be linked with additional tags...
           ) ) $link = $GLOBALS['url']->link2( $link );
                     
    if ( $isOn ){ // All states should be on.
      $cssCellClassOn    = $this->cssCellClassOn;
      $cssCellClassOff   = $this->cssCellClassOn;
      $cssCellClassHover = $this->cssCellClassOn;
    } else {
      $cssCellClassOn    = $this->cssCellClassOn;
      $cssCellClassOff   = $this->cssCellClassOff;
      $cssCellClassHover = $this->cssCellClassHover;
    }
    return  "\t<li class=\"".$cssCellClassOff."\">".
    /* The Linktext is cut by ../include/helper.inc->cutHTMLText( , $this->maxCharsPerMenuEntry ).
    *  The title tag contains the whole title so on  mouseover the user gets it. */
            "<a class=\"".$cssCellClassOff."\" href=\"".$link."\" ".( ( strpos( $link, "http://" ) !== false ) ? 'target="_blank" ' : '' )."title=\"".$strText."\">".cutHTMLText( $this->subMenuIndenter.$strText, $this->maxCharsPerMenuEntry )."</a>".
            ( !$dontClose ?  '</li>'  : '' )."\n";
  }
  
  /**
  * Creates a title. (Not hoverable and only for grouping purposes).
  * @param $strText   The titles text.
  *
  * @return           HTML-code.
  */
  function title( $strText ){
    return "\t<li class=\"".$this->cssCellClassTitle."\">".$strText."</li>\n";
  }  

  /**
  * Creates a spacer (empty menu entry).
  *
  * @return           HTML-code.
  */
  function spacer(){
    return "\t<li class=\"".$this->cssCellClassSpacer."\">&nbsp;</li>\n";
  }
}