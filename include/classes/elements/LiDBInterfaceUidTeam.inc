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
* This is the interface to the working databse's "input_answers_uid_team" table 
* which holds the mapping between UID and TEAM.
*/

/**
* The answers are stored for the TEAM.
* The USER might CHANGE its TEAM during the semester but the answers remain
* his because of this mapping.
*
* This mapping can not be done with the LlDBInterfaceUidStatus.inc beacause the
* input doesn't know (and shouldn't) to ehich lab it belongs.
*
* Also a mapping inside the input answer would have been more unflexible because
* then either the query would have got much more difficult or the number of
* teammembers (fixed fields) would have been fixed.
*
* @module     ../include/classes/elements/LiDBInterfaceUidTeam.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
require_once( INCLUDE_DIR."/classes/Wdbc.inc" );

class LiDBInterfaceUidTeam {
  var $myDBC;
  var $myTable;
  function LiDBInterfaceUidTeam(){
    $this->myDBC   = new Wdbc();
    $this->myTable = "input_answers_uid_team";
  }
  
  /**
  * Returns the team to the given uid and iIdx.
  * @param $uid               The user's unique identifier.
  * @param $iIdx              The index of the input the answer is for.
  * @param $team              Team the user should be mapped to. ( 0 = no mapping desired if non existent [don't create record] )
  * @return                   The team that is mapped to the given uid.
  */
  function getData4( $uid, $iIdx, $team ){
    global $cfg, $usr;
    $result = $this->myDBC->mkSelect( "team", $this->myTable, "uid='".$uid."' && i_idx='".$iIdx."'" );
    if ( !($data = mysql_fetch_array( $result )) ){
      /* no dataset found && $createNewRecord => create a new one with $usr->currentTeam */
      if ( $team!= 0 ) $this->setData4( $uid, $iIdx, $team );
      return $team;
    }else{
      return $data['team'];
    }
  }
  
  /**
  * Stores the mapping (uid, iIdx)->team into the database.
  * @param $uid               The user's unique identifier.
  * @param $iIdx              The index of the input the answer is for.
  * @param $team              The desired team.
  * @param $history           If present this is set after the user's name.
  */
  function setData4( $uid, $iIdx, $team, $history = "" ){
    global $usr;
    $toSet = "team='".$team."', ".
             "history=CONCAT( NOW(), ': ".$usr->foreName." ".$usr->surName.retIfTrue( ( $history != "" ), ": ".$history)."\\n', ".$this->myTable.".history )";
    $this->myDBC->mkUpdIns( $toSet, $this->myTable, "i_idx='".$iIdx."' && uid='".$uid."'" );
  }
}
