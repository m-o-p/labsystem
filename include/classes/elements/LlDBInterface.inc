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
* This module is the database interface for the l element.
* It connects to the data database's table "labs".
*
* @module     ../include/elements/LlDBInterface.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.5
*/
if ( isset( $GLOBALS["lDBI"] ) ) return; // require_once doesn't work well

require_once( INCLUDE_DIR."/classes/Ddbc.inc" );
require( INCLUDE_DIR."/classes/elements/LlElement.inc" );

class LlDBInterface {
  var $myDBC;
  var $myTable;
  
  function LlDBInterface(){
    $this->myDBC   = new Ddbc();
    $this->myTable = "labs";
  }
  
  /**
  * Returns the LlElement specified by $idx.
  * Calling this method is the normal way to get an instance of the fitting element.
  * This might seem strange but since constructors can't be overloaded it was the better
  * (more flexible) alternative to let the constructor create the object by values and not
  * just by an index like it is done here.
  *
  * @param $idx               The element's index (also idx in the database).
  * @param $isInColl          The element might be restricted to be visible only inside a collection.
  *                           Since this is kind of the elements constructor this argument is just
  *                           passed to the real constructor.
  *
  * @return LlElement
  */
  function getData2idx( $idx, $isInColl=false ){
    global $cfg;
    $result = $this->myDBC->mkSelect( "*", $this->myTable, "idx='".$idx."'" );
    if ( !($data = mysql_fetch_array( $result )) ){ 
      // trigger_error( mysql_error(). "lab with idx not found: ".$idx, E_USER_WARNING );
      return false;
      exit;
    }else
      return new LlElement( $data['idx'],                                   // $idx
                            // create a new ID if there is none (makes sure it is always shown and saved later...)
                            ( !isset($data['uniqueID']) || (($data['uniqueID'] == '') && $data['idx'] != 1) ) ?
                              urlencode($_SERVER['SERVER_NAME'].'-'.$cfg->get('SystemTitle').'-course_'.md5( time() ))
                              : $this->myDBC->unEscapeString($data['uniqueID']),                  // $uniqueID
                            $this->myDBC->unEscapeString($data['title']),                   // $title
                            ( isset( $data['authors'] ) ? // helpful for compatibility issues as the field is new
                              $this->myDBC->unEscapeString($data['authors'])
                              : '' ),     // $authors
                            $this->myDBC->unEscapeString($data['comment']),                 // $comment
                            $data['prelab_collection_idx'],                 // $prelabCollectionIdx
                            $data['lab_collection_idx'],                    // $labCollectionIdx
                            $this->myDBC->unEscapeString($data['matching_menu']),           // $matchingMenu
                            ($data['visible_before_first_sched'] == "1"),   // $visibleBefore1stSched
                            ($data['visible_during_sched'] == "1"),         // $visibleDuringSched
                            ($data['visible_after_first_sched'] == "1"),    // $visibleAfter1stSched
                            ($data['visible_only_in_collection'] == "1"),   // $visibleOnlyInCollection
                            $data['history'],                               // $history
                            $isInColl                                       // $isInColl
                          );
  }
  
  /**
  * For the menu only a little amount of data is needed so here only that stuff is loaded.
  *
  * Returns a LlElement to the specified $idx that is only PARTIALLY FILLED.
  * Menu entries appear only if the element is visible. The element is filled so
  * that it can tell whether it is visible and has all relevant properties to return
  * its menu.
  *
  * @param $idx               The element's index (also idx in the database).
  * @param $isInColl          The element might be restricted to be visible only inside a collection.
  *                           Since this is kind of the elements constructor this argument is just
  *                           passed to the real constructor.
  *
  * @return LlElement
  */
  function getMenuData2idx( $idx, $isInColl=false ){
    global $cfg;
    $result = $this->myDBC->mkSelect( "idx, title, prelab_collection_idx, lab_collection_idx, matching_menu, visible_before_first_sched, visible_during_sched, visible_after_first_sched, visible_only_in_collection", $this->myTable, "idx='".$idx."'" );
    if ( !($data = mysql_fetch_array( $result )) ){ 
      //trigger_error( mysql_error(). "lab with idx not found: ".$idx, E_USER_WARNING );
      return false;
      exit;
    }else
      return new LlElement( $data['idx'],                                   // $idx
                            '',//$uniqueID
                            $this->myDBC->unEscapeString($data['title']),                   // $title
                            '', // $authors
                            '',                 // $comment
                            $data['prelab_collection_idx'],                 // $prelabCollectionIdx
                            $data['lab_collection_idx'],                    // $labCollectionIdx
                            $this->myDBC->unEscapeString($data['matching_menu']),           // $matchingMenu
                            ($data['visible_before_first_sched'] == "1"),   // $visibleBefore1stSched
                            ($data['visible_during_sched'] == "1"),         // $visibleDuringSched
                            ($data['visible_after_first_sched'] == "1"),    // $visibleAfter1stSched
                            ($data['visible_only_in_collection'] == "1"),   // $visibleOnlyInCollection
                            false,  /* false indicates "only menu data" */  // $history
                            $isInColl                                       // $isInColl
                          );
  }
  
