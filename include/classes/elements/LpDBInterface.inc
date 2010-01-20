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
* This module is the database interface for the p element.
* It connects to the data database's table "pages".
*
* @module     ../include/elements/LpDBInterface.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
if ( isset( $GLOBALS["pDBI"] ) ) return; // require_once doesn't work well

require_once( INCLUDE_DIR."/classes/Ddbc.inc" );
require_once( INCLUDE_DIR."/classes/elements/LpElement.inc" );

class LpDBInterface {
  var $myDBC;
  var $myTable;
  
  function LpDBInterface(){
    $this->myDBC   = new Ddbc();
    $this->myTable = "pages";
  }
  
  /**
  * Returns the LpElement specified by $idx.
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
  * @return LpElement
  */
  function getData2idx( $idx, $isInColl=false ){
    global $cfg;
    $result = $this->myDBC->mkSelect( "*", $this->myTable, "idx='".$idx."'" );
    if ( !($data = mysql_fetch_array( $result )) ){
      //trigger_error( mysql_error(). "pge with idx not found: ".$idx, E_USER_WARNING );
      return false;
      exit;
    }else
      return new LpElement( $data['idx'],                                 // $idx
                            $this->myDBC->unEscapeString($data['title']),                 // $title
                            $this->myDBC->unEscapeString($data['contents']),              // $contents
                            $this->myDBC->unEscapeString($data['matching_menu']),         // $matchingMenu
                            $data['visible_for'],                         // $visibleFor
                            ($data['visible_only_in_collection'] == "1"), // $visibleOnlyInCollection
                            $data['history'],                             // $history
                            $isInColl                                     // $isInColl
                          );
  }

  /**
  * For the menu only a little amount of data is needed so here only that stuff is loaded.
  *
  * Returns a LpElement to the specified $idx that is only PARTIALLY FILLED.
  * Menu entries appear only if the element is visible. The element is filled so
  * that it can tell whether it is visible and has all relevant properties to return
  * its menu.
  *
  * @param $idx               The element's index (also idx in the database).
  * @param $isInColl          The element might be restricted to be visible only inside a collection.
  *                           Since this is kind of the elements constructor this argument is just
  *                           passed to the real constructor.
  *
  * @return LpElement         Only partially filled with data!
  */  
  function getMenuData2idx( $idx, $isInColl=false ){
    global $cfg;
    $result = $this->myDBC->mkSelect( "idx, title, matching_menu, visible_for, visible_only_in_collection", $this->myTable, "idx='".$idx."'" );
    if ( !($data = mysql_fetch_array( $result )) ){
      //trigger_error( mysql_error(). "pge with idx not found: ".$idx, E_USER_WARNING );
      return false;
      exit;
    }else
      return new LpElement( $data['idx'],                                 // $idx
                            $this->myDBC->unEscapeString($data['title']),                 // $title
                            "",                                           // $contents
                            $this->myDBC->unEscapeString($data['matching_menu']),         // $matchingMenu
                            $data['visible_for'],                         // $visibleFor
                            ($data['visible_only_in_collection'] == "1"), // $visibleOnlyInCollection
                            false, /* false indicates "only menu data" */ // $history
                            $isInColl                                     // $isInColl
                          );
  }
  
  /**
  * Stores the data into the database. Position specified by $eI->idx.
  *
  * @param $eP  The LpElement to store.
  */
  function setData( &$eP ){
    global $usr, $cfg;
    $toSet =  "title='".$this->myDBC->escapeString($eP->title)."', ".
              "contents='".$this->myDBC->escapeString($eP->contents)."', ".
              "matching_menu='".$this->myDBC->escapeString($eP->matchingMenu)."', ".
              "visible_for=".$eP->visibleFor.", ".
              "visible_only_in_collection=";
    if ( $eP->visibleOnlyInCollection ) $toSet .= "1"; else $toSet .= "0";
    $toSet .= ", history=CONCAT( NOW(), ': ".$usr->foreName." ".$usr->surName."\\n', ".$this->myTable.".history )";
    
    $this->myDBC->mkUpdIns( $toSet, $this->myTable, "idx=".$eP->idx );
  }

  /**
  * Creates a new dataset and returns its index in the database.
  *
  * @param $eP  The LpElement to store.
  * @return int The idx of the newly created element.
  */
  function newPage( &$eP ){
    global $usr, $lng;
    
    $identifier=md5( time() ); // used to retieve the ne entry
    $toSet =  "title='".$this->myDBC->escapeString($eP->title)."', ".
              "contents='".$this->myDBC->escapeString($eP->contents)."', ".
              "matching_menu='".$this->myDBC->escapeString($eP->matchingMenu)."', ".
              "visible_for=".$eP->visibleFor.", ".
              "history='".$identifier."', ".
              "visible_only_in_collection=";
    if ( $eP->visibleOnlyInCollection ) $toSet .= "1"; else $toSet .= "0";

    $this->myDBC->mkInsert( $toSet, $this->myTable );

    // reload dataset to get the given idx
    $result = $this->myDBC->mkSelect( "idx", $this->myTable, "history='".$identifier."'" );
    if ( !($data = mysql_fetch_array( $result )) ) trigger_error( mysql_error(). "Pge with history=".$identifier." not found!", E_USER_ERROR );
    else $idx = $data['idx'];

    // update history
    if ( $eP->history != "" ) $toSet = "history='".$this->myDBC->escapeString($eP->history)."'";
    else $toSet = "history=CONCAT( NOW(), ': ".$usr->foreName." ".$usr->surName.": ".$lng->get("dataSetCreated")."' )";
    $this->myDBC->mkUpdIns( $toSet, $this->myTable, "idx='".$idx."'" );

    return $idx;
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
   * Returns the amount of returned datasets from the query;
   * 
   * @return  amount of returned datasets from the query
   */
  function allSize(){
    return $this->myDBC->datasetsIn( $this->queryResult );
  }
  
  /**
  * Returns the next element from the getAllData() query or false (no more results).
  *
  * @return LpElement
  * @return FALSE     If no more datasets are available.
  */  
  function getNextData(){
    if ( $data = mysql_fetch_array($this->queryResult) )
      return new LpElement( $data['idx'],                                 // $idx
                            $this->myDBC->unEscapeString($data['title']),                 // $title
                            $this->myDBC->unEscapeString($data['contents']),              // $contents
                            $this->myDBC->unEscapeString($data['matching_menu']),         // $matchingMenu
                            $data['visible_for'],                         // $visibleFor
                            ($data['visible_only_in_collection'] == "1"), // $visibleOnlyInCollection
                            $data['history'],                             // $history
                            false                                         // $isInColl
                          );
    return false;
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
    return Array( $lng->get("title")            => "title",
                  $lng->get("rights")           => "visible_for",
                  $lng->get("onlyInCollection") => "visible_only_in_collection",
                  $lng->get("index")            => "idx"
                );
  }
}
$GLOBALS["pDBI"] = new LpDBInterface();
