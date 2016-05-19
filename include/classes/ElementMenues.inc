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
* The methods of this class are statically called to make the element's menus in the contents area.
* The standard menu implementations could directly be called from the elements but doing it like I
* did makes it easy to change an elements menu here!
*/

/**
* @module     ../include/classes/ElementMenues.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
class EM{ /* ElementMenues */
// V I E W
  /**
  * Returns the standard top menu of an element;
  * @param &$theEl        A Reference to the element the menu is for.
  * @param $fullAddress   The element's full address.
  * @param $elShortcut    The elements shortcut (p.e. p, m, i, ...).
  * @return               The html code of the menu.
  */
  static function viewTopStandard( &$theEl, $fullAddress, $elShortcut ){
    global $usr, $lng;
    if ( $usr->isOfKind( IS_CONTENT_EDITOR ) )
     return "\n\n\t<!-- *************************start of ".$elShortcut."-element************************* -->\n\n".
            "<div class=\"labsys_mop_elements_menu_".$elShortcut."\">\n".
            '<div class="labsys_mop_element_title_in_header">'.
            "<span class=\"labsys_mop_element_description_in_header\">".
            //             $fullAddress.
            $theEl->elementId.$theEl->idx.(isset($theEl->visibleFor)?' ('.$theEl->visibleFor.')':'').
            "</span>".PHP_EOL.
            
            '<a href="http'.( isset($_SERVER['HTTPS']) ? 's' : '').'://'.$_SERVER['SERVER_NAME'].$_SERVER['REQUEST_URI'].'#'.$fullAddress.'">'.$theEl->title.'</a> | </div>'.
            ( $theEl->visibleOnlyInCollection ?  "<span class=\"labsys_mop_element_note_in_header\">".$lng->get("VisibleOnlyColl")."</span> \n"  : '' ).
//             '<a href="http'.( isset($_SERVER['HTTPS']) ? 's' : '').'://'.$_SERVER['SERVER_NAME'].$_SERVER['REQUEST_URI'].'#'.$fullAddress.'">'.
//             EB::mail ( $fullAddress, $theEl->elementId.$theEl->idx.': '.$theEl->title ).
            EB::ticket ( $fullAddress ).
            EB::history( $theEl->history, $fullAddress ).
            EB::link2Element( $fullAddress, $theEl ).
            //EB::createNew( $elShortcut ).
            (strpos(strtolower($fullAddress),'c') ? EB::delete( $fullAddress ):'').
            // EB::manageElements().
            EB::edit( $fullAddress ).
            "</div>\n";
    else return "\n\n\t<!-- *************************start of ".$elShortcut."-element************************* -->\n\n";
  }

  /**
  * Returns the standard bottom menu of an element;
  * @param &$theEl        A Reference to the element the menu is for.
  * @param $fullAddress   The element's full address.
  * @param $elShortcut    The elements shortcut (p.e. p, m, i, ...).
  * @return               The html code of the menu.
  */
  static function viewBottomStandard( &$theEl, $fullAddress, $elShortcut ){
    global $usr, $lng;
    if ( $usr->isOfKind( IS_CONTENT_EDITOR ) )
      return "<div class=\"labsys_mop_elements_menu_".$elShortcut."_edit_bottom\">&nbsp;</div>\n".
             "\n\n\t<!-- *************************stop of ".$elShortcut."-element************************* -->\n\n";
    else
      return "<div class=\"labsys_mop_elements_menu_".$elShortcut."\">\n".
//              EB::mail ( $fullAddress, $theEl->elementId.$theEl->idx.': '.$theEl->title ).
             EB::ticket ( $fullAddress ).
             EB::history( $theEl->history, $fullAddress ).
             "</div>\n".
             "\n\n\t<!-- *************************stop of ".$elShortcut."-element************************* -->\n\n";
  }

  /**
  * The following functions return the element's top and bottom menus.
  * @param &$e?           A reference to the element the menu is for.
  * @param $fullAddress   The element's full address.
  * @return               The html code of the menu.
  */
  static function viewTopPage( &$eP, $fullAddress ){
    return EM::viewTopStandard( $eP, $fullAddress, "p" );
  }
  static function viewBottomPage( &$eP, $fullAddress ){
    return EM::viewBottomStandard( $eP, $fullAddress, "p" );
  }

  static function viewTopMultipleChoice( &$eMC, $fullAddress ){
    return EM::viewTopStandard( $eMC, $fullAddress, "m" );
  }
  static function viewBottomMultipleChoice( &$eMC, $fullAddress ){
    return EM::viewBottomStandard( $eMC, $fullAddress, "m" );
  }

  static function viewTopInput( &$eI, $fullAddress ){
    return EM::viewTopStandard( $eI, $fullAddress, "i" );
  }
  static function viewBottomInput( &$eI, $fullAddress ){
    return EM::viewBottomStandard( $eI, $fullAddress, "i" );
  }

  static function viewTopSchedule( &$eS, $fullAddress ){
    // return EM::viewTopStandard( $eS, $fullAddress, "s" );
    // The element can not be created by content editors but by schedulers:
    global $usr, $lng;

    $elShortcut = "s";

    return ( $usr->isOfKind( IS_CONTENT_EDITOR ) || $usr->isOfKind( IS_SCHEDULER ) ? "\n\n\t<!-- *************************start of ".$elShortcut."-element************************* -->\n\n".
                                                                                     "<div class=\"labsys_mop_elements_menu_".$elShortcut."\">\n" :'' ).
           ( $usr->isOfKind( IS_CONTENT_EDITOR ) ? EB::link2Element( 's1', $eS ) : '' ).
           ( $usr->isOfKind( IS_SCHEDULER ) ? EB::createNew( $elShortcut ) : '' ).
           ( $usr->isOfKind( IS_CONTENT_EDITOR ) || $usr->isOfKind( IS_SCHEDULER ) ?"</div>\n" : '' );
  }
  static function viewBottomSchedule( &$eS, $fullAddress ){
    return EM::viewBottomStandard( $eS, $fullAddress, "s" );
  }

  static function viewTopLab( &$eL, $fullAddress ){
    global $usr, $lng;
    $elShortcut = 'l';
    if ( $usr->isOfKind( IS_CONTENT_EDITOR ) )
     return "\n\n\t<!-- *************************start of ".$elShortcut."-element************************* -->\n\n".
            "<div class=\"labsys_mop_elements_menu_".$elShortcut."\">\n".
            "<span class=\"labsys_mop_element_description_in_header\">".
            //             $fullAddress.
            $eL->elementId.$eL->idx.
            "</span>".PHP_EOL.
            ( $eL->visibleOnlyInCollection ?  "<span class=\"labsys_mop_element_note_in_header\">".$lng->get("VisibleOnlyColl")."</span> \n"  : '' ).
            '<a href="http'.( isset($_SERVER['HTTPS']) ? 's' : '').'://'.$_SERVER['SERVER_NAME'].$_SERVER['REQUEST_URI'].'#'.$fullAddress.'">'.
            //"<span class=\"labsys_mop_element_description_in_header\">".
            //$fullAddress.
            //"</span></a>\n".
            //EB::createNew( $elShortcut ).
            //EB::delete( $fullAddress ).
            EB::link2epub( $fullAddress ).
            EB::printView( $fullAddress.".all" ).
            // EB::manageElements().
//             EB::mail ( $fullAddress, $eL->elementId.$eL->idx.': '.$eL->title ).
            EB::ticket ( $fullAddress ).
            EB::history( $eL->history, $fullAddress ).
            EB::link2Element( $fullAddress, $eL ).
            EB::edit( $fullAddress ).
            "</div>\n";
    else return "<div class=\"labsys_mop_elements_menu_".$elShortcut."\">\n".
                EB::link2epub( $fullAddress ).
                EB::printView( $fullAddress.".all" ).
//                 EB::mail ( $fullAddress, $eL->elementId.$eL->idx.': '.$eL->title ).
//                 EB::ticket ( $fullAddress ).
//                 EB::history( $eL->history, $fullAddress ).
                "</div>\n";
  }
  static function viewBottomLab( &$eL, $fullAddress ){
    return EM::viewBottomStandard( $eL, $fullAddress, "l" );
  }

  /**
  * The function returns the collection's top menu.
  * @param &$eC           The element collection.
  * @param $fullAddress   The element's full address.
  * @param $prevAddress   If present the back arrow is generated and links to this address.
  * @param $nextAddress   If present the forward arrow is generated and links to this address.
  * @param $topAddress    If present the up arrow is generated and links to this address.
  * @param $add2Style     If present this string is added to the style tag
  *                       (used to get a different div style for the bottom menu).
  * @return               The html code of the menu.
  */
  static function viewTopCollection( &$eC, $fullAddress, $prevAddress=false, $nextAddress=false, $topAddress=false, $add2Style="" ){
    global $usr, $lng;
     $navigation = (   $prevAddress ?                    EB::prev( $prevAddress )  : '' ).
             (   ($prevAddress || $nextAddress) ?  EB::toc( $topAddress )  : '' ).
             (   $nextAddress ?                    EB::next( $nextAddress )  : '' );
    if ( $usr->isOfKind( IS_CONTENT_EDITOR ) )
      $menu  = '<div class="labsys_mop_element_title_in_header">'.
			      "<span class=\"labsys_mop_element_description_in_header\">".
			      //             $fullAddress.
			      $eC->elementId.$eC->idx.
			      "</span>".PHP_EOL.
      		   '<a href="http'.( isset($_SERVER['HTTPS']) ? 's' : '').'://'.$_SERVER['SERVER_NAME'].$_SERVER['REQUEST_URI'].'#'.$fullAddress.'">'.$eC->title.'</a> | </div>'.
      		   ( $eC->visibleOnlyInCollection ?  "<span class=\"labsys_mop_element_note_in_header\">".$lng->get("VisibleOnlyColl")."</span> \n"  : '' ).
//                '<a href="http'.( isset($_SERVER['HTTPS']) ? 's' : '').'://'.$_SERVER['SERVER_NAME'].$_SERVER['REQUEST_URI'].'#'.$fullAddress.'">'.
//                "<span class=\"labsys_mop_element_description_in_header\">".
//                $fullAddress.
//                "</span></a>\n".
               EB::printView( $fullAddress.".all" ).
//                EB::mail ( $fullAddress, $eC->elementId.$eC->idx.': '.$eC->title ).
               EB::ticket ( $fullAddress ).
               EB::history( $eC->history, $fullAddress ).
               //EB::viewStructure( $fullAddress ).
               EB::link2Element( $fullAddress, $eC ).
               //EB::createNew( "c" ).
//                EB::delete( $fullAddress ).
               
    		   (strpos(strtolower($fullAddress), 'c') < strpos(strtolower($fullAddress), 'c'.$eC->idx) ? EB::delete( $fullAddress ):'').
               // EB::manageElements().
               EB::edit( $fullAddress );
    else $menu = EB::printView( $fullAddress.".all" );

    return "\n\n\t<!-- *************************start of c-element************************* -->\n\n".
           '<div class="labsys_mop_elements_menu_c'.$add2Style.'">'.PHP_EOL.
           $menu.
    	   $navigation.
    	   '</div>'.PHP_EOL;
  }

  /**
  * The function returns the collection's bottom menu.
  * @param &$eC           The element collection.
  * @param $fullAddress   The element's full address.
  * @param $prevAddress   If present the back arrow is generated and links to this address.
  * @param $nextAddress   If present the forward arrow is generated and links to this address.
  * @param $topAddress    If present the up arrow is generated and links to this address.
  * @return               The html code of the menu.
  */
  static function viewBottomCollection( &$eC, $fullAddress, $prevAddress=false, $nextAddress=false, $topAddress=false ){
    global $usr, $lng, $url;
    if ($usr->isOfKind( IS_CONTENT_EDITOR )){
      // only if navigation is present show bottom menu
        return  EM::viewTopCollection( $eC, $fullAddress, $prevAddress, $nextAddress, $topAddress, "_edit_bottom" ).
        		//($url->get('address') != $fullAddress ? '' :
                ( $url->available('insert') && $url->available('insertAt') && // we are inserting -> place anchor
                  $url->get('insertAt') == $eC->idx ? '<a name="atInsert"></a>' : '' ).
                '<div class="labsys_mop_elements_menu_c_edit_addElements">'.
                ' <a class="labsys_mop_button" href="'.$url->link2($_SERVER['PHP_SELF'].'?address='.$fullAddress.'&insertAt='.$eC->idx.'&insert=p').'#atInsert">&nbsp; *p &nbsp;</a>&nbsp; '.
                ' <a class="labsys_mop_button" href="'.$url->link2($_SERVER['PHP_SELF'].'?address='.$fullAddress.'&insertAt='.$eC->idx.'&insert=m').'#atInsert">&nbsp; *m &nbsp;</a>&nbsp;  '.
                ' <a class="labsys_mop_button" href="'.$url->link2($_SERVER['PHP_SELF'].'?address='.$fullAddress.'&insertAt='.$eC->idx.'&insert=i').'#atInsert">&nbsp; *i &nbsp;</a>&nbsp;  '.
                ' <a class="labsys_mop_button" href="'.$url->link2($_SERVER['PHP_SELF'].'?address='.$fullAddress.'&insertAt='.$eC->idx.'&insert=c').'#atInsert">&nbsp; *c &nbsp;</a>&nbsp;  '.
                ( $eC->elementId != 'C' ? '' : '<a class="labsys_mop_button" href="'.$url->link2($_SERVER['PHP_SELF'].'?address='.$url->get('address').'&insertAt='.$eC->idx).'&insert=C#atInsert">&nbsp; *C &nbsp;</a>&nbsp;  ' ).
                "\n".
                '</div>';
    }else if ($prevAddress || $nextAddress){
        return '<div class="labsys_mop_elements_menu_c">'.PHP_EOL.
	           (   $prevAddress ?                    EB::prev( $prevAddress )  : '' ).
               (   ($prevAddress || $nextAddress) ?  EB::toc( $topAddress )  : '' ).
               (   $nextAddress ?                    EB::next( $nextAddress )  : '' ).
               '</div>'.PHP_EOL.
               '<div class="labsys_mop_elements_menu_c">'.PHP_EOL.
	           EB::ticket ( $fullAddress ).
	           EB::history( $eC->history, $fullAddress ).
	           '</div>'.PHP_EOL.
               "\n\n\t<!-- *************************stop of c-element************************* -->\n\n";
    }
  }

  /**
  * The function builds the multiple choice element's answer history.
  * @param &$eMC          The element multiple choice.
  * @return               The html code of the menu.
  */
  static function viewMultipleChoiceAnswerHistory( &$eMC ){
    global $lng;

    return "<div class=\"labsys_mop_elements_menu_m_answer_history\">\n".
           EB::history( $lng->get("mNumber").$eMC->idx." ".$lng->get("answerHistory").":\n".$eMC->answerHistory, "m".$eMC->idx, true ).
           "</div>\n";
  }

  /**
  * The function builds the input element's answer history.
  * @param &$eMC          The element input.
  * @return               The html code of the menu.
  */
  static function viewInputAnswerHistory( &$eI ){
    global $lng;

    return "<div class=\"labsys_mop_elements_menu_i_answer_history\">\n".
           EB::history( $eI->answerHistory, 'i'.$eI->idx, true ).
           "</div>\n";
  }


