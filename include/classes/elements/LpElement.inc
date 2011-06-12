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
* implementation of the p (page/ html entity) element.
*/

/**
* This element represents a html entity.
*
* @module     ../include/classes/elements/LpElement.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
require_once( INCLUDE_DIR."/classes/elements/Element.inc" );

class LpElement extends Element{
  var $contents;    // The html code the page contains.
  var $visibleFor;  // Visibility constraints.
  
  /**
  * The constructor.
  *
  * @param $idx                       This instance's index.
  * @param $title                     The title (used as __ELEMENTTITLE__ and for html <title> if displayed alone).
  * @param $contents                  The html code the page contains.
  * @param $matchingMenu              The title of the menu that gets generated/ is highlighted to the element.
  * @param $visibleFor                The bitmask of rights a viewer has to match.
  * @param $visibleOnlyInCollection   Is the element visible only inside a collection.
  * @param $history                   This' history.
  * @param $isInColl                  Am I inside a collection? (important for visibility aspects).
  */
  function LpElement( $idx, $title, $contents, $matchingMenu, $visibleFor, $visibleOnlyInCollection, $history, $isInColl=false ){
    global $usr;
    $this->elementId                = "p";
    $this->idx                      = $idx;
    $this->title                    = $title;
    $this->contents                 = $contents;
    $this->matchingMenu             = $matchingMenu;
    $this->visibleFor               = $visibleFor;
    $this->visibleOnlyInCollection  = $visibleOnlyInCollection;
    $this->history                  = $history;
    
    $this->IamVisible               = ( ($this->visibleFor == "0") || $usr->isOfKind( $this->visibleFor ) )
                                      && ( !$this->visibleOnlyInCollection || ($this->visibleOnlyInCollection && $isInColl) );
                                      
    $this->serializationArray       = array( 'idx', 'title', 'contents', 'matchingMenu', 'visibleFor', 'visibleOnlyInCollection', 'history' ); // see Element.inc for details
  }


// The next functions are used for the element's view page. *************************************************

  /**
  * Returns the element's HTML-representation in the way the VIEWER should see it.
  *
  * @param $fullAddress   The element's full address (p.e. C23.c42.p13)
  * @param $paragraph     The element's paragraph number (p.e. 2.3). Will be added to the title if given.
  * @return string        The html code of the element's view view.
  */
  function show( $fullAddress, $paragraph="" ){
    global $usr, $lng;
    if (  !$this->isVisible() ){              // not enough rights->no content
      return "<div class=\"labsys_mop_note\">\n".$lng->get("TxtNotEnoughRights")."\n</div>";
      exit;
    }

    if ($this->contents == "") return "'()";  // is empty
    
    // rplace the tag __ELEMENTTITLE__ by $paragraph.$this->title
    $this->contents = str_replace( "__ELEMENTTITLE__", retIfTrue( $paragraph != "", $paragraph.". " ).$this->title, $this->contents );
    
    parseHTML( $this->contents );             // Process [HTML]. (../include/helper.inc).
    
    return EM::viewTopPage( $this, $fullAddress ).
           "<div class=\"labsys_mop_contentArea\">".$this->contents."</div>".
           EM::viewBottomPage( $this, $fullAddress );
  }
  

