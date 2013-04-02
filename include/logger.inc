<?php
/**
 *  labsystem.m-o-p.de -
 *                  the web based eLearning tool for practical exercises
 *  Copyright (C) 2011  Marc-Oliver Pahl
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
* This module contains the log file functions.
*
* @module     ../include/logger.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2011
* @version    2.0
*/
function makeLogEntry( $group, $operation, $resource='' ){
  global $cfg, $configPrefix;

  $doLogging = $cfg->doesExist('logDir') && ($cfg->get('logDir') != '');

  if (!isset($doLogging) || !$doLogging || !isset($_SESSION['uid'])) return; // only log for authenticated users

  if ( !file_exists($cfg->get('logDir')))
    mkdir($cfg->get('logDir'), 0755, true) or trigger_error("Error creating folder ".$cfg->get('logDir'), E_USER_ERROR);
  $theLogFile = $cfg->get('logDir').( substr( $cfg->get('logDir'), -1 ) != '/' ? '/' : '' ).$configPrefix.$GLOBALS['url']->get('config').'_'.date("Y-m-d").'.log';

  if ( !(
      $logFile = fopen( $theLogFile, 'a+' )     // w ^= write and create (if not exist)
  ) )
    // alert file open error
    $url->put( 'sysalert='.$GLOBALS['lng']->get('errorOpeningFile').' ('.$theLogFile.')' );

  $logEntry = date("Y-m-d\tH:i:s")."\t".
      $_SESSION['uid']."\t".
      $_SESSION['surName']."\t".
      $_SESSION['foreName']."\t".
      $_SESSION['userName']."\t".
      $_SESSION['currentTeam']."\t".
      $_SESSION['userRights']."\t".
      $group."\t". /* system, prelab, lab */
      $operation."\t". /* login, save, open */
      $resource."\t".
      $GLOBALS['url']->get('address')."\t";
  $lastAddress = 'unknown';
  if (isset($_SERVER['HTTP_REFERER'])){
    // try to get last address
    if(preg_match('#address=([^\&]*)#', $_SERVER['HTTP_REFERER'], $regMatchArray)!=0){
      $lastAddress = $regMatchArray[1];
    }
  }
  $logEntry .= $lastAddress."\r\n";
  // timestamp actorUID actorClearname team action resource
  //echo( $logEntry.'<br>' );

  if ( !(
      fwrite( $logFile, $logEntry )
  ) )
    // alert that it didn't work
    $url->put( 'sysalert='.$lng->get('errorWritingFile').' ('.$theLogFile.')' );

  fclose($logFile);
}

class logActions
{
  // highest index: 21
  const login            = 0; // Fired on login
  const logout           = 1; // Fired on logout

  // open close events for relevant types
  const cOpen            = 2; // Fired when the TOC of the c is shown
  const cClose           = 3; // Fired when a c was the last page visited and now another one is shown
  const cEditorOpen      = 4;
  const cEditorSave      = 5;

  const pOpen            = 6;
  const pClose           = 7; // Fired when a p was the last page visited and now another one is shown
  const pEditorOpen      = 8;
  const pEditorSave      = 9;

  const iStudentOpen     = 10;
  const iStudentSave     = 11;
  const iStudentCancel   = 12;
  const iCorrectorOpen   = 13;
  const iCorrectorSave   = 14;
  const iCorrectorQuick  = 15;
  const iCorrectorCancel = 16;
  const iEditorOpen      = 17;
  const iEditorSave      = 18;

  const mOpen            = 19;
  const mSave            = 20;
  const mClose           = 21; // Fired when a m was the last page visited and now another one is shown
  const mCheck           = 22;
  const mIsCorrectAnswer = 23; // Fired immediately after mCheck
  const mIsWrongAnswer   = 24; // Fired immediately after mCheck
  const mEditorOpen      = 25;
  const mEditorSave      = 26;

  const lOpen            = 27; // Fired when allQ is shown or other l pages
  const lClose           = 28; // Fired when a l was the last page visited and now another one is shown
  const lPreLabFinished  = 29;
  const lLabOpen         = 30; // Fired again when reopened
  const lLabClose        = 31;
  const lLabCorrected    = 32;
  const lEditorOpen      = 33;
  const lEditorSave      = 34;
}

require_once( INCLUDE_DIR."/classes/Wdbc.inc" );
class Logger{
  var $myDBC;
  var $myTable;
  var $referrerID;
  var $doTheLogging;
  function Logger(){
    global $cfg;
    $this->myDBC   = new Wdbc();
    $this->myTable = "event_log";
    $referrerID = false;
    $this->doTheLogging = $cfg->doesExist('logEvents') && $cfg->get('logEvents') == '1';
  }


  function getReferrerID(){
    if ($this->referrerID){
      return $this->referrerID;
    }
    if (isset($_SERVER['HTTP_REFERER'])){
      // try to get last address
      if(preg_match('#address=([^\&]*)#', $_SERVER['HTTP_REFERER'], $regMatchArray)!=0){
        return $this->referrerID = $regMatchArray[1];
      }
    }
    return $this->referrerID = '';
  }

  /**
   * Checks if the referrer sent by the browser contains an address that is
   * different from the one currently called. If so a possible close event is logged.
   */
  function logReferrerEvent(){
    global $url;
    $previousAddress = $GLOBALS['Logger']->getReferrerID();
    if ($url->get("address") != $previousAddress){
      $previousElementID = strtolower(substr(array_pop( explode ( ".", $previousAddress ) ),0,1));
      if (in_array($previousElementID, array('p', 'l', 'm', 'c'))){
        $GLOBALS['Logger']->logToDatabase($previousAddress, eval('return logActions::'.$previousElementID.'Close;'));
      }
    }
  }

  /**
   * Creates a log entry in the log table of the working database.
   * $GLOBALS['Logger']->logToDatabase($fullAddress, logActions::iStudentOpen);
   * @param unknown $resourceID The full address of the resource. (full resource identifier.)
   * @param unknown $action The action from the logActions class defined above.
   * @param unknown $otherTeamID If set this team ID is used instead of the one of the user (e.g. for the correction).
   */
  function logToDatabase($resourceID, $action, $otherTeamID=''){
    global $cfg, $configPrefix;
    $doLogging = $this->doTheLogging && $cfg->doesExist('logDir') && ($cfg->get('logDir') != '');
    if (!isset($doLogging) || !$doLogging || !isset($_SESSION['uid'])) return; // only log for authenticated users

    $what = 'resourceID="'.$this->myDBC->escapeString($resourceID).'",'.
            'referrerID="'.$this->myDBC->escapeString($this->getReferrerID()).'",'.
            'teamNr="'.$this->myDBC->escapeString( ($otherTeamID==''?$_SESSION['currentTeam']:$otherTeamID) ).'",'.
            'userID="'.$this->myDBC->escapeString($_SESSION['uid']).'",'.
            'action="'.$action.'"';
    $this->myDBC->mkInsert($what, $this->myTable);
  }
}
$Logger = new Logger();
?>