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
 * This page is part of the UserAdministration configuration and enables you to
 * GET A REMINDER EMAIL containing UserName and PassWord.
 *
 * @module ../pages/uaUnPwRemainder.php
 *
 * @author Marc-Oliver Pahl
 * @copyright Marc-Oliver Pahl 2016
 * @version 1.1
 *         
 */
require ("../include/init.inc");
require_once( "../include/config.inc" );

$pge->title = $lng->get ( "titleUaUnPwRem" );
$pge->matchingMenu = $lng->get ( "MnuEntryUaUnPwRem" );
$pge->visibleFor = IS_GUEST;
// write the pagetitle in any case
$pge->put ( "<div class=\"labsys_mop_h2\">__PAGETITLE__</div>\n" );
// we don't always need to write the body, control via variable
$writeBody = true;

// Something to process?
if (isset ( $_POST ['EMAIL'] ) || isset ( $_GET ['EMAIL'] )) {
	$requesterEmail = (isset ( $_POST ['EMAIL'] ) ? $_POST ['EMAIL'] : $_GET ['EMAIL']);
	// Load data to mail address
	// new Interface to the userDB
	$userDBC = new DBConnection ( $cfg->get ( 'UserDatabaseHost' ), $cfg->get ( 'UserDatabaseUserName' ), $cfg->get ( 'UserDatabasePassWord' ), $cfg->get ( 'UserDatabaseName' ) );
	
	// check if the mailAddress exists:
	$result = $userDBC->mkSelect ( 'pwReminderToken,pwReminderValidUntil,' . $cfg->get ( 'UserDBField_uid' ) . ' AS uid', $cfg->get ( 'UserDatabaseTable' ), 'UPPER(' . $cfg->get ( 'UserDBField_email' ) . ")=UPPER('" . $requesterEmail . "')" );
	if ($result->num_rows < 1)
		$pge->put ( "<div class=\"labsys_mop_note\">\n" . $requesterEmail . ' ' . $lng->get ( 'uaNotBelong2Usr' ) . "\n</div>" );
	else {
		$mailPage = $GLOBALS ["pDBI"]->getData2idx ( $cfg->get ( 'PidPasswordMail' ) );
		while ( $data = $result->fetch_assoc() ) {
			if (isset ( $_GET ['TOKEN'] ) && ($data ['pwReminderToken'] == $_GET ['TOKEN']) && ($data ['pwReminderValidUntil'] - time () > 0)) {
				// Token fits and is still valid
				// generate new Password:
				srand ( ( double ) microtime () * 1000000 );
				$newPW = substr ( sha1 ( uniqid ( rand () ) ), 13, 10 );
				
				// set the new password, reset token data
				$userDBC->mkUpdate ( 'pwReminderToken=DEFAULT, pwReminderValidUntil=DEFAULT, ' . $cfg->get ( 'UserDBField_password' ) . "='" . password_hash ( $newPW, PASSWORD_DEFAULT ) . "'", $cfg->get ( 'UserDatabaseTable' ), $cfg->get ( 'UserDBField_uid' ) . "='" . $data [$cfg->get ( 'UserDBField_uid' )] . "'" );
				
				// find out where the user can log in:
				$groupMemberships = '';
				$result = $userDBC->mkSelect ( '*', $cfg->get ( 'UserDatabaseTable' ), $cfg->get ( 'UserDBField_uid' ) . '="' . $data ['uid'] . '"' );
				while ( $memberData = $result->fetch_assoc() ) {
					foreach ( $memberData as $key => $value ) {
						if ($key [0] == '_' && $value == 1) {
							$groupMemberships .= substr ( $key, 1 ) . PHP_EOL;
						}
					}
				}
				$mailPage->contents .= $lng->get ( 'passWord' ) . ': ' . $newPW . PHP_EOL . PHP_EOL . $lng->get ( 'YouAreMemberOf' ) . ': ' . PHP_EOL . $groupMemberships;
			} else {
				// Store a new token and validity for 1 hour
				$token = sha1 ( uniqid ( rand () ) );
				$userDBC->mkUpdate ( 'pwReminderToken="' . $token . '", pwReminderValidUntil=' . (time () + 60 * 60), $cfg->get ( 'UserDatabaseTable' ), $cfg->get ( 'UserDBField_uid' ) . "='" . $data [$cfg->get ( 'UserDBField_uid' )] . "'" );
				$urlParts = parse_url ( 'http' . (isset ( $_SERVER ['HTTPS'] ) ? 's' : '') . '://' . $_SERVER ['HTTP_HOST'] . $_SERVER ['REQUEST_URI'] );
				
				$mailPage->contents .= 'https://' . $urlParts ['host'] . $urlParts ['path'] . '?config=useradmin&EMAIL=' . urlencode ( $requesterEmail ) . '&TOKEN=' . urlencode ( $token );
			}
			require_once (INCLUDE_DIR . "/classes/MailFunctionality.inc");
			$mailFunc->sendMail ( '', $data ['uid'], $mailPage->title, $mailPage->contents );
			$pge->put ( "<div class=\"labsys_mop_note\">" . $lng->get ( 'MailHasBeenSent' ) . "</div>" );
			// no further action from the user required for now, don't show the body
			$writeBody = false;
		}
	}
}

if (substr ( $url->get ( 'config' ), - 9 ) != 'useradmin')
	$pge->put ( "<div class=\"labsys_mop_note\">\n" . $lng->get ( "TxtNotConfigUA" ) . "\n</div>" );
else if ( $writeBody ) { // showing password fields
	// note
	if ($lng->get ( "uaUnPwRemNote" ) != "")
		$pge->put ( "<div class=\"labsys_mop_note\">\n" . $lng->get ( "uaUnPwRemNote" ) . "</div>\n" );
	
	$pge->put ( "<FORM class=\"labsys_mop_std_form\" NAME=\"UnPwRemainder\" METHOD=\"POST\" ACTION=\"#\">\n" . "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/uaUnPwReminder.php\">\n" . "<fieldset><legend>" . $lng->get ( "eMail" ) . "</legend>\n" . "<div class=\"labsys_mop_in_fieldset\">\n" );
	
	$pge->put ( 
			// email address
			'<label for="eMail" class="labsys_mop_input_field_label_top">' . $lng->get ( 'eMail' ) . '</label>' . "\n" . '<input tabindex="' . $pge->nextTab ++ . '" type="text" id="eMail" name="EMAIL" class="labsys_mop_input_fullwidth" value="user@server.tld" />' . "\n" );
	
	$pge->put ( "</div>\n" . "</fieldset>\n" . "<input tabindex=\"" . $pge->nextTab ++ . "\" type=\"submit\" class=\"labsys_mop_button\">\n" . "</FORM>" );
	
	// focus
	$pge->put ( '<script language="JavaScript" type="text/javascript">

                <!--

                if (document.UnPwRemainder){

                  document.UnPwRemainder.eMail.focus();

                  document.UnPwRemainder.eMail.select();

                }

                //-->

                </script>' );
} // /showing reminder stuff
  
// show!
require ($cfg->get ( "SystemPageLayoutFile" ));
?>
