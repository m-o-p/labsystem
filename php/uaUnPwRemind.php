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
* Called by ../pages/uaUnPwReminder.php to start the reminding service.
* If the provided $_POST['EMAIL'] is found a mail with username and a newly generated Password will
* be sent to this address.
*
* @module     ../php/uaUnPwRemind.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $_POST['REDIRECTTO']   The address to redirect to after saving.
* @param $_POST['EMAIL']        The mailaddress of the user.
*/

require( "../include/init.inc" );

if ( !isset( $_POST['REDIRECTTO'] ) ||
     !isset( $_POST['EMAIL'] )
   ){
      trigger_error( $lng->get( 'NotAllNecValPosted' ), E_USER_ERROR );
      exit;
     }

if (  (substr( $url->get('config'), -9 ) != 'useradmin') // only in this configuration you are allowed to make that call!
   ){
      trigger_error( $lng->get( 'NotAllowedToMkCall' ), E_USER_ERROR );
      exit;
     }

// new Interface to the userDB
$userDBC = new DBConnection($cfg->get('UserDatabaseHost'),
                            $cfg->get('UserDatabaseUserName'),
                            $cfg->get('UserDatabasePassWord'),
                            $cfg->get('UserDatabaseName'));

// check if the mailAddress exists:
$result = $userDBC->mkSelect( $cfg->get('UserDBField_username').', '.
                              $cfg->get('UserDBField_uid').' AS uid',
                              $cfg->get('UserDatabaseTable'),
                              'UPPER('.$cfg->get('UserDBField_email').")=UPPER('".$_POST['EMAIL']."')"
                             );
if ( mysql_num_rows( $result ) < 1)
  // alert
  $url->put( 'sysalert='.$_POST['EMAIL'].' '.$lng->get('uaNotBelong2Usr') );
else{
  while( $data = mysql_fetch_assoc( $result ) ){
    // generate new Password:
    srand((double)microtime()*1000000);
    $newPW = substr( md5( uniqid( rand() ) ), 13, 8 );

    // set the new password
    $userDBC->mkUpdate( $cfg->get('UserDBField_password')."='".crypt( $newPW, $data['uid'] )."'",  // the UID is used as salt
                        $cfg->get('UserDatabaseTable'),
                        $cfg->get('UserDBField_uid')."='".$data[ $cfg->get('UserDBField_uid') ]."'"
                       );

    // send the reminding mail
    // switch to the user for successfull field replacement.
    if (!$usr->isOfKind( IS_CORRECTOR )){
      $usr->userRights += IS_CORRECTOR; // only correctors can load user data...
    }
    $requestingUsr = $usr->getUserInformation($data[ 'uid' ]);
    $usr->userName    = $requestingUsr['userName'];
    $usr->foreName    = $requestingUsr['foreName'];
    $usr->surName     = $requestingUsr['name'];
    $usr->mailAddress = $_POST['EMAIL'];

    // Load mail element from pages:
    $mailPage = $GLOBALS["pDBI"]->getData2idx( $cfg->get('PidPasswordMail'));
    // replace constants using new user data from above:
    $pge->replaceConstants($mailPage->title);
    $pge->replaceConstants($mailPage->contents);

    $senderAddr = $cfg->get('SystemTitle')." <noreply@".$_SERVER['SERVER_NAME'].'>';

    mail( $usr->mailAddress,
         /*QPencode( */'['.$cfg->get("SystemTitle").'] '.$mailPage->title/* )*/,
         $mailPage->contents."\r\n\r\n".
         $lng->get('userName').': '.$data[ $cfg->get('UserDBField_username') ]."\r\n".
         $lng->get('passWord').': '.$newPW."\r\n".
         eval( 'return "'.$cfg->get('mailFooter').'";' ). // complicated? Well have to process \r\n and so on...
         "\r\n",
         "From: ".$senderAddr."\r\n".
         "X-Mailer: PHP/".phpversion()."\r\n".
         'X-Sending-Username: '.$usr->userName.'@'.$cfg->get("SystemTitle")."\r\n". // this is for identifying a user (username must be correct...)
         eval('return "'.$cfg->get("mailHeaderAdd").'";')); // necessary to process the \r\n ...
  }

  $url->put( "sysinfo=".$lng->get('MailHasBeenSent').' '.htmlentities( $senderAddr, ENT_QUOTES | ENT_IGNORE  )/*.' '.$newPW*/ );
}

// redirect
  header( "Location: ".$url->rawLink2( urldecode($_POST['REDIRECTTO']) ) );
?>
