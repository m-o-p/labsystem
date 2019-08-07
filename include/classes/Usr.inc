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
  private $iSeeTeam;
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
  	parent::__construct( FALSE, "guest", "M.", "Guest", 0, IS_GUEST, IS_GUEST, "none@localhost", time() );
    $this->iSeeUID='';
    $this->iSeeDesc='';
  }
  
  /**
   * Creates a new user object by retrieving all necessary information from the general and the course specific user database.
   * $uid The UID of the user.
   */
  function __construct1( $uid ){
  	global $urDBI, $cfg, $url;
  	// Load the instance specific user data:
  	$instanceData = $urDBI->getData4( $uid );

  	$this->uid                      = $uid;
  	$this->currentTeam              = $instanceData['currentTeam'];
  	$this->userRights               = $instanceData['enabledRights'];
  	$this->userMaximumRights        = $instanceData['rights'];
  	$this->history 			        = $instanceData['history'];

  	// Load the globally defined data:
  	// !This requires the rights to be set!
  	$globalData = $this->getGlobalUserData( $uid );
  	 
  	$this->userName                 = $globalData['userName'];
  	$this->foreName                 = $globalData['foreName'];
  	$this->surName                  = $globalData['name'];

  	$this->mailAddress              = $globalData['eMail'];

  	// It might be handy to have different rights in different tabs only... Here we go:
  	if ($url->available('myRights') && is_numeric($url->get('myRights'))){
  		$this->userRights = intval($url->get('myRights'));
  	}
  	// The maximum rights might have changed in the meantime...
  	if ($this->userRights > $this->userMaximumRights){
  		$this->userRights = $this->userMaximumRights; // ensure never to have more rights than currently allowed.
  	}

  	// The config specified user rights editor gets those rights for bootstrapping:
  	if ($cfg->get('RightsAdminUsername') == $this->userName && (($this->userMaximumRights & intval(IS_USER_RIGHTS_EDITOR)) != intval(IS_USER_RIGHTS_EDITOR)) /* Only give the rights if they do not exist. Otherwise this user can never take his edit right temporarily away... */){
  		$this->userRights =  $this->userRights | intval(IS_USER_RIGHTS_EDITOR);
  	}
  	
  	// Initialize them...
  	$this->iSeeUID='';
  	$this->iSeeTeam='';
  	$this->iSeeDesc='nobody is seen';
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
   * Whom is the user viewing?
   * @return The team of the watched user or false.
   */
  function theSeeingTeam(){
  	if ( $this->isSeeingSomeonesData() ) {
  		return $this->iSeeTeam;
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
   * Check if all users with access rights in the $uDBI (global) have the proper access rights here
   * and that all users that had rights here but do not have access anymore are removed.
   * Called on labstatus, manage users and mail.
   */
  function validateAllUsers(){
  	global $lng, $uDBI, $urDBI, $SYSINFO;
  	// import users
  	$uDBI->getAllData();
  	while( $nextUserDataSet = $uDBI->getNextData() ){
  		$usrRights = $urDBI->getData4($nextUserDataSet['uid']); // creates default record for non existent.
  		if ($usrRights['history'] == $lng->get("dataSetCreated")){
  			$SYSINFO .= $nextUserDataSet['uid'].' ('.$nextUserDataSet["userName"].' ['.$nextUserDataSet["foreName"].' '.$nextUserDataSet["name"].'] '.$usrRights["currentTeam"].'); ';
  		} 
  	}
  	// remove access rights for users without access (as they may have been removed
  	// from the instance and if they were part of a team they may cause trouble...)
  	$urDBI->getAllData();
  	while( $nextUserDataSet = $urDBI->getNextData() ){
  		if ($uDBI->getData4($nextUserDataSet['uid']) === false){
  			$urDBI->removeUID($nextUserDataSet['uid']);
  			$SYSINFO .= $nextUserDataSet['uid'].' '.$lng->get('deleted').'; ';
  		}
  	}
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
    $userDetailedData = false;
    if ( $this->iSeeUID != 'all' ){ // seeing one member's answers
                            $this->iSeeTeam = 'unk';
                            $foundRecord = FALSE;
                            // 1) Try to load user mapping from the lab status
                            if (($url->available('address') && $url->get('address')[0]=='l')){
                            	// We are viewing a lab => load team from the labs user data record
                            	require_once( INCLUDE_DIR."/classes/elements/LlDBInterfaceUidStatus.inc" );
                            	require( "../php/getFirstLastFinal.inc" ); $labIdx = substr( $firstFinal, 1);
                            	$uLabDBI = new LlDBInterfaceUidStatus( $labIdx );
                            	// 1a) Load team data by picking the first participant
                            	if (is_numeric($uid)){
//                             		trigger_error('Using TEAM record of '.$this->iSeeUID, E_USER_NOTICE);
                            		// Assume we have a team number here...
                            		$uLabDBI->getAllTeamData( $uid );
                            		// Just take the first team member...
                            		$userDetailedData = $uLabDBI->getNextData();
                            	}
                            	// 1b) Load individual user
                            	if (!$userDetailedData || empty($userDetailedData) || empty($userDetailedData['uid'])){
//                             		trigger_error('Using individual record of '.$this->iSeeUID, E_USER_NOTICE);
                            		$userDetailedData = $uLabDBI->getData4( $this->iSeeUID );
                            	}
                            }
                            
                            
                            if (!$userDetailedData || empty($userDetailedData) || empty($userDetailedData['uid'])){
                            	// We do not have data yet, e.g. are not viewing a lab => load current team number from the user rights data
                            	global $urDBI;
//                             	trigger_error('Using general record of '.$this->iSeeUID, E_USER_NOTICE);
                            	$userDetailedData = $urDBI->getData4( $this->iSeeUID, FALSE );
                            }
                            $this->iSeeUID = $userDetailedData['uid'];                            
                            if ($userDetailedData && !empty($userDetailedData) && !empty($userDetailedData['uid'])){
                            	$this->iSeeTeam = $userDetailedData['currentTeam'];
                            	$userData = $this->getGlobalUserData( $this->iSeeUID );
                            	$this->iSeeDesc = $userData["foreName"]." ".$userData["name"]." (".$this->iSeeTeam.")";
                            }else{
                            	// No user data found...
                            	trigger_error('No user data found for '.$uid.'; fallback: all.', E_USER_WARNING);
                            	$this->iSeeUID = 'all';
                            }
    }
    
    if ( $this->iSeeUID == 'all' ){ // could have been set as fallback directly above...
    	// seeing ALL member's answers
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
    global $url, $lng, $uDBI;
    if ( $this->isOfKind( IS_USER ) ){ // (not) only correctors need to know about other members
                                      // this function is called on the statistics page as well
                                      // if you restrict to correctors users will not see other user's names
                                      // you should remove the history then as well in the statistics
      $userData = $uDBI->getData4( $uid );
      if ( $userData == false ){
        $userData = Array();
        $userData['name'] = "UID ".$uid;
        $userData['foreName'] = $lng->get("unknown");
        $userData['userName'] = $lng->get("unknown");
        $userData['eMail'] = 'me@unknown.tld';
      } else if ($url->available('pseudoNames') && !($uid==$_SESSION['uid']) /* Do not scramble for me. Implications on history and mail, etc. */) {
        $userData['name'] = randomName();
        $userData['foreName'] = randomName();
        $userData['userName'] = randomName();
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
