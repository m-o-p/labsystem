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
* Called by pgeStdFileEdit.inc to save the modified file.
* To be able to save, php needs the write rights to the file!
* Only files registered in $allowedFiles get saved! (security)
*
* @module     ../php/saveFile.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $_POST['FILENAME']     Name and path of the file
* @param $_POST['FILECONTENT']  Contents of the file.
* @param $_POST['REDIRECTTO']   The address to redirect to after saving.
* @param $_POST['SESSION_ID']   To verify that the user is the user that set the call and is logged in.
*/

require( "../include/init.inc" );

if ( !isset($_POST['FILENAME']) ||
     !isset($_POST['FILECONTENT']) ||
     !isset($_POST['REDIRECTTO'])
   ){
      trigger_error( $lng->get( 'NotAllNecValPosted' ), E_USER_ERROR );
      exit;
     }

if (!($usr->isOfKind(IS_CONFIG_EDITOR) || // only those two can edit files!
      $usr->isOfKind(IS_CONTENT_EDITOR))){
      trigger_error( $lng->get("TxtNotEnoughRights"), E_USER_ERROR );
      exit;
}


// Only predefined files are allowed
// Otherwise this would be a security hole since any LOGGED IN IS_CONTENT_EDITOR
// or IS_CONFIG_EDITOR (sessionId) could save any file...
$allowedFiles = Array(  $cfg->get("SystemResourcePath").$cfg->get("SystemMenuFile"),
                        $cfg->get("UserStyleSheet")
                      );

if ( $usr->isOfKind(IS_CONFIG_EDITOR) &&
     isset($_POST['SAVEAS_PREFIX']) &&
     isset($_POST['SAVEAS_POSTFIX']) &&
     isset($_POST['SAVEAS'])
    ){
  $fileName = $_POST['SAVEAS_PREFIX'].$_POST['SAVEAS'].$_POST['SAVEAS_POSTFIX'];
} else {
  $fileName = $_POST['FILENAME'];
}
$fileExtension = substr( $fileName, strrpos( $fileName, '.' )+1 );

if ( !( isset($_POST['SESSION_ID']) &&
        ($_POST['SESSION_ID'] != "") &&
        ($_POST['SESSION_ID'] == session_id()) &&
        ( in_array ($fileName, $allowedFiles) || // from above...
          ($usr->isOfKind(IS_CONFIG_EDITOR) && ( // something in the ressource path:
                                                !( strpos( strtoupper($fileName), strtoupper($cfg->get("SystemResourcePath")) ) === false ) ||
                                                // something in the stylesheet path:
                                                !( strpos( strtoupper($fileName), substr( strtoupper($cfg->get("UserStyleSheet")), 0, strrpos( $cfg->get("UserStyleSheet"), '/' )) ) === false)
                                                ) &&
                                                (strpos($_POST['SAVEAS'], '/') === false) // no directory walks!
           )
         )
       ) /* valid call? */
   ){
      trigger_error( $lng->get( 'NotAllowedToMkCall' ), E_USER_ERROR );
      exit;
     }

// Tidy the CSS
if ((strtoupper($fileExtension) == 'CSS') && include_once( '../plugins/CSSTidy/class.csstidy.php') ){
  $css = new csstidy();
  $css->set_cfg('remove_last_;',TRUE);
  $css->set_cfg('preserve_css',TRUE);
  $css->parse($_POST['FILECONTENT']);
  $logText = 'CSSTidy: ';
  foreach ( $css->log as $logEntry ){
    $logText .= '  '.$logEntry[0]['t'].': '.$logEntry[0]['m']."\n";
  }
  if ( $logText != 'CSSTidy: ' ) $url->put( "sysalert=".$logText );
  $_POST['FILECONTENT'] = $css->print->plain();
}


// save
	if ( !(
          $theFile = fopen( $fileName, "w" )     // w ^= write and create (if not exist)
                                                      ) )
   // alert file open error
    $url->put( "sysalert=".$lng->get("errorOpeningFile")." (".$fileName.")" );
	elseif (
          fwrite( $theFile, html_entity_decode ($_POST['FILECONTENT']) )
                                                      )
       // note that it worked
        $url->put( "sysinfo=".$lng->get("DataHasBeenSaved")." (".$fileName.")" );
      else
       // alert that it didn't work
        $url->put( "sysalert=".$lng->get("errorWritingFile")." (".$fileName.")" );

	fclose( $theFile );

  makeLogEntry( 'system', 'saved file '.$fileName );

// redirect
  header( "Location: ".$url->rawLink2( urldecode($_POST['REDIRECTTO']).(isset( $_POST['SAVEAS'] ) ? '&file2edit='.urlencode($_POST['SAVEAS']) : '') ) );
?>