// E D I T
  /**
  * This function returns the standard top edit menu of an element;
  * @param &$theEl        A Reference to the element the menu is for.
  * @param $fullAddress   The element's full address.
  * @param $elShortcut    The elements shortcut (p.e. p, m, i, ...).
  * @return               The html code of the menu.
  */
  static function editTopStandard( &$theEl, $fullAddress, $elShortcut ){
    global $lng, $url;
    return  "<div class=\"labsys_mop_elements_menu_".$elShortcut."\">\n".
            ( $theEl->visibleOnlyInCollection ?  "<span class=\"labsys_mop_element_note_in_header\">".$lng->get("VisibleOnlyColl")."</span> \n"  : '' ).
            "<span class=\"labsys_mop_element_description_in_header\">".$fullAddress."</span>\n".
//             EB::mail ( $fullAddress, $theEl->elementId.$theEl->idx.': '.$theEl->title ).
            EB::ticket ( $fullAddress ).
            EB::history( $theEl->history, $fullAddress ).
//             EB::delete( $fullAddress ).
            // EB::manageElements().
            EB::link2Element(  $fullAddress, $theEl ).
            EB::view(   $fullAddress ).
            //EB::createNew( $elShortcut ).
            "</div>\n";
  }
  /**
  * This function returns the standard bottom edit menu of an element;
  * @param &$theEl        A Reference to the element the menu is for.
  * @param $fullAddress   The element's full address.
  * @param $elShortcut    The elements shortcut (p.e. p, m, i, ...).
  * @return               The html code of the menu.
  */
  static function editBottomStandard( &$theEl, $fullAddress, $elShortcut ){
    return EM::editTopStandard( $theEl, $fullAddress, $elShortcut );
  }


  /**
  * The following functions return the element's top and bottom edit menus.
  * @param &$e?           A reference to the element the menu is for.
  * @param $fullAddress   The element's full address.
  * @return               The html code of the menu.
  */
  static function editTopPage( &$eP, $fullAddress ){
    return EM::editTopStandard( $eP, $fullAddress, "p" );
  }
  static function editBottomPage( &$eP, $fullAddress ){
    return EM::editBottomStandard( $eP, $fullAddress, "p" );
  }


  static function editTopMultipleChoice( &$eMC, $fullAddress ){
    return EM::editTopStandard( $eMC, $fullAddress, "m" );
  }
  static function editBottomMultipleChoice( &$eMC, $fullAddress ){
    return EM::editBottomStandard( $eMC, $fullAddress, "m" );
  }

  static function editTopInput( &$eI, $fullAddress ){
    return EM::editTopStandard( $eI, $fullAddress, "i" );
  }
  static function editBottomInput( &$eI, $fullAddress ){
    return EM::editBottomStandard( $eI, $fullAddress, "i" );
  }

  static function editTopSchedule( &$eS, $fullAddress ){
    return EM::editTopStandard( $eS, $fullAddress, "s" );
  }
  static function editBottomSchedule( &$eS, $fullAddress ){
    return EM::editBottomStandard( $eS, $fullAddress, "s" );
  }

  static function editTopLab( &$eL, $fullAddress ){
    return EM::editTopStandard( $eL, $fullAddress, "l" );
  }
  static function editBottomLab( &$eL, $fullAddress ){
    return EM::editBottomStandard( $eL, $fullAddress, "l" );
  }

  static function editTopCollection( &$eC, $fullAddress ){
    return EM::editBottomCollection( $eC, $fullAddress ); // only editors get top menus
  }
  /**
  * The collection adds the "view structure" button.
  */
  static function editBottomCollection( &$eC, $fullAddress ){
    global $lng, $url;
    return  "<div class=\"labsys_mop_elements_menu_c\">\n".
            ( $eC->visibleOnlyInCollection ?  "<span class=\"labsys_mop_element_note_in_header\">".$lng->get("VisibleOnlyColl")."</span> \n"  : '' ).
            "<span class=\"labsys_mop_element_description_in_header\">".$fullAddress."</span>\n".
            EB::viewStructure( $fullAddress ).
            EB::link2Element(  $fullAddress, $eC ).
//             EB::mail ( $fullAddress, $eC->elementId.$eC->idx.': '.$eC->title ).
            EB::ticket ( $fullAddress ).
            EB::history( $eC->history, $fullAddress ).
            EB::view(   $fullAddress ).
            //EB::createNew( "c" ).
            //EB::delete( $fullAddress ).
            // EB::manageElements().
            "</div>\n";
  }