  /**
  * !$createNew: Stores the data into the database. Position specified by $eL->idx.
  *  $createNew: Creates a new dataset and returns its index in the database.
  *
  * @param $eL  The LlElement to store.
  * @param $createNew  Shall a new element be created in the DB?
  * @return !$createNew: possible error 
  *          $createNew: int The idx of the newly created element.
  */
  function setData( &$eL, $createNew = false ){
    global $usr, $cfg, $lng;
    tidyHTML( $eL->comment );
    $toSet =  "uniqueID='".$this->myDBC->escapeString($eL->uniqueID)."', ".
              "title='".$this->myDBC->escapeString($eL->title)."', ".
              "authors='".$this->myDBC->escapeString($eL->authors)."', ".
              "comment='".$this->myDBC->escapeString($eL->comment)."', ".
              "prelab_collection_idx='".$this->myDBC->escapeString($eL->prelabCollectionIdx)."', ".
              "lab_collection_idx='".$this->myDBC->escapeString($eL->labCollectionIdx)."', ".
              "matching_menu='".$this->myDBC->escapeString($eL->matchingMenu)."', ".
              "visible_only_in_collection=".( $eL->visibleOnlyInCollection ?  "1" :  "0"  ).
              ", visible_before_first_sched=".( $eL->visibleBefore1stSched ?  "1" :  "0"  ).
              ", visible_during_sched=".( $eL->visibleDuringSched ?  "1" :  "0"  ).
              ", visible_after_first_sched=".( $eL->visibleAfter1stSched ?  "1" :  "0"  ).', ';

    if ( $createNew )
      return $this->myDBC->mkInsertReturnIDX( $toSet.
                                              "history=CONCAT( NOW(), ': ".$usr->foreName." ".$usr->surName.' '.$lng->get("dataSetCreated")."\\n' )", 
                                              $this->myTable );
    else
      return $this->myDBC->mkUpdIns( $toSet.
                                     "history=CONCAT( NOW(), ': ".$usr->foreName." ".$usr->surName."\\n".$eL->history."' )", 
                                     $this->myTable, 
                                     "idx=".$eL->idx );
  }

  /**
  * Deletes the element with $idx from the database.
  *
  * @param $idx   The element's index (also idx in the database).
  * @return BOOL  The return value of the query.
  */
  function deleteData( $idx ){
    return $this->myDBC->mkDelete( $this->myTable, "idx='".$idx."'" );
  }
  
  
  var $queryResult;
  /**
  * Queries for all elements. P.e. used at the manage page.
  *
  * @param $orderBy    Can be any of the table's keys. The result will be ordered by this key.
  * @param $asc        Should the datasets be in ascending order?
  */
  function getAllData( $orderBy="", $asc=true ){
    if ( $orderBy != "" ) if ( $asc ) $orderBy .= " ASC"; else $orderBy .= " DESC";
    $this->queryResult = $this->myDBC->mkSelect( "*", $this->myTable, "", $orderBy );
  }
  
  /**
  * Returns the next element from the getAllData() query or false (no more results).
  *
  * @return LiElement
  * @return FALSE     If no more datasets are available.
  */
  function getNextData(){
    global $cfg;
    if ($data = mysql_fetch_array($this->queryResult))
      return new LlElement( $data['idx'],                                   // $idx
                            ( !isset($data['uniqueID']) || ($data['uniqueID'] == '') ) ?
                              urlencode($_SERVER['SERVER_NAME'].'-'.$cfg->get('SystemTitle').'-course_'.md5( time() ))
                              : $this->myDBC->unEscapeString($data['uniqueID']),                   // $uniqueID
                            $this->myDBC->unEscapeString($data['title']),                   // $title
                            ( isset( $data['authors'] ) ? // helpful for compatibility issues as the field is new
                              $this->myDBC->unEscapeString($data['authors'])
                              : '' ),                   // $authors
                            $this->myDBC->unEscapeString($data['comment']),                 // $comment
                            $data['prelab_collection_idx'],                 // $prelabCollectionIdx
                            $data['lab_collection_idx'],                    // $labCollectionIdx
                            $this->myDBC->unEscapeString($data['matching_menu']),           // $matchingMenu
                            ($data['visible_before_first_sched'] == "1"),   // $visibleBefore1stSched
                            ($data['visible_during_sched'] == "1"),         // $visibleDuringSched
                            ($data['visible_after_first_sched'] == "1"),    // $visibleAfter1stSched
                            ($data['visible_only_in_collection'] == "1"),   // $visibleOnlyInCollection
                            $data['history'],                               // $history
                            false                                           // $isInColl
                          );
    return false;
  }
  
  /**
   * Returns the amount of returned datasets from the query;
   * 
   * @return  amount of returned datasets from the query
   */
  function allSize(){
    return $this->myDBC->datasetsIn( $this->queryResult );
  }
  
  /**
  * Returns an Array( description => database key, ... ).
  * While database key is one of the keys the data can be sorted by getAllData( $orderBy ... 
  * The first entry is the default soring key.
  * This is used for the order by selector on top of the manage page.
  *
  * @return array An array of pairs ( description, database key ).
  */
  function sortableByArray(){
    global $lng;
    return Array( $lng->get("index")            => "idx",
                  $lng->get("title")            => "title",
                  $lng->get("onlyInCollection") => "visible_only_in_collection"
                 );
  }
}
$GLOBALS["lDBI"] = new LlDBInterface();
