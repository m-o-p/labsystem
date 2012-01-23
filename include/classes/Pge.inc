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
* Contains the Pge class.
*/

/**
* This class encapsulates the current Page (the one that will be diplayed).
* This class extends the elements/LpElement.inc.
*
* An instance of this class gets created at the end: $pge.
*
* @module     ../include/classes/ElementMenues.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
class Pge extends LpElement{
  var $nextTab = 1; // the value of the next available TabNumber (for forms and menu elements)
  var $replaceKey;
  var $replaceValue;

  /**
  * In this function the strings that will be replaced when displaying and their replacements are initialized.
  */
  function Pge(){
    global $cfg, $usr;
    $this->LpElement( 0, "?untitled page!", "", "", 0, false, "" );
    $this->replaceKey   = Array( "__LINKQUERY__", 
                                 "__PAGETITLE__", 
                                 "__SYSTEMTITLE__",         
                                 "__FORENAME__", 
                                 "__NAME__",    
                                 "__USERNAME__", 
                                 "__ELEMENTTITLE__",         
                                 "__CURRENT_URL__",       
                                 "__LINK2ADMIN_USER_INTERFACE__" );
    $this->sanitizedKey   = Array( "awob6ope69", 
                                "uhuk8afa95", 
                                "uvur2awu94",         
                                "onot5oka70", 
                                "elid5upe10",    
                                "anus9iri39", 
                                "avod1ubo72",         
                                "ugud9eka32",       
                                "udan1obo94" );
    $this->replaceValue = "";
  }
  
  /**
  * Override to the Element::isVisible();
  * Is the element visible?
  * Necessary because IamVisible is not set correctly! (LpElement on construction)
  * @return BOOL  Is the element visible?
  */
  function isVisible(){
    global $usr;
    $this->IamVisible = ( ($this->visibleFor == "0") || $usr->isOfKind( $this->visibleFor ) );
    return parent::isVisible();
  }

  /**
  * Adds (puts) content to the page.
  * @param $string  The content to be added.
  */
  function put( $string ) {
    $this->contents .= $string;
  }
  
  /**
  * In some markup languages the chosen constants $this->replaceKey have markup meaning.
  * Replaces the  $this->replaceKey by  $this->sanitizedKey which should be save.
  * Became necessary with the introduction of MarkUp as they use __ as mark for emphasis.
  * @param $string  The content the replacements should be done inside.
  * @return $string  The content with the replacements done.
  */
  function sanitizeKeys( $string ){
    return str_replace( $this->replaceKey, $this->sanitizedKey, $string );
  }
  
  /**
  * Replaces the constants with the values.
  * @param String The content the constants should be replaced in.
  */
  function replaceConstants( &$text ){
    global $url, $usr, $lng, $cfg;
    // initialize replacements on first use:
    if($this->replaceValue==""){
    	$this->replaceValue = Array( "!_LINKQUERY_!",
    	                             "!_PAGETITLE_!", 
    	                             $cfg->get("SystemTitle"),
    	                             $usr->foreName,
    	                             $usr->surName,
    	                             $usr->userName,
    	                             "WillBeReplacedByElsShow",
    	                             urlencode( $_SERVER['REQUEST_URI'] ),
    	                             $cfg->get("Link2UserAdminInterface") );
    }
    // Some replacements must be set here since they may not be correct before.
    $counter=0;
    foreach ( $this->replaceValue as $key => $value ){
      $counter++;
      if ( $counter == 1 ) $this->replaceValue[$key] = $url->get("newQueryString");
      if ( $counter == 2 ) $this->replaceValue[$key] = $this->title;
      if ( $counter == 3 ) break;
    }

    $text = ( str_replace( array_merge( $this->replaceKey, $this->sanitizedKey ), // replace keys and sanitized keys.
                           array_merge( $this->replaceValue, $this->replaceValue ), 
                           $text ) 
                           );
  }
  
  /**
  * This function is called to display the contents. It gets called in ../include/page_layout.inc.
  * This function overrides the one from LpElement.
  */
  function show(){
    global $lng;
    if ( !$this->isVisible() ) // not enough rights->no content
      $this->contents = "<div class=\"labsys_mop_note\">\n".$lng->get("TxtNotEnoughRights")."\n</div>";
              
    // Some replacements must be set here since they may not be correct before.
    $this->replaceConstants($this->contents);
    echo( $this->contents );
  }
  
  /**
  * This function returns the predefined constants that will be replaced when showing (p.e. __LINKQUERY__).
  * It gets called when one edits a page element.
  */
  function getConstants(){
    $ret = "";
    foreach( $this->replaceKey as $value ) $ret .= ", ".$value;
    return substr( $ret, 2 );
  }
}
$pge = new Pge(); /** create instance */
?>