// M A N A G E
  /**
  * Used on top of the "manage users" page.
  * @return               The html code of the menu.
  */
  static function userManageTop(){
    return EM::userManageBottom();
  }
  /**
  * Used on bottom of the "manage users" page.
  * @return     The html code of the menu.
  */
  static function userManageBottom(){
    return  "<div class=\"labsys_mop_elements_menu_u\">\n".
            EB::link2Url( "../pages/manageUser.php?&#x005F;_LINKQUERY__" ).
            "</div>\n";
  }
  /**
  * Used on bottom of the "manage users" page.
  * @param $eU  The element "user".
  * @return     The html code of the menu.
  */
  static function manageRowUser( &$eU ){ /* one row on the overview page */
    global $lng, $url;
    return  EB::history( $eC->history, "p1", true ).
            EB::view(           "C".$eC->idx ).
    		EB::link2Element(   "C".$eC->idx );
//             EB::delete(         "C".$eC->idx );
            //EB::viewStructure(  "C".$eC->idx ).
            // EB::manageElements().
  }

  /**
  * Used on top of the elements' manage page.
  * @param $id  The element's id (p.e. p, i, m, ...).
  * @return     The html code of the menu.
  */
  static function manageTop( $id ){
    return EM::manageBottom( $id );
  }
  /**
  * Used on top of the elements' manage page.
  * @param $id  The element's id (p.e. p, i, m, ...).
  * @return     The html code of the menu.
  */
  static function manageBottom( $id ){
    return  "<div class=\"labsys_mop_elements_menu_".$id{0}."\">\n".
            EB::link2Element( $id ).
            EB::createNew( $id ).
            // EB::manageElements().
            "</div>\n";
  }

  /**
  * This function returns the standard manage row of an element.
  * @param &$theEl        A Reference to the element the menu is for.
  * @param $elShortcut    The elements shortcut (p.e. p, m, i, ...).
  * @return               The html code of the menu.
  */
  static function manageRowStandard( &$theEl, $elShortcut ){
    global $lng, $url;
    return  EB::history( $theEl->history, $elShortcut.$theEl->idx ).
    		EB::link2Element(  $elShortcut.$theEl->idx, $theEl ).
            EB::view(   $elShortcut.$theEl->idx ).
    		EB::cloneMe(   $elShortcut.$theEl->idx );
//             EB::delete( $elShortcut.$theEl->idx ).
  }

  /**
  * The following functions return the elements' manage rows.
  * @param &$e?   A reference to the element the row is for.
  * @return       The html code of the menu.
  */
  static function manageRow_p( &$eP ){
    return EM::manageRowStandard( $eP, "p" );
  }

  static function manageRow_m( &$eMC ){
    return EM::manageRowStandard( $eMC, "m" );
  }

  static function manageRow_i( &$eI ){
    return EM::manageRowStandard( $eI, "i" );
  }

  static function manageRow_s( &$eS ){
    return EM::manageRowStandard( $eS, "s" );
  }

  static function manageRow_l( &$eL ){
    return EM::manageRowStandard( $eL, "l" );
  }

  /**
  * The collection adds the "view structure" button.
  */
  static function manageRow_C( &$eC ){ /* one row on the overview page */
    global $lng, $url;
    return  //EB::viewStructure(  "c".$eC->idx ).
            EB::history( $eC->history, "c".$eC->idx ).
            EB::link2Element(       "c".$eC->idx, $eC ).
            EB::view(           "c".$eC->idx ).
//             EB::delete(         "c".$eC->idx ).
            // EB::manageElements().
            EB::edit(           "c".$eC->idx ).
    		EB::cloneMe(        'c'.$eC->idx );
  }

