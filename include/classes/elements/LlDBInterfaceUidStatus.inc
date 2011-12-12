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
* This is the interface to the working databse's "lab_uid_status" table which holds
*  the users current status (p.e. isPrelabFinished?, isLabClosed?, howManyPoints? etc.).
*/

/**
* @module     ../include/classes/elements/LlDBInterfaceUidStatus.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
require_once( INCLUDE_DIR."/classes/Wdbc.inc" );

class LlDBInterfaceUidStatus {
  var $myDBC;
  var $myTable;
  var $labIdx;
  /**
  * As you can see the class holds to which lab it belongs.
  */
  function LlDBInterfaceUidStatus( $labIdx ){
    $this->myDBC   = new Wdbc();
    $this->myTable = "lab_uid_status";
    $this->labIdx   = $labIdx;
  }
  
  /**
  * Returns the team to the given uid and iIdx.
  *
  * @param $uid             The user's unique identifier.
  * @return Array(  "currentTeam" => int,
  *                 "preLabFinished" => bool,
  *                 "preLabGivenCredits" => float,
  *                 "preLabPossibleCredits" => float,
  *                 "preLabAllTeammFinished" => bool,
  *                 "labClosed" => bool,
  *                 "labCorrected" => bool,
  *                 "whichCorrected" => string, // contains "ttfttt" 1,2 corrected; 3 not etc.; only present if labCorrected=false;
  *                 "labGivenCredits" => float,
  *                 "labPossibleCredits" => float,
  *                 "history" => string );
  */
  function getData4( $uid ){
    global $cfg, $usr;
    $result = $this->myDBC->mkSelect( "*", $this->myTable, "l_idx='".$this->labIdx."' && uid='".$uid."'" );
    if ( !($data = mysql_fetch_array( $result )) ){
      /* no dataset found */
      return Array( "currentTeam" => false,
                    "preLabFinished" => false, 
                    "preLabGivenCredits" => 0,
                    "preLabPossibleCredits" => 0,
                    "preLabAllTeammFinished" => false, 
                    "labClosed" => false,
                    "labCorrected" => false,
                    "whichCorrected" => "",
                    "labGivenCredits" => 0,
                    "labPossibleCredits" => 0,
                    "history" => "" );
    }else{
      return Array( "currentTeam" => $data['current_team'],
                    "preLabFinished" => ($data['prelab_finished'] == 1), 
                    "preLabGivenCredits" => ($data['prelab_given_credits'] / 100),
                    "preLabPossibleCredits" => ($data['prelab_possible_credits'] / 100),
                    "preLabAllTeammFinished" => ($data['prelab_all_teammembers_finished'] == 1), 
                    "labClosed" => ($data['lab_closed'] == 1),
                    "labCorrected" => ($data['lab_corrected'] == 1),
                    "whichCorrected" => $data['which_corrected'],
                    "labGivenCredits" => ($data['lab_given_credits'] / 100),
                    "labPossibleCredits" => ($data['lab_possible_credits'] / 100),
                    "history" => $data['history'] );
    }
  }
  
  /**
  * Stores the data into the database. Position specified by $labIdx & $uid.
  *
  * @param $uid                           The user's unique identifier.
  * @param $currentTeam                   The users currentTeam. (some as in DBInterfaceInputAnswersUidTeam.inc but on lab level)
  * @param $prelabFinished                Is the prelab finished?
  * @param $prelabGivenCredits            FLOAT of given credits (p.e. 0.75).
  * @param $prelabPossibleCredits         FLOAT of possible credits.
  * @param $prelabAllTeammembersFinished  Did all teammembers finish the prelab?
  * @param $labClosed                     Is the lab closed?
  * @param $labCorrected                  Is the lab corrected?
  * @param $givenCredits                  FLOAT of given credits (p.e. 0.75).
  * @param $possibleCredits               FLOAT of possible credits.
  * @param $history                       If present this is set after the user's name.
  */
  function setData4( $uid, 
                     $currentTeam, 
                     $prelabFinished=false, 
                     $prelabGivenCredits=0,
                     $prelabPossibleCredits=0,
                     $prelabAllTeammembersFinished=false, 
                     $labClosed=false,
                     $labCorrected=false,
                     $whichCorrected="",
                     $givenCredits=0,
                     $possibleCredits=0,
                     $history = "" ){
    global $usr;
    $toSet = "current_team='".$currentTeam."', ".
             "prelab_finished='".( $prelabFinished ?  "1" :  "0" )."', ".
             "prelab_given_credits='".($prelabGivenCredits * 100)."', ".
             "prelab_possible_credits='".($prelabPossibleCredits * 100)."', ".
             "prelab_all_teammembers_finished='".( $prelabAllTeammembersFinished ?  "1" :  "0" )."', ".
             "lab_closed='".( $labClosed ?  "1" :  "0" )."', ".
             "lab_corrected='".( $labCorrected ?  "1" :  "0" )."', ".
             "which_corrected='".$whichCorrected."', ".
             "lab_given_credits='".($givenCredits * 100)."', ".
             "lab_possible_credits='".($possibleCredits * 100)."', ".
             "history=CONCAT( NOW(), ': ".$this->myDBC->escapeString( $usr->foreName )." ".$this->myDBC->escapeString( $usr->surName ).( ( $history != "" ) ?  ": ".$this->myDBC->escapeString( $history ) : '' )."\\n', ".$this->myTable.".history )";
    $this->myDBC->mkUpdIns( $toSet, $this->myTable, "l_idx='".$this->labIdx."' && uid='".$uid."'" );
  }
  
  /**
  * Sets the prelab_all_teammembers_finished bit on the position specified by $labIdx & $team.
  * @param $team       The team to set the value for.
  */
  function setPrelabFinished( $team ){
    global $usr, $lng;
    $toSet = "prelab_all_teammembers_finished='1', ".
             "history=CONCAT( NOW(), ': ".$lng->get("preLabAllTeamFin")."\\n', ".$this->myTable.".history )";
    $this->myDBC->mkUpdate( $toSet, $this->myTable, "l_idx='".$this->labIdx."' && current_team='".$team."'" );
  }
  
  /**
  * Sets the lab_closed bit on the position specified by $labIdx & $team.
  * @param $team       The team to set the value for.
  * @param $history    If present this is set sfter the user's name.
  */
  function closeLab( $team,
                     $history = "" ){
    global $usr;
    $toSet = "lab_closed='1', ".
             "history=CONCAT( NOW(), ': ".$this->myDBC->escapeString( $usr->foreName )." ".$this->myDBC->escapeString( $usr->surName ).( ( $history != "" ) ?  ": ".$this->myDBC->escapeString( $history ) : '' )."\\n', ".$this->myTable.".history )";
    $this->myDBC->mkUpdate( $toSet, $this->myTable, "l_idx='".$this->labIdx."' && current_team='".$team."'" );
  }
  
  /**
  * Sets the lab_closed=0 an lab_corrected=0 bits on the position specified by $labIdx & $team.
  * @param $team       The team to set the value for.
  * @param $history    If present this is set after the user's name.
  */
  function reOpenLab( $team,
                     $history = "" ){
    global $usr;
    $toSet = "lab_closed='0', ".
             "lab_corrected='0', ".
             "which_corrected='', ".
             "history=CONCAT( NOW(), ': ".$this->myDBC->escapeString( $usr->foreName )." ".$this->myDBC->escapeString( $usr->surName ).( ( $history != "" ) ?  ": ".$this->myDBC->escapeString( $history ) : '' )."\\n', ".$this->myTable.".history )";
    $this->myDBC->mkUpdate( $toSet, $this->myTable, "l_idx='".$this->labIdx."' && current_team='".$team."'" );
  }
  
  var $queryResult;
  /**
  * Queries all datasets for the lab specified by $labIdx.
  * @param $orderBy    Can be any of the table's keys. The result will be ordered by this key.
  * @param $asc        Should the datasets be in ascending order?
  */
  function getAllData( $orderBy="", $asc=true ){
    if ( $orderBy != "" ) if ( $asc ) $orderBy .= " ASC"; else $orderBy .= " DESC";
    $this->queryResult = $this->myDBC->mkSelect( "*", $this->myTable, "l_idx='".$this->labIdx."'", $orderBy );
  }
  
  /**
  * Queries all datasets of the members one team for the lab specified by $this->labIdx.
  * @param $team  The team to be queried.
  */
  function getAllTeamData( $team ){
    $this->queryResult = $this->myDBC->mkSelect( "*", $this->myTable, "current_team='".$team."' && l_idx='".$this->labIdx."'", "" );
  }
  
  /**
  * Queries all datasets of one member for the lab specified by $this->labIdx.
  * @param $uid  The id of the user to be queried.
  */
  function getAllUidData( $uid ){
    $this->queryResult = $this->myDBC->mkSelect( "*", $this->myTable, "uid='".$uid."' && l_idx='".$this->labIdx."'", "" );
  }
  
  /**
  * Queries all datasets of one member for ALL LABS.
  * @param $uid  The id of the user to be queried.
  */
  function getAllUidDataALLLabs( $uid ){
  // SELECT *, schedules.start FROM `lab_uid_status` LEFT JOIN `schedules` ON lab_uid_status.l_idx = schedules.num WHERE `uid` = "$uid" ORDER BY schedules.start, lab_uid_status.l_idx ASC
    $this->queryResult = $this->myDBC->mkSelect( "*", $this->myTable.'  LEFT JOIN `schedules` ON lab_uid_status.l_idx = schedules.num', "`uid`='".$uid."' && `l_idx` != 1", "schedules.start, lab_uid_status.l_idx ASC" );
  }
  
  /**
  * Returns the next result of the getAllData() query or false (no more results).
  * @return Array(  "lIdx" => int,
  *                 "uid" => string,
  *                 "currentTeam" => int,
  *                 "preLabFinished" => bool,
  *                 "preLabGivenCredits" => float,
  *                 "preLabPossibleCredits" => float,
  *                 "preLabAllTeammFinished" => bool,
  *                 "labClosed" => bool,
  *                 "labCorrected" => bool,
  *                 "whichCorrected" => string,
  *                 "labGivenCredits" => float,
  *                 "labPossibleCredits" => float,
  *                 "history" => string );
  * @return FALSE   If no more datasets are available.
  */
  function getNextData(){
    if ($data = mysql_fetch_array( $this->queryResult ))
      return Array( "lIdx" => $data['l_idx'],
                    "uid" => $data['uid'],
                    "currentTeam" => $data['current_team'],
                    "preLabFinished" => ($data['prelab_finished'] == 1), 
                    "preLabGivenCredits" => ($data['prelab_given_credits'] / 100),
                    "preLabPossibleCredits" => ($data['prelab_possible_credits'] / 100),
                    "preLabAllTeammFinished" => ($data['prelab_all_teammembers_finished'] == 1), 
                    "labClosed" => ($data['lab_closed'] == 1),
                    "labCorrected" => ($data['lab_corrected'] == 1),
                    "whichCorrected" => $data['which_corrected'],
                    "labGivenCredits" => ($data['lab_given_credits'] / 100),
                    "labPossibleCredits" => ($data['lab_possible_credits'] / 100),
                    "history" => $data['history'] );
    else
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
    return Array( $lng->get("team")             => "current_team",
                  $lng->get("orderPrelabFin")   => "idx",
                  'm-'.$lng->get("Credits")      => "prelab_given_credits",
                  'i-'.$lng->get("Credits")      => "lab_given_credits"
                 );
  }
}
