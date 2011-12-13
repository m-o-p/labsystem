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
* implementation of the C (collection) element.
* The difference to the c element is that all entities are shown seperately here.
*/

/*
* This element extends LcElement.inc.
*
* @module     ../include/classes/elements/BCElement.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
require_once( INCLUDE_DIR."/classes/elements/LcElement.inc" );

class BCElement extends LcElement{

  /**
  * See constructor of LcElement since it is called 1:1.
  */
  function BCElement( $idx, $title, $contents, $matchingMenu, $visibleBefore1stSched, $visibleDuringSched, $visibleAfter1stSched, $visibleOnlyInCollection, $history, $isInColl=false, $isInLab=false ){
    parent::LcElement( $idx, $title, $contents, $matchingMenu, $visibleBefore1stSched, $visibleDuringSched, $visibleAfter1stSched, $visibleOnlyInCollection, $history, $isInColl, $isInLab );
  }

// The next functions are used for the element's view page. *************************************************

// helper (if this exists some time they should be declared private)  - - - - - - - - - - - - - - - - - - - 
  /**
  * Returns the Table Of Contents.
  *
  * @param $fullAddress     The element's full address (p.e. C23.c42.p13)
  * @param $paragraph       The element's paragraph number (p.e. 2.3). Will be added to the title if given.
  *
  * @return                 HTML-code of the TOC.
  */
  function getTOC( $fullAddress, $extParagraph="" ){
    global $url, $lng;

    $structureArray = $this->getMyVisibleElements();

    $myRetVal = "<!-- start of group --><div class=\"labsys_mop_C_TOC_group\">\n"; 

    foreach( $structureArray as $value ){
        $paragraph = ( $extParagraph != "" ?  $extParagraph."."  : '' ).$this->getParagraph( $value );

       // show the element
        $id = $value{0}; $num = substr( $value, 1 );
        $element = $GLOBALS[ $id."DBI" ]->getMenuData2idx( $num, true, $this->isInLab );
        
        if ( $element ){ $myRetVal .= "<div class=\"labsys_mop_C_TOC_entry\">".
                                      "<a href=\"".$url->link2( "../pages/view.php?address=".$fullAddress.".".$value )."\">".
                                      ( $paragraph != "" ?  $paragraph.". "  : '' ).$element->title.
                                      "</a></div>\n";
                        // call the C elements getTOC to get the submenu
                         if ( $id == "C" ) $myRetVal .= $element->getTOC( $fullAddress.".".$value, $paragraph );
                        }
                  else   $myRetVal .= "<div class=\"labsys_mop_C_TOC_entry\">".
                                      $value." ".$lng->get("doesNotExist").
                                      "</div>\n";
    }
    return $myRetVal."<!-- stop of group --></div>\n";
  }

  /**
  * Returns the links to the toc, following and previous element.
  * Uses the PARAGRAPH ARRAY since it contains all visible elements...
  *
  * @param $relativeAddress   The address of the element inside the collection.
  * @param &$prevEl           The variable to hold the link to the previous element.
  * @param &$nextEl           The variable to hold the link to the next element.
  */
  function getLinks( $relativeAddress, &$prevEl, &$nextEl ){
   // The structure with all C elements unfolded:
    $menuStructure = explode( " ", $this->buildStructure( true, false ) );
    
    $found = ($relativeAddress == "");
    $current = false;
    $parentArray = Array(); // This stack holds the las parent addresses
                            // So if we are C123 and descend to C234 it holds
                            // "C123", "C123.C234" (so it is not incremental "C123", "C234").

    foreach( $menuStructure as $value ){
      if ($value{0} == "*") continue;
      if ($value == "("){ array_push( $parentArray, $nextEl ); continue; }
      if ($value == ")"){ array_pop( $parentArray ); continue; }
      
      if ( count($parentArray) > 0 ) $parent = $parentArray[ count($parentArray)-1 ];
                                else $parent = "";

      $nextEl = ( $parent != "" ?  $parent."."  : '' ).$value;
      
      if ( $found ) break;
      
      if ( $nextEl == $relativeAddress ) $found=true;
                                   else $prevEl = $nextEl;
    }
    
    if ( $nextEl == $relativeAddress ) $nextEl = false; // Happens if we are at the last element (no break but foreach breaks).
  }
  
