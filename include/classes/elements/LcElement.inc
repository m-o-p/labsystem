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
  protected $containedElementIdArray; // The list of shortcuts to the other elements.

  protected $visibleBefore1stSched; // Will the element be visible before the first schedule?
  protected $visibleDuringSched;    // Will the element be visible while it is on schedule?
  protected $visibleAfter1stSched;  // Will the element be visible after the first schedule?

  protected $isInLab;               // Am I inside a lab? (important for view visibility)

  protected $allowedElementIDs;     // save() check from this array for valid ID letters

  protected $containedElementsRecursive;  // An array with pointers to the contained element instances
  protected $containedElements;     // An array with pointers to the directly (no children) contained element instances
  protected $paragraphArray;        //  Stores a mapping elementID->paragraph label (e.g. 1.3.2)

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
    $this->containedElementIdArray  = explode(' ', $contents);
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

    $this->containedElementsRecursive = Array();
    $this->containedElements          = Array();
    $this->paragraphArray             = Array();

    // Other element IDs are simply ignored
    $this->allowedElementIDs        = array('C', 'c', 'm', 'i', 'p', 's');

    // Those fields get serialized and de-serialized. Others not.
    $this->serializationArray       = array(  'idx',
                                              'title',
                                              'contents',
                                              'matchingMenu',
                                              'visibleBefore1stSched',
                                              'visibleDuringSched',
                                              'visibleAfter1stSched',
                                              'history' ); // see Element.inc for details

    // Was this element changed and should be saved?
    if (isset( $_POST["SAVEc"] ) && $_POST['IDX'] == $this->getIdx()){
      $this->save();
    }
  }

  // Getter
  function getContainedElementIdArray(){
    return $this->containedElementIdArray;
  }

  function getVisibleBefore1stSchedule(){
    return $this->visibleBefore1stSched;
  }

  function getVisibleDuringSchedule(){
    return $this->visibleDuringSched;
  }

  function getVisibleAfter1stSchedule(){
    return $this->visibleAfter1stSched;
  }

  function getThisInLab(){
    return $this->isInLab;
  }
  // Element-specific functions
  /**
   * Prepends the given $prependString to each key of the given $array and returns the resulting array.
   * @param unknown $array
   * @param unknown $prependString
   * @return unknown|multitype:unknown
   */
  private function prependToArrayKeys(&$array, $prependString){
    if(empty($prependString)){
      return $array;
    }else{
      // attach the prefix to each key
      $prefixedArrayKeys = Array();
      foreach($array as $key=>$value){
        $prefixedArrayKeys[$prependString.$key] = $value;
      }
      return $prefixedArrayKeys;
    }
  }

  /**
   * Returns an array with the direct children using the element id as key and the element as value.
   * This method loads all direct children by loading all elements of a certain type at once.
   * @param string $parentAddress If given, this String is prepended to each element ID in the array key.
   * @return Ambiguous <multitype:, unknown>|multitype:Ambigous <multitype:, unknown>
   */
  function getDirectChildrenArray($parentAddress=''){
    // If empty, fill $this->containedElements first.
    if (empty($this->containedElements)){
      // 1) Collect the participating element IDs and NUMs.
      $containedElementIds=Array();
      foreach ( $this->containedElementIdArray as $value ){
        if (empty($value)){
          continue;
        }
        $id = strtolower($value[0]);
        if ( !in_array($id, $containedElementIds)){
          // Create a new bin.
          $containedElementIds[$id]=Array();
        }
        // Add the element ID to the right bin.
        $num = substr( $value, 1 );
        if (!in_array($num, $containedElementIds[$id])){
          $containedElementIds[$id][] = $num;
        }
      }
      // Now $containedElements contains elementID=>{indices of contained elements}

      // 2) Load the elements from the DB.
      foreach ($containedElementIds as $key=>$value){
        $GLOBALS[ $key.'DBI' ]->loadIdcs2Array( $value, true, $this->isInLab );
        // The result is cached in the DB interfaces. Therefore we just drop it here.
      }

      // 3) Save the pointer to the elements in the $this->containedElements array.
      $skippedElements = false;
      foreach ($this->containedElementIdArray as $value){
        if (empty($value)){
          continue;
        }
        $id = $value[0];
        $num = substr($value, 1);
        $nextElement = $GLOBALS[ strToLower($id).'DBI' ]->getData2idx( $num, true, $this->isInLab );
        if ($nextElement !== false){ // ignore not found elements
          $this->containedElements[$value] = $nextElement;
        }else{
          trigger_error( 'auto removed '.$value.' from '.$this->getElementId().$this->getIdx(), E_USER_NOTICE );
          $this->prependToHistory( 'auto removed '.$value.' '.PHP_EOL);
          $skippedElements = true;
        }
      }

      // Update the $this->containedElementIdArray to reflect what is there and update the element in the DB.
      if ($skippedElements){
        $this->containedElementIdArray = array_keys($this->containedElements);
        //$GLOBALS['cDBI']->setData( $this );
      }
    }
    return $this->prependToArrayKeys($this->containedElements, $parentAddress);
  }

  /**
   * Returns an array with all children using the element id as key and the element as value.
   * @param string $parentAddress If given, this String is prepended to each element ID in the array key.
   * @return Ambiguous <multitype:, unknown>|multitype:Ambigous <multitype:, unknown>
   */
  function getAllChildrenArray($parentAddress=''){
    if (empty($this->containedElementsRecursive)){
      // fill $this->containedElementsRecursive;
      foreach($this->getDirectChildrenArray() as $key=>$value){
        $this->containedElementsRecursive[$key] = $value;
        if (strtolower($key[0]) == 'c'){
          $this->containedElementsRecursive = array_merge($this->containedElementsRecursive, $value->getAllChildrenArray($key.'.'));
        }
      }
    }
    return $this->prependToArrayKeys($this->containedElementsRecursive, $parentAddress);
  }

  /**
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
   * @return string  The structure (c expanded).
   */
  function getStructureString(){
    $structureString = '';
    foreach($this->getDirectChildrenArray() as $key=>$value){
      $structureString .= ($value->isVisible()?'':'*').$key;
      if(strtolower($key[0]) == 'c'){
        $structureString .= '('.$value->getStructureString().')';
      }
      $structureString .= ' ';
    }
    return trim($structureString);
  }

  /**
   * Returns the getStructure string with underlined groups.
   *
   * @return   Formatted getStructure()-string.
   */
  function getFormattedStructureString(){
    return '<div class="labsys_mop_c_structure">'.
        str_replace(  Array( '(', ')' ),
            Array( '<span class="labsys_mop_c_structure_substructure"><span class="labsys_mop_red">(</span>',
                   '<span class="labsys_mop_red">)</span></span>' ),
            $this->getStructureString()
        ).
        '</div>'.PHP_EOL;
  }

