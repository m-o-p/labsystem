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
* This is the interface to the working databse's "input_answers" table which holds the user's answers to the inputs.
*/

/**
* @module     ../include/classes/elements/LiDBInterfaceAnswers.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
require_once( INCLUDE_DIR."/classes/Wdbc.inc" );

class LiDBInterfaceAnswers {
  var $myDBC;
  var $myTable;
  function LiDBInterfaceAnswers(){
    $this->myDBC   = new Wdbc();
    $this->myTable = "input_answers";
  }
  
  /**
  * Returns the requested dataset.
  * @param $team              The users currentTeam.
  * @param $iIdx              The index of the input the answer is for.
  * @param $createNewRecord   If true a new datasetwill be created if none is found.
  * @return Array(  "answer" => string,
  *                 "comment" => string,
  *                 "givenCredits" => float,
  *                 "isClosed" => bool,
  *                 "isCorrected" => bool,
  *                 "history" => string,
  */
  function getData4( $team, $iIdx, $createNewRecord ){
    global $cfg, $usr, $lng;

    $result = $this->myDBC->mkSelect( "*", $this->myTable, "team='".$team."' && i_idx='".$iIdx."'" );
    if ( !($data = mysql_fetch_array( $result )) ){
      /* no dataset found && $createNewRecord => create a new one */
      if ( $createNewRecord ) $this->setData4( $team, $iIdx, "", 0, "", false, false, $lng->get("dataSetCreated") );
      return Array( "answer" => "", 
                    "comment" => "", 
                    "givenCredits" => 0, 
                    "isClosed" => false, 
                    "isCorrected" => false, 
                    "history" => "" );
    }else{
      return Array( "answer" => $this->myDBC->unEscapeString( $data['answer'] ), 
                    "comment" => $this->myDBC->unEscapeString( $data['comment'] ), 
                    "givenCredits" => ($data['given_credits']/100), 
                    "isClosed" => ($data['closed'] == 1), 
                    "isCorrected" => ($data['corrected'] == 1), 
                    "history" => $this->myDBC->unEscapeString( $data['history'] ) );
    }
  }
  
  /**
  * Stores the data into the database. Position specified by $team & $iIdx.
  * @param $team              The users currentTeam.
  * @param $iIdx              The index of the input the answer is for.
  * @param $answer            The users answer.
  * @param $givenCredits      FLOAT of given credits (p.e. 0.75).
  * @param $comment           The correctors comment.
  * @param $closed            isClosed?
  * @param $corrected         isCorrected?
  * @param $history           If present this is set after the user's name.
  */
  function setData4( $team, $iIdx, $answer="", $givenCredits="0", $comment="", $closed=false, $corrected=false, $history = "" ){
    global $usr;
    $toSet = "answer='".$this->myDBC->escapeString($answer)."', ".
             "comment='".$this->myDBC->escapeString($comment)."', ".
             "given_credits='".($givenCredits*100)."', ". // *100 to relize a float
             "closed=".( $closed ?  "1" :  "0"  ).", ".
             "corrected=".( $corrected ?   "1" :  "0"  ).", ".
             "history=CONCAT( NOW(), ': ".$usr->foreName." ".$usr->surName.( ( $history != "" ) ?  ": ".$history : '' )."\\n', ".$this->myTable.".history )";
    $this->myDBC->mkUpdIns( $toSet, $this->myTable, "i_idx='".$iIdx."' && team='".$team."'" );
  }
  
  var $queryResult;
  /**
  * Queries for one input the numbers of all teams that answered.
  *
  * @param $iIdx       The index of the input the answer is for.
  * @param $orderBy    Can be any of the table's keys. The result will be ordered by this key.
  * @param $asc        Should the datasets be in ascending order?
  */
  function getAllTeams( $iIdx, $orderBy="", $asc=true ){
    global $cfg;
    $what = "team";
    if ( $orderBy != "" ) if ( $asc ) $orderBy .= " ASC"; else $orderBy .= " DESC";

    $this->queryResult = $this->myDBC->mkSelect( $what, $this->myTable, "i_idx='".$iIdx."'", $orderBy );
  }
  
  /**
  * Returns the next result of the getAllData() query or false (no more results).
  * @return int     Next teams number.
  * @return FALSE   If no more datasets are available.
  */
  function getNextData(){
    if ( $data = mysql_fetch_array($this->queryResult) ) return $data['team'];
                                                    else return false;
  }
}
