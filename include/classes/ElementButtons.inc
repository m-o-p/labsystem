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
* This classes Methods are used to create the buttons for the control menus in the contents area.
*
*/

/**
* The methods of this class are statically called to make the buttons.
*
* @module     ../include/classes/ElementButtons.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
class EB{ /* ElementButtons */
  
  /**
  * This method is only called internally and returns the link.
  *
  * @param $link  The link to link to.
  * @param $img   The image that is used as button.
  *
  * @return       The link's html-code.
  */
  /*private*/ function mkLink( $link, $img ){
    global $pge;
    return "\t<a tabindex=\"".$pge->nextTab++."\" href=\"".$link."\">".$img."</a>\n";
  }
  
  /**
  * This method is only called internally and returns a hidden div with the link in a textbox for copying.
  *
  * @param $link  The link to link to.
  * @param $img   The image that is used as button.
  *
  * @return       The link's html-code.
  */
  /*private*/ function mkHiddenDiv( $link, $img, $content ){
    global $pge;
    return " <a tabindex=\"".$pge->nextTab++."\" href=\"".
                $link.
                "\"".
                " onMouseUp=\"if (document.getElementById('hiddenDiv_".$pge->nextTab.
                  "').style.display != 'block') document.getElementById('hiddenDiv_".$pge->nextTab.
                  "').style.display = 'block'; else document.getElementById('hiddenDiv_".$pge->nextTab.
                  "').style.display = 'none';\" onClick=\"return false;\"".
                ">".
           $img.
           "</a>".
           "<div id=\"hiddenDiv_".$pge->nextTab."\" class=\"labsys_mop_history\">\n".
           '<div style="display: inline; float: right; text-align: center; padding-right: 2px; color: #555555;">'.
           '<a href="#" onClick="document.getElementById(\'hiddenDiv_'.$pge->nextTab.'\').style.display = \'none\'; return false;">X</a>'.
           '</div>'.
            $content.
           "\n</div>"
           ;
  }
   
  /**
  * This button contains the url of the given element with all necessary values for integrating it on a page.
  * So you can copy the elements address and paste it into a page element.
  *
  * @param $address  The element's address.
  *
  * @return          The button's html-code.
  */
  function link2Element( $address ){
    global $lng;
    return EB::mkHiddenDiv( "../pages/view.php?address=".$address."&amp;amp;&#x005F;_LINKQUERY__",
                            "<img src=\"../syspix/button_link2_13x12.gif\" width=\"13\" height=\"12\" border=\"0\" alt=\"link to\" title=\"".$lng->get("explainLink2")."\">",
                            '<input type="text" style="width: 100%" value="&lt;a href=&quot;../pages/view.php?address='.$address.'&amp;amp;&#x005F;_LINKQUERY__&quot;&gt;Link to '.$address.'&lt;/a&gt;" onClick="this.select();">' );
  }
  
  /**
  * Creates the link button linking to the given $link-url. Adds __LINKQUERY__
  *
  * @param $link     The url..
  *
  * @return          The button's html-code.
  */
  function link2Url( $link ){
    global $lng;
    $link .= '?&#x005F;_LINKQUERY__';
    return EB::mkHiddenDiv( $link,
                            "<img src=\"../syspix/button_link2_13x12.gif\" width=\"13\" height=\"12\" border=\"0\" alt=\"link to\" title=\"".$lng->get("explainLink2")."\">",
                            '<input type="text" style="width: 100%" value="&lt;a href=&quot;'.$link.'&quot;&gt;Text&lt;/a&gt;" onClick="this.select();">' );
  }
  
  /**
  * This button links to "view the element".
  *
  * @param $address  The element's address.
  *
  * @return          The button's html-code.
  */
  function view( $address ){
    global $lng, $url;
    $link = $url->link2( "../pages/view.php?address=".$address.'#'.$address ); // #$address will jump directly to the element
    return EB::mkLink( $link, "<img src=\"../syspix/button_view_13x12.gif\" width=\"13\" height=\"12\" border=\"0\" alt=\"view\" title=\"".$lng->get("explainView")."\">" );
  }

  /**
  * This button links to "edit the element".
  *
  * @param $address  The element's address.
  *
  * @return          The button's html-code.
  */
  function edit( $address ){
    global $lng, $url;
    $link = $url->link2( "../pages/edit.php?address=".$address );
    return EB::mkLink( $link, "<img src=\"../syspix/button_edit_13x12.gif\" width=\"13\" height=\"12\" border=\"0\" alt=\"edit\" title=\"".$lng->get("explainEdit")."\">" );
  }
  
  /**
  * This button links to "print view (all on one)".
  *
  * @param $address  The element's address.
  *
  * @return          The button's html-code.
  */
  function printView( $address ){
    global $lng, $url;
    $link = $url->link2( "../pages/view.php?address=".$address );
    return EB::mkLink( $link, "<img src=\"../syspix/button_print_13x12.gif\" width=\"13\" height=\"12\" border=\"0\" alt=\"view\" title=\"".$lng->get("explainPrint")."\">" );
  }
  
  /**
  * This button links to "create a new element of this type".
  *
  * @param $elementID The ID of the element that should get created (p.e. "p").
  *
  * @return           The button's html-code.
  */
  function createNew( $elementID ){
    global $lng, $url;
    $link = $url->link2( "../pages/edit.php?address=".$elementID."1&amp;saveAsNew=true" );
    return EB::mkLink( $link, "<img src=\"../syspix/button_new_13x12.gif\" width=\"13\" height=\"12\" border=\"0\" alt=\"new\" title=\"".$lng->get("explainCreateNew")."\">" );
  }  
  
  /**
  * This button links to the elements' manage page.
  *
  * @return          The button's html-code.
  */
  function manageElements( ){
    global $lng, $url;
    return EB::mkLink( $url->link2("../pages/view.php?address=p2"), "<img src=\"../syspix/button_manage_elements_13x12.gif\" width=\"13\" height=\"12\" border=\"0\" alt=\"new\" title=\"".$lng->get("explainManageElmnts")."\">" );
  }

  /**
  * This button links to "delete the element".
  *
  * @param $address  The element's address.
  *
  * @return          The button's html-code.
  */
  function delete( $address ){
    global $lng, $pge, $url;
    $link = $url->link2( "../php/delete.php?address=".$address );
    return "<a tabindex=\"".$pge->nextTab++."\" href=\"".$link."\" onClick=\"return confirmLink(this, '".$address.": ".$lng->get("confirmDelete")."');\">".
           "<img src=\"../syspix/button_delete_13x12.gif\" width=\"13\" height=\"12\" border=\"0\" alt=\"delete\" title=\"".$lng->get("explainDeleteElemnt")."\">".
           "</a>";
  }

  /**
  * This button links to the collection's "view structure".
  *
  * @param $address  The element's address.
  *
  * @return          The button's html-code.
  */
  function viewStructure( $address ){
    global $lng, $url;
    $link = $url->link2( "../pages/view.php?address=".$address.".structure" );
    return EB::mkLink( $link, "<img src=\"../syspix/button_view_structure_13x12.gif\" width=\"13\" height=\"12\" border=\"0\" alt=\"view_structure\" title=\"".$lng->get("explainViewStructure")."\">" );
  }  
  
  /**
  * This button links to the element's history (normally via hidden layer by mouseclick).
  *
  * If !$url->available( 'fullHistory' ) the history is shortened to $cfg->get( 'limitHistLns2' )
  * and a Link thisURL&fullHistory=$address is provided if something was cut.
  *
  * @param $history         HTML-Code of the history.
  *                         Used for the popUp and $sendAsAddHist.
  * @param $address         The address of the element the history should be shown of.
  *                         The history is reloaded from the element with this address on history.php.
  * @param $sendAsAddHist   If set all (shortened) content of $history is sent via http-url-push.
  *                         Used for answer histories so that they don't have to be recovered on history.php.
  *
  * @return                 The button's html-code.
  */
  function history ( $history, $address, $sendAsAddHist=false ){
    global $pge, $lng, $url, $cfg;
    
    if ($url->available( 'hideHistory' )) return ''; // temporarily handy for HTML diffs...
    
    // Shortening for Link (no stylesheet+JavaScript enabled)
    $shrtHistURL = "";
    if ( $sendAsAddHist ){
      if ( strlen($history) > 500 ) // IE doesn't allow links with endless length, so we have to cut
                                    $shrtHistURL = substr( $history, 0, 400 )."...\n...".substr( $history, -100 );
                               else $shrtHistURL = $history;
      $shrtHistURL = urlencode( nl2br( $shrtHistURL ) );
    }
    
    // shorten History to $cfg->get( 'defHistLns' ) Lines?
    if ( $url->available( 'fullHistory' ) || !$cfg->doesExist( 'limitHistLns2' ) )
        $maxLines = false;
      else
        $maxLines = $cfg->get( 'limitHistLns2' );

    if ( $maxLines ) $historyLines = explode( "\n", $history, $maxLines+1 );
                else $historyLines = explode( "\n", $history );
    
    $historyText = '';
    if ( $maxLines && (count( $historyLines ) == $maxLines+1) )
         $isLimited = 1; else $isLimited = 0;
    for ( $i=0; $i < count( $historyLines ) - $isLimited; $i++ ) 
      $historyText .= $historyLines[$i].'<br>'."\n";

    if ( $isLimited ) 
      $historyText .= '<a href="'.$_SERVER['REQUEST_URI'].'&amp;fullHistory='.$address.'#history_'.$address.'">[...]</a>'."\n";
    
    return " <a tabindex=\"".$pge->nextTab++."\" name=\"history_".$address."\" href=\"".
                $url->link2("../pages/history.php?address=".$address.( $sendAsAddHist ?  "&amp;history=".$shrtHistURL : '' ) ).
                "\"".
                " onMouseUp=\"if (document.getElementById('history_".$pge->nextTab."').style.display != 'block') document.getElementById('history_".$pge->nextTab."').style.display = 'block'; else document.getElementById('history_".$pge->nextTab."').style.display = 'none';\" onClick=\"return false;\"".
                ">".
           "<img src=\"../syspix/button_history_13x12.gif\" width=\"13\" height=\"12\" border=\"0\" alt=\"".$lng->get("explainHistory")."\">".
//           "<div>\n".nl2br($history)."\n</div>".
           "</a>".
           "<div id=\"history_".$pge->nextTab."\" class=\"labsys_mop_history\"".( $url->available( 'fullHistory' ) && ($url->get( 'fullHistory' ) == $address) ?  ' style="display: block;"' : '' ).">\n".
// could be added to clode on click on the whole element:        onClick=\"this.style.display = 'none';\"
           '<div style="display: inline; float: right; text-align: center; padding-right: 2px; color: #555555;">'.
//           '&lt;a href="../pages/view.php?address='.$address.'&amp;amp;&#x005F;_LINKQUERY__"&gt;Text&lt;/a&gt;'.
           '<a href="#" onClick="document.getElementById(\'history_'.$pge->nextTab.'\').style.display = \'none\'; return false;">X</a>'.
           '</div>'.
            $historyText.
           "\n</div>"
           ;
  }
  
  /**
  * This button is used for navigation inside a collection.
  * Link to the next page.
  *
  * @param $address  The link2element's address.
  *
  * @return          The button's html-code.
  */
  function prev( $address ){
    global $lng, $url;
    $link = $url->link2( "../pages/view.php?address=".$address );
    return EB::mkLink( $link, "<img src=\"../syspix/button_previous_13x12.gif\" width=\"13\" height=\"12\" border=\"0\" alt=\"previous\" title=\"".$lng->get("explainPrev")."\">" );
  }
  
  /**
  * This button is used for navigation inside a collection.
  * Link to the previous page.
  *
  * @param $address  The link2element's address.
  *
  * @return          The button's html-code.
  */
  function next( $address ){
    global $lng, $url;
    $link = $url->link2( "../pages/view.php?address=".$address );
    return EB::mkLink( $link, "<img src=\"../syspix/button_next_13x12.gif\" width=\"13\" height=\"12\" border=\"0\" alt=\"next\" title=\"".$lng->get("explainNext")."\">" );
  }

  /**
  * This button is used for navigation inside a collection.
  * Link to the table of contents.
  *
  * @param $address  The link2element's address.
  *
  * @return          The button's html-code.
  */
  function toc( $address ){
    global $lng, $url;
    $link = $url->link2( "../pages/view.php?address=".$address );
    return EB::mkLink( $link, "<img src=\"../syspix/button_toc_13x12.gif\" width=\"13\" height=\"12\" border=\"0\" alt=\"TOC\" title=\"".$lng->get("explainTOC")."\">" );
  }
}
?>
