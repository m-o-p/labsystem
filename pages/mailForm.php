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
* This page is for sending mails to the users.
*
* @module     ../pages/mailForm.php
* @author     Marc-Oliver Pahl
*/
require( "../include/init.inc" );

$pge->title       = $lng->get("titleSendMail");
$pge->matchingMenu= "send mail";
$pge->visibleFor  = IS_USER;

if ( !$pge->isVisible() ){ // directly show warning and close.
  require( $cfg->get("SystemPageLayoutFile") );
  exit;
}

if (isset($_POST['SESSION_ID'])){
  require("../php/sendMail.inc");
}

require_once( INCLUDE_DIR."/classes/DBInterfaceUser.inc" );
$uDBI = new DBInterfaceUser();

require_once( INCLUDE_DIR."/classes/DBInterfaceUserRights.inc" );

$content = "
<script type=\"text/javascript\" language=\"javascript\">
<!--
/**
 * Checks/unchecks all MailReceivers
 *
 * @param   boolean  whether to check or to uncheck the element
 *
 * @return  boolean  always true
 */
function setCheckboxes(do_check)
{
  var i = 0;
  while (typeof(document.MailForm.elements[i]) != 'undefined')
    if ( document.MailForm.elements[i].type == 'checkbox' && document.MailForm.elements[i].name != 'NO_COPY_2_ME')
      document.MailForm.elements[i++].checked = do_check;
    else i++;
  return true;
} // end of the 'setCheckboxes()' function
//-->
</script>
";


$allSupporter = array();
$allOther = array();

$urDBI = new DBInterfaceUserRights();
$urDBI->getAllData();
// iterate over all users that have rights
while( $userRightsData = $urDBI->getNextData() ){
  $userData = $uDBI->getData4( $userRightsData["uid"] );
 // create the user element
  $user = new ElementUser( $userData["uid"], $userData["userName"], $userData["foreName"], $userData["name"], $userRightsData["currentTeam"], $userRightsData["rights"], $userData["eMail"], $userRightsData["history"] );
 // ignore user that have no mailaddress or are no IS_MAIL_RECEIVER
  if ( !$user->isOfKind( IS_MAIL_RECEIVER ) || !$userData || ($userData["eMail"] == "") ) continue;
 // Distinguish between mail supporters and others
  if ( $user->isOfKind( IS_MAIL_SUPPORTER ) )
    $allSupporter[$user->surName.', '.$user->foreName] = $user->foreName.' '.$user->surName.' <'.$user->mailAddress.'>'; // format '"'  NAME  '"'  ' '  '<'  ADDRESS  '>'
  else
    $allOther[( ($user->surName != '') || ($user->foreName != '') ?
                            $user->surName.", ".$user->foreName :
                            $user->userName
                           ).
                 ' ('.$user->currentTeam.')'] = $user->foreName.' '.$user->surName.' <'.$user->mailAddress.'>'; // format '"'  NAME  '"'  ' '  '<'  ADDRESS  '>'
}

// For useradmin show all groups:
if ( (substr( $url->get('config'), -9 ) == 'useradmin') && $usr->isOfKind( IS_ALL_MAILER ) ){
  // new Interface to the userDB
  $userDBC = new DBConnection($cfg->get('UserDatabaseHost'),
                              $cfg->get('UserDatabaseUserName'),
                              $cfg->get('UserDatabasePassWord'),
                              $cfg->get('UserDatabaseName'));
  $result = $userDBC->query( 'SHOW COLUMNS FROM '.$cfg->get('UserDatabaseTable') );
  while( $data = mysql_fetch_array( $result ) ){
    if ( substr( $data[0], 0, 1 ) == '_' ){
      $allOther['#'.$data[0]] = '#'.$data[0];
    }
  }
}

// sort them alphabetically (for different ordering change here and above at the insertion code).
ksort( $allSupporter );
ksort( $allOther );

$counter=0;

$checkAll = $GLOBALS['url']->available('checkAll');

$allSupporterInputs = "";
foreach( $allSupporter as $key => $value ) $allSupporterInputs .= "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" id=\"MAIL2_".++$counter."\" name=\"MAIL2_".$counter."\" value=\"".htmlentities($value)."\"".( $checkAll || isset($_POST['MAIL2_'.$counter]) ?  " checked=\"checked\" "  : '' )." onchange='isDirty=true'>".
                                                                  "<label for=\"MAIL2_".$counter."\" class=\"labsys_mop_input_field_label\">".$key."</label><br />\n";
