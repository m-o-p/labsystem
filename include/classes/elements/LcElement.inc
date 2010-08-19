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
* implementation of the c (collection) element.
*
* This element consists of shortcuts of other elements (p.e. p1 c17 m4).
*
* This element gets extended by BCElement.inc.
*
* @module     ../include/classes/elements/LcElement.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/

require_once( INCLUDE_DIR."/classes/elements/Element.inc" );
// all other elements must get included before!

class LcElement extends Element{
  var $contents;              // The list of shortcuts to the other elements.

  var $structure;             // The abstract syntax of the structure ( -> getStructure() ).
  var $paragraphArray;        // A mappinng between structrure and paragraph numbers.
  
  var $visibleBefore1stSched; // Will the element be visible before the first schedule?
  var $visibleDuringSched;    // Will the element be visible while it is on schedule?
  var $visibleAfter1stSched;  // Will the element be visible after the first schedule?
 
  var $isInLab;               // Am I inside a lab? (important for view visibility)
  
  var $allowedElementIDs;     // save() check from this array for valid ID letters

  /**
  * The constructor.
  *
  * @param $idx                       This instance's index.
  * @param $title                     The title (used as __ELEMENTTITLE__ and for html <title> if displayed alone).
  * @param $contents                  The list of shortcuts to the other elements.
  * @param $matchingMenu              The title of the menu that gets generated/ is highlighted to the element.
  * @param $visibleBefore1stSched     Is the element visible before the first schedule?
  * @param $visibleDuringSched        Is the element be visible while it is on schedule?
  * @param $visibleAfter1stSched      Is the element be visible after the first schedule?
  * @param $visibleOnlyInCollection   Is the element visible only inside a collection.
  * @param $history                   This' history.
  * @param $isInColl                  Am I inside a collection? (important for visibility aspects).
  * @param $isInLab                   Am I inside a lab? (important for view visibility).
  */   
  function LcElement( $idx, $title, $contents, $matchingMenu, $visibleBefore1stSched, $visibleDuringSched, $visibleAfter1stSched, $visibleOnlyInCollection, $history, $isInColl=false, $isInLab=false ){
    
    $this->elementId                = "c";
    
    $this->idx                      = $idx;
    $this->title                    = $title;
    $this->contents                 = $contents;
    $this->matchingMenu             = $matchingMenu;

    $this->visibleBefore1stSched    = $visibleBefore1stSched;
    $this->visibleDuringSched       = $visibleDuringSched;
    $this->visibleAfter1stSched     = $visibleAfter1stSched;
    $this->visibleOnlyInCollection  = $visibleOnlyInCollection;

    $this->isInLab                  = $isInLab;
    
    $this->history                  = $history;

    $this->IamVisible               = $this->isAtLeastOneElementVisibleForMe() && 
                                      // schedule
                                      ( $isInLab /*ignore schedule*/ || LsElement::visiblePerSchedule( "c", $this->idx, $this->visibleBefore1stSched, $this->visibleDuringSched, $this->visibleAfter1stSched ) )
                                      // onlyVisibleInsideCollection
                                      && ( !$this->visibleOnlyInCollection || ($this->visibleOnlyInCollection && $isInColl) );
    
    $this->structure                = false; // To build the structure causes many (slow) sql queries, so it is only done if needed.
    $this->mainStructure            = false;
    $this->paragraphArray           = false; // Filled when structure gets filled.
    
    $this->allowedElementIDs        = array('C', 'c', 'm', 'i', 'p', 's');
  }
  
    
// The next functions are used for the element's view page. *************************************************

// helper (if this exists some time they should be declared private)  - - - - - - - - - - - - - - - - - - - 
  /**
  * Returns the getStructure string with underlined groups.
  *
  * @return   Formatted getStructure()-string.
  */
  function getFormattedStructureString(){
    return '<div class="labsys_mop_c_structure">'.
           str_replace(  Array( " C", "(", ")" ), 
                         Array( " <br />\nC", "<span class=\"labsys_mop_c_structure_substructure\"><span class=\"labsys_mop_red\">(</span>", "<span class=\"labsys_mop_red\">)</span></span>" ), 
                         $this->buildStructure(true, true) 
                        ).
           "</div>\n";
  }
  
