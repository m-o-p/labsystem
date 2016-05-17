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

require_once( INCLUDE_DIR."/classes/DBInterfaceUser.inc" );
require_once( INCLUDE_DIR."/classes/DBInterfaceUserRights.inc" );

class Usr extends ElementUser{
  private $iSeeUID;
  private $iSeeDesc;
  function __construct()
  {
  	$a = func_get_args();
  	$i = func_num_args();
  	if (method_exists($this,$f='__construct'.$i)) {
  		call_user_func_array(array($this,$f),$a);
  	}
  }
  
  function __construct0(){
  	$this->ElementUser( "noID", "guest", "M.", "Guest", 0, IS_GUEST, IS_GUEST, "none@localhost", time() );
    $this->iSeeUID='';
    $this->iSeeDesc='';
  }
  
  /**
   * Creates a new user object by retrieving all necessary information from the general and the course specific user database.
   * $uid The UID of the user.
   */
  function __construct1( $uid ){
  	global $urDBI, $cfg, $url;
  	// Load the user data from the two data bases:
  	$globalData = $this->getGlobalUserData( $uid );
  	$instanceData = $urDBI->getData4( $uid );

  	$this->uid                      = $uid;
  	$this->userName                 = $globalData['userName'];
  	$this->foreName                 = $globalData['foreName'];
  	$this->surName                  = $globalData['name'];
  	$this->currentTeam              = $instanceData['currentTeam'];
  	$this->userRights               = $instanceData['enabledRights'];
  	// The config specified user rights editor gets those rights for bootstrapping:
  	if ($cfg->get('RightsAdminUsername') == $this->userName){
  		$this->userRights =  $this->userRights | intval(IS_USER_RIGHTS_EDITOR);
  	}
  	$this->userMaximumRights        = $instanceData['rights'];
  	$this->mailAddress              = $globalData['eMail'];

  	// It might be handy to have different rights in different tabs only... Here we go:
  	if ($url->available('myRights') && is_numeric($url->get('myRights'))){
  		$this->userRights = intval($url->get('myRights'));
  	}
  	// The maximum rights might have changed in the meantime...
  	if ($this->userRights > $this->userMaximumRights){
  		$this->userRights = $this->userMaximumRights; // ensure never to have more rights than currently allowed.
  	}
  	
  	// Initialize them...
  	$this->iSeeUID='';
  	$this->iSeeDesc='nobody is seen';
  }
  
  function printMe(){
  	print_r( get_class_vars( $this ));
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
    return ( $this->isOfKind( IS_CORRECTOR ) && !empty( $this->iSeeUID ) && !($url->available('hideAnswers')) /* paused */ );
  }

  /**
  * Whom is the user viewing?
  * @return The UID of the watched user or false.
  */
  function theSeeingUid(){
    if ( $this->isSeeingSomeonesData() ) {
      return $this->iSeeUID;
    } else return false;
  }

  /**
  * What is the description of the viewed user?
  * @return The description of the watched user or false.
  */
  function theSeeingDescr(){
    if ( $this->isSeeingSomeonesData() ) return $this->iSeeDesc;
    else return false;
  }


  /**
  * Modifier to set whom the user is watching.
  * @param $uid The UID of the user to be watched.
  */
  function seesDataOf( $uid ){
    global $lng,$url;
    if ( !($this->isOfKind( IS_CORRECTOR ) ||
          (defined(IS_DB_USER_ADMIN) && IS_DB_USER_ADMIN!=0 && $this->isOfKind( IS_DB_USER_ADMIN ))
          )
        ) return false; // only correctors can do that
    $this->iSeeUID = $uid;
    if ( $this->iSeeUID != 'all' ){ // seeing one member's answers
                            $userData = $this->getGlobalUserData( $this->iSeeUID );
                            $currentTeam = 'unk';
                            if (!($url->available('address') && $url->get('address')[0]=='l')){
                            	// We are not viewing a lab => load current team number from the user rights data
                            	global $urDBI;
                            	$userDetailedData = $urDBI->getData4( $this->iSeeUID );
                            } else {
                            	// We are viewing a lab => load team from the labs user data record
                            	require_once( INCLUDE_DIR."/classes/elements/LlDBInterfaceUidStatus.inc" );
                            	require( "../php/getFirstLastFinal.inc" ); $labIdx = substr( $firstFinal, 1);
                            	$uLabDBI = new LlDBInterfaceUidStatus( $labIdx );
                            	$userDetailedData = $uLabDBI->getData4( $this->iSeeUID );
                            }
                            $currentTeam = $userDetailedData['currentTeam'];
                            $this->iSeeDesc = $userData["foreName"]." ".$userData["name"]." (".$currentTeam.")";
    }else{ // seeing ALL member's answers
                            $this->iSeeDesc = $lng->get("showAllUADesc");
    }
  }

  /**
  * Asks the user DB for forename, name and username.
  *
  * @param $uid   The user unique identifier
  *
  * @return Array( "name"     => string,
  *                "foreName" => string,
  *                "userName" => string,
  *                "eMail"    => string,
  *               );
  */

  function getGlobalUserData( $uid ){
    global $usr, $url, $lng, $uDBI;
    if ( $usr->isOfKind( IS_USER ) ){ // (not) only correctors need to know about other members
                                      // this function is called on the statistics page as well
                                      // if you restrict to correctors users will not see other user's names
                                      // you should remove the history then as well in the statistics
      $globalUserData = $uDBI->getData4( $uid );
      $userData = Array();
      if ( $globalUserData == false ){
        $userData['name'] = "UID ".$uid;
        $userData['foreName'] = $lng->get("unknown");
        $userData['userName'] = $lng->get("unknown");
      }else{
      	$userData['name'] = ( $url->available('pseudoNames') ? randomName():$globalUserData['name'] );
      	$userData['foreName'] = ( $url->available('pseudoNames') ? randomName():$globalUserData['foreName']);
      	$userData['userName'] = ( $url->available('pseudoNames')? randomName():$globalUserData['userName'] );
      }
      return $userData; 
    }
  }
  
  /**
   * Saves the given user rights as enabledRights for the current user.
   * @param unknown $newRights The new rights as bit mask.
   */
  function saveCurrentRights( $newRights ){
  	global $urDBI;
  	$approvedRights = ($this->isOfKind($newRights, $this->userMaximumRights)?$newRights:$this->userMaximumRights);
  	$this->userRights = $approvedRights; // set the rights for this execuion cycle as well...
  	$urDBI->setData4( $this->uid, '', $approvedRights);
  }
}
$usr = new Usr; /** create instance */
?>
