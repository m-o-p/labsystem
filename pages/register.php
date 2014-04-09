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
* This page is part is used to register for the course.
* The data is written into the current userDB.
* The users get only the _ua right.
* The current course ID is stored in the applyFor field.
*
* @module     ../pages/register.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2011
* @version    1.0
*/

/**
 *  Checks if the registered mail address already exists in the system.
 *  Yes: The user gets the registerFor, reasonToParticipate, desiredTeamPartner fields set.
 *       Rest of personal data does not get changed.
 *  No:  New user is created with Username = Email.
 *       User gets only subscribed to $UA_CourseID.
 *       Password reminder Mail is sent with the new PW.
 *
 *  Thank you mail ($cfg->get('afterRegisterMailText')) is sent.
 *
 *  ! new users automatically belong to the course _unassigned !
 */

/**
 * Used config fields:
 *  - registerNote                  contains a note for the register page of this lab.
 *  - thankYouForRegisteringNote    shown after registration is done.
 *  - maxRegistrations              number of maximum people to register with this course id.
 *  - afterRegisterMailSubject
 *  - afterRegisterMailText
 *  - registerIPprefix              If set a string prefix matching is done for these addresses.
 *                                  Only computers with these address prefixes can register OR
 *  $_GET[$cfg->doesExist('registerAllowExternal')]=$cfg->doesExist('registerAllowExternal')    For external users...
 */

$UA_CourseID='_ua'; // to this ID new users get registered to automatically
$DEFAULT_PLACES=20; // the number of places offered when $cfg->get('maxRegistrations') is not set

require( "../include/init.inc" );

$pge->title        = $lng->get("titleRegister");
$pge->matchingMenu = $lng->get("MnuEntryRegister");

 // If the registerIPprefix variable is set in the config
 // one can only register if one is inside this prefix.
 // Prefixes can be separated by commas.
 $canRegister = true;
 if ($cfg->doesExist('registerIPprefix') &&
     !($cfg->doesExist('registerAllowExternal') && isset($_GET[$cfg->get('registerAllowExternal')]) && ($_GET[$cfg->get('registerAllowExternal')] == $cfg->get('registerAllowExternal')))
     ){
   $canRegister = false;
   $IPranges = explode(',', $cfg->get('registerIPprefix'));
   foreach ( $IPranges as $nextIPrange ){
     $nextIPrange = trim($nextIPrange);
     $canRegister |= (substr( $_SERVER['REMOTE_ADDR'], 0, strlen( $nextIPrange ) ) == $nextIPrange);
     if ($canRegister ) break;
   }
 }

if ( !$canRegister ) unset($_POST); // forget about all postet data...

// check form data
// eMail must contain @ and have a dot behind
if ( isset( $_POST['EMAIL'] ) &&
     ( (strpos( $_POST['EMAIL'], '@' ) === FALSE) ||
       (strrpos( $_POST['EMAIL'], '@' ) > strrpos( $_POST['EMAIL'], '.' ) ) ) )
    // alert
    $SYSALERT = $lng->get('uaMailInvalid');

if( isset($_POST['NAME']) && (( $_POST['NAME'] == '' ) || ( $_POST['FORENAME'] == '' )) )
    // alert
    $SYSALERT = $lng->get('uaSurNameEmpty');

 // title
 $pge->put( "<div class=\"labsys_mop_h2\">__PAGETITLE__</div>\n" );

 // new Interface to the userDB
 $userDBC = new DBConnection($cfg->get('UserDatabaseHost'),
                             $cfg->get('UserDatabaseUserName'),
                             $cfg->get('UserDatabasePassWord'),
                             $cfg->get('UserDatabaseName'));