  /**
  * Shows a row for each element in the collection.
  * Uses $element->showPropertyRowColl() ( EM::collectionRow_[elementID] ).
  *
  * @param $fullAddress     The element's full address (p.e. C23.c42.p13)
  * @param $extParagraph    The element's paragraph number (p.e. 2.3). Will be added to the title if given.
  *
  * @return   The html code of the page.
  */
  function showStructure( $fullAddress, $extParagraph="" ){ 

    $structureArray = explode( " ", $this->getStructure() );

    $myRetVal  = $this->showPropertyRowColl( $fullAddress, true, retIfTrue( $extParagraph != "", $extParagraph.". " ) ).
                 "<!-- start of group --><div class=\"labsys_mop_c_group\">\n";

    foreach( $structureArray as $value ){
      if ( $value{0} == "*" ) $value = substr( $value, 1 ); /* Cut the * since it is not important if the element is visible. */
      $id = $value{0};
      $num = substr( $value, 1 );
      
      $element = $GLOBALS[ $id."DBI" ]->getData2idx( $num, true, $this->isInLab );
      
      if ( !$element ) eval( "\$myRetVal .= ".retIfTrue( strtoupper($id) == $id, "B").retIfTrue( strtolower($id) == $id, "L").$id."Element::showNoFoundRowColl( \$id.\$num );" );
                 else if ( ($id == "c") || ($id == "C") ) $myRetVal .= $element->showStructure( $fullAddress.".".$value, retIfTrue( $extParagraph != "", $extParagraph."." ).$this->getParagraph( $value ) );
                                   else $myRetVal .= $element->showPropertyRowColl( $fullAddress.".".$value, true, retIfTrue( $extParagraph != "", $extParagraph."." ).$this->getParagraph( $value ).". " );
    }
    
    return $myRetVal."<!-- stop of group --></div>\n";
  }
  
  /**
  * Returns the HTML-code of the "not found visible"-notification.
  *
  * @param $fullAddress   The address of the element to be displayed.
  *
  * @return               HTML-representation of the not found notice.
  */
  function notFoundVisibleNote( $fullAddress ){
    global $lng;
    return "<div class=\"labsys_mop_note\">\n".
           "<b>".$lng->get("elNotFoundVisible")."</b> (".$fullAddress."):\n".
           $this->getFormattedStructureString().
           "</div>\n";
  }
  
  /**
  * Shows the whole collection on one html page.
  *
  * @param $fullAddress     The element's full address (p.e. C23.c42.p13)
  * @param $extParagraph    The element's paragraph number (p.e. 2.3). Will be added to the title if given.
  * @param $allSeparate     Show all elements as separate entity? (and make pagebreaks below)
  *
  * @return string          The html code.
  */
  function showAllOnOne( $fullAddress, $extParagraph, $allSeparate ){
   
    $myRetVal = "";
    $visibleArray = $this->getMyVisibleElements();

    foreach( $visibleArray as $value ){
       $id = $value{0};
       $num = substr( $value, 1 );

      // only enumerate if shown separately (otherwhise each m element etc. has a number...).
       if ( $allSeparate || 
            $id == "i" || $id == "m"// so the i and m elements get numbered
                         ) $paragraph = retIfTrue( $extParagraph != "", $extParagraph."." ).$this->getParagraph( $value );
                      else $paragraph = "";

       require( "../php/getDBIbyID.inc" );
       $element = $DBI->getData2idx( $num, true, $this->isInLab );

      // show the element
      // separator?
       if ( $allSeparate && ($id != "C") /* otherwise double div */ )
        $myRetVal .= "<!-- start of collection entity --><div class=\"labsys_mop_c_inner_coll_entity\">\n";

       $myRetVal .= $element->show( $fullAddress.".".$value.retIfTrue( $id == "C", ".all" ), 
                                    $paragraph );

      // separator?
       if ( $allSeparate && ($id != "C") /* otherwise double div */ )
        $myRetVal .= "<!-- stop of collection entity --></div>\n";
    }
   
    return $myRetVal;
  }

