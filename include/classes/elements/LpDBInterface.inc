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
    global $cfg,$url,$lng;
    $result = $this->myDBC->mkSelect( "*", $this->myTable, "idx='".$idx."'" );
    if ( !($data = mysql_fetch_array( $result )) ){
      //trigger_error( mysql_error(). "pge with idx not found: ".$idx, E_USER_WARNING );
      return false;
      exit;
    }else{
      // load old version...
      $myId = 'p';
      if ($cfg->doesExist('versioning') && $cfg->get('versioning')=='1' &&
      $url->available('historicVersion') &&
      strpos($url->get('historicVersion'), $myId.$idx.'-') !== FALSE){
        $historyIdx = substr($url->get('historicVersion'), strpos($url->get('historicVersion'),'-')+1);
        $backupIdx = $data['idx'];
        $result = $this->myDBC->mkSelect( "*", 'bak_'.$this->myTable, "idx='".$historyIdx."'" );
        if ( !($data = mysql_fetch_array( $result )) ){
          trigger_error( mysql_error(). $myId." backup with idx not found: ".$idx, E_USER_WARNING );
          return false;
          exit;
        }
        $data['idx'] = $backupIdx;
        $data['history'] = '<a href="'.$url->link2( $_SERVER['PHP_SELF'].'?address='.$url->get('address') ).'">'.$lng->get('lastChange').'</a>'."\n".$data['history'];
      }

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
  * !$createNew: Stores the data into the database. Position specified by $eP->idx.
  *  $createNew: Creates a new dataset and returns its index in the database.
  *
  * @param $eP  The LpElement to store.
  * @param $createNew  Shall a new element be created in the DB?
  * @return !$createNew: possible error
  *          $createNew: int The idx of the newly created element.
  */
  function setData( &$eP, $createNew = false ){
    global $usr, $cfg, $lng;
    tidyHTML( $eP->contents );
    $toSet =  "title='".$this->myDBC->escapeString($eP->title)."', ".
              "contents='".$this->myDBC->escapeString($eP->contents)."', ".
              "matching_menu='".$this->myDBC->escapeString($eP->matchingMenu)."', ".
              "visible_for=".$eP->visibleFor.", ".
              "visible_only_in_collection=".( $eP->visibleOnlyInCollection ?  1 :  0  ).', ';

    $element = $eP;
    $element->history = ( $element->history[0] == ']' ?
                          substr( $element->history, 1 ) : // import... -> no stamping please
                          date("Y-m-d H:i:s", time()).' '.$usr->foreName.' '.$usr->surName."\n".( $createNew ? '' : $element->history)
                        );
    // versioning
    if ($cfg->doesExist('versioning') && $cfg->get('versioning')=='1'){
      $versionedIdx = $this->myDBC->mkInsertReturnIDX( $toSet.
          "history='".$this->myDBC->escapeString( $element->history )."'",
          'bak_'.$this->myTable );
      $element->history = '@@'.$versionedIdx.' '.$element->history;
    }

    if ( $createNew )
      return $this->myDBC->mkInsertReturnIDX( $toSet.
                                              "history='".$this->myDBC->escapeString( $element->history )."'",
                                              $this->myTable );
    else
      return $this->myDBC->mkUpdIns( $toSet.
          'history=\''.$this->myDBC->escapeString($element->history)."'",
                                     $this->myTable,
                                     "idx=".$element->idx );
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