if ( isset( $_POST['EMAIL'] ) && !isset($SYSALERT) ){ // data posted and no errors found.
  // process the custom fields (all fields that are added in the DB can be set...
    $customFields = '';
    // The following fields and those starting with "_" (course id)  will not be processed:
    $doNotListFromUser = Array( $cfg->get('UserDBField_username'),
                                $cfg->get('UserDBField_name'),
                                $cfg->get('UserDBField_forename'),
                                $cfg->get('UserDBField_email'),
                                $cfg->get('UserDBField_uid'),
                                $cfg->get('UserDBField_password'),
                                'last_registered',
                                'labsys_mop_last_change'
                               );
    foreach ( $_POST as $key => $value )
      if( substr( $key, 0, 11 ) == 'LABSYS_MOP_' ){ // all start with that prefix
        $key = substr( $key, 11 );
        if ( in_array( $key, $doNotListFromUser ) || ( $key[0] == '_' ) ) /* do nothing */;
        else $customFields .= $key."=CONCAT( '".$userDBC->escapeString( $value )." | ', ".$key.' ), ';
      }

  // switch to the user for successful field replacement.
    srand((double)microtime()*1000000);
    $usr->uid         = md5( $_POST['EMAIL'].uniqid( rand() ) );
    $usr->userName    = $_POST['EMAIL'];
    $usr->foreName    = $_POST['FORENAME'];
    $usr->surName     = $_POST['NAME'];
    $usr->mailAddress = $_POST['EMAIL'];

    // log the registration:
    $_SESSION['surName']=$usr->surName;
    $_SESSION['foreName']=$usr->foreName;
    $_SESSION['userName']=$usr->userName;
    $_SESSION['uid']='foo';
    $_SESSION['currentTeam']='';
    $_SESSION['userRights']=0;
    makeLogEntry( "system", "register", $configPrefix.$GLOBALS['url']->get('config') );

  // Is this email already registered?
   $result = $userDBC->mkSelect( "*",
                                 $cfg->get('UserDatabaseTable'),
                                 'UPPER('.$cfg->get('UserDBField_email').")=UPPER('".$userDBC->escapeString( $usr->mailAddress )."')"
                                );
   if ($userDBC->datasetsIn( $result ) > 0) // yes => just update the interest
    $userDBC->mkUpdate( $customFields.
                        'registerFor=\''.$cfg->get('User_courseID').' ('.$configPrefix.$GLOBALS['url']->get('config').')\', '.
                        "last_registered='".date('Y-m-d H:i:s')."', ".
                        '_unassigned=1, '.
                        "history=CONCAT( NOW(), ': ".$this->myDBC->escapeString( $cfg->get('User_courseID').' ('.$configPrefix.$GLOBALS['url']->get('config').')\'' ).
                                         "\\n', ".$this->myTable.".history )",
                        $cfg->get('UserDatabaseTable'),
                        'UPPER('.$cfg->get('UserDBField_email').")=UPPER('".$userDBC->escapeString( $_POST['EMAIL'] )."')" );
   else{ // email is new => create new entry

    // generate new Password:
        $newPW = substr( md5( uniqid( rand() ) ), 13, 8 );

        $userDBC->mkInsert( $customFields.
                            'registerFor=\''.$cfg->get('User_courseID').' ('.$configPrefix.$GLOBALS['url']->get('config').')\', '.
                            '_unassigned=1, '.
                            $cfg->get('UserDBField_username')."='".$userDBC->escapeString( $usr->userName )."', ".
                            $cfg->get('UserDBField_name')."='".$userDBC->escapeString( $usr->surName )."', ".
                            $cfg->get('UserDBField_forename')."='".$userDBC->escapeString( $usr->foreName )."', ".
                            $cfg->get('UserDBField_password')."='".crypt( $newPW, $usr->uid )."', ".
                            $cfg->get('UserDBField_email')."='".$userDBC->escapeString( $usr->mailAddress )."', ".
                            $cfg->get('UserDBField_uid')."='".$userDBC->escapeString( $usr->uid )."', ".
                            "last_registered='".date('Y-m-d H:i:s')."', ".
                            $UA_CourseID.'=1',
                            $cfg->get('UserDatabaseTable') );


    // new user... send password mail
    // Load mail element from pages:
    $mailPage = $GLOBALS["pDBI"]->getData2idx( $cfg->get('PidPasswordMail'));
    // replace constants using new user data from above:
    $pge->replaceConstants($mailPage->title);
    $pge->replaceConstants($mailPage->contents);

    mail( $_POST['EMAIL'],
         /*QPencode( */'['.$cfg->get("SystemTitle").'] '.$mailPage->title/* )*/,
         $mailPage->contents."\r\n\r\n".
         $lng->get('userName').': '.$_POST['EMAIL']."\r\n".
         $lng->get('passWord').': '.$newPW."\r\n".
         eval( 'return "'.$cfg->get('mailFooter').'";' ). // complicated? Well have to process \r\n and so on...
         "\r\n",
         "From: ".$cfg->get('SystemTitle')." <noreply@".$_SERVER['SERVER_NAME'].">\r\n".
         "X-Mailer: PHP/".phpversion()."\r\n".
         'X-Sending-Username: '.$usr->userName.'@'.$cfg->get("SystemTitle")."\r\n". // this is for identifying a user (username must be correct...)
         eval('return "'.$cfg->get("mailHeaderAdd").'";')); // necessary to process the \r\n ...
   }
   if ( $lng->get('thankYouForRegisteringNote') != '' ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get('thankYouForRegisteringNote')."</div>\n" );

   if ( $cfg->doesExist('PidAfterRegistrationPage') ){
   	$afterRegistrationPage = $GLOBALS["pDBI"]->getData2idx( $cfg->get('PidAfterRegistrationPage'));
   	$pge->title = $afterRegistrationPage->title;
   	parseHTML($afterRegistrationPage->contents);
   	$pge->put( "<div class=\"labsys_mop_note\">\n".$afterRegistrationPage->contents."</div>\n" );
   }

    // send after register mail
   // Load mail element from pages:
   $mailPage = $GLOBALS["pDBI"]->getData2idx( $cfg->get('PidRegistrationMail'));
   // replace constants using new user data from above:
   $pge->replaceConstants($mailPage->title);
   $pge->replaceConstants($mailPage->contents);

    mail( $_POST['EMAIL'],
         /*QPencode( */'['.$cfg->get("SystemTitle").'] '.$mailPage->title/* )*/,
         $mailPage->contents."\r\n\r\n".
         eval( 'return "'.$cfg->get('mailFooter').'";' ). // complicated? Well have to process \r\n and so on...
         "\r\n",
         "From: ".$cfg->get('SystemTitle')." <noreply@".$_SERVER['SERVER_NAME'].">\r\n".
         "X-Mailer: PHP/".phpversion()."\r\n".
         'X-Sending-Username: '.$usr->userName.'@'.$cfg->get("SystemTitle")."\r\n". // this is for identifying a user (username must be correct...)
         eval('return "'.$cfg->get("mailHeaderAdd").'";')); // necessary to process the \r\n ...

} // data posted and no errors found.
else{ // no data posted or errors found
     // note
     if ( $lng->get("registerNote") != "" ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("registerNote")."</div>\n" );

     // In this note may be specific information for this course thus it is bound to the cfg.
     if ( $cfg->doesExist("PidRegistrationPage") ){
     	$registrationPage = $GLOBALS["pDBI"]->getData2idx( $cfg->get('PidRegistrationPage'));
     	$pge->title = $registrationPage->title;
     	parseHTML($registrationPage->contents);
     	$pge->put( "<div class=\"labsys_mop_note\">\n".$registrationPage->contents."</div>\n" );
     }

     // query ALL column names
     $result = $userDBC->query( "SHOW COLUMNS FROM ".$cfg->get('UserDatabaseTable') ); // Attention: Breaks abstraction!
     $data = array();
     while( $data2 = mysql_fetch_array( $result ) ) $data[] = $data2['Field'];

     $pge->put( "<FORM class=\"labsys_mop_std_form\" NAME=\"myDataEdit\" METHOD=\"POST\" ACTION=\"#\">\n".
                "<input type=\"hidden\" name=\"REGISTER4\" value=\"".$cfg->get('User_courseID')."\">\n".
                "<fieldset><legend>".$lng->get("MnuEntryRegister").' | '.$cfg->get('User_courseID').' ('.$configPrefix.$GLOBALS['url']->get('config').') '."</legend>\n".
                "<div class=\"labsys_mop_in_fieldset\">\n" );

     $pge->put(
     // Warning when IP prefix not matched
                ( !$canRegister & $lng->doesExist('registerIPprefixViolationNote') ? "<div class=\"labsys_mop_note\" style=\"color: #ff5555;\">\n".$lng->get('registerIPprefixViolationNote').' ['.$_SERVER['REMOTE_ADDR'].']'."</div>\n" : '' ).
     // surName
                '<label for="surName" class="labsys_mop_input_field_label_top">'.$lng->get('surName').'</label>'."\n".
                '<input'.( $canRegister ? '' : ' disabled="disabled"' ).' tabindex="'.$pge->nextTab++.'" type="text" id="surName" name="NAME" class="labsys_mop_input_fullwidth" value="'.( isset( $_POST['NAME'] ) ? $_POST['NAME'] : $lng->get('surName') ).'" onchange="isDirty=true">'."\n".
     // foreName
                '<label for="name" class="labsys_mop_input_field_label_top">'.$lng->get('foreName').'</label>'."\n".
                '<input'.( $canRegister ? '' : ' disabled="disabled"' ).' tabindex="'.$pge->nextTab++.'" type="text" id="name" name="FORENAME" class="labsys_mop_input_fullwidth" value="'.( isset( $_POST['FORENAME'] ) ? $_POST['FORENAME'] : $lng->get('foreName') ).'" onchange="isDirty=true">'."\n".
     // email
                '<label for="email" class="labsys_mop_input_field_label_top">'.$lng->get('eMail').'</label>'."\n".
                '<input'.( $canRegister ? '' : ' disabled="disabled"' ).' tabindex="'.$pge->nextTab++.'" type="text" id="eMail" name="EMAIL" class="labsys_mop_input_fullwidth" value="'.( isset( $_POST['EMAIL'] ) ? $_POST['EMAIL'] : $lng->get('eMail') ).'" onchange="isDirty=true">'."\n"
               );

     // The rest of the fields.
     // Any additional database fields will be listed.
     // So if a field like "Matrikelnummer" is wanted just add it in the order you want to the table.
     // The following fields and those starting with "_" (course id)  will not be listed:
     $doNotListFromUser = Array( $cfg->get('UserDBField_username'),
                                 $cfg->get('UserDBField_name'),
                                 $cfg->get('UserDBField_forename'),
                                 $cfg->get('UserDBField_email'),
                                 $cfg->get('UserDBField_uid'),
                                 $cfg->get('UserDBField_password'),
                                 'last_registered',
                                 'labsys_mop_last_change',
                                 'registerFor'
                                );
     foreach ( $data as $key )
      if ( in_array( $key, $doNotListFromUser ) || ( $key[0] == '_' ) ) /* do nothing */;
      else $pge->put(
                     // new key
                        '<label for="labsys_mop_'.$key.'" class="labsys_mop_input_field_label_top">'.( $lng->doesExist($key) ? $lng->get($key) : $key ).'</label>'."\n".
                        '<input'.( $canRegister ? '' : ' disabled="disabled"' ).' tabindex="'.$pge->nextTab++.'" type="text" id="labsys_mop_'.$key.'" name="LABSYS_MOP_'.$key.'" class="labsys_mop_input_fullwidth" value="'.( isset( $_POST['LABSYS_MOP_'.$key] ) ? $_POST['LABSYS_MOP_'.$key]: $key ).'" onchange="isDirty=true">'."\n"
                     );

    // How many people registered from this course already?
     $result = $userDBC->mkSelect( 'registerFor',
                                   $cfg->get('UserDatabaseTable'),
                                   '_unassigned=1 && registerFor=\''.$cfg->get('User_courseID').' ('.$configPrefix.$GLOBALS['url']->get('config').')\''
                                  );
     $registrations = $userDBC->datasetsIn( $result ); // number of registrations under this courseID
     $max = ($cfg->doesExist('maxRegistrations') ? $cfg->get('maxRegistrations') : $registrations + $DEFAULT_PLACES);
     $fullplaces = min($registrations, $max);
     $icons = '';
     for ($i=0; $i<$fullplaces; $i++) $icons.='<img src="../syspix/fullPlace_11x12.gif" width="11" height="12" alt="X">';
     for (; $i<$max; $i++) $icons.='<img src="../syspix/freePlace_11x12.gif" width="11" height="12" alt="O">';
     for (; $i<$registrations; $i++) $icons.='<img src="../syspix/waitingPlace_11x12.gif" width="11" height="12" alt="+" title="on waiting list...">';

     $pge->put( "</div>\n".
                "</fieldset>\n".
                "<input".( $canRegister ? '' : ' disabled="disabled"' )." tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("apply")."\" onclick='isDirty=false'>\n".
                ' <div class="registerPlacesLeft">'.$icons.' '.$lng->get('placesLeft').': '.($max - $registrations).'/'.$max."</div>\n".
                "</FORM>"
               );

// focus
     $pge->put(
                '<script language="JavaScript" type="text/javascript">
                <!--
                if (document.myDataEdit) document.myDataEdit.surName.focus();
                //-->
                </script>'
               );
} // /no data posted or errors found

// show!
  require( $cfg->get("SystemPageLayoutFile") );
?>
