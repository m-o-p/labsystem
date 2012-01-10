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
* This module contains some often used helper functions.
*
* @module     ../include/helper.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/


/**
* Returns an info arrow with describing $text as title (appears at mousevoer in most browsers)
* @param $text    The description the arrow should show.
* @param $grayed  Should the arrow be grayed?
*/
function infoArrow( $text, $grayed=false ){
  return "<img src=\"../syspix/arrow_left_9_12".( $grayed ? "_gray" : '' ).".gif\" width=\"9\" height=\"12\" border=\"0\" alt=\"&lt;\" title=\"".$text."\">";
}

/**
* Returns a checkbox input with infoArrow on its right.
* @param $name      The name the input should have for further form processing.
* @param $value     The (bitmask-)value the input should have for further form processing.
* @param $rights    The user's rights bitmask. If ($rights & $value) = $value the checkbox is checked.
* @param $disabled  Should the checkbox be disabled?
* @constant Explain_UR_ The "Explain_UR_[bitmask of the current right]" entries should be present in the language file!
*/
function rightsBox( $name, $value, $rights, $disabled=false ){ 
  global $lng, $usr, $pge;
  return "<input type=\"checkbox\" id=\"".$name."\" name=\"".$name."\" value=\"".$value."\" tabindex=\"".$pge->nextTab++."\"".
         ( $usr->isOfKind($value, $rights) ?  " checked=\"checked\"" : '').
         ( $disabled ? " disabled=\"disabled\"" : '' ).
         " onchange='isDirty=true'><label for=\"".$name."\" class=\"labsys_mop_input_field_label\">".infoArrow( $lng->get("Explain_UR_".$value), $disabled )."</label>"/*."\n" removed to save space... */;
}

/**
* Returns all possible user rights with the fitting checkmarks set HORIZONTALLY.
* @param $prefix    The $prefix_[bitmask of the current right] gives the name of the input for further form processing.
* @param $rights    The user's rights bitmask. If ($rights & $value) = $value the checkbox is checked.
* @param $disabled  Should the checkbox be disabled?
* @constant MAX_USER_ROLE Used to determine the amount of Userright-Boxes.
*/
function showRightsHorizontal( $prefix, $rights, $disabled=false ){ 
  $ret = "";
  for ($i=1; $i<=MAX_USER_ROLE; $i=$i<<1)
    $ret .= rightsBox( $prefix."_".$i, $i, $rights, $disabled );
  return $ret;
}

/**
* Returns all possible user rights with the fitting checkmarks set VERTICALLY.
* @param $prefix    The $prefix_[bitmask of the current right] gives the name of the input for further form processing.
* @param $rights    The user's rights bitmask. If ($rights & $value) = $value the checkbox is checked.
* @param $disabled  Should the checkbox be disabled?
* @constant MAX_USER_ROLE Used to determine the number of Userright-Boxes.
*/
function showRightsVertical( $prefix, $rights, $disabled=false ){
    global $lng;
  $ret = "";
  for ($i=1; $i<=MAX_USER_ROLE; $i=$i<<1)
    $ret .= rightsBox( $prefix."_".$i, $i, $rights, $disabled )."<label for=\"".$prefix."_".$i."\" class=\"labsys_mop_input_field_label\">".$lng->get("Explain_UR_".$i)."</label><br>\n";
  return $ret;
}

/**
* This function is called to cut $text at position $where if longer.
* The special thing about that function is that it decodes html entities before counting (p.e. &nbsp; -> " ").
*
* Used in ../include/classes/Menu.inc and in the element's getPropertyRow().
*
* @param $text      The text to be eventually cut.
* @param $where     The number of characters it should be cut after.
*/
function cutHTMLText( $text, $where ){
  $decodedText = html_entity_decode( $text );                    // remove html escapes (p.e. &nbsp; -> " ")...
  if ( strlen( $decodedText ) <= $where )
    return $text;
  else
    return htmlentities ( substr( $decodedText, 0, $where-2)."..." );
}

