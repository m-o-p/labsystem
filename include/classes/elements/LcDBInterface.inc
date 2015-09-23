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
* This module is the database interface for the c element.
* It connects to the data database's table "collections".
*
* @module     ../include/elements/LcDBInterface.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
if ( isset( $GLOBALS["cDBI"] ) ) return; // require_once doesn't work well

require_once( INCLUDE_DIR."/classes/Ddbc.inc" );
require_once( INCLUDE_DIR."/classes/elements/DBInterface.inc" );
require( INCLUDE_DIR."/classes/elements/LcElement.inc" );

class LcDBInterface extends DBInterface{

  function LcDBInterface(){
    parent::__construct();
    $this->myDBC   = new Ddbc();
    $this->myTable = "collections";
  }

  /**
   * Creates a new element of the required type using the database query result given as parameter.
   * @param unknown $data The result of the database query.
   * @return LcElement The new element filled with the data from the DB.
   */
  function createElementFromDbResult( &$data, $isInColl=false, $isInLab=false ){
    $newElement = new LcElement(   $data['idx'],                                 // $idx
                                  $this->myDBC->unEscapeString($data['title']),                 // $title
                                  $this->myDBC->unEscapeString($data['contents']),              // $contents
                                  $this->myDBC->unEscapeString($data['matching_menu']),         // $matchingMenu
                                  ($data['visible_before_first_sched'] == "1"), // $visibleBefore1stSched
                                  ($data['visible_during_sched'] == "1"),       // $visibleDuringSched
                                  ($data['visible_after_first_sched'] == "1"),  // $visibleAfter1stSched
                                  ($data['visible_only_in_collection'] == "1"), // $visibleOnlyInCollection
                                  $data['history'],                             // $history
                                  $isInColl,                                    // $isInColl
                                  $isInLab                                      // $isInLab
                              );
    // cache
    $this->cache($newElement);
    return $newElement;
  }

  /**
  * Returns the LcElement specified by $idx.
  * Calling this method is the normal way to get an instance of the fitting element.
  * This might seem strange but since constructors can't be overloaded it was the better
  * (more flexible) alternative to let the constructor create the object by values and not
  * just by an index like it is done here.
  *
  * @param $idx               The element's index (also idx in the database).
  * @param $isInColl          The element might be restricted to be visible only inside a collection.
  *                           Since this is kind of the elements constructor this argument is just
  *                           passed to the real constructor.
  * @param $isInColl          As $isInColl this parameter is just passed to the real constructor.
  *                           The collection behaves differen if it is inside a lab (not schedule dependent).
  *
  * @return LcElement
  */
  function getData2idx( $idx, $isInColl=false, $isInLab=false ){
    global $cfg,$url,$lng,$usr;
    $cachedResult = $this->getFromCache($idx);
    if ($cachedResult){
      return $cachedResult;
    }
    $result = $this->myDBC->mkSelect( "*", $this->myTable, "idx='".$idx."'" );
    if ( !($data = mysql_fetch_array( $result )) ){
      trigger_error( mysql_error(). "collection with idx not found: ".$idx, E_USER_WARNING );
      return false;
      exit;
    }else{
      // load old version...
      $myId = 'c';
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
        $data['history'] = preg_replace('/^(@@'.$historyIdx.' )(.*)/m','$1<span class="selectedVersion">$2</span>',$backupHistory);
      }
      //if ( $data['contents'] == '' ) $data['contents'] = 'p1 m1 i1'; // if empty fill with standard filler
      return $this->createElementFromDbResult( $data, $isInColl, $isInLab );
    }
  }

  /**
   * Creates the database set string from the element.
   * @param unknown $element
   * @return string
   */
  function createSetStringFromElement( &$element ){
    return  "title='".$this->myDBC->escapeString($eC->getTitle())."', ".
            "contents='".$this->myDBC->escapeString(implode( ' ', $eC->getContainedElementIdArray() ))."', ".
            "matching_menu='".$this->myDBC->escapeString($eC->getMatchingMenuRaw())."', ".
            "visible_only_in_collection=".( $eC->getVisibleOnlyInCollection() ?  "1" :  "0"  ).
            ", visible_before_first_sched=".( $eC->getVisibleBefore1stSchedule() ?  "1" :  "0"  ).
            ", visible_during_sched=".( $eC->getVisibleDuringSchedule() ?  "1" :  "0"  ).
            ", visible_after_first_sched=".( $eC->getVisibleAfter1stSchedule() ?  "1" :  "0"  ).', ';
  }

  /**
  * !$createNew: Stores the data into the database. Position specified by $eC->idx.
  *  $createNew: Creates a new dataset and returns its index in the database.
  *
  * @param $eC  The LcElement to store.
  * @param $createNew  Shall a new element be created in the DB?
  * @return !$createNew: possible error
  *          $createNew: int The idx of the newly created element.
  */
  function setData( &$eC, $createNew = false ){
    global $usr, $cfg, $lng;
    $toSet =  $this->createSetStringFromElement($eC);

    $element = $eC;
    $element->setHistory = trim( $element->getHistory()[0] == ']' ?
                            substr( $element->getHistory(), 1 ) : // import... -> no stamping please
                            date("Y-m-d H:i:s", time()).' '.$usr->foreName.' '.$usr->surName.( $createNew ? '' : PHP_EOL.$element->getHistory())
                          );
    // versioning
    if ($cfg->doesExist('versioning') && $cfg->get('versioning')=='1'){
      $versionedIdx = $this->myDBC->mkInsertReturnIDX( $toSet.
          "history='".$this->myDBC->escapeString( $element->getHistory() )."'",
          'bak_'.$this->myTable );
      $element->prependToHistory('@@'.$versionedIdx.' ');
    }

    if ( $createNew )
      return $this->myDBC->mkInsertReturnIDX( $toSet.
                                              "history='".$this->myDBC->escapeString( $element->getHistory() )."'",
                                              $this->myTable );
    else
      return $this->myDBC->mkUpdIns( $toSet.
          'history=\''.$this->myDBC->escapeString($element->getHistory())."'",
                                     $this->myTable,
                                     "idx=".$element->getIdx() );
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
                  $lng->get("onlyInCollection") => "visible_only_in_collection",
                  $lng->get("index")            => "idx"
                );
  }
}
$GLOBALS["cDBI"] = new LcDBInterface();