// The next functions are used for the element's edit page. *************************************************

  /**
  * Returns the element's HTML-representation in the way the EDITOR should see it.
  *
  * @param $_GET["saveAsNew"] If $_GET["saveAsNew"] is set the save as new checkbox on the edit page should be set
  * @param $fullAddress       The element's full address (p.e. C23.c42.p13)
  * @return string            The html code of the element's edit view.
  */ 
  function showEdit( $fullAddress ){
    global $lng, $url, $pge, $cfg;
     return EM::editTopPage( $this, $fullAddress ).
            "<FORM class=\"labsys_mop_std_form\" NAME=\"pageEdit\" METHOD=\"POST\" ACTION=\"".$url->link2("../php/executeCommandOnElement.php?address=p")."\">\n".
            "<input type=\"hidden\" name=\"FUNCTIONNAME\" value=\"save()\">\n".           // this function will statically be called by ../php/executeCommandOnElement.php
            "<input type=\"hidden\" name=\"SESSION_ID\" value=\"".session_id()."\">\n".   // to check whether the call is valid
            "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/edit.php\">\n".  // index of saved el. will be added on save.php!
            "<input type=\"hidden\" name=\"FULLADDRESS\" value=\"".$fullAddress."\">\n".
            "<input type=\"hidden\" name=\"IDX\" value=\"".$this->idx."\">\n".            // needed by ../php/executeCommandOnElement.php
            
          // title input
            "<label for=\"title\" class=\"labsys_mop_input_field_label_top\">".$lng->get("inputTitle")."</label>".
            "<input tabindex=\"".$pge->nextTab++."\" type=\"text\" id=\"title\" name=\"TITLE\" class=\"labsys_mop_input_fullwidth\" maxlength=\"255\" value=\"".$this->title."\">\n".

          // content input
            "<label for=\"contents\" class=\"labsys_mop_input_field_label_top\">".$lng->get("inputContents")." ".$lng->get("inputContentsHtml")."</label>\n".
            "<div class=\"labsys_mop_input_field_note\">".$lng->get("inputReplaceKeys").
            "<div class=\"labsys_mop_p_field_constants\">".str_replace( "_", "&#x005F;", $pge->getConstants() )."</div>".
            "</div>\n".
            "<textarea tabindex=\"".$pge->nextTab++."\" id=\"contents\" name=\"CONTENTS\" class=\"labsys_mop_textarea\" rows=\"".$cfg->get("DefaultTextareaRows")."\">".
            returnEditable( $this->contents ).    // Process [HTML]. (../include/helper.inc).
            "</textarea>\n".

            "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("save")."\" accesskey=\"s\">\n".

          // element properties
            "<fieldset><legend>".$lng->get("properties")."</legend>\n".
              "<label for=\"mM\" class=\"labsys_mop_input_field_label_top\">".$lng->get("inputMatchingMnu")."</label>".
              "<input tabindex=\"".$pge->nextTab++."\" type=\"text\" id=\"mM\" name=\"MATCHINGMENU\" class=\"labsys_mop_input_fullwidth\" maxlength=\"255\" value=\"".$this->matchingMenu."\">\n".

              "<table width=\"100%\"><tr><td valign=\"top\" width=\"40%\">\n".
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
              "</td><td valign=\"top\" width=\"60%\">\n".
                "<fieldset><legend>".$lng->get("rights")."</legend>\n".
                "<div class=\"labsys_mop_input_field_note\">".$lng->get("visibleFor")."</div>\n".
                "<div class=\"labsys_mop_in_fieldset\">\n".
                showRightsVertical( "UR", $this->visibleFor, false ).
                "</div>\n".
                "</fieldset>\n".
              "</td></tr></table>\n".
            "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" id=\"saveAsNew\" name=\"SAVEASNEW\" value=\"1\"".retIfTrue( ( isSet( $_GET["saveAsNew"] ) || ($this->idx == 1) ),  " checked=\"checked\"" ).">".
            "<label for=\"saveAsNew\" class=\"labsys_mop_input_field_label\">".$lng->get("saveAsNew")."</label>".
            "</fieldset>\n".

            "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("save")."\">\n".            

            "</FORM>\n".
            '<script language="JavaScript" type="text/javascript">
            <!--
            if (document.pageEdit) document.pageEdit.title.focus();
            //-->
            </script>'.
            EM::editBottomPage( $this, $fullAddress );
  }
  
  /**
  * Used to save the edited element.
  *
  * This function gets STATICALLY called from ../php/executeCommandOnElement.php?address=[elementID].
  * It handles the $_POSTed data and then redirects to the $_POST['REDIRECTTO'] with the address of the element added.
  */  
  /*static*/ function save(){
    global $pDBI, $url, $lng, $usr;
    
    // check for all necessary values posted
    if ( !isset($_POST['IDX']) || 
         !isset($_POST['REDIRECTTO']) || 
         !isset($_POST['FULLADDRESS']) || 
         !isset($_POST['TITLE']) || 
         !isset($_POST['CONTENTS']) || 
         !isset($_POST['MATCHINGMENU'])     
       ){
          trigger_error( $lng->get("notAllNecPosted"), E_USER_ERROR );
          exit;
         }
       
    // Validity (session ID) is checked by the calling script yet.
    
    // only content editors are allowed to save
    if ( !$usr->isOfKind( IS_CONTENT_EDITOR ) ){
                                                  trigger_error( $lng->get("notAllowed"), E_USER_ERROR );
                                                  exit;
                                                }
    
    // compute user rights:
    $userRights=0;
    for ($i=1; $i<=MAX_USER_ROLE; $i=$i<<1) if ( isset( $_POST[ "UR_".$i] ) ) $userRights += $_POST["UR_".$i];
    
    // create a new element with the given values
    $newPage = new LpElement( $_POST['IDX'],
                                $_POST['TITLE'], 
                                $_POST['CONTENTS'], 
                                $_POST['MATCHINGMENU'], 
                                $userRights, 
                                ($_POST["VISIBLE_ONLY_IN_COLLECTION"] == "1"),
                                ""
                               );
                               
    // if $_POST["SAVEASNEW"] is set the element is saved as new element
    if ( isset($_POST["SAVEASNEW"]) && ($_POST["SAVEASNEW"] == "1") ){
                                      $newPage->idx = $pDBI->setData( $newPage, true );
                                      $url->put( "address=p".$newPage->idx );         // the address of the new element
                                     }
    else{
                                      $pDBI->setData( $newPage );
                                      $url->put( "address=".$_POST['FULLADDRESS'] );  // the address remains
    }
    
    // some user information
    $url->put( "sysinfo=".urlencode( $lng->get("DataHasBeenSaved")." ".$lng->get("pNumber")." ".$newPage->idx ) );
      
    // One could also display the page here but that way it is left to the normal mechanism...
    header( "Location: ".$url->rawLink2( $_POST['REDIRECTTO'] ) );
  }
  
  