// The next functions are used for the element's view page. *************************************************

  /**
  * Shows a row for each element in the collection.
  * Uses $element->showPropertyRowColl() ( EM::collectionRow_[elementID] ).
  *
  * @param $fullAddress     The element's full address (p.e. C23.c42.p13)
  * @param $extParagraph    The element's paragraph number (p.e. 2.3). Will be added to the title if given.
  *
  * @return   The html code of the page.
  */
  function showStructure( $fullAddress, $extParagraph='', $recurse=true ){

    $myRetVal  = $this->showPropertyRowColl( $fullAddress, true, ( !empty($extParagraph) ?  $extParagraph.'. '  : '' ) ).
                 '<!-- start of group --><div class="labsys_mop_c_group">'.PHP_EOL;

    $resArray = $this->getDirectChildrenArray();
    if (empty($resArray)){
      $myRetVal .= "'()";
    }else{
      $elementCounter = 0;
      foreach( $this->getDirectChildrenArray() as $key=>$value ){
        $elementCounter++;
        $elementFullAddress = $fullAddress.'.'.$key;
        $elementParagraph = ( !empty($extParagraph) ?  $extParagraph.'.'.$elementCounter  : $elementCounter );
        if ($recurse && strtolower($key[0]) == 'c'){
          $myRetVal .= $value->showStructure( $elementFullAddress, $elementParagraph );
        } else {
          $myRetVal .= $value->showPropertyRowColl( $elementFullAddress, true, $elementParagraph.'. ' );
        }
      }
    }

    return $myRetVal.'<!-- stop of group --></div>'.PHP_EOL;
  }

  /**
  * Returns the HTML-code of the "not found visible"-notification.
  *
  * @param $fullAddress   The address of the element to be displayed.
  *
  * @return               HTML-representation of the not found notice.
  */
  private function notFoundVisibleNote( $fullAddress ){
    global $lng, $usr;
    if ($usr->isLoggedIn()){
      return '<div class="labsys_mop_note">'.PHP_EOL.
             '<b>'.$lng->get('elNotFoundVisible').'</b> ('.$fullAddress.'):'.PHP_EOL.
             $this->getFormattedStructureString().
             '</div>'.PHP_EOL;
    } else {
      return  '<div class="labsys_mop_note">'.PHP_EOL.
              ( $usr->isLoggedIn()?
                  '<b>'.$lng->get('elNotFoundVisible').'</b> ('.$fullAddress.'):'.PHP_EOL.$this->getFormattedStructureString()
                  :
                  $lng->get('TxtNotEnoughRights')
                  ).PHP_EOL.
              '</div>'.PHP_EOL;
    }
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
    $elementCounter = 0;
    $returnValue = '';
    foreach( $this->getDirectChildrenArray() as $key=>$value ){
      $elementCounter++;
      if (!$value->isVisible()){ // Skip non visible.
        continue;
      }
      $id = $key[0];

      // only enumerate if shown separately (otherwhise each m element etc. has a number...).
      if ( $allSeparate ||
           $id == "i" || $id == "m"// so the i and m elements get numbered
           ){
        $paragraph = $extParagraph.$elementCounter;
      } else {
        $paragraph = '';
      }

       $returnValue .= ( $allSeparate && ($id != 'C') ? '<!-- start of collection entity --><div class="labsys_mop_c_inner_coll_entity">'.PHP_EOL : '' ).
                       $value->show( $fullAddress.'.'.$key.( $id == 'C' ?  '.all'  : '' ), $paragraph ).
                       ( $allSeparate && ($id != 'C') ? '<!-- stop of collection entity --></div>'.PHP_EOL : '' );
    }
    return $returnValue;
  }

  /**
  * Produces the ePub version of this element.
  * Called by LlElement.inc
  *
  * @param $fullAddress       The element's full address (p.e. C42.l23)
  * @param $extParagraph      The element's paragraph number (p.e. 2.3). Will be added to the title if given.
  */
  function showEPub( $fullAddress, $extParagraph ){
    $epubExporter = LSE_Exporter::getInstance();
    $epubExporter->save('Lc', $fullAddress, '', array(
        'title'   => parseHTML( ( !empty($extParagraph) ?  $extParagraph.' '  : '' ).$this->title ),
    ));

    $this->showEpubifiedChildren( $fullAddress, $extParagraph, false );
  }

  function getePubContents( $paragraph='' ){
    $returnVal = '';
    foreach( $this->getDirectChildrenArray() as $key=>$value ){
      if (!$value->isVisible()){
        continue;
      }
      $id = $key[0];

      if (($id == 'p') || ($id == 'c') ){
        $returnVal .= $value->getePubContents( $paragraph );
      }
    }
    return $returnVal;
  }

  /**
  * Generates the ePub output for the children.
  *
  * @param $fullAddress       The element's full address (p.e. C42.l23)
  * @param $extParagraph      The element's paragraph number (p.e. 2.3). Will be added to the title if given.
  */
  function showEpubifiedChildren( $fullAddress, $extParagraph, $labelPelements=true ){
    $elementCounter = 0;
    foreach( $this->getDirectChildrenArray() as $key=>$value ){
      $elementCounter++;
      if (!$value->isVisible()){
        continue;
      }
      $id = $key[0];

      $paragraph = '';
        if ($labelPelements || $id != 'p' ){
          $paragraph = ( !empty($extParagraph) ?  $extParagraph.'.'  : '' ).$elementCounter;
        }

        // show the element
        if ( strtoupper($id) == 'C' ){
          $value->show( $fullAddress.'.'.$key.'.epub', $paragraph );
        }else{ // call directly the show ePub method of non collections
          $element->showEPub( $fullAddress.'.'.$key, $paragraph );
        }
    }
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
  function showAllElementsOfType( $typePrefix, $fullAddress, $extParagraph='', $omitStats=false ){
    global $lng, $cfg;
    $elementsToCollectPointsFrom = Array('m','i');
    $returnValue = '';

    // For some elements we want to have a credits statistic...
    if (!$omitStats &&
        ((is_array($typePrefix) && count(array_intersect($elementsToCollectPointsFrom, $typePrefix))>0) ||
         in_array($typePrefix, $elementsToCollectPointsFrom))){
      $returnValue .= '<div class="labsys_mop_question_stats_c_allType"><div class="labsys_mop_l_all_inputs_stats">'.PHP_EOL;
      $elementCounter = 0;
      $alreadyShown = Array(); // show each element only once
      $sumPossibleCredits = 0;
      $sumGivenCredits = 0;
      foreach( $this->getAllChildrenArray() as $key=>$value ){
        if (!$value->isVisible() || in_array($key, $alreadyShown)){ continue; }
        $id = $value->getElementId();
        $alreadyShown[] = $key;
        if (in_array($id, $elementsToCollectPointsFrom) &&
            ((is_array($typePrefix) && in_array($id, $typePrefix)) || $typePrefix == $id)
            ){
          $elementCounter++;
          $elementFullAddress = $fullAddress.'.'.$key;
          $returnValue .= '<a href="#'.$elementFullAddress.'">'.
              LlElement::percentStick($value->getGivenCredits(), $value->getPossibleCredits(), $value->getTitle()).
              '</a>';
          $sumGivenCredits += $value->getGivenCredits();
          $sumPossibleCredits += $value->getPossibleCredits();
        }
      }
      $percentage = ($sumPossibleCredits>0?$sumGivenCredits/$sumPossibleCredits:1);
      $percentBar = '<div class="labsys_mop_l_all_inputs_stats_percentBar">'.
                    LlElement::percentBar( $percentage, $cfg->get('percentageFailedLab') ).
                    '<span class="labsys_mop_grayed">'.
                    $sumGivenCredits."/ ".$sumPossibleCredits.
                    '</span></div>'.PHP_EOL;

      $returnValue .= '<div class="labsys_mop_l_all_inputs_stats_elementCount">'.
                      $elementCounter.' '.(is_array($typePrefix) ? '{'.implode(', ',array_intersect($elementsToCollectPointsFrom, $typePrefix)).'}' : $typePrefix ).
                      '</div>'.
                      $percentBar.
                      '</div>'.
                      '<div class="labsys_mop_l_all_inputs_stats_clearBoth"></div></div>'.PHP_EOL;
    }

    // Show the elements
    $elementCounter = 0;
    $alreadyShown = Array(); // show each element only once
    foreach( $this->getDirectChildrenArray() as $key=>$value ){
      $elementCounter++;
      if (!$value->isVisible() || in_array($key, $alreadyShown)){ continue; }
      $id = $key[0];
      $alreadyShown[] = $key;
      $elementFullAddress = $fullAddress.'.'.$key;
      $elementParagraph = ( !empty($extParagraph) ?  $extParagraph.'.'.$elementCounter  : $elementCounter );
      if (strToLower($id) == 'c' && ((is_array($typePrefix) && !in_array($id, $typePrefix)) || $typePrefix != $id)){
        // Show big C element titles as structure...
        $returnValue .= '<div class="labsys_mop_contentArea">'.PHP_EOL. // header inclusive link to the enclosing element (this)
                        '<div class="labsys_mop_h2"><a href="'.
                        str_replace( array('&#x005F;','&amp;'), array('_', '&'),
                                     EB::getLink2ElementURL( $elementFullAddress ) ).
                        '">'.
                        $elementParagraph.'. '.$value->title.'</a> '.
                        EB::link2Element( $elementFullAddress ).
                        '</div>'.PHP_EOL.
                        '</div>'.PHP_EOL;
      }
      if ((is_array($typePrefix) && in_array($id, $typePrefix)) || $typePrefix == $id){
        // This element should be shown...
        $returnValue .= $value->show( $elementFullAddress,
                      '<a href="'.str_replace( array('&#x005F;','&amp;'),
                                               array('_', '&'),
                                               EB::getLink2ElementURL( $elementFullAddress ) ).
                      '">'.$elementParagraph.'</a>' );
      }
      if (strtolower($id) == 'c'){ // dive in the c's
        $returnValue .= $value->showAllElementsOfType( $typePrefix, $elementFullAddress, $elementParagraph, TRUE );
      }
    }

    return $returnValue;
  }
// /helper - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  /**
   * Handles the inserts that happen when the edit buttons to insert new *p *i *m *c *C
   * are pressed.
   *
   */
  protected function handleInsertElement( $fullAddress ){
    global $usr,$url;
    if ($usr->isOfKind(IS_CONTENT_EDITOR) &&
    $url->available('insert') &&
    $url->available('insertAt') &&
    $url->get('insertAt') == $this->idx
    ){
      // inserts an element of the given type at the end of the collection
      $url->rem('insert');
      $url->rem('insertAt');
      $this->insertNew( $GLOBALS['url']->get('insert') );
    }
  }
  /**
  * Returns the element's HTML-representation in the way the VIEWER should see it.
  *
  * @param $fullAddress     The element's full address (p.e. C23.c42.p13)
  * @param $extParagraph    The element's paragraph number (p.e. 2.3). Will be added to the title if given.
  *
  * @return string          The html code of the element's view view.
  */
  function show( $fullAddress, $extParagraph='' ){
    global $lng, $usr,$url,$pge;

    if ( !$this->isVisible() ){ return $this->notFoundVisibleNote( $fullAddress ); exit; }

    $this->handleInsertElement( $fullAddress );

   // Address splitting.
    $parentAddr = $fullAddress;
    $this->splitAddress( $parentAddr, $myAddrArray );
                                     $me        = $myAddrArray[0];
    if ( count( $myAddrArray ) > 1 ) $first     = $myAddrArray[1]; else $first = '';
    if ( count( $myAddrArray ) > 1 ) $last      = $myAddrArray[ count( $myAddrArray )-1 ]; else $last = '';
    if ( count( $myAddrArray ) > 1 ) $scndLast  = $myAddrArray[ count( $myAddrArray )-2 ]; else $scndLast = '';

    $myRetVal = '';
   // structure
        if ( ($last == 'structure') && ($scndLast == $me) && $usr->isOfKind( IS_CONTENT_EDITOR ) )
                $myRetVal .= '<div class="labsys_mop_contentArea">'.
                            $this->getFormattedStructureString().
                            $this->showStructure( $parentAddr.$me, $extParagraph ).
                            '</div>';

   // ePub
    elseif ( $last == 'epub' ){
                                $this->showEPub( $parentAddr.$me, $extParagraph );
                                return;
                               }

   // all of Type $last
    elseif ($scndLast == 'allType' ){
                if (strpos($last, ',') !== FALSE){
                  $last = explode(',', $last);
                }
                $myRetVal .= $this->showAllElementsOfType( $last, $parentAddr.$me, $extParagraph );
    }

   // all on one ( Collection c catches all behind in the address C12.c13 = C12.c13.m23 ).
       else{
                if ( $fullAddress != $parentAddr.$me ){
                  $_GET['address'] = $parentAddr.$me; // Also reset the given address -> correct menu is "on".
                }
                $myRetVal .= $this->showAllOnOne( $parentAddr.$me, $extParagraph, ($me == 'C'.$this->idx) );
            }

    // Show check up button
    $myRetVal .=  LlElement::getMCcheckButton();

    return  EM::viewTopCollection( $this, $parentAddr.$me ).
            '<a name="'.$parentAddr.$me.'"></a>'.PHP_EOL.
            '<div class="labsys_mop_contentArea">'.PHP_EOL.
            '<div class="labsys_mop_h2">'.( !empty($extParagraph) ?  $extParagraph.' '  : '' ).$this->title.'</div>'.PHP_EOL.
            '</div>'.PHP_EOL.
            $myRetVal.
            EM::viewBottomCollection( $this, $parentAddr.$me );
  }

// The next functions are used for the element's edit page. *************************************************
/**
 * Returns the buttons to conveniently add elements.
 * @return string
 */
  function getCreateElementButtons($fullAddress){
    global $url;
    return ' <a class="labsys_mop_button" href="'.$url->link2($_SERVER['PHP_SELF'].'?address='.$fullAddress).'&insert=p&insertAt='.$this->getIdx().'">&nbsp; *p &nbsp;</a>&nbsp; '.
           ' <a class="labsys_mop_button" href="'.$url->link2($_SERVER['PHP_SELF'].'?address='.$fullAddress).'&insert=m&insertAt='.$this->getIdx().'">&nbsp; *m &nbsp;</a>&nbsp;  '.
           ' <a class="labsys_mop_button" href="'.$url->link2($_SERVER['PHP_SELF'].'?address='.$fullAddress).'&insert=i&insertAt='.$this->getIdx().'">&nbsp; *i &nbsp;</a>&nbsp;  '.
           ' <a class="labsys_mop_button" href="'.$url->link2($_SERVER['PHP_SELF'].'?address='.$fullAddress).'&insert=c&insertAt='.$this->getIdx().'">&nbsp; *c &nbsp;</a>'.
           ' <a class="labsys_mop_button" href="'.$url->link2($_SERVER['PHP_SELF'].'?address='.$fullAddress).'&insert=C&insertAt='.$this->getIdx().'">&nbsp; *C &nbsp;</a>';
  }
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

    $GLOBALS['Logger']->logToDatabase($fullAddress, logActions::cEditorOpen);

    if ( $url->available('insert') ){ $url->rem('insert'); $this->insertNew( $GLOBALS['url']->get('insert') ); } // inserts an element of the given type at the end of the collection

    $myRetVal =  '<FORM class="labsys_mop_std_form" NAME="collectionEdit" METHOD="POST" ACTION="#">'.PHP_EOL.
                 '<input type="hidden" name="SAVEc" value="SAVEc">'.PHP_EOL.
                 '<input type="hidden" name="IDX" value="'.$this->getIdx().'>'.PHP_EOL.

                // title
                 '<label for="title" class="labsys_mop_input_field_label_top">'.$lng->get('inputTitle').'</label>'.PHP_EOL.
                 '<input tabindex="'.$pge->nextTab++.'" type="text" id="title" name="TITLE" class="labsys_mop_input_fullwidth" maxlength="255" value="'.returnEditable( $this->title ).'" onchange="isDirty = true;">'.PHP_EOL.

                // contents
                 '<label for="contents" class="labsys_mop_input_field_label_top">'.$lng->get('inputContents').'</label>'.PHP_EOL.
                 '<textarea tabindex="'.$pge->nextTab++.'" id="contents" name="CONTENTS" class="labsys_mop_textarea" rows="'.($cfg->get('DefaultTextareaRows')/2).'" onchange="isDirty=true">'.PHP_EOL;
                // parse the entries
                 foreach ( $this->getDirectChildrenArray() as $key=>$value ){
                   $myRetVal .= $key."\t\t; ".returnEditable( $value->getTitle() ).PHP_EOL;
                 }
                 $myRetVal .= '</textarea>'.PHP_EOL.

                // save
                 '<input tabindex="'.$pge->nextTab++.'" type="submit" class="labsys_mop_button" value="'.$lng->get("save").'" onclick="isDirty=false">&nbsp;  '.
                 $this->getCreateElementButtons($fullAddress).PHP_EOL.
                 $this->showStructure($fullAddress, '', FALSE).
                // properties
                 '<fieldset><legend>'.$lng->get('properties').'</legend>'.PHP_EOL.
               // matching menu
                '<label for="mM" class="labsys_mop_input_field_label_top">'.$lng->get('inputMatchingMnu').'</label>'.
                '<input tabindex="'.$pge->nextTab++.'" type="text" id="mM" name="MATCHINGMENU" class="labsys_mop_input_fullwidth" maxlength="255" value="'.$this->matchingMenu.'" onchange="isDirty=true">'.PHP_EOL.

               // visibility
                '<table width="100%"><tr><td valign="top" width="33%">'.PHP_EOL.
                  '<fieldset><legend>'.$lng->get('visibility').'</legend>'.PHP_EOL.
                    '<div>'.
                      '<input tabindex="'.$pge->nextTab++.'" type="radio" id="visAlways" name="VISIBLE_ONLY_IN_COLLECTION" value="0"'.( !$this->visibleOnlyInCollection ?  ' checked="checked"' : '' ).' onchange="isDirty=true">'.
                      '<label for="visAlways" class="labsys_mop_input_field_label">'.$lng->get('visAlways').'</label>'.
                    '</div>'.PHP_EOL.
                    '<div>'.
                      '<input tabindex="'.$pge->nextTab++.'" type="radio"  id="visOnlyColl" name="VISIBLE_ONLY_IN_COLLECTION" value="1"'.( $this->visibleOnlyInCollection ?  ' checked="checked"' : '' ).' onchange="isDirty=true">'.
                      '<label for="visOnlyColl" class="labsys_mop_input_field_label">'.$lng->get('visOnlyInColl').'</label>'.
                    '</div>'.PHP_EOL.
                  '</fieldset>'.PHP_EOL.
                '</td><td valign="top" width="67%">'.PHP_EOL.
               // schedule
                '<fieldset><legend>'.$lng->get('schedule').'</legend>'.PHP_EOL.
                  '<div>'.
                    '<input tabindex="'.$pge->nextTab++.'" type="checkbox" id="visBef1stSched" name="VISIBLE_BEFORE_FIRST_SCHED" value="1"'.( $this->visibleBefore1stSched ?  ' checked="checked"' : '' ).' onchange="isDirty=true">'.
                    '<label for="visBef1stSched" class="labsys_mop_input_field_label">'.$lng->get('visibleBef1stSched').'</label>'.
                  '</div>'.PHP_EOL.
                  '<div>'.
                    '<input tabindex="'.$pge->nextTab++.'" type="checkbox" id="visDuringSched" name="VISIBLE_DURING_SCHED" value="1"'.( $this->visibleDuringSched ?  ' checked="checked"' : '' ).' onchange="isDirty=true">'.
                    '<label for="visDuringSched" class="labsys_mop_input_field_label">'.$lng->get('visibleDuringSched').'</label>'.
                  '</div>'.PHP_EOL.
                  '<div>'.
                    '<input tabindex="'.$pge->nextTab++.'" type="checkbox" id="visAft1stSched" name="VISIBLE_AFTER_FIRST_SCHED" value="1"'.( $this->visibleAfter1stSched ?  ' checked="checked"' : '' ).' onchange="isDirty=true">'.
                    '<label for="visAft1stSched" class="labsys_mop_input_field_label">'.$lng->get('visibleAftr1stSched').'</label>'.
                '</div>'.PHP_EOL.
              '</fieldset>'.PHP_EOL.

              '</td></tr></table>'.PHP_EOL.
              '<fieldset>'.
              '<input tabindex="'.$pge->nextTab++.'" type="checkbox" id="saveAsNew" name="SAVEASNEW" value="1"'.( ( $GLOBALS['url']->available('saveAsNew') || ($this->idx == 1) ) ?   ' checked="checked"'  : '' ).' onchange="isDirty=true">'.
              '<label for="saveAsNew" class="labsys_mop_input_field_label">'.$lng->get('saveAsNew').'</label>'.
              '</fieldset>'.PHP_EOL.

             // save
              '<input tabindex="'.$pge->nextTab++.'" type="submit" class="labsys_mop_button" value="'.$lng->get('save').'" onclick="isDirty=false">'.PHP_EOL.

              '</FORM>'.PHP_EOL.
              '<script language="JavaScript" type="text/javascript">
              <!--
              if (document.collectionEdit) document.collectionEdit.title.focus();
              //-->
              </script>';

    return  EM::editBottomCollection( $this, $fullAddress ).
            $myRetVal.
            EM::editBottomCollection( $this, $fullAddress );
  }

  // catch the save posting...
  /**
  * Used to save the edited element.
  *
  * This function gets STATICALLY called from ../php/executeCommandOnElement.php?address=[elementID].
  * It handles the $_POSTed data and then redirects to the $_POST['REDIRECTTO'] with the address of the element added.
  */
  /*static*/ function save(){
    global $cDBI, $url, $lng;
    if (!$usr->isOfKind(IS_CONTENT_EDITOR)){
      return "<div class=\"labsys_mop_note\">\n".$lng->get("TxtNotEnoughRights")."\n</div>";
      exit;
    }
    if ( !isset($_POST['IDX']) ||
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
    $lines = explode (PHP_EOL, $_POST['CONTENTS']);
    foreach( $lines as $value ){
      $i = 0;
      $elType = $value[$i++];
      if (!(in_array($elType, $this->allowedElementIDs))){
        continue; // invalid type -> ignore
      }
      $elNum = '';
      while( is_numeric( $value[$i] ) ){
        $elNum .= $value[$i++];
      }
      if ( $elNum == '' ){
        continue; // no number -> continue
      }
      if ( ( strtoupper($elType) == 'C') && ( $elNum == $this->idx ) ){
        $elNum = '1'; // no loops/ self-recursiveness -> put the placeholder in
      }
      array_push( $contents, $elType.$elNum );
    }

                                    $this->title                    = htmlentities($_POST['TITLE']);
                                    $this->containedElementIdArray  = $contents;
                                    $this->matchingMenu             = $_POST['MATCHINGMENU'];

                                    $this->visibleBefore1stSched    = ( isset( $_POST["VISIBLE_BEFORE_FIRST_SCHED"] ) && $_POST["VISIBLE_BEFORE_FIRST_SCHED"] == 1 );
                                    $this->visibleDuringSched       = ( isset( $_POST["VISIBLE_DURING_SCHED"] ) && $_POST["VISIBLE_DURING_SCHED"] == 1 );
                                    $this->visibleAfter1stSched     = ( isset( $_POST["VISIBLE_AFTER_FIRST_SCHED"] ) && $_POST["VISIBLE_AFTER_FIRST_SCHED"] == 1 );
                                    $this->visibleOnlyInCollection  = ( $_POST["VISIBLE_ONLY_IN_COLLECTION"] == "1" );

    // if $_POST["SAVEASNEW"] is set the element is saved as new element
    if ( isset($_POST["SAVEASNEW"]) && ($_POST["SAVEASNEW"] == "1") ){
                                      $this->setIdx($cDBI->setData( $this, true ));
                                      $GLOBALS['Logger']->logToDatabase('c'.$this->idx, logActions::cEditorSave);
                                     }
    else{ // change 4this and save it...
                                      $cDBI->setData( $this );
                                      $GLOBALS['Logger']->logToDatabase($url->get('address'), logActions::cEditorSave);
    }

    // some user information
    $SYSINFO .= $lng->get("DataHasBeenSaved")." ".$lng->get("cNumber")." ".$this->idx;
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
    return    "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" ".( !$disabled ?  "name=\"".$prefix."_VISIBLE_ONLY_IN_COLLECTION\" "  : '' )."value=\"0\"".( !$this->visibleOnlyInCollection ?  " checked=\"checked\"" : '' ).( $disabled ?  " disabled=\"disabled\"" : '' )." onchange='isDirty=true'>".
              infoArrow( $lng->get("visAlways"), $disabled ).
              "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" ".( !$disabled ?  "name=\"".$prefix."_VISIBLE_ONLY_IN_COLLECTION\" "  : '' )."value=\"1\"".( $this->visibleOnlyInCollection ?  " checked=\"checked\"" : '' ).( $disabled ?  " disabled=\"disabled\"" : '' )." onchange='isDirty=true'>".
              infoArrow( $lng->get("visOnlyInColl"), $disabled ).
            ( $disabled ?  "<span class=\"labsys_mop_grayed\">"  : '' ).
            " | ".
            ( $disabled ?  "</span>"  : '' ).
              "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"".$prefix."_VISIBLE_BEFORE_FIRST_SCHED\" value=\"1\"".( $this->visibleBefore1stSched ?  " checked=\"checked\"" : '' ).( $disabled ?  " disabled=\"disabled\"" : '' )." onchange='isDirty=true'>".
              infoArrow( $lng->get("visibleBef1stSched"), $disabled ).
              "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"".$prefix."_VISIBLE_DURING_SCHED\" value=\"1\"".( $this->visibleDuringSched ?  " checked=\"checked\"" : '' ).( $disabled ?  " disabled=\"disabled\"" : '' )." onchange='isDirty=true'>".
              infoArrow( $lng->get("visibleDuringSched"), $disabled ).
              "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"".$prefix."_VISIBLE_AFTER_FIRST_SCHED\" value=\"1\"".( $this->visibleAfter1stSched ?  " checked=\"checked\"" : '' ).( $disabled ?  " disabled=\"disabled\"" : '' )." onchange='isDirty=true'>".
              infoArrow( $lng->get("visibleAftr1stSched"), $disabled ).
            " ".
              $this->title;
  }