  /**
  * Generates a page with all the collection's elements of a special type (given by $typePrefix ).
  *
  * @param $typePrefix    Element's type (p.e. p, m, i).
  * @param $fullAddress   The element's full address (p.e. C23.c42.p13)
  * @param $extParagraph  The element's paragraph number (p.e. 2.3). Will be added to the title if given.
  *
  * @return string        The html code of the element's view view.
  */
  function showAllElementsOfType( $typePrefix, $fullAddress, $extParagraph="" ){
    $parameter = Array( 'addr' => $fullAddress, 
                        'paragraph' => $extParagraph, 
                        'myRetVal' => '' );
    $functionCode = "
    \$elementAddr = retIfTrue( \$parameter['addr'] != '', \$parameter['addr'].'.' ).\$value;
    \$elParagraph = retIfTrue( \$parameter['paragraph'] != '', \$parameter['paragraph'].'.' ).\$this->getParagraph( \$value );

    if ( \$element->elementId == '".$typePrefix."') \$parameter['myRetVal'] .= \$element->show( \$elementAddr, \$elParagraph );
    elseif ( (\$element->elementId == 'c') || (\$element->elementId == 'C') ){
      // recursion
      \$newParam = Array( 'addr' => \$elementAddr, 'paragraph' => \$elParagraph, 'myRetVal' => '' );
      \$element->extMap( \$functionCode, \$newParam );
      if ( \$newParam['myRetVal'] != '' ) 
        \$parameter['myRetVal'] .=  \"<div class=\\\"labsys_mop_contentArea\\\">\\n\".
                                    // header inclusive link to the enclosing element (this)
                                    '<div class=\"labsys_mop_h2\">'.retIfTrue( \$elParagraph != '', \$elParagraph.'. ' ).\$element->title.' '.EB::link2( \$elementAddr ).\"</div>\\n\".
                                    \"</div>\\n\".
                                    \$newParam['myRetVal'];
    }";
    
    $this->extMap( $functionCode, $parameter );
    
    return $parameter['myRetVal'];
  }
// /helper - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  /**
  * Returns the element's HTML-representation in the way the VIEWER should see it.
  *
  * @param $fullAddress     The element's full address (p.e. C23.c42.p13)
  * @param $extParagraph    The element's paragraph number (p.e. 2.3). Will be added to the title if given.
  *
  * @return string          The html code of the element's view view.
  */ 
  function show( $fullAddress, $extParagraph="" ){
    global $lng, $usr;
    
    if ( !$this->isVisible() ){ return $this->notFoundVisibleNote( $fullAddress ); exit; }
  
   // Address splitting.
    $parentAddr = $fullAddress;
    $this->splitAddress( $parentAddr, $myAddrArray );
                                     $me        = $myAddrArray[0];
    if ( count( $myAddrArray ) > 1 ) $first     = $myAddrArray[1]; else $first = "";
    if ( count( $myAddrArray ) > 1 ) $last      = $myAddrArray[ count( $myAddrArray )-1 ]; else $last = "";
    if ( count( $myAddrArray ) > 1 ) $scndLast  = $myAddrArray[ count( $myAddrArray )-2 ]; else $scndLast = "";

    $myRetVal = "";

   // structure
        if ( ($last == 'structure') && ($scndLast == $me) && $usr->isOfKind( IS_CONTENT_EDITOR ) ) 
                $myRetVal = "<div class=\"labsys_mop_contentArea\">".
                            $this->getFormattedStructureString().
                            $this->showStructure( $parentAddr.$me, $extParagraph ).
                            "</div>";

   // all of Type $last
    elseif ($scndLast == 'allType' ) 
                $myRetVal = $this->showAllElementsOfType( $last, $parentAddr.$me, $extParagraph );

   // all on one ( Collection c catches all behind in the address C12.c13 = C12.c13.m23 ).
       else{ 
                if ( $fullAddress != $parentAddr.$me ) $_GET['address'] = $parentAddr.$me; // Also reset the given address -> correct menu is "on".
                $myRetVal = $this->showAllOnOne( $parentAddr.$me, $extParagraph, ($me == "C".$this->idx) );
            }


    return  EM::viewTopCollection( $this, $parentAddr.$me ).
            "<div class=\"labsys_mop_contentArea\">\n".
            "<div class=\"labsys_mop_h2\">".retIfTrue( $extParagraph != "", $extParagraph.". " ).$this->title."</div>\n".
            "</div>\n".
            $myRetVal.
            EM::viewBottomCollection( $this, $parentAddr.$me );
  }
  
// The next functions are used for the element's edit page. *************************************************

