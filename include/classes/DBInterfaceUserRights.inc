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
* This is the interface to the working databse's "user_rights" table which
* holds the user's system rights and team mapping.
*
* DBInterfaceUser.inc is used to authenticate the user. After that the
* user's UID is known and he can get its rights from here.
*
* @module     ../include/classes/DBInterfaceUserRights.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
if ( class_exists( 'DBInterfaceUserRights' ) || function_exists( 'DBInterfaceUserRights' ) ) return;

require_once( INCLUDE_DIR."/classes/Wdbc.inc" );

class DBInterfaceUserRights {
  var $myDBC;
  var $myTable;
  function DBInterfaceUserRights(){
    $this->myDBC = new Wdbc();
    $this->myTable = "user_rights";
  }

  /**
  * Returns the requested dataset.
  * @param $uid     The user's unique identifier.
  * @return Array(  "rights" => int,
  *                 "currentTeam" => int,
  *                 "history" => string );
  */
  function getData4( $uid ){
    global $usr, $cfg, $lng;
    $result = $this->myDBC->mkSelect( "rights, currentTeam, history", $this->myTable, "uid='".$uid."'" );
    if ( !($data = mysql_fetch_array( $result )) ){
      /* no dataset found => create a new one with the default values */
      // each new member should get his own team number
      // therefore we insert the record and add the index to the given start index
      $rights = $cfg->get("DefaultAuthUserRights");
      $currentTeam = $cfg->get("DefaultAuthUserTeam");
      $toSet = "";
      $toSet = ( $rights != "" ?       "rights='".$rights."', " : '' ).
               ( $currentTeam != "" ?  "currentTeam='".$currentTeam."', "  : '' ).
               "uid='".$uid."', ".
               "history=CONCAT( NOW(), ': ".$usr->foreName." ".$usr->surName.' (rights: '.$rights.'; team: '.$currentTeam.') '."\\n', ".$this->myTable.".history )";
      $newIDX = $this->myDBC->mkInsertReturnIDX( $toSet, 'user_rights' );
      $this->setData4( $uid, $cfg->get("DefaultAuthUserRights"), $cfg->get("DefaultAuthUserTeam")+$newIDX, $lng->get("dataSetCreated") );
      return Array( "rights"      => $cfg->get("DefaultAuthUserRights"),
                    "currentTeam" => $cfg->get("DefaultAuthUserTeam")+$newIDX,
                    "history"     => "");
    }else
      return Array( "rights"      => $data['rights'],
                    "currentTeam" => $data['currentTeam'],
                    "history"     => $data['history'] );
  }

  /**
   * Removes the user rights entry for the given $uid.
   * @param $uid The UID to be removed from the user rights table
   */
  function removeUID($uid){
    $this->myDBC->mkDelete($this->myTable, "uid='".$uid."'");
  }

  /**
  * Stores the data into the database. Position specified by $uid.
  * @param $uid               The user's unique identifier.
  * @param $rights            The users rights (bitmask).
  * @param $currentTeam       The users currentTeam.
  * @param $history           If present this is set after the user's name.
  */
  function setData4( $uid, $rights="", $currentTeam="", $history = "" ){
    global $usr, $cfg;
    $toSet = "";
    $toSet = ( $rights != "" ?       "rights='".$rights."', " : '' ).
             ( $currentTeam != "" ?  "currentTeam='".$currentTeam."', "  : '' ).
             "history=CONCAT( NOW(), ': ".$usr->foreName." ".$usr->surName.' (rights: '.$rights.'; team: '.$currentTeam.') '.( ( $history != "" ) ?  ": ".$history : '' )."\\n', ".$this->myTable.".history )";

    $this->myDBC->mkUpdIns( $toSet, "user_rights", "uid='".$uid."'" );
  }


  var $queryResult;
  /**
  * Queries for all datasets.
  * @param $orderBy    Can be any of the table's keys. The result will be ordered by this key.
  * @param $asc        Should the datasets be in ascending order?
  * @param $restrictBy You can restrict the query (p.e. "currentTeam='".$usr->currentTeam."'").
  *                    Example for restricting to a special usergroup (untested!):
  *                    "(user_rights.rights | ".$restrictByRights." = ".$restrictByRights.")"
  */
  function getAllData( $orderBy="", $asc=true, $restrictBy="" ){
    if ( $orderBy != "" ) if ( $asc ) $orderBy .= " ASC"; else $orderBy .= " DESC";
    $this->queryResult = $this->myDBC->mkSelect( "uid, rights, currentTeam, history", "user_rights", $restrictBy, $orderBy );
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
  * Returns the next result of the getAllData() query or false (no more results).
  * @return Array(  "uid" => string,
  *                 "rights" => int,
  *                 "currentTeam" => int,
  *                 "history" => string );
  * @return FALSE   If no more datasets are available.
  */
  function getNextData(){
  /* This call returns the next $userStruct or FALSE. */
    if ( $resArray = mysql_fetch_array($this->queryResult) ) return Array( "uid"         => $resArray['uid'],
                                                                           "rights"      => $resArray['rights'],
                                                                           "currentTeam" => $resArray['currentTeam'],
                                                                           "history"     => $resArray['history'] );
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
    global $lng, $cfg;
    return Array( $lng->get("team")     => "currentTeam",
                  $lng->get("rights")   => "rights"
                 );
  }
}
