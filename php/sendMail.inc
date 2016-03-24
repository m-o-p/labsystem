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
 * Called via require by ../pages/mailform.php to send a mail.
 *
 * @module     ../php/sendMail.inc
 * @author     Marc-Oliver Pahl
 *
 * @param $_POST['POSSIBLE_RECVR'] Index of the highest MAIL2 possible.
 * @param $_POST['MAIL2_?']        The mail receiver's address.
 */

if ( !( $usr->isOfKind( IS_USER ) ) /* valid call? */
){
  trigger_error( $lng->get("notAllowed"), E_USER_ERROR );
  exit;
}

require_once( INCLUDE_DIR."/classes/MailFunctionality.inc" );

if ( isset( $_POST['POSSIBLE_RECVR'] ) && ($_POST['POSSIBLE_RECVR'] > 0) ){
	$sender = $usr->foreName.' '.$usr->surName.' <'.$usr->mailAddress.'>';
	$i = 0;
	while( ++$i <= $_POST['POSSIBLE_RECVR'] ) if ( isset( $_POST['MAIL2_'.$i] ) ){
		$nextID = $_POST['MAIL2_'.$i];
		
		// Useradmin "whole course" special treatment
		// The fields to be selected are labelled "MAIL2_[number]=#*[courseID]"
		if (substr($nextID, 0, 2) == '#*' && (substr( $url->get('config'), -9 ) == 'useradmin') && $usr->isOfKind( IS_ALL_MAILER )){  
			// collect all participants of the course
			// new Interface to the userDB
			$userDBC = new DBConnection($cfg->get('UserDatabaseHost'),
					$cfg->get('UserDatabaseUserName'),
					$cfg->get('UserDatabasePassWord'),
					$cfg->get('UserDatabaseName'));
			// The field names are taken from the config*.ini file
			$what = $cfg->get("UserDBField_name")." AS name, ".
					$cfg->get("UserDBField_forename")." AS forename, ".
					$cfg->get('UserDBField_username')." AS username, ".
					$cfg->get('UserDBField_uid').' AS uid, '.
					$cfg->get("UserDBField_email")." AS email";
			// add all receivers
			$where= substr($nextID,2)."='1'";
			$result = $userDBC->mkSelect( $what, $cfg->get('UserDatabaseTable'), $where );
			while( $data = mysql_fetch_array( $result ) ){
				$mailFunc->sendMail($sender, $data[ 'uid' ], $_POST['SUBJECT'], $_POST['MAILTEXT'], isset($_POST['COPY2ME']) && ($_POST['COPY2ME'] == 1));
			}
		} else {
		// // Useradmin "whole course" special treatment

			$mailFunc->sendMail($sender, $nextID, $_POST['SUBJECT'], $_POST['MAILTEXT'], isset($_POST['COPY2ME']) && ($_POST['COPY2ME'] == 1));
		}
		
		makeLogEntry( 'system', 'mail sent' );
		
		$url->put( "sysinfo=".$lng->get("MailHasBeenSent")." ".htmlentities($sender) );
	}
}else{
	$url->put( "sysalert=".$lng->get("NoReceiver") );
}
?>