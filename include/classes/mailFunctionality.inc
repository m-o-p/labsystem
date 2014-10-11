<?php
class MailFunctionality{
  /**
   * Send a mail.
   * @param String $sender              The sender.
   * @param String $receiver            The list of receivers.
   * @param String $subject             The mail subject.
   * @param String $text                The mail text.
   * @param String $CCsender            Send mail via CC to sender?
   * @param String $BCC                 Send mail via BCC?
   */
  function sendMail( $sender, $receiver, $subject, $text, $CCsender= FALSE, $BCC=FALSE ){
    global $cfg, $usr;
    mail( ($BCC && !empty($sender) ? $sender : $receiver), // TO
          '['.$cfg->get("SystemTitle").'] '.$subject,
          $text."\r\n\r\n".
              eval( 'return "'.$cfg->get('mailFooter').'";' ). // complicated? Well have to process \r\n and so on...
              "\r\n",
              ($CCsender ? 'CC: '.$sender."\r\n":''). // CC
              ($BCC && !empty($sender) ? $receiver : ''). // BCC
              'From: '.(!empty($sender) ? $sender : $cfg->get('SystemTitle').' <noreply@'.$_SERVER['SERVER_NAME'].'>')."\r\n".
              "X-Mailer: PHP/".phpversion()."\r\n".
              'Content-Type: text/plain; charset=UTF-8'."\r\n".
              'X-Sending-Username: '.$usr->userName.'@'.$cfg->get("SystemTitle")."\r\n". // this is for identifying a user (username must be correct...)
                  eval('return "'.$cfg->get("mailHeaderAdd").'";')); // necessary to process the \r\n ...
  }

  /**
   * Sends a mail to all users that belong to all groups given in the $accessRightsMask.
   * @param Integer $accessRightsMask   The mask of access rights that qualify the receiver.
   * @param String $subject             The mail subject.
   * @param String $text                The mail text.
   */
  function mail2groupWithRights( $accessRightsMask, $subject, $text ){
    $urDBI = new DBInterfaceUserRights();
    $urDBI->getAllData();
    // iterate over all users that have rights
    while( $userRightsData = $urDBI->getNextData() ){
      if ($userRightsData["uid"] & $accessRightsMask) {
        $userData = $uDBI->getData4( $userRightsData["uid"] );
        $this->sendMail('', $userData['eMail'], $subject, $text);
      }
    }
  }
}
$mailFunc = new MailFunctionality();
?>