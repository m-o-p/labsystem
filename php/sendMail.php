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
* Called by ../pages/mailform.php to send a mail.
*
* @module     ../php/sendMail.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $_POST['POSSIBLE_RECVR'] Index of the highest MAIL2 possible.
* @param $_POST['MAIL2_?']        The mail receiver's address.
* @param $_POST['REDIRECTTO']     The address to redirect to after saving.
* @param $_POST['SESSION_ID']     To verify that the user is the user that set the call and is logged in.
*
* If you look at the code you will find out that the script mails to anyone in
* $_POST['MAIL2_'.$i]. So you can indeed mail to anyone if you manipulate these
* fields.
* I think thats toleratable since your user accounts mailaddress, name and account name get added anyway 
* and so you will probably be identified...
*/
require( "../include/init.inc" );

if ( !( isset($_POST['SESSION_ID']) && 
          ($_POST['SESSION_ID'] != "") && 
          ($_POST['SESSION_ID'] == session_id()) ) /* valid call? */   
       ){
          trigger_error( $lng->get("notAllowed"), E_USER_ERROR );
          exit;
         }

/**
  * Things like Umlaute must be encoded.
  * Since I was not able to look a proper function up I replace them by now...
  * This could be improved...
  */
function encodeMailconform( $text ){
  return str_replace( array( 'ä', 'Ä', 'ö', 'Ö', 'ü', 'Ü', 'ß' ),
                      array( 'ae', 'Ae', 'oe', 'Oe', 'ue', 'Ue', '3' ),
                      $text );
}
       
// format '"'  NAME  '"'  ' '  '<'  ADDRESS  '>'
  $mailFrom = '"'.$usr->foreName.' '.$usr->surName.'" <'.$usr->mailAddress.'>';

$i = 0;

$mailto = "";
if ( isset( $_POST['POSSIBLE_RECVR'] ) && ($_POST['POSSIBLE_RECVR'] > 0) )
  while( ++$i <= $_POST['POSSIBLE_RECVR'] ) if ( isset( $_POST['MAIL2_'.$i] ) ) 
    $mailto .= ", ".$_POST['MAIL2_'.$i];
  
$mailto = substr( $mailto, 2 );

/* User gets a copy? */
if ( isset( $_POST['NO_COPY_2_ME'] ) && ($_POST['NO_COPY_2_ME']==1) )
  $myAddr4Copy = '';
else
  $myAddr4Copy = $mailFrom;

$sendViaBCC = isset( $_POST['MAIL_VIA_BCC'] ) && $_POST['MAIL_VIA_BCC'] == '1';

if ( $mailto == "" ) $url->put( "sysalert=".$lng->get("NoReceiver") );
else{
    mail( encodeMailconform( ( $sendViaBCC ?  $mailFrom :  $mailto  ) ), // Bcc? -> mail to sender
          encodeMailconform( '['.$cfg->get("SystemTitle").'] '.$_POST['SUBJECT'] ), 
          encodeMailconform( str_replace( "\r\n", "\n", $_POST['MAILTEXT']).
                             eval( 'return "'.$cfg->get('mailFooter').'";' ) ). // complicated? Well have to process /r/n and so on...
                             "\r\n",
                             "From: ".encodeMailconform( $mailFrom )."\r\n".
                             encodeMailconform( ( $sendViaBCC ?  'Bcc: '.$mailto :  'Cc: '.$myAddr4Copy  ) )."\r\n". // Bcc? -> receivers
         "X-Mailer: PHP/".phpversion()."\r\n".
         'X-Sending-Username: '.$usr->userName.'@'.$cfg->get("SystemTitle")."\r\n". // this is for identifying a user (username must be correct...)
         eval('return "'.$cfg->get("mailHeaderAdd").'";')); // necessary to process the \r\n ...

    makeLogEntry( 'system', 'mail sent' );
    
    $url->put( "sysinfo=".$lng->get("MailHasBeenSent")." ".$mailFrom );
}

header( "Location: ".$url->rawLink2( $_POST['REDIRECTTO'] ) );
?>