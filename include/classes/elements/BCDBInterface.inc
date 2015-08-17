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
* Since the c and the C element differ only in their capabilities the only difference
* here is that instead of a LcElement a BCElement is created. Only those methods that
* are affected by that difference are implemented. The rest is taken from LcDBInterface.
*
* This module is the database interface for the C element.
* It connects to the data database's table "collections".
*
* @module     ../include/elements/BCDBInterface.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
if ( isset( $GLOBALS["CDBI"] ) ) return; // require_once doesn't work well

require_once( INCLUDE_DIR."/classes/Ddbc.inc" );
require( INCLUDE_DIR."/classes/elements/BCElement.inc" );

class BCDBInterface extends LcDBInterface {
  var $myDBC;
  function BCDBInterface(){
    $this->LcDBInterface();
  }

  /* The following functions have to return a BCElement instead of an LcElement.
     Since the member variables are identical a typecast would perfectly do the job.
     But this typecasts are not possible in this php version.
     So later you might replace the following by:
       return (BCElement) parent:: getData2idx( $idx );
  */
  function getData2idx( $idx, $isInColl=false, $isInLab=false ){
    //Something like "" return (BCElement) parent::getData2idx( $idx );""
    global $cfg,$url,$lng,$usr;
    $result = $this->myDBC->mkSelect( "*", "collections", "idx='".$idx."'" );
    if ( !($data = mysql_fetch_array( $result )) ){
      trigger_error( mysql_error(). "collection with idx not found: ".$idx, E_USER_WARNING );
      return false;
      exit;
    }else{
      // load old version...
      $myId = 'C';
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

      return new BCElement( $data['idx'],                                 // $idx
                            stripslashes($data['title']),                 // $title
                            stripslashes($data['contents']),              // $contents
                            stripslashes($data['matching_menu']),         // $matchingMenu
                            ($data['visible_before_first_sched'] == "1"), // $visibleBefore1stSched
                            ($data['visible_during_sched'] == "1"),       // $visibleDuringSched
                            ($data['visible_after_first_sched'] == "1"),  // $visibleAfter1stSched
                            ($data['visible_only_in_collection'] == "1"), // $visibleOnlyInCollection
                            $data['history'],                             // $history
                            $isInColl,                                    // $isInColl
                            $isInLab                                      // $isInLab
                          );
    }
  }

  /**
  * For the menu only a little amount of data is needed so here only that stuff is loaded.
  *
  * Returns a BCElement to the specified $idx that is only PARTIALLY FILLED.
  * Menu entries appear only if the element is visible. The element is filled so
  * that it can tell whether it is visible and has all relevant properties to return
  * its menu.
  *
  * @param $idx               The element's index (also idx in the database).
  * @param $isInColl          The element might be restricted to be visible only inside a collection.
  *                           Since this is kind of the elements constructor this argument is just
  *                           passed to the real constructor.
  *
  * @return BCElement         Only partially filled with data!
  */
  function getMenuData2idx( $idx, $isInColl=false, $isInLab=false ){
    global $cfg;
    $result = $this->myDBC->mkSelect( "idx, title, contents, matching_menu, visible_before_first_sched, visible_during_sched, visible_after_first_sched, visible_only_in_collection", $this->myTable, "idx='".$idx."'" );
    if ( !($data = mysql_fetch_array( $result )) ){
      //trigger_error( mysql_error(). "collection with idx not found: ".$idx, E_USER_WARNING );
      return false;
      exit;
    }else
      return new BcElement( $data['idx'],                                 // $idx
                            stripslashes($data['title']),                 // $title
                            stripslashes($data['contents']),              // $contents
                            stripslashes($data['matching_menu']),         // $matchingMenu
                            ($data['visible_before_first_sched'] == "1"), // $visibleBefore1stSched
                            ($data['visible_during_sched'] == "1"),       // $visibleDuringSched
                            ($data['visible_after_first_sched'] == "1"),  // $visibleAfter1stSched
                            ($data['visible_only_in_collection'] == "1"), // $visibleOnlyInCollection
                            false, /* false indicates "only menu data" */ // $history
                            $isInColl,                                    // $isInColl
                            $isInLab                                      // $isInLab
                          );
  }


  /**
  * Returns the next element from the getAllData() query or false (no more results).
  *
  * @return BCElement
  * @return FALSE     If no more datasets are available.
  */
  function getNextData(){
    // Something like ""return (BCElement) parent::getNextData();""
    if ($data = mysql_fetch_array($this->queryResult))
      return new BCElement( $data['idx'],                                 // $idx
                            stripslashes($data['title']),                 // $title
                            stripslashes($data['contents']),              // $contents
                            stripslashes($data['matching_menu']),         // $matchingMenu
                            ($data['visible_before_first_sched'] == "1"), // $visibleBefore1stSched
                            ($data['visible_during_sched'] == "1"),       // $visibleDuringSched
                            ($data['visible_after_first_sched'] == "1"),  // $visibleAfter1stSched
                            ($data['visible_only_in_collection'] == "1"), // $visibleOnlyInCollection
                            $data['history'],                             // $history
                            false,                                        // $isInColl
                            false                                         // $isInLab
                          );
    return false;
  }
}
$GLOBALS["CDBI"] = new BCDBInterface();