  /**
  * Returns the element's HTML-representation in the way the EDITOR should see it.
  *
  * @param $_GET["saveAsNew"] If $_GET["saveAsNew"] is set the save as new checkbox on the edit page should be set
  * @param $fullAddress       The element's full address (p.e. C23.c42.p13)
  *
  * @return string            The html code of the element's edit view.
  */  
  function showEdit( $fullAddress ){
    global $lng, $usr, $url, $cfg, $pge, $cDBI;
    
    if ( $url->available('insertI') ){ $url->rem('insertI'); $this->insertI(); }
    if ( $url->available('insertM') ){ $url->rem('insertM'); $this->insertM(); }
    if ( $url->available('insertP') ){ $url->rem('insertP'); $this->insertP(); }
    if ( $url->available('insertC') ){ $url->rem('insertC'); $this->insertC(); }
    
    $myRetVal =  "<FORM class=\"labsys_mop_std_form\" NAME=\"collectionEdit\" METHOD=\"POST\" ACTION=\"".$url->link2("../php/executeCommandOnElement.php?address=c")."\">\n".
                 "<input type=\"hidden\" name=\"FUNCTIONNAME\" value=\"save()\">\n".           // this function will statically be called by ../php/executeCommandOnElement.php
                 "<input type=\"hidden\" name=\"IDX\" value=\"".$this->idx."\">\n".            // needed by ../php/executeCommandOnElement.php
                 "<input type=\"hidden\" name=\"SESSION_ID\" value=\"".session_id()."\">\n".
                 "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/edit.php\">\n".  // index of saved el. will be added by save() below
                 "<input type=\"hidden\" name=\"FULLADDRESS\" value=\"".$fullAddress."\">\n".
                             
                // title
                 "<label for=\"title\" class=\"labsys_mop_input_field_label_top\">".$lng->get("inputTitle")."</label>".
                 "<input tabindex=\"".$pge->nextTab++."\" type=\"text\" id=\"title\" name=\"TITLE\" class=\"labsys_mop_input_fullwidth\" maxlength=\"255\" value=\"".$this->title."\">\n".

                // contents
                 "<label for=\"contents\" class=\"labsys_mop_input_field_label_top\">".$lng->get("inputContents")."</label>\n".
                 "<textarea tabindex=\"".$pge->nextTab++."\" id=\"contents\" name=\"CONTENTS\" class=\"labsys_mop_textarea\" rows=\"".($cfg->get("DefaultTextareaRows")/2)."\">";
                // parse the entries 
                 $tokens = $this->getTokens();
                 foreach ( $tokens as $value ){
                   $id = $value{0}; $num = substr( $value, 1 );
                   $prefix = retIfTrue( $id == strtoupper( $id ), "B", "L" ).$id;
                       if ( !is_numeric($num) )  $myRetVal .= $value."\t;***".$id.$num."\t; ".$lng->get("parseErrNotNum")."\n";
                   elseif ( !file_exists ( INCLUDE_DIR."/classes/elements/".$prefix."DBInterface.inc" ) )
                                                 $myRetVal .= $value."\t;***".$id.$num."\t; ".$lng->get("parseErrIdNotEx")."\n";
                   else{                                                
                         $element = $GLOBALS[ strtolower($id)."DBI" ]->getMenuData2idx( $num, true, $this->isInLab );
                         if ( $element ) $myRetVal .= $value."\t\t; ".htmlentities( $element->title )."\n";
                                    else $myRetVal .= $value."\t;***\t; ".$lng->get("parseErrNumNotEx")."\n";
                   }
                 }
    $myRetVal .= "</textarea>\n".

             // save
              "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("save").'">&nbsp;  '.
              ' <a class="labsys_mop_button" href="'.$url->link2($_SERVER['PHP_SELF'].'?address='.$fullAddress).'&insertP">&nbsp; *p &nbsp;</a>&nbsp; '.
              ' <a class="labsys_mop_button" href="'.$url->link2($_SERVER['PHP_SELF'].'?address='.$fullAddress).'&insertM">&nbsp; *m &nbsp;</a>&nbsp;  '.
              ' <a class="labsys_mop_button" href="'.$url->link2($_SERVER['PHP_SELF'].'?address='.$fullAddress).'&insertI">&nbsp; *i &nbsp;</a>&nbsp;  '.
              ' <a class="labsys_mop_button" href="'.$url->link2($_SERVER['PHP_SELF'].'?address='.$fullAddress).'&insertC">&nbsp; *c &nbsp;</a>'.
              "\n". 

             // properties
              "<fieldset><legend>".$lng->get("properties")."</legend>\n".
               // matching menu
                "<label for=\"mM\" class=\"labsys_mop_input_field_label_top\">".$lng->get("inputMatchingMnu")."</label>".
                "<input tabindex=\"".$pge->nextTab++."\" type=\"text\" id=\"mM\" name=\"MATCHINGMENU\" class=\"labsys_mop_input_fullwidth\" maxlength=\"255\" value=\"".$this->matchingMenu."\">\n".
  
               // visibility
                "<table width=\"100%\"><tr><td valign=\"top\" width=\"33%\">\n".
                  "<fieldset><legend>".$lng->get("visibility")."</legend>\n".
                    "<div>".
                      "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" id=\"visAlways\" name=\"VISIBLE_ONLY_IN_COLLECTION\" value=\"0\"".retIfTrue( !$this->visibleOnlyInCollection, " checked=\"checked\"").">".
                      "<label for=\"visAlways\" class=\"labsys_mop_input_field_label\">".$lng->get("visAlways")."</label>".
                    "</div>\n".
                    "<div>".
                      "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\"  id=\"visOnlyColl\"name=\"VISIBLE_ONLY_IN_COLLECTION\" value=\"1\"".retIfTrue( $this->visibleOnlyInCollection, " checked=\"checked\"").">".
                      "<label for=\"visOnlyColl\" class=\"labsys_mop_input_field_label\">".$lng->get("visOnlyInColl")."</label>".
                    "</div>\n".
                  "</fieldset>\n".
                "</td><td valign=\"top\" width=\"67%\">\n".
                 // schedule
                  "<fieldset><legend>".$lng->get("schedule")."</legend>\n".
                    "<div>".
                      "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" id=\"visBef1stSched\" name=\"VISIBLE_BEFORE_FIRST_SCHED\" value=\"1\"".retIfTrue( $this->visibleBefore1stSched, " checked=\"checked\"").">".
                      "<label for=\"visBef1stSched\" class=\"labsys_mop_input_field_label\">".$lng->get("visibleBef1stSched")."</label>".
                    "</div>\n".
                    "<div>".
                      "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" id=\"visDuringSched\" name=\"VISIBLE_DURING_SCHED\" value=\"1\"".retIfTrue( $this->visibleDuringSched, " checked=\"checked\"").">".
                      "<label for=\"visDuringSched\" class=\"labsys_mop_input_field_label\">".$lng->get("visibleDuringSched")."</label>".
                    "</div>\n".
                    "<div>".
                      "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" id=\"visAft1stSched\" name=\"VISIBLE_AFTER_FIRST_SCHED\" value=\"1\"".retIfTrue( $this->visibleAfter1stSched, " checked=\"checked\"").">".
                      "<label for=\"visAft1stSched\" class=\"labsys_mop_input_field_label\">".$lng->get("visibleAftr1stSched")."</label>".
                    "</div>\n".
                  "</fieldset>\n".
                  
                "</td></tr></table>\n".
              "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" id=\"saveAsNew\" name=\"SAVEASNEW\" value=\"1\"".retIfTrue( ( isSet( $_GET["saveAsNew"] ) || ($this->idx == 1) ),  " checked=\"checked\"" ).">".
              "<label for=\"saveAsNew\" class=\"labsys_mop_input_field_label\">".$lng->get("saveAsNew")."</label>".
              "</fieldset>\n".

             // save
              "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("save").'">'.
              "\n".            
  
              "</FORM>\n".
              '<script language="JavaScript" type="text/javascript">
              <!--
              if (document.collectionEdit) document.collectionEdit.title.focus();
              //-->
              </script>';

    return  EM::editBottomCollection( $this, $fullAddress ).
            $myRetVal.
            EM::editBottomCollection( $this, $fullAddress );
  }

