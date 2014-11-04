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
* This class is the motherclass of all elements. It contains fundamental implementations of
* the necessary methods and variables that are common to all elements.
*/

/**
* If you create a ne element you have to implement at least:
     class NewElement extends Element{
       var $elementsAdditionalVariable1;
       var $elementsAdditionalVariable2;

       function NewElement()( $v1, $v2 ){
         $this->elementId                = "yourID";
         $this->idx                      = $idx;
         $this->title                    = "your element's name ".$idx;
         $this->matchingMenu             = $this->title; // what will ge the element's menu entry
         $this->visibleFor               = $visibleFor;
         $this->visibleOnlyInCollection  = $visibleOnlyInCollection;
         $this->history                  = $history;

         $this->elementsAdditionalVariable1 = $v1;
         $this->elementsAdditionalVariable2 = $v2;
       }

       function showEdit( $fullAddress ){
         return "your element's edit page html code";
       }

       function save(){
        // This function is called to save the edited values from above.
        // You have to deal the $_POST['myVal'] values here and send them to your elements database interface.
        // You should make a header redirect after saving.
       }
     }
* The other methods have default implementation you can override.
* Don't forget NOT to HARDCODE STRINGS. For internationalization they are provided in ,,/ini/{en, de, ...}.lng
* and used with $lng->get("yourStringID"); .
*
* A good and not too complicated example might be the implementation of the page element:
*  - LpElement.inc
*  - LpDBInterface.inc
*
* The naming of your classes is important. It is:
*  - {L, B} [yourElementsIDcaseSensitive] Element.inc
*  - {L, B} [yourElementsIDcaseSensitive] DBInterface.inc
* The first two letters characterize your element:
* L stands for lower case (little), B for upper case (big). This is necessary since on windows OS cElement = CElement :(.
* The second character is the ID.
* P.e. "C" element gets BC, "c" element gets "Lc".
*
* @module     ../include/classes/elements/Element.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/

class Element{
  // The following variables are common to all elements.
  var $elementId;               // Used for some Identifiers in the stylesheet (see rows below)

  var $idx;                     // The element's idx in the database.
  var $title;                   // The element's page title (p.e. for html-title).
  var $matchingMenu;            // If present this will get the element's menu entry.
  var $visibleOnlyInCollection; // Is the element visible "standalone" or only aggregated in a collection.
  var $history;                 // The element's history.

  var $IamVisible;              // Am I visible? At initializantion this variable should be set according to the user's rights etc.

  var $serializationArray;      // This array is used for serialization and unserialization. It contains all variable names to be (un-)serialized as value.

  /**
  * Is the element visible?
  * @return BOOL  Is the element visible?
  */
  function isVisible(){
    global $usr;
    return  $this->IamVisible
            || $usr->isOfKind( IS_CONTENT_EDITOR ); /* contents editors are able to view any page */
  }

  /**
  /**
  * Serialises the element. Uses $this->serializationArray
  *
  * @return String serializedVersionOfElement
  */
  function getSerialized(){
    $output = '<element>'."\r\n";
    foreach ( $this->serializationArray as $value ){
      $output .= '<'.$value.'>';
      if( is_array($this->$value) ){
        $output .=  "<arrayElement>".implode("</arrayElement>\n<arrayElement>", $this->$value)."</arrayElement>"; // transform the array elements to XML
      }else{
        $output .= $this->$value;
      }
      $output .= '</'.$value.'>'."\r\n";
    }
    $output.='</element>'."\r\n";
    return $output;
  }

  /**
  * UnSerializes the element.
  *
  * @param string $serializedData The serialized version of the element producerd by the getSerialized()-function.
  *
  * @return - Sets the element's parameters.
  */
  function initFromSerialized( &$serializedData ){
    $inputLines = explode( "\r\n", $serializedData );
    foreach ( $this->serializationArray as $value ){
      $openTag = '<'.$value.'>';
      $closeTag = '</'.$value.'>';
      $this->$value = ''; // start with empty as we restore!
      if ( // start searching for $openTag
           (strlen( current( $inputLines ) ) >= strlen($openTag)) && // compare needs at least this length... otherwise error
           (substr_compare( current( $inputLines ), $openTag, 0, strlen($openTag) ) == 0) &&
           // remove start tag
           ($inputLines[ key( $inputLines ) ] = substr( current( $inputLines ), strlen($openTag) )) ){
	           while( // not yet end tag there?
	                  ((strlen( current( $inputLines ) ) < strlen($closeTag)) || // if smaller substr_compare returns true?!?
	                   (substr_compare( current( $inputLines ), $closeTag, -1*strlen($closeTag), strlen($closeTag) ) != 0)
	                   ) &&
	                  // then add it
	                  ( $this->$value .= current( $inputLines )."\n" ) &&
	                  // get next line if one is there
	                  (next( $inputLines ) !== FALSE)
	                 ) ;
            // end tag present
            $this->$value .= substr( current( $inputLines ), 0, strlen(current( $inputLines ))-strlen($closeTag) ); // unserialize only occurs on one line values
            if ( (strlen( $this->$value ) >= 14) &&
                 (substr($this->$value, 0,14) == '<arrayElement>') ){
                 $this->$value = substr($this->$value, 14); //cut front
                 $this->$value = substr($this->$value, 0, strlen($this->$value)-15); //cut rear
                 $this->$value = explode( "</arrayElement>\n<arrayElement>", $this->$value ); // post process arrays
            }
            // get next line and continue if one is there
            if (!next( $inputLines )) break;
      }
    // catch error
    if ( !current( $inputLines ) ) trigger_error( htmlentities( $closeTag ).' missing when unserializing: '.htmlentities( $serializedData ), E_USER_ERROR );
    }
  }

  /**
  * Is called by the ../menu.inc to build this element's menu entry.
  * @param $fullAddress     The element's full address (p.e. C12.c4.p13)
  * @param &$menu2Populate  Reference to the ../Menu.inc instance the entry() function should be called of.
  * @param $paragraph       If given this is put before the title (p.e. 1.2.3. menuTitle).
  * @param $isActive        Is the Element activ (shall I highlight)?
  */
  function getMenu( $fullAddress, &$menu2Populate, $paragraph="", $isActive=false ){
    global $url;
    if ( $this->isVisible() ) return $menu2Populate->entry( ( $paragraph != "" ?  $paragraph.". "  : '' ).$this->getMatchingMenu(), "../pages/view.php?address=".$fullAddress, $isActive );
                         else return "";
  }

  /**
  * Called to get the elements matching menu ( -> getMenu() ).
  * @return string $this->mathingMenu or $this->title if ($this->matchingMenu == "").
  */
  function getMatchingMenu(){
    if ( $this->matchingMenu != "" )
      return $this->matchingMenu;
    else
      return $this->title;
  }

  /**
  * Returns the element's HTML-representation in the way the VIEWER should see it.
  *
  * This function might be overriden to show different pages on different addresses.
  * ( -> LlElement.inc, l2.preLabStatus, ...).
  *
  * @param $fullAddress     The element's full address (p.e. C23.c42.p13)
  * @param $paragraph       The element's paragraph number (p.e. 2.3). Will be added to the title if given.
  * @param $hideNavigation  Used by BCElement.inc (-> no navigation arrows).
  *
  * @return string          The html code of the element's view view.
  */
  function show( $fullAddress, $paragraph="", $hideNavigation=false ){
    global $lng;
    if (  !$this->isVisible() ){ // not enough rights->no content
      return "<div class=\"labsys_mop_note\">\n".$lng->get("TxtNotEnoughRights")."\n</div>";
      exit;
    }

    return $this->contents;
  }

  /**
  * Returns the element's HTML-representation in the way the EDITOR should see it.
  *
  * This function must be overriden in order to give the user the ability to edit your element.
  *
  * @param $_GET["saveAsNew"] If $_GET["saveAsNew"] is set the save as new checkbox on the edit page should be set
  * @param $fullAddress       The element's full address (p.e. C23.c42.p13)
  * @return string            The html code of the element's edit view.
  */
  function showEdit( $fullAddress ){
    return "Editing functionality!";
  }

  /**
  * This function gets STATICALLY called from ../php/saveElement.php?address=[elementID].
  * It handles the $_POSTed data and then redirects to the $_POST['REDIRECTTO'] with the address of the element added.
  *
  * This function must be overriden in order to give the user the ability to save the element he edited in the view above.
  */
  /*static*/ function save(){
    echo( "saving..." );
    // You have to make a header redirect after saving.
  }


