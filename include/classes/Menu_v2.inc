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
 * @author     Christoph Schwarzenberg
 * @version    1.0
 */
require_once(INCLUDE_DIR . "/classes/Url.inc");

class Menuv2 {
  var $cssRowClass;
  var $cssCellClassOn;
  var $cssCellClassOff;
  var $cssCellClassHover;
  var $cssCellClassTitle;
  var $cssCellClassSpacer;
  var $subMenuIndenter; // used to indent submenu entries...
  var $gotOneMenuEntry; // gets true if a menu entry got highlighted


  public $entries;

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
  function __construct($cssRowClass, $cssCellClassOn, $cssCellClassOff, $cssCellClassHover, $cssCellClassTitle, $cssCellClassSpacer) {
    global $cfg;
    $this->cssRowClass = $cssRowClass;
    $this->cssCellClassOn = $cssCellClassOn;
    $this->cssCellClassOff = $cssCellClassOff;
    $this->cssCellClassHover = $cssCellClassHover;
    $this->cssCellClassTitle = $cssCellClassTitle;
    $this->cssCellClassSpacer = $cssCellClassSpacer;
    $this->subMenuIndenter = "";
    $this->gotOneMenuEntry = false;

    $this->entries = [];
  }

  /**
   * Creates a menu entry.
   * To non-absolute urls ( characterized by their beginning with "http://" ) the current environment is added (like config=...).
   * @param $strText   The menu entries title.
   * @param $link      The link the menu entry should link to.
   * @param $isOn      Is the menu active? This gets set if the ../menu.inc determines that this entry is the one that is currently shown (by address=).
   * @param $unfold bool  Render opened collapsed
   *
   * @return           HTML-code.
   */
  function entry($strText, $link, $isOn = false, $unfold = true, $children = []) {
    $this->gotOneMenuEntry |= $isOn;
    // The second possibility of setting a menu active is by its title:
    global $pge;
    if (!$isOn &&
      !$this->gotOneMenuEntry &&
      (($GLOBALS['url']->available('address') && ("../pages/view.php?address=" . $GLOBALS['url']->get('address') == $link))
        || // this link
        (!$GLOBALS['url']->available('address') && ($strText == $pge->matchingMenu))
      )
    ) $isOn = true;
    // If not absolute link add url stuff.
    // TODO: improve the interface so we don't have to magically rewrite stuff
    if (!(strtoupper(substr($link, 0, 4)) == 'HTTP' &&
      (strpos($link, '/pages/login.php') === false) // pages within the system should be linked with additional tags...
    )) $link = $GLOBALS['url']->rewriteExistingUrl($link);

    return [
      'css_class' => $isOn ? $this->cssCellClassOn : $this->cssCellClassOff,
      'href' => $link,
      'target_blank' => (strpos($link, "http://") !== false),
      'title' => $strText,
      'text' => $this->subMenuIndenter . $strText,
      'unfold' => $unfold,
      'children' => $children,
    ];
  }

  function entry_group($strText, $children) {
    return [
      'css_class' => '',
      'href' => false,
      'target_blank' => '',
      'title' => $strText,
      'text' => $strText,
      'unfold' => true,
      'children' => $children,
    ];
  }
}