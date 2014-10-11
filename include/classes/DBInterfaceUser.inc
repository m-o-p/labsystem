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
* This class is the ONLY INTERFACE to the USERDATAbase.
*
* So if you want to authenticate somehow else you only have to change the functions here.
* In our scenario at the university of t&uuml;bingen it would make sense to authenticate
* against our kerberos.
*
* If you want to adopt to another user directory like LDAP+Kereros have a
* look at the minimal version of this: 'UserInterface_proto.inc'
*
* The example here authenticates against a mysql database that has a structure like that:
* user-table:
*    idx                            bigint
*    username                       char[255]
*    name                           char[255]
*    forename                       char[255]
*    email                          char[255]
*    uid                            char[25]    -> This is the unique id (see below)
*                                                  I could also have used the idx field but for any reason
*                                                  someone might want to change the order and then I had the
*                                                  problem...
*    _labws0405                     bool        These flags are also found in the config*.ini.
*    _labss05                       bool        They qualify the user to log in and to get rights set.
*    takes_part_at_the_xy_seminar   bool
*
* The fieldnames are taken from the config*.ini.
*
* The uid field is the unique identifier!
*   - It must be unique
*   - It must be persistent
* Because it is used as connector between the user database and the internal rights database.
* Inside the labsystem the user is only identified by this UID.
*
* If you don't have such a thing in your authentication source you might use an index
* or a hash over the username or even the username but be aware that each change of this
* value breaks the connection! (and therefore causes a dataloss for this user because he
* is only connected through this uid!)
*
*
* @module     ../include/classes/DBInterfaceUser.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
if ( class_exists("DBInterfaceUser") ) return; // include once

require_once( INCLUDE_DIR."/classes/DBConnection.inc" );

class DBInterfaceUser
{
  var $myDBC;
  var $myTable;

  function DBInterfaceUser( ){
    global $cfg;

    $this->myDBC = new DBConnection($cfg->get("UserDatabaseHost"),
                                    $cfg->get("UserDatabaseUserName"),
                                    $cfg->get("UserDatabasePassWord"),
                                    $cfg->get("UserDatabaseName"));
    $this->queryResult = false;
    $this->myTable = $cfg->get("UserDatabaseTable");
  }

  /**
  * This method does the authentication. If successfull some userdata are returned.
  * @param $username          The user's loginname.
  * @param $password          The user's password.
  * @return Array(  "name" => string,
  *                 "foreName" => string,
  *                 "eMail" => string,
  *                 "uid" => string,
  *                 "userName" => string ); // Doesn't seem to make sense but is necessary for consistence wtih getNextData().
  *                                         // If you want to know more details -> ../pages/manageUsers.php.
  * @return FALSE   If the user could not be authenticated.
  */
  function authenticate( $username, $password ){
    global $cfg;
    // The field names are taken from the config*.ini file
    $from = $this->myTable;
    $what = $cfg->get("UserDBField_name")." AS name, ".
            $cfg->get("UserDBField_forename")." AS forename, ".
            $cfg->get("UserDBField_email")." AS email, ".
            $cfg->get("UserDBField_uid")."  AS uid, ".
            $cfg->get("UserDBField_password")." AS password";
    // mysql_real_escape_string() against SQL-Injection!
    $where= $cfg->get("UserDBField_username")."='".$this->myDBC->escapeString( Ddbc::escapeString($username) )."' && ".
            $cfg->get("User_courseID")."='1'";
    $result = $this->myDBC->mkSelect( $what, $from, $where );
    if ( mysql_num_rows( $result ) != 1 )
      return false; // not found/ error
    else{
      // entry found
      $myPWdata = mysql_fetch_array($result);
      if ( !password_verify($password, $myPWdata['password']) ){
        return false; // not correct
      }
      $resArray = $myPWdata;
      return Array( "name"     => $resArray["name"],
                    "foreName" => $resArray["forename"],
                    "eMail"    => $resArray["email"],
                    "uid"      => $resArray["uid"],
                    "userName" => $username );
    }
  }

  /**
  * Returns some userdata to the given UID or false.
  * @param $uid     The user's unique identifier.
  * @return Array(  "name" => string,
  *                 "foreName" => string,
  *                 "eMail" => string,
  *                 "uid" => string,
  *                 "userName" => string );
  * @return FALSE   If no more datasets are available.
  */
  function getData4( $uid ){
    global $cfg, $lng;
    $what = $cfg->get("UserDBField_name")." AS name, ".
            $cfg->get("UserDBField_forename")." AS forename, ".
            $cfg->get("UserDBField_email")." AS email, ".
            $cfg->get("UserDBField_uid")."  AS uid, ".
            $cfg->get("UserDBField_username")." AS username";
    $from = $this->myTable;
    $where= $cfg->get("UserDBField_uid")."='".$uid."' && ".
            $cfg->get("User_courseID")."='1'";
    $result = $this->myDBC->mkSelect( $what, $from, $where );
    if ( mysql_num_rows( $result ) != 1 )
      return false; // -> UserInterface_proto.inc!
/*
      return Array( "name"     => $lng->get('unknown'),
                    "foreName" => false,
                    "eMail"    => $lng->get('unknown'),
                    "uid"      => $uid,
                    "userName" => $lng->get('unknown') );
*/
    else{
      $resArray = mysql_fetch_array( $result );
      return Array( "name"     => $resArray["name"],
                    "foreName" => $resArray["forename"],
                    "eMail"    => $resArray["email"],
                    "uid"      => $resArray["uid"],
                    "userName" => $resArray["username"] );
    }
  }


  var $queryResult;
  /**
  * Queries for all users able to authenticate ( $cfg["User_courseID"] set ). This is necessary to set the rights.
  * @param $orderBy    Can be any of the table's keys. The result will be ordered by this key.
  * @param $asc        Should the datasets be in ascending order?
  */
  function getAllData( $orderBy="", $asc=true ){
    global $cfg;
    $what = $cfg->get("UserDBField_name")." AS name, ".
            $cfg->get("UserDBField_forename")." AS forename, ".
            $cfg->get("UserDBField_email")." AS email, ".
            $cfg->get("UserDBField_uid")."  AS uid, ".
            $cfg->get("UserDBField_username")." AS username";
    $from = $this->myTable;
    $where= $cfg->get("User_courseID")."='1'";
    if ( $orderBy != "" ) if ( $asc ) $orderBy .= " ASC"; else $orderBy .= " DESC";

    $this->queryResult = $this->myDBC->mkSelect( $what, $from, $where, $orderBy );
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
  * @return Array(  "name" => string,
  *                 "foreName" => string,
  *                 "eMail" => string,
  *                 "uid" => string,
  *                 "userName" => string );
  * @return FALSE   If no more datasets are available.
  */
  function getNextData(){
    if ($resArray = mysql_fetch_array($this->queryResult))     return Array(  "name"     => $resArray["name"],
                                                                              "foreName" => $resArray["forename"],
                                                                              "eMail"    => $resArray["email"],
                                                                              "uid"      => $resArray["uid"],
                                                                              "userName" => $resArray["username"] );
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
    return Array( $lng->get("surName")  => $cfg->get("UserDBField_name"),
                  $lng->get("foreName") => $cfg->get("UserDBField_forename"),
                  $lng->get("userName") => $cfg->get("UserDBField_username"),
                  $lng->get('lastRegistered') => 'last_registered'
                 );
  }
}
?>