/**
* Gets called to handle the "[HTML]" beginning tag of a string (page content etc.) appropriately.
* If the string starts with [HTML] no further processing is necessary (The browser will inerprete the following tags).
* If there is no [HTML] at the beginning of the string it gets nl2br( htmlspecialchars( string ) ); so all tags get filtered.
* Since the object comes by reference it is directly manipulated! (Doesn't work on array elements in php 4.3).
* @param &$object   The string to be parsed.
*/
function parseHTML( &$object ){
  // handle multiple spaces as Indention
  $toReplace = array( '   ', '  ', "\b([^ ])" );
  $replaceWith = array ( '&nbsp; &nbsp;', '&nbsp; ', "&nbsp;\\1" );
  
  if ( (substr( $object, 0, 4 ) == '[MD]') && file_exists( '../plugins/phpMarkDown/markdown.php' ) && include_once( '../plugins/phpMarkDown/markdown.php' ) ){ //phpMarkDown http://michelf.com/projects/php-markdown/
    $object = Markdown( $GLOBALS['pge']->sanitizeKeys( substr( $object, 4 ) ) ); // sanitize necessary as __ has a meaning in MarkDown...
  }
  else if ( substr( $object, 0, 6 ) == '[HTML]' ){ // HTML
    $object = substr( $object, 6 ); // just remove [HTML]
  }else{ // assume it is plain text
//    $object = preg_replace('/ ([\w\s])/', '&nbsp;\\1', nl2br( htmlspecialchars( $object ) ) );
//    $object = nl2br( htmlspecialchars( $object ) );
    $object = str_replace(array('  ', "\t"), array('&nbsp; ', '&nbsp; &nbsp; '),
                           nl2br( htmlentities( $object ) )
              );
  }
}

function tidyHTML( &$string ){
  if ( ($GLOBALS['cfg']->get('tidyHTML') == 'TRUE') && // tidying is configured on via config file option
       ( substr( $string, 0, 6 ) == '[HTML]' ) &&  // this is and HTML element
       class_exists( 'tidy' ) ){ // tidy exists on this system
    if (!isset( $GLOBALS['tidy'] )) $GLOBALS['tidy'] = new tidy();
    $string = "[HTML]\n".$GLOBALS['tidy']->repairString( substr( $string, 6 ), array('indent' => TRUE, 'show-body-only' => TRUE, 'vertical-space' => TRUE, 'wrap' => 100) );
  }
}

/**
* Difference to the previous: Doesn't directly alter the data but return it.
* Gets called to handle the "[HTML]" beginning tag of a string (page content etc.) appropriately.
* If the string starts with [HTML] no further processing is necessary (The browser will inerprete the following tags).
* If there is no [HTML] at the beginning of the string it gets nl2br( htmlspecialchars( string ) ); so all tags get filtered.
* @param  $object The string to be parsed.
* @return $string The altered $object.
*/
function returnHTML( $object ){
  parseHTML( $object );
  return $object;
}

/**
* Rremoves anything with special purpose from the $text for editing.
* @param  $text   The string to be parsed.
* @return $string The altered $text.
*/
function returnEditable( $text ){ /* removes anything with special purpose for editing */
  return str_replace( "_", "&#x005F;", htmlspecialchars( $text ) );
}

/**
* Creates a new element of type $elementID using the [ID]1 element as template.
*
* @param $elementID     ID of the element to be created (e{p,m,i,c}).
* @return $newIdx;
*/
function createNew( $elementID ){
  $elementIDsmall = strtolower( $elementID ); // for C we want to give a capital letter but the interfaces always hav small ones
  if ( !in_array ($elementIDsmall, array('c', 'p', 'm', 'i', 'l', 's')) ) return;
  $newElement = $GLOBALS[ $elementIDsmall.'DBI' ]->getData2idx( 1 ); // load default prototype
  $newElement->history .= ': '.$elementID.' auto created';

  $newIdx = $GLOBALS[ $elementIDsmall.'DBI' ]->setData( $newElement, true ); // insert the new element in the DB and get the new idx back
  return $newIdx;
}

/**
* Code from http://www.tipsntutorials.com/tips/PHP/41
*
* Removes all HTML from the given content.
*
* @param HTMLcontent The HTML string.
* @return String The content without HTML.
*/
function html2txt( $HTMLcontent ){
  $search = array ("'<script[^>]*?>.*?</script>'si",  // Strip out javascript 
                   "'<[/!]*?[^<>]*?>'si",          // Strip out HTML tags 
                   "'([rn])[s]+'",                // Strip out white space 
                   "'&(quot|#34);'i",                // Replace HTML entities 
                   "'&(amp|#38);'i", 
                   "'&(lt|#60);'i", 
                   "'&(gt|#62);'i", 
                   "'&(nbsp|#160);'i", 
                   "'&(iexcl|#161);'i", 
                   "'&(cent|#162);'i", 
                   "'&(pound|#163);'i", 
                   "'&(copy|#169);'i", 
                   "'&#(d+);'e");                    // evaluate as php 
  
  $replace = array ("", 
                   "", 
                   "\1", 
                   "\"", 
                   "&", 
                   "<", 
                   ">", 
                   " ", 
                   chr(161), 
                   chr(162), 
                   chr(163), 
                   chr(169), 
                   "chr(\1)"); 
  
  return preg_replace($search, $replace, $HTMLcontent);
}
?>