// The next functions are used for the element's manage page. ***********************************************

  /**
  * Displays the element's property legend on the manage page.
  * All entries of getPropertyRow() should be explained here.
  *
  * This implementation shows the legend for the implementation below and might be overriden for your element.
  */
  function showPropertyLegend(){
    global $lng, $pge;
    $retVal = "";
    $i = 0;
    foreach( get_class_vars( get_class( $this ) ) as $key=>$value ){
      $i++;
      for ( $j=1; $j<$i; $j++ ) $retVal .=  "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"legend4_VISIBLE_BEFORE_FIRST_SCHED\" value=\"1\" disabled=\"disabled\">".
                                            infoArrow( "legend_".$key, true );
      $retVal .=  "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"legend_".$key."\" checked=\"checked\">".
                  infoArrow( "legend_".$key, false )." ".$key."<br>\n";
    }

    return "<div class=\"labsys_mop_".$this->elementId."_row\">\n".
           "<div class=\"labsys_mop_h3\">".$lng->get("legend")."</div>\n".
           $retVal.
           "</div>";
  }

  /**
  * This function is called to display the element's properties. Probably you will OVERRIDE it for your Element.
  * @param $prefix    If given this is put before the element's title (p.e. 1.2.3 title).
  * @param $disabled  Should the checkboxes, radio buttons etc. be disabled?
  * @return string    The html-code of the elements properties horizontally arranged.
  */
  function getPropertyRow( $prefix, $disabled=false ){
    global $pge;
    $disabled = true; // With this implementation saving wont be possible
    $retVal = "";
    foreach( get_class_vars( get_class( $this ) ) as $key=>$value )
      $retVal .=  "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"legend_".$key."\" checked=\"checked\"".( $disabled ?  " disabled=\"disabled\"" : '' )." onchange='isDirty=true'>".
                  infoArrow( $prefix."_".$key, $disabled )." ";

    return $retVal."| ".$this->title;
  }

  /**
  * Displays the element's properties for the manage page.
  * Just integrates the getPropertyRow() into a labsys_mop_".$this->elementId."_row style division.
  * @param $prefix    If given this is put before the element's title (p.e. 1.2.3 title). ( -> getPropertyRow() ).
  * @param $disabled  Should the checkboxes, radio buttons etc. be disabled? ( -> getPropertyRow() ).
  * @return string    The html-code of the elements property row.
  */
  function showPropertyRow( $prefix, $disabled=false ){
    return  "<div class=\"labsys_mop_".$this->elementId."_row\">\n".
            $this->getPropertyRow( $prefix, $disabled )." ".
            eval( "return EM::manageRow_".$this->elementId."( \$this );" ).
            "</div>\n";
  }

  /**
  * Displays the element's properties for the manage page.
  * Just integrates the getPropertyRow() into a labsys_mop_".$this->elementId."_row style division.
  * @param $prefix    If given this is put before the element's title (p.e. 1.2.3 title). ( -> getPropertyRow() ).
  * @param $isExport  Show export or import row?
  * @param $disabled  Should the checkboxes, radio buttons etc. be disabled? ( -> getPropertyRow() ).
  * @return string    The html-code of the elements property row.
  */
  function showExportImportRow( $prefix, $isExport=true, $disabled=false ){
    global $lng;
    return  "<div class=\"labsys_mop_".$this->elementId."_row\">\n".
            $this->getExportImportRow( $prefix, $isExport, $disabled )." ".
            EB::history( $lng->get($this->elementId."Number").$this->idx.":<hr>".$this->history, $this->elementId.$this->idx ).
            "</div>\n";
  }