  /**
  * Used to save the edited element.
  *
  * This function gets STATICALLY called from ../php/executeCommandOnElement.php?address=[elementID].
  * It handles the $_POSTed data and then redirects to the $_POST['REDIRECTTO'] with the address of the element added.
  */  
  /*static*/ function save(){
    global $cDBI, $url, $lng;
    if ( !isset($_POST['IDX']) || 
         !isset($_POST['REDIRECTTO']) || 
         !isset($_POST['FULLADDRESS']) || 
         !isset($_POST['TITLE']) || 
         !isset($_POST['CONTENTS']) || 
         !isset($_POST['MATCHINGMENU']) ||
         !isset($_POST['VISIBLE_ONLY_IN_COLLECTION'])
       ){
          trigger_error( $lng->get("notAllNecPosted"), E_USER_ERROR );
          exit;
         }

    // Validity (session ID) is checked by the calling script yet.

    // reformat contents:
    $contents = Array();
    $lines = explode ("\r\n", $_POST['CONTENTS']);
    foreach( $lines as $value ){
      $i = 0;
      $elType = $value[$i++];
      if (!(in_array($elType, $this->allowedElementIDs))) continue; // invalid type -> ignore
      $elNum = '';
      while( is_numeric( $value[$i] ) ) $elNum .= $value[$i++];
      if ( $elNum == '' ) continue; // no number -> continue
      if ( ( strtoupper($elType) == 'C') && ( $elNum == $this->idx ) ) $elNum = '1'; // no loops/ self-recursiveness -> put the placeholder in
      array_push( $contents, $elType.$elNum );
    }

    $contents = implode( $contents, " " );

    $newCollection = new LcElement( $_POST['IDX'],
                                    $_POST['TITLE'], 
                                    $contents, 
                                    $_POST['MATCHINGMENU'], 
                                    ( isset( $_POST["VISIBLE_BEFORE_FIRST_SCHED"] ) && $_POST["VISIBLE_BEFORE_FIRST_SCHED"] == 1 ), 
                                    ( isset( $_POST["VISIBLE_DURING_SCHED"] ) && $_POST["VISIBLE_DURING_SCHED"] == 1 ),
                                    ( isset( $_POST["VISIBLE_AFTER_FIRST_SCHED"] ) && $_POST["VISIBLE_AFTER_FIRST_SCHED"] == 1 ),
                                    ( $_POST["VISIBLE_ONLY_IN_COLLECTION"] == "1" ),
                                    ""
                                   );

    // if $_POST["SAVEASNEW"] is set the element is saved as new element
    if ( isset($_POST["SAVEASNEW"]) && ($_POST["SAVEASNEW"] == "1") ){
                                      $newCollection->idx = $cDBI->newCollection( $newCollection );
                                      $url->put( "address=c".$newCollection->idx );   // the address of the new element
                                     }
    else{
                                      $cDBI->setData( $newCollection );
                                      $url->put( "address=".$_POST['FULLADDRESS'] );  // the address remains
    }
    
    // some user information
    $url->put( "sysinfo=".urlencode( $lng->get("DataHasBeenSaved")." ".$lng->get("cNumber")." ".$newCollection->idx ) );
 
    // One could also display the page here but that way it is left to the normal mechanism...
    header( "Location: ".$url->rawLink2( $_POST['REDIRECTTO'] ) );
  }

// The next functions are used for the element's manage page. ***********************************************

