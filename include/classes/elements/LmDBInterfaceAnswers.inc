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
* This is the interface to the working databse's "multiple_choice_answers" table which holds
* the users answers to the multiple choice questions.
*/

/**
* The answers of the multiple choice question are permutated for each user so it is not so easy
* just to go over the questions by knowing 3, 5,  are correct from your neighbor. Instead you have
* to read the answers at least and so I think you learn something even though someone tells you,
* what is right.
*
* @module     ../include/classes/elements/LmDBInterfaceAnswers.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
require_once( INCLUDE_DIR."/classes/Wdbc.inc" );

class LmDBInterfaceAnswers {
  var $myDBC;
  var $myTable;
  function LmDBInterfaceAnswers(){
    $this->myDBC   = new Wdbc();
    $this->myTable = "multiple_choice_answers";
  }
  
  /**
  * Returns the dataset specified by $uid & $mIdx.
  *
  * @param $uid               The user's unique identifier.
  * @param $mIdx              The index of the multiple choice element the answer is for.
  * @param $countOfAnswers    The number of answers the multiple choice element has.
  *                           The answers are permutated. This happens once on entering the question
  *                           since it would be annoying to have the order changed on each reload.
  *                           It could happen that suddenly there are more or less answers (someone edited
  *                           the m-element). Then it is necessary to rebuild the permutation array!
  * @param $createDataset     If set a new record is created on entering. (False is for correctors).
  *
  * @return Array(  "permutation" => Array containing the permutation of the indices,
  *                 "answers" => Array( answerMask_0, ..., answerMask_N ),
  *                 "history" => history );
  */
  function getData4( $uid, $mIdx, $countOfAnswers, $createDataset ){
    global $cfg, $lng;
    $result = $this->myDBC->mkSelect( "*", $this->myTable, "uid='".$uid."' && mc_idx='".$mIdx."'" );
    if ( !($data = mysql_fetch_array( $result )) ){ // no dataset found => create a new one
      // first: The permutation aray of the answers
      $permutationArray = range (0, $countOfAnswers-1);
      srand ((float)microtime()*1000000);
      shuffle ($permutationArray);
      
      if ($createDataset) $this->setData4( $uid, $mIdx, $permutationArray, Array( "0" ), $lng->get("dataSetCreated") ); // the last entry of the array ("0" here) is always editable
      return Array( "permutation" => $permutationArray, 
                    "answers" => Array( "0" ), 
                    "history" => "" );
    }else{
      $answerArray = unserialize( $this->myDBC->unEscapeString($data['answer_array']) );
      $permutationArray = array_shift( $answerArray );
      if ( count( $permutationArray ) != $countOfAnswers ){
        // The permutation array doesn't math the number of possible answers (one was addded or deleted).
        $permutationArray = range (0, $countOfAnswers-1);
        srand ((float)microtime()*1000000);
        shuffle ($permutationArray);
        $this->setData4( $uid, $mIdx, $permutationArray, $answerArray );
      }
      return Array( "permutation" => $permutationArray, 
                    "answers" => $answerArray, 
                    "history" => $data['history'] );
    }
  }
  
  /**
  * Stores the data into the database. Position specified by $uid & $mIdx.
  * @param $uid               The user's unique identifier.
  * @param $mIdx              The index of the multiple choice element the answer is for.
  * @param $permutationArray  The display permutation of the answers.
  * @param $answerArray       The array containing the answer bitmasks.
  * @param $history           If present this is set after the user's name.
  */
  function setData4( $uid, $mIdx, $permutationArray, $answerArray, $history = "" ){
    global $usr;
    array_unshift( $answerArray, $permutationArray ); // add at front;
    $toSet = "answer_array='".$this->myDBC->escapeString( serialize($answerArray) )."', ".
             "history=CONCAT( NOW(), ': ".$usr->foreName." ".$usr->surName.retIfTrue( ( $history != "" ), ": ".$history)."\\n', ".$this->myTable.".history )";
    $this->myDBC->mkUpdIns( $toSet, $this->myTable, "mc_idx='".$mIdx."' && uid='".$uid."'" );
  }

  var $queryResult;
  /**
  * Queries for one multiple choice the uids of all members who answered.
  *
  * @param $mIdx       The index of the multiple choice.
  * @param $orderBy    Can be any of the table's keys. The result will be ordered by this key.
  * @param $asc        Should the datasets be in ascending order?
  */
  function getAllUIDs( $mIdx, $orderBy="", $asc=true ){
    global $cfg;
    $what = "uid";
    if ( $orderBy != "" ) if ( $asc ) $orderBy .= " ASC"; else $orderBy .= " DESC";

    $this->queryResult = $this->myDBC->mkSelect( $what, $this->myTable, "mc_idx='".$mIdx."'", $orderBy );
  }
  
  /**
  * Returns the next result of the getAllData() query or false (no more results).
  * @return int     Next members uid.
  * @return FALSE   If no more datasets are available.
  */
  function getNextData(){
    if ( $data = mysql_fetch_array($this->queryResult) ) return $data['uid'];
                                                    else return false;
  }
}