// I N  C O L L E C T I O N  O V E R V I E W
  /**
  * This function returns the standard element row for the "what is inside the collection" page.
  * @param &$theEl        A Reference to the element the menu is for.
  * @param $fullAddress   The element's full address.
  * @param $elShortcut    The elements shortcut (p.e. p, m, i, ...).
  * @return               The html code of the menu.
  */
  static function collectionRowStandard( &$theEl, $fullAddress, $elShortcut ){
    global $lng, $url;
    return EB::history(     $theEl->history, $fullAddress ).
    	   EB::link2Element(  $fullAddress ).
//            EB::createNew(     $elShortcut  ).
//            EB::delete(        $fullAddress ).
           EB::view(          $fullAddress ).
           EB::edit(          $fullAddress ).
           EB::cloneMe(   $theEl->elementId.$theEl->idx );
  }

  /**
  * The following functions return the elements' collection overview rows.
  * @param &$e?   A reference to the element the row is for.
  * @param $fullAddress   The element's full address.
  * @return       The html code of the menu.
  */
  static function collectionRow_p( &$eP, $fullAddress ){
    return EM::collectionRowStandard( $eP, $fullAddress, "p" );
  }

  static function collectionRow_m( &$eMC, $fullAddress ){
    return EM::collectionRowStandard( $eMC, $fullAddress, "m" );
  }

  static function collectionRow_i( &$eI, $fullAddress ){
    return EM::collectionRowStandard( $eI, $fullAddress, "i" );
  }

  static function collectionRow_s( &$eS, $fullAddress ){
    return EM::collectionRowStandard( $eS, $fullAddress, "s" );
  }

  static function collectionRow_l( &$eL, $fullAddress ){
    return EM::collectionRowStandard( $eL, $fullAddress, "l" );
  }

  /**
  * The collection adds the "view structure" button.
  */
  static function collectionRow_c( &$eC, $fullAddress ){
    global $lng, $url;
    return //EB::viewStructure( $fullAddress ).
           EB::history( $eC->history, $fullAddress ).
           EB::link2Element( $fullAddress, $eC ).
           EB::createNew( "c" ).
//            EB::delete( $fullAddress ).
           EB::view( $fullAddress ).
           EB::edit( $fullAddress ).
           EB::cloneMe( 'c'.$eC->idx );
  }
}
?>