// If this element gets aggregated inside a collection the following rows are taken *************************

  /**
  * Called to display the element's properties for the collection's structure page.
  * The setable elements should be grayed here since they wont be savable.
  * Just integrates the getPropertyRow() into a labsys_mop_".$this->elementId."_row style division.
  * @param $address The address is submitted to be able to link correct from the menu buttons.
  *                 Since the $address is unique I use it as prefix (the control elements are grayed anyway).
  * @param $disabled  Should the checkboxes, radio buttons etc. be disabled? ( -> getPropertyRow() ).
  * @param $prefix    If given this is put before the element's title (p.e. 1.2.3 title). ( -> getPropertyRow() ).
  */
  function showPropertyRowColl( $address, $disabled=true, $prefix="" ){
    return  "<div class=\"labsys_mop_".$this->elementId."_row\">\n".
              '<div class="labsys_mop_c_structure_address">'.$prefix.$address.'</div>'.
              $this->getPropertyRow( $address, $disabled )." ".
              eval( "return EM::collectionRow_".$this->elementId."( \$this, \$address );" ).
            "</div>\n";
  }

  /**
  * Called when an element of this type is included inside a collection but could not be found.
  * @param $address The address is submitted to be able to link correct from the menu buttons.
  */
  function showNoFoundRowColl( $address ){
    global $lng;
    $GLOBALS["SYSALERT"] = $address." ".$lng->get("doesNotExist");
    return  "<div class=\"labsys_mop_not_found_row\">\n".
              $address." ".$lng->get("doesNotExist").
            "</div>\n";
  }

  /**
  * Splits the Address given in &$parentAddr into $parentAddr part and $myAddr part.
  * p.e. I am c12, $fullAddress = l2.C3.c12.m2 -> $parentAddr = "l2.C3.", $myAddr = Array( [0]=>"c12", [1]=>"m2" );
  *
  * @param &$parentAddr   The full address gets passed here. The parent address string gets in here.
  * @param &$myAddr       The my address array gets put in here.
  */
  function splitAddress( &$parentAddr, &$myAddrArray ){
    $fullAddress = $parentAddr;
    $parentAddr = "";
    $myAddrArray = explode( ".", $fullAddress );
    for( ; 0<count($myAddrArray); )
      if ( !strcasecmp( $myAddrArray[0], $this->elementId.$this->idx ) ) break;
      else{
            $parentAddr .= $myAddrArray[0].".";
            array_shift( $myAddrArray ); // throw away
      }
  }
}
?>