// The next functions are additional functions of the element. *********************************************


  function getChildrenArray( $recurse=FALSE, $parentAddress='' ){
    if ($recurse && !empty($this->containedElements)){
      return $this->containedElements;
    }
    if (!$recurse && !empty($this->directlyContainedElements)){
      return $this->directlyContainedElements;
    }

    // Elements are not loaded... do so!
    $tokens = $this->getTokens();

    $containedElementIds=Array();
    // collect the participating element IDs
    foreach ( $tokens as $value ){
      $id = $value{0};
      if ( !in_array($id, $containedElementIds)){
        array_push($containedElementIds, $id);
      }
    }

    // Make the required element IDs to the keys of the array.
    $containedElementIds=array_flip($containedElementIds);
    // Attach an array to each element Id for collecting the indices of the elements of the given type
    foreach ($containedElementIds as $key=>$value){
      $containedElementIds[$key] = Array();
    }

    // Each key (element Id) gets filled with an index now.
    foreach ( $tokens as $value ){
      $id = $value{0};
      $num = substr( $value, 1 );
      if (!in_array($num, $containedElementIds[$id])){
        array_push($containedElementIds[$id], $num);
      }
    }
    // Now $containedElements contains elementID=>{indices of contained elements}

    // Now load the elements:
    foreach ($containedElementIds as $key=>$value){
      $GLOBALS[ strtolower($key).'DBI' ]->loadIdcs2Array( $value, true, $this->isInLab );
      // Just withdraw the result as it is cached anyways
    }

    // Now put the elements into the array
    // for the paragraph numering we use this counter
    $elementNumber = 0;
    foreach($tokens as $nextElementId){
      $id = $nextElementId{0};
      $num = substr( $nextElementId, 1 );
      $nextElement = $GLOBALS[ strtolower($id).'DBI' ]->getData2idx( $num, true, $this->isInLab );
      $this->directlyContainedElements[$parentAddress.$nextElementId] = $nextElement;
      $this->paragraphArray[$parentAddress.$nextElementId] = $parentParagraph.++$elementNumber;
      if ($recurse){
        $this->containedElements[$parentAddress.$nextElementId] = $nextElement;
      }
      if (strtolower($id)=='c' && $recurse){
        $this->containedElements = array_merge($this->containedElements, $nextElement->getChildrenArray($recurse,$parentAddress.$nextElementId.'.', $parentParagraph.$elementNumber.'.'));
      }
    }

    // Now create an array with all the elements
    return ($recurse?$this->containedElements:$this->directlyContainedElements);
  }

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

    $structure = '';

    $tokens = $this->getTokens();
    $containedElements=Array();
    // collect IDs
    foreach ( $tokens as $value ){
      $id = $value{0};
      if ( !in_array($id, $containedElements)){
        array_push($containedElements, $id);
      }
    }
    $containedElements=array_flip($containedElements);
    // Now the keys are all required element IDs.
    foreach ($containedElements as $key=>$value){
      $containedElements[$key] = Array();
    }
    // The values get filled with the indices now.

    foreach ( $tokens as $value ){
      $id = $value{0};
      $num = substr( $value, 1 );
      if (!in_array($num, $containedElements[$id])){
        array_push($containedElements[$id], $num);
      }
    }
    // Now $containedElements contains elementID=>{indices of contained elements}

    // Now load the elements:
    foreach ($containedElements as $key=>$value){
      $containedElements[$key] = $GLOBALS[ strtolower($key).'DBI' ]->loadIdcs2Array( $value, true, $this->isInLab );
      // Just withdraw the result as it is cached anyways
    }
    // Now the elements are loaded as object instances into the array.

    // After the caching we can continue with the regular processing...
    foreach ( $tokens as $value ){
      $id = $value{0};
      $num = substr( $value, 1 );
      $element = $GLOBALS[ strtolower($id)."DBI" ]->getData2idx( $num, true, $this->isInLab );

     // element invisible?
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

    return trim( $structure ); // cut last " "
  }

  /**
  * Queries the elements in the collection until it finds at least one visible element.
  * So not the whole structure must be initialized since the first element can be visible and then the function ends.
  * THIS FUNCTION IS INDEPENDENT OF A FILLED STRUCTURE (used in getStructure)
  */
  function isAtLeastOneElementVisibleForMe(){
    foreach( $this->getDirectChildrenArray() as $key=>$value ){
      if ($value->isVisible()){ // Skip non visible.
        return TRUE;
      }
    }
    return FALSE;
  }

  /**
  * Returns an array of all visible elements in this collection.
  *
  * @return             An array of all visible elements.
  */
  function getMyVisibleElements( $openC=false, $openc=false ){
    $retArray = Array();
    foreach( ($openC ? $this->getAllChildrenArray() : $this->getDirectChildrenArray() ) as $key=>$value ){
      if (!$value->isVisible()){ // Skip non visible.
        continue;
      }else{
        $retArray[] = $key;
      }
    }
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
    if (empty($this->paragraphArray)){
      // fill $this->paragraphArray
      $this->paragraphArray = Array();
      $paragraphCounter = Array();
      foreach($this->getAllChildrenArray() as $key=>$value){
        $level = substr_count($key, '.');
        $currentParagraphCount = '';
        if (!array_key_exists($level, $paragraphCounter)){
          $paragraphCounter[$level] = 1;
        }else{
          $paragraphCounter[$level] = ++$paragraphCounter[$level];
          for ($i=$level+1; $i<count($paragraphCounter); $i++){
            $paragraphCounter[$i] = 1;
          }
        }
        for ($i=0; $i<=$level; $i++){
          $currentParagraphCount .= $paragraphCounter[$i].'.';
        }
        $this->paragraphArray[$key] = $currentParagraphCount;
      }
    }

    return $this->paragraphArray;
  }

  /**
  * Queries for the paragraph number belonging to the relative address.
  *
  * @param $relativeAddress   A relative address of an element. (p.e. c43.m2 if we are C23 and the fulladdress is C23.c43.m2).
  *
  * @return                   The paragraph number of the element (p.e. 1.2.3).
  */
  function getParagraph( $relativeAddress ){
    if ( empty($relativeAddress) ) return '';

    $paragraphArray = $this->getParagraphArray();

    if ( array_key_exists( $relativeAddress, $paragraphArray ) ){
      return $paragraphArray[ $relativeAddress ];
    } else {
      return " (C".$this->idx.": no ".$relativeAddress.") ";
    }
  }

  /**
  * Returns an array of all $thus->contents tokens.
  *
  * @return      An array of all $thus->contents tokens or Array() if empty.
  */
  function getTokens(){
    return $this->getContainedElementIdArray();
  }
  function extMap2( $functionCode, &$parameter, $onlyId=false ){
    $myVisEl = $this->getMyVisibleElements( $onlyId!=false, $onlyId!=false ); // if onlyID, we want to walk through all elements here!
    foreach( $myVisEl as $value ){
      // load element
      $id = $value{0}; $num = substr( $value, 1 );
      if ($onlyId && $id!=$onlyId){
        continue;
      }
      require( "../php/getDBIbyID.inc" );
      $element = $DBI->getData2idx( $num, true, $this->isInLab );
      // evaluate $functionCode
      eval( $functionCode );
    }
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
  function extMap( $functionCode, &$parameter, $onlyId=false ){
    foreach( $this->getDirectChildrenArray() as $key=>$value ){
      $elementCounter++;
      if (!$value->isVisible() || ($onlyId && $onlyId!=$value->getElementId())){ // Skip non visible.
        continue;
      }
      $element = $value;
      eval( $functionCode );
    }
  }

  /**
  * Inserts a new element of type $elementID using the [ID]1 element as template.
  *
  * @param $elementID     ID of the element to be created (e{p,m,i,c}).
  * @return The index of the new element.
  */
  function insertNew( $elementID ){
    global $cDBI;
    $newIdx = createNew( $elementID );
    $this->containedElementIdArray[] = $elementID.$newIdx;
    $cDBI->setData( $this );
    return $newIdx;
  }

}
?>