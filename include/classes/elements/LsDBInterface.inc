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
*               ** ATTENTION **
* In contrast to the other elements the schedule data is stored
* in the Working Database not in the Data DB.
*
* @module     ../include/elements/LsDBInterface.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
if ( isset( $GLOBALS["sDBI"] ) ) return; // require_once doesn't work well

require_once( INCLUDE_DIR."/classes/Wdbc.inc" );
require( INCLUDE_DIR."/classes/elements/LsElement.inc" );

class LsDBInterface {
  var $myDBC;
  var $myTable;

  function LsDBInterface(){
    $this->myDBC   = new Wdbc();
    $this->myTable = "schedules";
  }

  /**
  * Returns the schedule information to a specified element.
  * The element is specified by $id & $num (p.e. c1).
  * This function is called STATICALLY by the elements collection and lab (the only ones depending from a schedule).
  * This function is a little more soffisticated because it walks thru all schedules for the given element.
  *
  * @param $id                An element's id.
  * @param $num               An element's number.
  *
  * @return Array(  "isBefore1stSched"  => bool,
  *                 "isInSched"         => bool,
  *                 "isAfter1stSched"   => bool,
  *                 "notFound"          => bool
  *               );
  */
  function getSchedule4( $id, $num ){
    global $cfg, $usr;
    $result = $this->myDBC->mkSelect( "start, stop", $this->myTable, "id='".$id."' && num='".$num."'", "'start' ASC" );
    if ( mysql_num_rows($result) == 0 ){ // no dataset found
      return Array( "isBefore1stSched" => false,
                    "isInSched" => false,
                    "isAfter1stSched" => false,
                    "notFound" => true
                  );
    }else{ // at least one dataset exists for the specified element
      $isBefore1stSched = false;
      $isInSched = false;
      $isAfter1stSched = false;
      $is1stSched = true;

      // walk thru all schedules for this element
      while( $data = mysql_fetch_array($result) ){
        $start = strtotime($data['start']);
        $stop = strtotime($data['stop']);
        if ( $is1stSched ){ // some values are only influenced by the first schedule
          $is1stSched = false;
          if ( date("YmdHis") < date("YmdHis", $start ) ) $isBefore1stSched = true;
          if ( date("YmdHis") > date("YmdHis", $stop ) )  $isAfter1stSched = true;
        }
        if ( ( date("YmdHis", $start ) < date("YmdHis") ) && ( date("YmdHis") < date("YmdHis", $stop ) ) ) $isInSched = true;
      }
      return Array( "isBefore1stSched"  => $isBefore1stSched,
                    "isInSched"         => $isInSched,
                    "isAfter1stSched"   => $isAfter1stSched,
                    "notFound"          => false
                  );
    }
  }

// The "normal" DBI starts here...

  /**
  * Returns the LsElement specified by $idx.
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
  * @return LsElement
  */
  function getData2idx( $idx, $isInColl=false ){
    global $cfg;
    $result = $this->myDBC->mkSelect( "*", $this->myTable, "idx='".$idx."'" );
    if ( !($data = mysql_fetch_array( $result )) ){
      //trigger_error( mysql_error(). "mc with idx not found: ".$idx, E_USER_WARNING );
      return false;
      exit;
    }else{
      return new LsElement( $data['idx'],                   // $idx
                            $data['id'],                    // $id
                            $data['num'],                   // $num
                            strtotime(  ( $idx == 1 ? 'today 0:00:00' : $data['start'] ) ),   // $start
                            strtotime(  ( $idx == 1 ? '+2 weeks 23:59:59' : $data['stop'] ) ),    // $stop
                            $this->myDBC->unEscapeString($data['comment']), // $comment
                            $this->myDBC->unEscapeString($data['history']), // $history
                            $isInColl                       // $isInColl
                           );
    }
  }

