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

// Only predefined files are allowed
// Otherwise this would be a security hole since any LOGGED IN USER (sessionId) could save any file...
$allowedFiles = Array(  $cfg->get("SystemResourcePath").$cfg->get("SystemMenuFile"),
                        $cfg->get("UserStyleSheet")
                      );
   
if ( !( isset($_POST['SESSION_ID']) && 
      ($_POST['SESSION_ID'] != "") && 
      ($_POST['SESSION_ID'] == session_id()) &&
      in_array ($_POST['FILENAME'], $allowedFiles) 
       ) /* valid call? */   
   ){
      trigger_error( $lng->get( 'NotAllowedToMkCall' ), E_USER_ERROR );
      exit;
     }


// Tidy the CSS
if (($_POST['FILENAME'] == $cfg->get("UserStyleSheet")) && include_once( '../plugins/CSSTidy/class.csstidy.php') ){
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
          $theFile = fopen( $_POST['FILENAME'], "w" )     // w ^= write and create (if not exist)
                                                      ) )
   // alert file open error
    $url->put( "sysalert=".$lng->get("errorOpeningFile")." (".$_POST['FILENAME'].")" );
	elseif (
          fwrite( $theFile, stripslashes( html_entity_decode ($_POST['FILECONTENT']) ) ) // slashes automatically added by posting
                                                      )
       // note that it worked
        $url->put( "sysinfo=".$lng->get("DataHasBeenSaved")." (".$_POST['FILENAME'].")" );
      else
       // alert that it didn't work
        $url->put( "sysalert=".$lng->get("errorWritingFile")." (".$_POST['FILENAME'].")" );
    
	fclose( $theFile );

  makeLogEntry( 'system', 'saved file '.$_POST['FILENAME'] );

// redirect
  header( "Location: ".$url->rawLink2( urldecode($_POST['REDIRECTTO']) ) );
?>