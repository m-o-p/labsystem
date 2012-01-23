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
* The userdata a connected via ../include/classes/DBInterfaceUser.inc
*
* This is the interface you have to implement to realize your own
* authentication system (p.e. authenticate against NIS, Kerberos, ...).
*
* Afterwards just replace the ../include/classes/DBInterfaceUser.inc.
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
*/

/**
* @module     ../include/classes/DBInterfaceUser.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
class DBInterfaceUser
{
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
    if ( true /* put authentication conditional here */ )
      return false; // unsuccessful
    else
      return Array( "name"     => $resArray["name"],
                    "foreName" => $resArray["forename"],
                    "eMail"    => $resArray["email"],
                    "uid"      => $resArray["uid"],
                    "userName" => $username );
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
    if ( true ) // unsuccessful
      return false;
    else
      return Array( "name"     => $resArray["name"],
                    "foreName" => $resArray["forename"],
                    "eMail"    => $resArray["email"],
                    "uid"      => $resArray["uid"],
                    "userName" => $resArray["username"] );
    }
  

  var $queryResult;
  /**
  * Queries for all users able to authenticate ( $cfg["User_courseID"] set ). This is necessary to set the rights.
  * @param $orderBy    Can be any of the table's keys. The result will be ordered by this key.
  * @param $asc        Should the datasets be in ascending order?
  */
  function getAllData( $orderBy="", $asc=true ){
    $this->queryResult = $yourData;
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
    if ( $this->queryResult->hasMoreData() )
      return Array( "name"     => $resArray["name"],
                    "foreName" => $resArray["forename"],
                    "eMail"    => $resArray["email"],
                    "uid"      => $resArray["uid"],
                    "userName" => $resArray["username"] );
    else
      return false;
  }
}
?>