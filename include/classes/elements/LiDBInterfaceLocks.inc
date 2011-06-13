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
* This is the interface to the working databse's "input_answers_locks" table 
* which holds the user's answers to the inputs.
*/

/**
* @module     ../include/classes/elements/LiDBInterfaceLocks.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
require_once( INCLUDE_DIR."/classes/Wdbc.inc" );

class LiDBInterfaceLocks {
  var $myDBC;
  var $myTable;
  function LiDBInterfaceLocks(){
    $this->myDBC   = new Wdbc();
    $this->myTable = "input_answers_locks";
  }
  
  /**
  * Returns the locked input's idx address or false.
  * @param $team  The users currentTeam.
  * @return       The locked element's idx (p.e. 23 for i23) or false.
  */
  function getData4( $team ){
    global $cfg, $usr;
    $result = $this->myDBC->mkSelect( "*", $this->myTable, "team='".$team."'" );
    if ( !($data = mysql_fetch_array( $result )) ) return false; // no dataset found
                                              else return $data['lock_on_idx'];
  }
  
  /**
  * Sets the lock.
  * @param $team          The users currentTeam.
  * @param $lockIdx       The locked element's idx or false.
  * @param $history       If present this is set after the user's name.
  */
  function setData4( $team, $lockIdx, $history = "" ){
    global $usr;
    $toSet = "lock_on_idx='".$lockIdx."', ".
             "history=CONCAT( NOW(), ': ".$usr->foreName." ".$usr->surName.( ( $history != "" ) ?  ": ".$history  : '' )."')"; 
             // discards old history entries: a "one row history" should be enough. Otherwise it would get really big...    
    $this->myDBC->mkUpdIns( $toSet, $this->myTable, "team='".$team."'" );
  }
  
  /**
  * Removes all locks (only one should exist) for the given team.
  * @param $team          The team to remove all locks of.
  */
  function remLock4( $team ){
    $this->myDBC->mkDelete( $this->myTable, "team='".$team."'" );
  }
}