// /helper - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  /**
  * Returns the element's HTML-representation in the way the VIEWER should see it.
  *
  * @param $fullAddress     The element's full address (p.e. C23.c42.p13)
  * @param $paragraph       The element's paragraph number (p.e. 2.3). Will be added to the title if given.
  * @param $hideNavigation  If set the navigation arrows are hidden (useful for collection inside collection).
  *
  * @return string          The html code of the element's view view.
  */ 
  function show( $fullAddress, $extParagraph="", $hideNavigation=false ){
    global $lng, $cDBI, $usr, $url;

    if ( !$this->isVisible() ){ return $this->notFoundVisibleNote( $fullAddress ); exit; }
    
   // Address splitting.
    $parentAddr = $fullAddress;
    $this->splitAddress( $parentAddr, $myAddrArray );
                                     $me        = $myAddrArray[0];
    if ( count( $myAddrArray ) > 1 ) $first     = $myAddrArray[1]; else $first = "";
    if ( count( $myAddrArray ) > 1 ) $last      = $myAddrArray[ count( $myAddrArray )-1 ]; else $last = "";
    if ( count( $myAddrArray ) > 1 ) $scndLast  = $myAddrArray[ count( $myAddrArray )-2 ]; else $scndLast = "";

    $myRetVal = "";
   // show toc
        if ( $last == "" ) $myRetVal .= "<div class=\"labsys_mop_contentArea\">".
                                        "<div class=\"labsys_mop_h2\">".( $extParagraph != "" ?  $extParagraph.". "  : '' ).$this->title."</div>".
                                        $this->getTOC( $fullAddress, $extParagraph ).
                                        "</div>";
                                        
   // ePub
    elseif ( $last == "epub" ){
                                // tell the ePub to create a new chapter
                                echo( '[C-Element '.$parentAddr.$me.'] tell the ePub to create a new chapter "'.$this->title.'"<br>' );
                                $this->showEPub( $parentAddr.$me, $extParagraph ); // called in the LcElement
                                return;
                               }

   // show allOnOne
    elseif ( ($last == 'all') &&
             ($scndLast == $me) ){
                     return "<!-- start of collection entity --><div class=\"labsys_mop_c_inner_coll_entity\">\n".
                            $this->show( $parentAddr.$me, $extParagraph, true ).
                            "<!-- stop of collection entity --></div>\n".
                            $this->showAllOnOne( $parentAddr.$me, $extParagraph, true );
    }

   // show final
    elseif ( $this->isVisibleFinal( $first ) ){
                                          $id = $first{0};
                                          $num = substr( $first, 1 );
                                          require( "../php/getDBIbyID.inc" );
                                          $element = $DBI->getData2idx( $num, true, $this->isInLab );
                                          $GLOBALS["pge"]->title        = $element->title;
                                          $GLOBALS["pge"]->matchingMenu = $element->getMatchingMenu();
                                          $myRetVal .= $element->show( $fullAddress, ( $extParagraph != "" ?  $extParagraph."."  : '' ).$this->getParagraph( $first ), true );
                                        }
   // we don't handle so maybe our parent does... ( p.e. .structure )
    else { return parent::show( $fullAddress, $extParagraph, $hideNavigation ); }

   // the links for the arrows
    $prevEl = $nextEl = $topEl = false;
    if ( !$hideNavigation ){ 
                             $relAddr = implode( ".", array_slice ( $myAddrArray, 1 ) );
                             $this->getLinks( substr( stristr( $url->get('address'), $me ), strlen( $me )+1 ), // trim the address to be correct even if the adress was rewritten (e.g.: l7.C84.C184.c188.p195 -> l7.C84.C184.c188)
                                              $prevEl, 
                                              $nextEl 
                                             );
                             $topEl  = $parentAddr.$me;
                             if ( $prevEl ) $prevEl = $parentAddr.$me.".".$prevEl;
                             if ( $nextEl ) $nextEl = $parentAddr.$me.".".$nextEl;
                            }
    
    return  EM::viewTopCollection( $this, $parentAddr.$me, $prevEl, $nextEl, $topEl ).
            $myRetVal.
            EM::viewBottomCollection( $this, $parentAddr.$me, $prevEl, $nextEl, $topEl ); 
  }
  
  
  /**
  * Returns the html-code of this element's menu.
  *
  * @param $fullAddress     Since the Menu might be part of another Collection the $fullAddress might have another
  *                         collection as root ( p.e. C7.C4.C[this->idx] ).
  * @param $menu2Populate   The instance of the Menu.inc-class the entries should get in.
  * @param $unfold          Should the submenu-entries be visible or just the title-row?
  *
  * @return                 The menu's html-code.
  */
  function getMenu( $fullAddress, &$menu2Populate, $extParagraph="", $unfold=false ){
    global $lng, $url;
    if ( !$this->isVisible() ) return "";
    
    if ( isset( $GLOBALS["firstFinal"] ) && ($GLOBALS["firstFinal"] == "C".$this->idx) ) $unfold = true;
    
    if ( !$unfold )
      $retVal = $menu2Populate->entry( ( $extParagraph != "" ?  $extParagraph.". "  : '' ).$this->getMatchingMenu(), "../pages/view.php?address=".$fullAddress, ($url->get('address') == $fullAddress) );
    else{
      $retVal = $menu2Populate->startSubmenu( ( $extParagraph != "" ?  $extParagraph.". "  : '' ).$this->getMatchingMenu(), "../pages/view.php?address=".$fullAddress, ($url->get('address') == $fullAddress) );
    /** parse the submenu entries **/
      $structureArray = $this->getMyVisibleElements();
  
      foreach( $structureArray as $value ){
          $linkAddr = $fullAddress.".".$value;
          $paragraph = ( $extParagraph != "" ?  $extParagraph."."  : '' ).$this->getParagraph( $value );
          $isActive = ( $linkAddr == substr( $url->get('address'), 0, strlen( $linkAddr ) ) );
          
         // show the element's menu entry
          $id = $value{0}; $num = substr( $value, 1 );
          $element = $GLOBALS[ $id."DBI" ]->getMenuData2idx( $num, true, $this->isInLab );
          if ( !$element ) $retVal .= $menu2Populate->entry( $value." ".$lng->get("doesNotExist"), "../pages/view.php?address=".$linkAddr );
                      else $retVal .= $element->getMenu(  $linkAddr, 
                                                          $menu2Populate, 
                                                          $paragraph, 
                                                          $isActive
                                                        );
      }
    /** +++ **/
      $retVal .= $menu2Populate->stopSubmenu();
    }
    
    return $retVal;
  }
}
?>