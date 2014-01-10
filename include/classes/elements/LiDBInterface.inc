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
* This module is the database interface for the i element.
* It connects to the data database's table "inputs".
*
* @module     ../include/elements/LiDBInterface.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
if ( isset( $GLOBALS["iDBI"] ) ) return; // require_once doesn't work well

require_once( INCLUDE_DIR."/classes/Ddbc.inc" );
require( INCLUDE_DIR."/classes/elements/LiElement.inc" );

class LiDBInterface extends Ddbc{
  var $myDBC;
  var $myTable;

  function LiDBInterface(){
    $this->myDBC   = new Ddbc();
    $this->myTable = "inputs";
  }

  /**
  * Returns the LiElement specified by $idx.
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
  * @return LiElement
  */
  function getData2idx( $idx, $isInColl=false ){
    global $cfg,$url,$lng,$usr;
    $result = $this->myDBC->mkSelect( "*", $this->myTable, "idx='".$idx."'" );
    if ( !($data = mysql_fetch_array( $result )) ){
      trigger_error( mysql_error(). "i with idx not found: ".$idx, E_USER_WARNING );
      return false;
      exit;
    }else{
      // load old version...
      $myId = 'i';
      if ($usr->isOfKind( IS_CONTENT_EDITOR ) && $cfg->doesExist('versioning') && $cfg->get('versioning')=='1' &&
      $url->available('historicVersion') &&
      strpos($url->get('historicVersion'), $myId.$idx.'-') !== FALSE){
        $historyIdx = substr($url->get('historicVersion'), strpos($url->get('historicVersion'),'-')+1);
        $backupIdx = $data['idx'];
        $backupHistory = $data['history'];
        $result = $this->myDBC->mkSelect( "*", 'bak_'.$this->myTable, "idx='".$historyIdx."'" );
        if ( !($data = mysql_fetch_array( $result )) ){
          trigger_error( mysql_error(). 'Version '.$historyIdx.' of '.$myId.$idx.' not found.', E_USER_WARNING );
          return false;
          exit;
        }
        $data['idx'] = $backupIdx;
        $data['history'] = '<a href="'.$url->link2( $_SERVER['PHP_SELF'].'?address='.$url->get('address') ).'">['.$lng->get('lastChange').']</a>'."\n".$backupHistory;
      }

      $nEl = new LiElement( $data['idx'],                                   // $idx
                            $this->myDBC->unEscapeString($data['question']),                // $question
                            $this->myDBC->unEscapeString($data['example_solution']),        // $exampleSolution
                            ($data['possible_credits'] / 100),              // float; $possibleCredits
                            $data['visible_for'],                           // $visibleFor
                            ($data['visible_only_in_collection'] == "1"),   // $visibleOnlyInCollection
                            $data['history'],                               // $history
                            $isInColl                                       // $isInColl
                          );
       $nEl->hasFileUpload = ( isset($data['hasFileUpload']) && $data['hasFileUpload'] == 1 ? true : false );
       return $nEl;
    }
  }

  /**
  * For the menu only a little amount of data is needed so here only that stuff is loaded.
  *
  * Returns a LiElement to the specified $idx that is only PARTIALLY FILLED.
  * Menu entries appear only if the element is visible. The element is filled so
  * that it can tell whether it is visible and has all relevant properties to return
  * its menu.
  *
  * @param $idx               The element's index (also idx in the database).
  * @param $isInColl          The element might be restricted to be visible only inside a collection.
  *                           Since this is kind of the elements constructor this argument is just
  *                           passed to the real constructor.
  *
  * @return LiElement         Only partially filled with data!
  */
  function getMenuData2idx( $idx, $isInColl=false ){
    global $cfg;
    $result = $this->myDBC->mkSelect( "idx, question, visible_for, visible_only_in_collection", $this->myTable, "idx='".$idx."'" );
    if ( !($data = mysql_fetch_array( $result )) ){
      //trigger_error( mysql_error(). "i with idx not found: ".$idx, E_USER_WARNING );
      return false;
      exit;
    }else{
      return new LiElement( $data['idx'],                                 // $idx
                            $data['question'],                                           // $question
                            "",                                           // $exampleSolution
                            "",                                           // float; $possibleCredits
                            $data['visible_for'],                         // $visibleFor
                            ($data['visible_only_in_collection'] == "1"), // $visibleOnlyInCollection
                            false, /* false indicates "only menu data" */ // $history
                            $isInColl );                                  // $isInColl
    }
  }

  /**
  * !$createNew: Stores the data into the database. Position specified by $eI->idx.
  *  $createNew: Creates a new dataset and returns its index in the database.
  *
  * @param $eI  The LIElement to store.
  * @param $createNew  Shall a new element be created in the DB?
  * @return !$createNew: possible error
  *          $createNew: int The idx of the newly created element.
  */
  function setData( &$eI, $createNew = false ){
    global $usr, $cfg, $lng;
    tidyHTML( $eI->question );
    tidyHTML( $eI->exampleSolution );
    $toSet =  "question='".$this->myDBC->escapeString($eI->question)."', ".
              "hasFileUpload='".( $eI->hasFileUpload ? 1 : 0 )."', ".
              "example_solution='".$this->myDBC->escapeString( $eI->exampleSolution )."', ".
              "possible_credits='".$this->myDBC->escapeString($eI->possibleCredits * 100)."', ". // stored as int but float :2
              "visible_for=".$eI->visibleFor.", ".
              "visible_only_in_collection=".( $eI->visibleOnlyInCollection ?  1 :  0  ).", ";

    $element = $eI;
    $element->history = trim( $element->history[0] == ']' ?
                          substr( $element->history, 1 ) : // import... -> no stamping please
                          date("Y-m-d H:i:s", time()).' '.$usr->foreName.' '.$usr->surName.( $createNew ? '' : "\n".$element->history)
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
  * @return LiElement
  * @return FALSE     If no more datasets are available.
  */
  function getNextData(){
    if ( $data = mysql_fetch_array($this->queryResult) )
      return new LiElement( $data['idx'],                                   // $idx
                            $this->myDBC->unEscapeString($data['question']),                // $question
                            $this->myDBC->unEscapeString($data['example_solution']),        // $exampleSolution
                            ($data['possible_credits'] / 100),              // float; $possibleCredits
                            $data['visible_for'],                           // $visibleFor
                            ($data['visible_only_in_collection'] == "1"),   // $visibleOnlyInCollection
                            $data['history'],                               // $history
                            false                                           // $isInColl
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
    return Array( $lng->get("index")            => "idx",
                  $lng->get("question")         => "question",
                  $lng->get("rights")           => "visible_for",
                  $lng->get("onlyInCollection") => "visible_only_in_collection"
                 );
  }
}
$GLOBALS["iDBI"] = new LiDBInterface();
