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

  const iOpen            = 10;
  const iStudentOpen     = 11;
  const iStudentSave     = 12;
  const iStudentCancel   = 13;
  const iCorrectorOpen   = 14;
  const iCorrectorSave   = 15;
  const iCorrectorQuick  = 16;
  const iCorrectorCancel = 17;
  const iEditorOpen      = 18;
  const iEditorSave      = 19;
  const iClose           = 20; // Fired by the view.php and the edit.php.

  const mOpen            = 21;
  const mClose           = 22; // Fired when a m was the last page visited and now another one is shown
  const mStudentOpen     = 23;
  const mStudentSave     = 24;
  const mCheck           = 25;
  const mIsCorrectAnswer = 26; // Fired immediately after mCheck
  const mIsWrongAnswer   = 27; // Fired immediately after mCheck
  const mEditorOpen      = 28;
  const mEditorSave      = 29;

  const lOpen            = 30; // Fired when allQ is shown or other l pages
  const lClose           = 31; // Fired when a l was the last page visited and now another one is shown
  const lPreLabFinished  = 32;
  const lLabOpen         = 33; // Fired again when reopened
  const lLabClose        = 34;
  const lLabCorrected    = 35;
  const lEditorOpen      = 36;
  const lEditorSave      = 37;

  const implicitClose    = 38; // A close the function for evaluating the data infers from the data.

  const ePubLoad         = 39;
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
      $previousFinal = array_pop( explode ( ".", $previousAddress ) );
      $previousElementID = strtolower(substr($previousFinal,0,1));
      if (in_array($previousElementID, array('p', 'l', 'm', 'c', 'i'))){
        $GLOBALS['Logger']->logToDatabase($previousAddress, eval('return logActions::'.$previousElementID.'Close;'));
      }else{
        if (in_array($previousFinal, array('allPrelabQ', 'preLabStatus', 'allLabQ', 'ALLlabStatus', 'labStatus', 'all', 'epub'))){
          $GLOBALS['Logger']->logToDatabase($previousAddress, eval('return logActions::lClose;'));
        }
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

    $what = array('resourceID' => $this->myDBC->escapeString($resourceID),
                  'referrerID' => $this->myDBC->escapeString($this->getReferrerID()),
                  'teamNr' => $this->myDBC->escapeString( ($otherTeamID==''?$_SESSION['currentTeam']:$otherTeamID) ),
                  'userID' => $this->myDBC->escapeString($_SESSION['uid']),
                  'action' => $action );
    $multiRowToInsert = '("'.$what['resourceID'].'","'.$what['referrerID'].'",'.$what['teamNr'].',"'.$what['userID'].'",'.$what['action'].')';

//     // Create implicit closes in all parents.
//     // Rationale: A problem when evaluating the log is that we do not always get
//     // close events. Thus we have to infer them. In this step we add potential close
//     // events.
//     // In the evaluation the wrong ones get removed again so that the database gets
//     // garbage collected.
//     if (($action == logActions::cClose)
//      || ($action == logActions::pClose)
//      || ($action == logActions::pEditorSave)
//      || ($action == logActions::iStudentSave)
//      || ($action == logActions::iStudentCancel)
//      || ($action == logActions::iCorrectorSave)
//      || ($action == logActions::iCorrectorQuick)
//      || ($action == logActions::iCorrectorCancel)
//      || ($action == logActions::mSave)
//      || ($action == logActions::mClose)
//      || ($action == logActions::lClose)
//      || ($action == logActions::lPreLabFinished)
//      || ($action == logActions::lLabClose)
//      || ($action == logActions::lLabCorrected)
//      || ($action == logActions::cEditorSave)
//      || ($action == logActions::iEditorSave)
//      || ($action == logActions::mEditorSave)
//      || ($action == logActions::lEditorSave)
//       ){
//       $what['referrerID'] = $what['resourceID'];
//       $what['action'] = logActions::implicitClose;
//       $what['resourceID'] = '';
//       foreach(explode('.', $resourceID) as $nextIDtoken){
//         if ($what['resourceID'] != ''){
//           $what['resourceID'] .= '.';
//         }
//         $what['resourceID'] .= $nextIDtoken;
//         $multiRowToInsert .= ',("'.$what['resourceID'].'","'.$what['referrerID'].'",'.$what['teamNr'].',"'.$what['userID'].'",'.$what['action'].')';
//       }
//    }

    $this->myDBC->mkMultiRowInsert('(resourceID,referrerID,teamNr,userID,action) VALUES '.$multiRowToInsert,
        $this->myTable);
  }
}
$Logger = new Logger();
?>