  /**
  * For the menu only a little amount of data is needed so here only that stuff is loaded.
  *
  * Returns a LsElement to the specified $idx that is only PARTIALLY FILLED.
  * Menu entries appear only if the element is visible. The element is filled so
  * that it can tell whether it is visible and has all relevant properties to return
  * its menu.
  *
  * @param $idx               The element's index (also idx in the database).
  * @param $isInColl          The element might be restricted to be visible only inside a collection.
  *                           Since this is kind of the elements constructor this argument is just
  *                           passed to the real constructor.
  *
  * @return LsElement         Only partially filled with data!
  */
  function getMenuData2idx( $idx, $isInColl=false ){
    global $cfg;
    $result = $this->myDBC->mkSelect( "idx, num, id", $this->myTable, "idx='".$idx."'" );
    if ( !($data = mysql_fetch_array( $result )) ){
      //trigger_error( mysql_error(). "mc with idx not found: ".$idx, E_USER_WARNING );
      return false;
      exit;
    }else{
      return new LsElement( $data['idx'],                   // $idx
                            $data['id'],                    // $id
                            $data['num'],                   // $num
                            strtotime(  $data['start'] ),   // $start
                            strtotime(  $data['stop'] ),    // $stop
                            "",                             // comment
                            false, /* "only menu data" */   // $history
                            $isInColl                       // $isInColl
                           );
    }
  }

  /**
  * !$createNew: Stores the data into the database. Position specified by $eS->idx.
  *  $createNew: Creates a new dataset and returns its index in the database.
  *
  * @param $eS  The LsElement to store.
  * @param $createNew  Shall a new element be created in the DB?
  * @return !$createNew: possible error
  *          $createNew: int The idx of the newly created element.
  */
  function setData( &$eS, $createNew = false ){
    global $usr, $cfg, $lng;
    tidyHTML( $eS->comment );
    $toSet =  "id='".$eS->id."', ".
              "num='".$eS->num."', ".
              "start='".date("YmdHis", $eS->start)."', ".
              "stop='".date("YmdHis", $eS->stop )."', ".
              "comment='".$this->myDBC->escapeString($eS->comment)."', ";

    if ( $createNew )
      return $this->myDBC->mkInsertReturnIDX( $toSet.
                                              "history=CONCAT( NOW(), ': ".$this->myDBC->escapeString( $usr->foreName )." ".$this->myDBC->escapeString( $usr->surName ).' '.$lng->get("dataSetCreated")."\\n' )",
                                              $this->myTable );
    else
      return $this->myDBC->mkUpdIns( $toSet.
                                     ( $eS->history != '' ?
                                        ( $eS->history[0] == ']' ?
                                            'history=\''.substr( $eS->history, 1 )."'" : // import... -> no stamping please
                                            "history=CONCAT( NOW(), ': ".$this->myDBC->escapeString( $usr->foreName )." ".$this->myDBC->escapeString( $usr->surName )."\\n".$this->myDBC->escapeString( $eS->history )."' )" )
                                        : // if the program changed the history: take it
                                        "history=CONCAT( NOW(), ': ".$this->myDBC->escapeString( $usr->foreName )." ".$this->myDBC->escapeString( $usr->surName )."\\n', ".$this->myTable.".history )"
                                     ),
                                     $this->myTable,
                                     "idx=".$eS->idx );
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
  * @return LsElement
  * @return FALSE     If no more datasets are available.
  */
  function getNextData(){
    if ($data = mysql_fetch_array($this->queryResult))
      return new LsElement( $data['idx'],                   // $idx
                            $data['id'],                    // $id
                            $data['num'],                   // $num
                            strtotime(  $data['start'] ),   // $start
                            strtotime(  $data['stop'] ),    // $stop
                            $this->myDBC->unEscapeString($data['comment']), // $comment
                            $this->myDBC->unEscapeString($data['history']), // $history
                            false                           // $isInColl
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
    return Array( $lng->get("start")  => "start",
                  $lng->get("stop")   => "stop",
                  $lng->get("index")  => "idx"
                 );
  }
}
$GLOBALS["sDBI"] = new LsDBInterface();
