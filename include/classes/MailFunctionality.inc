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
  function sendMail( $sender, $receiver, $subject, $text, $CCsender= FALSE, $BCC=FALSE, $additionalHeaders='' ){
    global $cfg, $usr;
    mail( ($BCC && !empty($sender) ? $sender : $receiver), // TO
          '['.$cfg->get("SystemTitle").'] '.$subject,
          $text.PHP_EOL.
              eval( 'return "'.$cfg->get('mailFooter').'";' ). // complicated? Well have to process \r\n and so on...
              PHP_EOL,
              ($CCsender ? 'CC: '.$sender.PHP_EOL:''). // CC
              ($BCC && !empty($sender) ? $receiver : ''). // BCC
              'From: '.(!empty($sender) ? $sender : $cfg->get('SystemTitle').' <noreply@'.$_SERVER['SERVER_NAME'].'>').PHP_EOL.
              "X-Mailer: PHP/".phpversion().PHP_EOL.
              (!empty($additionalHeaders)&&(strpos(strtoupper($additionalHeaders),'CONTENT-TYPE:') === false)?'Content-Type: text/plain; charset=UTF-8'.PHP_EOL:'').
              $additionalHeaders.
              'X-Sending-Username: '.$usr->userName.'@'.$cfg->get("SystemTitle").PHP_EOL. // this is for identifying a user (username must be correct...)
                  eval('return "'.$cfg->get("mailHeaderAdd").'";')); // necessary to process the \r\n ...
  }

  /**
   * Sends a mail to all users that belong to all groups given in the $accessRightsMask.
   * @param Integer $accessRightsMask   The mask of access rights that qualify the receiver.
   * @param String $subject             The mail subject.
   * @param String $text                The mail text.
   */
  function mail2groupWithRights( $accessRightsMask, $subject, $text ){
    require_once( INCLUDE_DIR."/classes/DBInterfaceUser.inc" );
    $uDBI = new DBInterfaceUser();
    require_once( INCLUDE_DIR."/classes/DBInterfaceUserRights.inc" );
    $urDBI = new DBInterfaceUserRights();
    $urDBI->getAllData();
    // iterate over all users that have rights
    while( $userRightsData = $urDBI->getNextData() ){
      if (ElementUser::isOfKind($accessRightsMask, $userRightsData["rights"])) {
        $userData = $uDBI->getData4( $userRightsData["uid"] );
        $this->sendMail('', $userData['eMail'], $subject, $text);
      }
    }
  }
}
$mailFunc = new MailFunctionality();
?>