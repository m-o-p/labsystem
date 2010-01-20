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
* Contains the Usr class.
*/

/**
* This class encapsulates the current user with its rights, name etc. .
*
* An instance of this class gets created at the end: $usr.
*
* @module     ../include/classes/Usr.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
class Usr extends ElementUser{
  function Usr(){
    $this->ElementUser( "noID", "guest", "M.", "Guest", 0, IS_GUEST, "none@localhost", time() );
  }

  /**
  * Selector to determine if the user is logged in.
  * @return BOOL  True if the user is logged in, false otherwise.
  */
  function isLoggedIn() {
    return ( $this->userRights != IS_GUEST );
  }
  
  /**
  * Selector to determine if the user is seeing someone else's data (for correcting).
  * @return BOOL  True if the user is seeing someone else's data, false otherwise.
  */
  function isSeeingSomeonesData(){
    global $url;
    return ( isset( $_SESSION['seeingUID'] ) && !($url->available('hideAnswers')) /* paused */ );
  }

  /**
  * Whom is the user viewing?
  * @return The UID of the watched user or false.
  */  
  function theSeeingUid(){
    if ( $this->isSeeingSomeonesData() ) return $_SESSION['seeingUID'];
    else return false;
  }
  
  /**
  * What is the description of the viewed user?
  * @return The description of the watched user or false.
  */  
  function theSeeingDescr(){
    if ( $this->isSeeingSomeonesData() ) return $_SESSION['seeingDESCR'];
    else return false;
  }
  
  /**
  * Modifier to set whom the user is watching.
  * @param $uid The UID of the user to be watched.
  */
  function seesDataOf( $uid ){
    global $lng;
    if ( !$this->isOfKind( IS_CORRECTOR ) ) return false; // only correctors can do that
    
    if ( $uid == "" ){ /* Turn off. */
      if ( isset( $_SESSION['seeingUID'] ) ){
        unset( $_SESSION['seeingUID'] );
        unset( $_SESSION['seeingDESCR'] );
      }
    }else{
      if ( $uid != "all" ){ // seeing one member's answers
                              $userData = $this->getUserInformation( $uid );
                              $_SESSION['seeingDESCR'] = $userData["foreName"]." ".$userData["name"]." (".$userData["userName"].")";
      }else{ // seeing ALL member's answers
                              $_SESSION['seeingDESCR'] = $lng->get("showAllUADesc");
      }
      $_SESSION['seeingUID'] = $uid;
    }
  }
  
  /**
  * Asks the user DB fo forename, name and username.
  *
  * @param $uid   The user unique identifier
  *
  * @return Array( "name"     => string,
  *                "foreName" => string,
  *                "userName" => string,
  *                "link"     => string    // links to ../php/setSeeingUID.php and should be 
  *                                        // used wherever the username is needed to enable switching to the user's data
  *               );
  */
  function getUserInformation( $uid ){
    global $usr, $url, $lng;
    if ( $usr->isOfKind( "IS_CORRECTOR" ) ){ // only correctors need to know about othe members
      require_once( INCLUDE_DIR."/classes/DBInterfaceUser.inc" );
      $uDBI = new DBInterfaceUser();
      $userData = $uDBI->getData4( $uid );
      if ( $userData == false ){
        $userData["name"] = "UID ".$uid;
        $userData["foreName"] = $lng->get("unknown");
        $userData["userName"] = $lng->get("unknown");
      }
      return Array( "name"     => $userData["name"],
                    "foreName" => $userData["foreName"],
                    "userName" => $userData["userName"],
                    "link"     => "<a href=\"".$url->link2( "../php/executeOnLab.php?address=l1&function=setSeeingUID&param=".urlencode( "\"".$uid."\"" )."&redirectTo=".urlencode( $_SERVER['REQUEST_URI'] ) )."\">".
                                  $userData["name"].", ".$userData["foreName"]."</a>"
                   );
    }
  }
}
$usr = new Usr; /** create instance */
?>