$allOtherInputs = "";
if ( $usr->isOfKind( IS_ALL_MAILER ) )
  foreach( $allOther as $key => $value ) $allOtherInputs         .= "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" id=\"MAIL2_".++$counter."\" name=\"MAIL2_".$counter."\" value=\"".htmlentities($value)."\"".( $checkAll || isset($_POST['MAIL2_'.$counter]) ?  " checked=\"checked\" "  : '' ).
  ($key[0]=='#' ? " onclick='document.getElementById(\"mailViaBcc\").checked=true'":'')." onchange='isDirty=true'>".
                                                                    "<label for=\"MAIL2_".$counter."\" class=\"labsys_mop_input_field_label\">".$key."</label><br />\n";

$content .= "<FORM class=\"labsys_mop_std_form\" NAME=\"MailForm\" METHOD=\"POST\" ACTION=\"".$url->link2("../pages/mailForm.php")."\">\n".
            "<input type=\"hidden\" name=\"SESSION_ID\" value=\"".session_id()."\">\n".
            "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/mailForm.php\">\n". /* index of saved el. will be added on save.php! */
            "<input type=\"hidden\" name=\"POSSIBLE_RECVR\" value=\"".$counter."\">\n".
            "<table class=\"labsys_mop_mailform_table\">\n".
            "<tr><td class=\"labsys_mop_mailform_table_mail2\"></td><td class=\"labsys_mop_mailform_table_the_mail\"></td></tr>\n".

           // the mail2 row
            "<tr><td class=\"labsys_mop_mailform_table_mail2\">\n".

            ( $usr->isOfKind( IS_ALL_MAILER ) ?
                        "<fieldset><legend>".$lng->get("roundmail")."</legend>\n".
                        "<a href=\"".$url->link2("../pages/mailForm.php?checkAll")."\" onclick=\"setCheckboxes(true); return false;\">".$lng->get("checkAll")."</a>/ \n".
                        "<a href=\"".$url->link2("../pages/mailForm.php")."\" onclick=\"setCheckboxes(false); return false;\">".$lng->get("unCheckAll")."</a><br />\n".
                        "</fieldset>\n" : ''
                      ).

            "<fieldset><legend>".$lng->get("labSupporter")."</legend>\n".
            $allSupporterInputs.
            "</fieldset>\n".

            ( $usr->isOfKind( IS_ALL_MAILER )?
                        "<fieldset><legend>".$lng->get("otherUser")."</legend>\n".
                        $allOtherInputs.
                        "</fieldset>\n" : ''
                      ).

            "</td>".

           // the mail subject/ body side
            "<td class=\"labsys_mop_mailform_table_the_mail\">\n".

            "<fieldset><legend>".$lng->get("yourMail")."</legend>\n".
           // subject
            "<label for=\"subject\" class=\"labsys_mop_input_field_label_top\">".$lng->get("subject")."</label>".
            "<input tabindex=\"".$pge->nextTab++."\" type=\"text\" id=\"subject\" name=\"SUBJECT\" class=\"labsys_mop_input_fullwidth\" maxlength=\"255\" value=\"".
            (isset($_POST['SUBJECT']) ? $_POST['SUBJECT'] :
                ( $url->available('subject') ? $url->get('subject') : '') )."\" onchange='isDirty=true'>\n".
           // mailbody
            "<label for=\"mailtext\" class=\"labsys_mop_input_field_label_top\">".$lng->get("message")."</label>".
            "<textarea tabindex=\"".$pge->nextTab++."\" id=\"mailtext\" name=\"MAILTEXT\" class=\"labsys_mop_textarea\" rows=\"".$cfg->get("sendMailBodyRows")."\" onchange='isDirty=true'>".
            (isset($_POST['MAILTEXT']) ? $_POST['MAILTEXT'] :
            	( $url->available('text') ? $url->get('text') : '') ).
            "</textarea>\n".
            "</fieldset>\n".

            "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" id=\"noCopy2Me\" name=\"NO_COPY_2_ME\" value=\"1\" checked=\"checked\" onchange='isDirty=true'>".
            "<label for=\"noCopy2Me\" class=\"labsys_mop_input_field_label\">".$lng->get("noCopy2Me")."</label><br />\r\n".

            "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" id=\"mailViaBcc\" name=\"MAIL_VIA_BCC\" value=\"1\"".( $cfg->get("mailViaBcc") == '1' ?  " checked=\"checked\""  : '' )." onchange='isDirty=true'>".
            "<label for=\"mailViaBcc\" class=\"labsys_mop_input_field_label\">".$lng->get("mailViaBcc")."</label>".

            "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button_fullwidth\" value=\"".$lng->get("sendMail")."\" accesskey=\"s\" onclick='isDirty=false'>\n".

            "</td></tr>\n".
            "</table>\n".

            "</FORM>\n".
            '<script language="JavaScript" type="text/javascript">
            <!--
            if (document.MailForm) document.MailForm.subject.focus();
            //-->
            </script>';

$pge->put('<div class="labsys_mop_h2">'.$pge->title."</div>\n".
          $content );

require( $cfg->get("SystemPageLayoutFile") );
?>