// The next functions are used for the element's manage page. ***********************************************

  /**
  * Displays the element's property legend on the manage page.
  * All entries of getPropertyRow() should be explained here.
  */
  function showPropertyLegend(){
    global $lng, $pge;
    $lgnd = "<div class=\"labsys_mop_".$this->elementId."_row\">\n".
            "<div class=\"labsys_mop_h3\">".$lng->get("legend")."</div>\n".

          // visible only in collection?
            "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" name=\"legend_VISIBLE_ONLY_IN_COLLECTION\" value=\"0\" checked=\"checked\">".
                infoArrow( $lng->get("visAlways") ).
                $lng->get("visAlways")."<br>\n".
               
             "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" name=\"legend2_VISIBLE_ONLY_IN_COLLECTION\" value=\"0\" disabled>".
                infoArrow( $lng->get("visAlways"), false ).
                "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" name=\"legend2_VISIBLE_ONLY_IN_COLLECTION\" value=\"1\" checked=\"checked\">".
                infoArrow( $lng->get("visOnlyInColl") ).
                $lng->get("visOnlyInColl")."<br>\n";
                
    // user rights
    for ($i=1; $i<=MAX_USER_ROLE; $i=$i<<1){
      $lgnd .= "<input type=\"radio\" disabled>".infoArrow( $lng->get("visAlways"), true )."<input type=\"radio\" disabled>".infoArrow( $lng->get("visOnlyInColl"), true )." | ";
      for ($j=1; $j<$i; $j=$j<<1) $lgnd .= '<input type="checkbox" disabled>'.infoArrow( $lng->get("Explain_UR_".$j), true )/* ."\n" saves space! */;
      $lgnd .= rightsBox( "legend_".$i, $i, $i )." ".$lng->get("Explain_UR_".$i)."<br>\n";    
    }
    
    $lgnd .= "</div>\n";
    
    return $lgnd;
  }
  
  /**
  * This function is called to display the element's properties.
  * @param $prefix    If given this is put before the element's title (p.e. 1.2.3 title).
  * @param $disabled  Should the checkboxes, radio buttons etc. be disabled?
  * @return string    The html-code of the elements properties horizontally arranged.
  */
  function getPropertyRow( $prefix, $disabled=false ){
    global $pge, $lng, $cfg;
    return  
              "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" ".retIfTrue( !$disabled, "name=\"".$prefix."_VISIBLE_ONLY_IN_COLLECTION\" " )."value=\"0\"".retIfTrue( !$this->visibleOnlyInCollection, " checked=\"checked\"").retIfTrue( $disabled, " disabled=\"disabled\"").">".
              infoArrow( $lng->get("visAlways"), $disabled ).
              "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" ".retIfTrue( !$disabled, "name=\"".$prefix."_VISIBLE_ONLY_IN_COLLECTION\" " )."value=\"1\"".retIfTrue( $this->visibleOnlyInCollection, " checked=\"checked\"").retIfTrue( $disabled, " disabled=\"disabled\"").">".
              infoArrow( $lng->get("visOnlyInColl"), $disabled ).
            retIfTrue( $disabled, "<span class=\"labsys_mop_grayed\">" ).
            " | ".
            retIfTrue( $disabled, "</span>" ).
              showRightsHorizontal( $prefix, $this->visibleFor, $disabled ).
            " ".
              cutHTMLText( $this->title, $cfg->get("maxCharMngRowTitleShort") );
  }
}
?>