  /**
  * Displays the element's property legend on the manage page.
  * All entries of getPropertyRow() should be explained here.
  */
  function showPropertyLegend(){ /* Shows the legend to the property rows */
    global $lng, $pge;
    $lgnd = "<div class=\"labsys_mop_".$this->elementId."_row\">\n".
            "<div class=\"labsys_mop_h3\">".$lng->get("legend")."</div>\n".
           // visibleOnlyInCollection?
            "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" name=\"legend1_VISIBLE_ONLY_IN_COLLECTION\" value=\"0\" checked=\"checked\">".
            infoArrow( $lng->get("visAlways") ).
            $lng->get("visAlways")."<br />\n".
            
            "<input type=\"radio\" name=\"legend2_VISIBLE_ONLY_IN_COLLECTION\" value=\"0\" disabled=\"disabled\">".
            infoArrow( $lng->get("visAlways"), true ).
            "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" name=\"legend2_VISIBLE_ONLY_IN_COLLECTION\" value=\"1\" checked=\"checked\">".
            infoArrow( $lng->get("visOnlyInColl") ).
            $lng->get("visOnlyInColl")."<br />\n".
            
            "<input type=\"radio\" name=\"legend2a_VISIBLE_ONLY_IN_COLLECTION\" value=\"0\" disabled=\"disabled\">".
            infoArrow( $lng->get("visAlways"), true ).
            "<input type=\"radio\" name=\"legend2a_VISIBLE_ONLY_IN_COLLECTION\" value=\"1\" disabled=\"disabled\">".
            infoArrow( $lng->get("visOnlyInColl"), true ).
          " | ".
           // schedule
            "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"legend3_VISIBLE_BEFORE_FIRST_SCHED\" value=\"1\" checked=\"checked\">".
            infoArrow( $lng->get("visibleBef1stSched") ).
            $lng->get("visibleBef1stSched")."<br />\n".
            
            "<input type=\"radio\" name=\"legend2a_VISIBLE_ONLY_IN_COLLECTION\" value=\"0\" disabled=\"disabled\">".
            infoArrow( $lng->get("visAlways"), true ).
            "<input type=\"radio\" name=\"legend2a_VISIBLE_ONLY_IN_COLLECTION\" value=\"1\" disabled=\"disabled\">".
            infoArrow( $lng->get("visOnlyInColl"), true ).
          " | ".
            "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"legend4_VISIBLE_BEFORE_FIRST_SCHED\" value=\"1\" disabled=\"disabled\">".
            infoArrow( $lng->get("visibleBef1stSched"), true ).
            "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"legend4_VISIBLE_DURING_SCHED\" value=\"1\" checked=\"checked\">".
            infoArrow( $lng->get("visibleDuringSched") ).
            $lng->get("visibleDuringSched")."<br />\n".
            
            "<input type=\"radio\" name=\"legend2a_VISIBLE_ONLY_IN_COLLECTION\" value=\"0\" disabled=\"disabled\">".
            infoArrow( $lng->get("visAlways"), true ).
            "<input type=\"radio\" name=\"legend2a_VISIBLE_ONLY_IN_COLLECTION\" value=\"1\" disabled=\"disabled\">".
            infoArrow( $lng->get("visOnlyInColl"), true ).
          " | ".
            "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"legend5_VISIBLE_BEFORE_FIRST_SCHED\" value=\"1\" disabled=\"disabled\">".
            infoArrow( $lng->get("visibleBef1stSched"), true ).
            "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"legend5_VISIBLE_DURING_SCHED\" value=\"1\" disabled=\"disabled\">".
            infoArrow( $lng->get("visibleDuringSched"), true ).
            "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"legend5_VISIBLE_AFTER_FIRST_SCHED\" value=\"1\" checked=\"checked\">".
            infoArrow( $lng->get("visibleAftr1stSched") ).
            $lng->get("visibleAftr1stSched")."\n".
            "</div>\n";
    return $lgnd;
  }  
  
  /**
  * This function is called to display the element's properties.
  * @param $prefix    If given this is put before the element's title (p.e. 1.2.3 title).
  * @param $disabled  Should the checkboxes, radio buttons etc. be disabled?
  * @return string    The html-code of the elements properties horizontally arranged.
  */
  function getPropertyRow( $prefix, $disabled=false ){
    global $pge, $lng;
    return    "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" ".retIfTrue( !$disabled, "name=\"".$prefix."_VISIBLE_ONLY_IN_COLLECTION\" " )."value=\"0\"".retIfTrue( !$this->visibleOnlyInCollection, " checked=\"checked\"").retIfTrue( $disabled, " disabled=\"disabled\"").">".
              infoArrow( $lng->get("visAlways"), $disabled ).
              "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" ".retIfTrue( !$disabled, "name=\"".$prefix."_VISIBLE_ONLY_IN_COLLECTION\" " )."value=\"1\"".retIfTrue( $this->visibleOnlyInCollection, " checked=\"checked\"").retIfTrue( $disabled, " disabled=\"disabled\"").">".
              infoArrow( $lng->get("visOnlyInColl"), $disabled ).
            retIfTrue( $disabled, "<span class=\"labsys_mop_grayed\">" ).
            " | ".
            retIfTrue( $disabled, "</span>" ).
              "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"".$prefix."_VISIBLE_BEFORE_FIRST_SCHED\" value=\"1\"".retIfTrue( $this->visibleBefore1stSched, " checked=\"checked\"").retIfTrue( $disabled, " disabled=\"disabled\"").">".
              infoArrow( $lng->get("visibleBef1stSched"), $disabled ).
              "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"".$prefix."_VISIBLE_DURING_SCHED\" value=\"1\"".retIfTrue( $this->visibleDuringSched, " checked=\"checked\"").retIfTrue( $disabled, " disabled=\"disabled\"").">".
              infoArrow( $lng->get("visibleDuringSched"), $disabled ).
              "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"".$prefix."_VISIBLE_AFTER_FIRST_SCHED\" value=\"1\"".retIfTrue( $this->visibleAfter1stSched, " checked=\"checked\"").retIfTrue( $disabled, " disabled=\"disabled\"").">".
              infoArrow( $lng->get("visibleAftr1stSched"), $disabled ).
            " ".
              $this->title;
  }
  
  
