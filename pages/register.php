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
 */

$UA_CourseID='_ua'; // to this ID new users get registered to automatically
$DEFAULT_PLACES=20; // the number of places offered when $cfg->get('maxRegistrations') is not set

require( "../include/init.inc" );

$pge->title        = $lng->get("titleRegister");
$pge->matchingMenu = $lng->get("MnuEntryRegister");

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
                                'labsys_mop_last_change'
                               );
    foreach ( $_POST as $key => $value )
      if( substr( $key, 0, 11 ) == 'LABSYS_MOP_' ){ // all start with that prefix
        $key = substr( $key, 11 );
        if ( in_array( $key, $doNotListFromUser ) || ( $key[0] == '_' ) ) /* do nothing */;
        else $customFields .= $key."=CONCAT( '".$userDBC->escapeString( $value )." | ', ".$key.' ), ';
      }

  // Is this email already registered?
   $result = $userDBC->mkSelect( "*", 
                                 $cfg->get('UserDatabaseTable'), 
                                 'UPPER('.$cfg->get('UserDBField_email').")=UPPER('".$userDBC->escapeString( $_POST['EMAIL'] )."')" 
                                );
   if ($userDBC->datasetsIn( $result ) > 0) // yes => just update the interest
    $userDBC->mkUpdate( $customFields.
                        'registerFor=\''.$cfg->get('User_courseID').' ('.$configPrefix.$_GET['config'].')\', '.
                        '_unassigned=1',
                        $cfg->get('UserDatabaseTable'),
                        'UPPER('.$cfg->get('UserDBField_email').")=UPPER('".$userDBC->escapeString( $_POST['EMAIL'] )."')" );
   else{ // email is new => create new entry
       
    // generate new Password:
        srand((double)microtime()*1000000);
        $newPW = substr( md5( uniqid( rand() ) ), 13, 8 );
        
        $userDBC->mkInsert( $customFields.
                            'registerFor=\''.$cfg->get('User_courseID').' ('.$configPrefix.$_GET['config'].')\', '.
                            '_unassigned=1, '.
                            $cfg->get('UserDBField_username')."='".$userDBC->escapeString( $_POST['EMAIL'] )."', ".
                            $cfg->get('UserDBField_name')."='".$userDBC->escapeString( $_POST['NAME'] )."', ".
                            $cfg->get('UserDBField_forename')."='".$userDBC->escapeString( $_POST['FORENAME'] )."', ".
                            $cfg->get('UserDBField_password')."='".sha1( $newPW )."', ".
                            $cfg->get('UserDBField_email')."='".$userDBC->escapeString( $_POST['EMAIL'] )."', ".
                            $cfg->get('UserDBField_uid')."='".md5( $_POST['EMAIL'].uniqid( rand() ) )."', ".
                            $UA_CourseID.'=1',
                            $cfg->get('UserDatabaseTable') );
                            
    // new user... send password mail
    mail( $_POST['EMAIL'],
         /*QPencode( */'['.$cfg->get("SystemTitle").'] '.str_replace( $pge->replaceKey, $pge->replaceValue, $lng->get('uaUnPwRemSubject') )/* )*/, 
         str_replace( $pge->replaceKey, $pge->replaceValue,  eval( 'return "'.$lng->get('uaUnPwRemMailText').'";' ) )."\r\n\r\n".
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
   
   if ( $cfg->doesExist('thankYouForRegisteringNote') && ($cfg->get('registerNote') != '') ) $pge->put( "<div class=\"labsys_mop_note\">\n".$cfg->get('thankYouForRegisteringNote')."</div>\n" );
   
    // send after register mail
    mail( $_POST['EMAIL'],
         /*QPencode( */'['.$cfg->get("SystemTitle").'] '.str_replace( $pge->replaceKey, $pge->replaceValue, $cfg->get('afterRegisterMailSubject') )/* )*/, 
         str_replace( $pge->replaceKey, $pge->replaceValue,  eval( 'return "'.$cfg->get('afterRegisterMailText').'";' ) )."\r\n\r\n".
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
     if ( $cfg->doesExist("registerNote") && ($cfg->get("registerNote") != '') ) $pge->put( "<div class=\"labsys_mop_note\">\n".$cfg->get("registerNote")."</div>\n" );
                   
     // query ALL column names
     $result = $userDBC->query( "SHOW COLUMNS FROM ".$cfg->get('UserDatabaseTable') ); // Attention: Breaks abstraction!
     $data = array();
     while( $data2 = mysql_fetch_array( $result ) ) $data[] = $data2['Field'];

     $pge->put( "<FORM class=\"labsys_mop_std_form\" NAME=\"myDataEdit\" METHOD=\"POST\" ACTION=\"#\">\n".
                "<input type=\"hidden\" name=\"REGISTER4\" value=\"".$cfg->get('User_courseID')."\">\n".
                "<fieldset><legend>".$lng->get("MnuEntryRegister").' | '.$cfg->get('User_courseID').' ('.$configPrefix.$_GET['config'].')'."</legend>\n".
                "<div class=\"labsys_mop_in_fieldset\">\n" );

     $pge->put( 
     // surName
                '<label for="surName" class="labsys_mop_input_field_label_top">'.$lng->get('surName').'</label>'."\n".
                '<input tabindex="'.$pge->nextTab++.'" type="text" id="surName" name="NAME" class="labsys_mop_input_fullwidth" value="'.( isset( $_POST['NAME'] ) ? $_POST['NAME'] : $lng->get('surName') ).'" onchange="isDirty=true">'."\n".
     // foreName
                '<label for="name" class="labsys_mop_input_field_label_top">'.$lng->get('foreName').'</label>'."\n".
                '<input tabindex="'.$pge->nextTab++.'" type="text" id="name" name="FORENAME" class="labsys_mop_input_fullwidth" value="'.( isset( $_POST['FORENAME'] ) ? $_POST['FORENAME'] : $lng->get('foreName') ).'" onchange="isDirty=true">'."\n".
     // email
                '<label for="email" class="labsys_mop_input_field_label_top">'.$lng->get('eMail').'</label>'."\n".
                '<input tabindex="'.$pge->nextTab++.'" type="text" id="eMail" name="EMAIL" class="labsys_mop_input_fullwidth" value="'.( isset( $_POST['EMAIL'] ) ? $_POST['EMAIL'] : $lng->get('eMail') ).'" onchange="isDirty=true">'."\n"
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
                                 'labsys_mop_last_change',
                                 'registerFor'
                                );
     foreach ( $data as $key )
      if ( in_array( $key, $doNotListFromUser ) || ( $key[0] == '_' ) ) /* do nothing */;
      else $pge->put(
                     // new key
                        '<label for="labsys_mop_'.$key.'" class="labsys_mop_input_field_label_top">'.( $lng->doesExist($key) ? $lng->get($key) : $key ).'</label>'."\n".
                        '<input tabindex="'.$pge->nextTab++.'" type="text" id="labsys_mop_'.$key.'" name="LABSYS_MOP_'.$key.'" class="labsys_mop_input_fullwidth" value="'.( isset( $_POST['LABSYS_MOP_'.$key] ) ? $_POST['LABSYS_MOP_'.$key]: $key ).'" onchange="isDirty=true">'."\n"
                     );
        
    // How many people registered from this course already?
     $result = $userDBC->mkSelect( 'registerFor', 
                                   $cfg->get('UserDatabaseTable'), 
                                   '_unassigned=1 && registerFor=\''.$cfg->get('User_courseID').' ('.$configPrefix.$_GET['config'].')\''
                                  );
     $registrations = $userDBC->datasetsIn( $result ); // number of registrations under this courseID
     $max = ($cfg->doesExist('maxRegistrations') ? $cfg->get('maxRegistrations') : $registrations + $DEFAULT_PLACES);
     $remaining = $max - $registrations;
     if ($remaining<0) $remaining = 0;
     $icons = '';
     for ($i=0; $i<$remaining; $i++) $icons.='<img src="../syspix/freePlace_11x12.gif" width="11" height="12" alt="O">';
     for ($i=0; $i<$registrations; $i++) $icons.='<img src="../syspix/fullPlace_11x12.gif" width="11" height="12" alt="X">';
     
     $pge->put( "</div>\n".
                "</fieldset>\n".
                "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("apply")."\" onclick='isDirty=false'>\n".
                ' <div class="registerPlacesLeft">'.$lng->get('placesLeft').': '.$remaining.'/'.$max.' '.$icons."</div>\n".
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