// The next functions are adfditional functions of the element. *********************************************
  /**
  * Builds the structure of the collection.
  * To build the structure causes many (slow) sql queries... so it is only done by need.
  *
  * Returns the structure of the collection in one String as follows (EBNF):
  *
  *   STRUCTURE ::= FINAL | CNUM ( STRUCTURE ) | {*}STRUCTURE | STRUCTURE STRUCTURE
  *   FINAL     ::= cNUM | pNUM | mNUM | iNUM | fNUM
  *   NUM       ::= {0..9}+
  *
  *   "(" means descend one level
  *   ")" ascend one
  *   "*" (asterisk) means that the following FINAL is NOT VISIBLE to the current user
  *
  *   p.e. p1 C2 ( p2 *p3 ) p2 c2 ( p1 m1 )
  *
  * @param $openC   Shall the C elements be expanded? [C2 ( p2 *p3 ) or just C2]
  * @param $openc   Shall the c elements be expanded? [c2 ( p1 m1 ) or just c2]
  */  
  function buildStructure( $openC=false, $openc=false ){
    global $cDBI, $usr;

    $structure = "";
    
    $tokens = $this->getTokens();
    foreach ( $tokens as $value ){
      $id = $value{0};
      $num = substr( $value, 1 );
      
      $element = $GLOBALS[ strtolower($id)."DBI" ]->getMenuData2idx( $num, true, $this->isInLab );
      
     // elemetnt invisible?
      if ( !$this->isVisible() || !$element || !$element->isVisible( true ) ) $structure.="*";
     // add element ID 
      $structure .= $value;

     // add contents of c to $fineStructure
      if ( ( ($openC && ($id == "C")) || 
             ($openc && ($id == "c")) )
           && $element ) $structure .= " ( ".$element->buildStructure( $openC, $openc )." )";

     // add spacer 
      $structure .= " ";
    }
    
    return substr( $structure, 0, -1 ); // cut last " "
  }
  
  /**
  * This selector returns the main structure (c not expanded) and initializes it if not done yet.
  *
  * @return string  The main structure (c not expanded).
  */
  function getStructure(){
    if ( !$this->structure ) $this->structure = $this->buildStructure(); // init

    return $this->structure;
  } 

    
  /**
  * Queries the elements in the collection until it finds at least one visible element.
  * So not the whole structure must be initialized since the first element can be visible and then the function ends.
  * THIS FUNCTION IS INDEPENDENT OF A FILLED STRUCTURE (used in getStructure)
  */
  function isAtLeastOneElementVisibleForMe(){
    global $cDBI, $usr;
    $tokens = $this->getTokens();
    foreach ( $tokens as $value ){
      $id = $value{0};
      $num = substr( $value, 1 );
        $element = $GLOBALS[ $id."DBI" ]->getMenuData2idx( $num, true, $this->isInLab );
        if ( $element && $element->isVisible( true ) ) return true; /* found an element */
    }
    return false; /* collection is done but no visible element was found */
  }

  /**
  * Returns an array of all visible elements in this collection.
  *
  * @return             An array of all visible elements.
  */
  function getMyVisibleElements(){
    $structureArray = explode( " ", $this->getStructure() );
    $retArray = Array();

    foreach( $structureArray as $value )
      if ($value{0} == "*") continue; // not visible
      else array_push ($retArray, $value);

    return $retArray;
  }
  
  /**
  * Checks whethter the given final (p.e. m12) is visible or not.
  *
  * @param $final   An element identifier idNum (p.e. m23).
  *
  * @return BOOL    Is the element visible?
  */
  function isVisibleFinal( $final ){
    return in_array( $final, $this->getMyVisibleElements() );
  }
  
  /**
  * Computes the paragraph numbering of all visible elements taking respect of all elements (also the invisibles!).
  *
  * @return     Array [relativeAddress] => [paragraph] (p.e. "c42" => "3")
  *
  * This function is non-recursive.
  */
  function getParagraphArray(){
    $structureArray = explode( " ", $this->getStructure() );

    $arrayText = "";
    $elNumber = 0;
    
    foreach( $structureArray as $value ){
      $elNumber++;
      if ($value{0} != "*") $arrayText .= ", \"".$value."\"=>\"".$elNumber."\"";
    }

    eval( "\$ret = Array( ".substr( $arrayText, 2 )." );" );

    return $ret;
  }
  
  /**
  * Queries for the paragraph number belonging to the relative address.
  *
  * @param $relativeAddress   A relative address of an element. (p.e. c43.m2 if we are C23 and the fulladdress is C23.c43.m2).
  *
  * @return                   The paragraph number of the element (p.e. 1.2.3).
  */
  function getParagraph( $relativeAddress ){
    if ( $relativeAddress == "" ) return "";
    
    if ( !$this->paragraphArray ) $this->paragraphArray = $this->getParagraphArray(); // init
    
    if ( array_key_exists( $relativeAddress, $this->paragraphArray ) )
      return $this->paragraphArray[ $relativeAddress ];
    else return " (C".$this->idx.": no ".$relativeAddress.") ";
  }
  
  /**
  * Returns an array of all $thus->contents tokens.
  *
  * @return      An array of all $thus->contents tokens or Array() if empty.
  */
  function getTokens(){
    if ( $this->contents == "" ) return array(); // empty! (explode would return array( "" ) -> problems by walking through!
                            else return explode ( " ", $this->contents );
  }
  
  /**
  * Loads each visible element into $element and evaluates $functionCode.
  * You can use $value      IdNum of current element
  *             $element    Reference to the element
  *             $parameter  The parameter.
  *
  * The function is very mighty. Only the passing of the function is kind of lame...
  * So I used this function rarely since the code get's kind of unreadable (but short).
  * You can find an example in showAllElementsOfType() above.
  *
  * @param $functionCode  Code to be exectued on each element.
  * @param &$parameter    You can use $parameter in $functionCode.
  *                                     example: ( adds the possible credits of the inputs )
  *                                         $information = Array( "count" => 0, "sumPossibleCredits" => 0 );                
  *                                         $myRetVal .= $this->lab->showAllElementsOfTypePlus( "i", $fullAddress.".C".$this->labCollectionIdx, $lng->get("labTOCparagraph"), "\$parameter['count']++; \$parameter['sumPossibleCredits'] += \$element->possibleCredits;", $information );
  */
  function extMap( $functionCode, &$parameter ){
    $myVisEl = $this->getMyVisibleElements();
    
    foreach( $myVisEl as $value ){
     // load element
      $id = $value{0}; $num = substr( $value, 1 );
      require( "../php/getDBIbyID.inc" );
      $element = $DBI->getData2idx( $num, true, $this->isInLab );
     // evaluate $functionCode
      eval( $functionCode );
    }
  }
  
  function insertI(){
    global $iDBI, $cDBI, $url, $lng;
    $newInput = new LiElement(  '0',
                                'i in c'.$this->idx.' by '.$_SESSION['foreName'].' '.$_SESSION['surName'], 
                                '',
                                '',
                                0, 
                                true,
                                ""
                               );
    $newIdx = $iDBI->newInput( $newInput );
    $this->contents = $this->contents.' i'.$newIdx;
    $cDBI->setData( $this );
  }
  
  function insertM(){
    global $mDBI, $cDBI, $url, $lng;
    $newMultipleChoice = new LmElement( 0,
                                        'm in c'.$this->idx.' by '.$_SESSION['foreName'].' '.$_SESSION['surName'], 
                                        array( 'answer' ),
                                        '',
                                        0, 
                                        0, 
                                        true,
                                        ''
                                       );
    $newIdx = $mDBI->newMultipleChoice( $newMultipleChoice );
    $this->contents = $this->contents.' m'.$newIdx;
    $cDBI->setData( $this );
  }
  
  function insertP(){
    global $pDBI, $cDBI, $url, $lng;
    $newPage = new LpElement(   '0',
                                'p in c'.$this->idx.' by '.$_SESSION['foreName'].' '.$_SESSION['surName'], 
                                '- empty -', 
                                '', 
                                0, 
                                true,
                                ''
                               );
    $newIdx = $pDBI->newPage( $newPage );
    $this->contents = $this->contents.' p'.$newIdx;
    $cDBI->setData( $this );
  }
  
  function insertC(){
    global $cDBI, $url, $lng;
    $newCollection = new LcElement( 0,
                                    'C in c'.$this->idx.' by '.$_SESSION['foreName'].' '.$_SESSION['surName'],  
                                    'p1',
                                    '', 
                                    false, 
                                    false,
                                    false,
                                    true,
                                    ''
                                   );
    $newIdx = $cDBI->newCollection( $newCollection );
    $this->contents = $this->contents.' C'.$newIdx;
    $cDBI->setData( $this );
  }

}
?>