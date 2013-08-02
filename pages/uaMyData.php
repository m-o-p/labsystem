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
* CHANGE YOUR DATA.
*
* @module     ../pages/uaMyData.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
require( "../include/init.inc" );

$pge->title        = $lng->get("titleUaMyData");
$pge->matchingMenu = $lng->get("MnuEntryUaMyData");
$pge->visibleFor   = IS_USER;

if ( !$pge->isVisible() ){ // directly show warning and close.
  require( $cfg->get("SystemPageLayoutFile") );
  exit;
}

  if ( substr( $url->get('config'), -9 ) != 'useradmin' ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("TxtNotConfigUA")."\n</div>" );
  else{ // showing myData
     // title
     $pge->put( "<div class=\"labsys_mop_h2\">__PAGETITLE__</div>\n" );

     // note
     if ( $lng->get("uaMyDataNote") != "" ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("uaMyDataNote")."</div>\n" );


     // new Interface to the userDB
     $userDBC = new DBConnection($cfg->get('UserDatabaseHost'),
                                 $cfg->get('UserDatabaseUserName'),
                                 $cfg->get('UserDatabasePassWord'),
                                 $cfg->get('UserDatabaseName'));

     // query ALL fields
     $result = $userDBC->mkSelect( "*",
                                   $cfg->get('UserDatabaseTable'),
                                   $cfg->get('UserDBField_uid')."='".( $usr->isOfKind( IS_DB_USER_ADMIN ) && $usr->isSeeingSomeonesData() ? $usr->theSeeingUid()  : $usr->uid  )."'"
                                  );
     $data = mysql_fetch_assoc( $result ); // -> only associative array

     $pge->put( "<FORM class=\"labsys_mop_std_form\" NAME=\"myDataEdit\" METHOD=\"POST\" ACTION=\"".$url->link2("../php/uaMyDataSave.php")."\">\n".
                "<input type=\"hidden\" name=\"SESSION_ID\" value=\"".session_id()."\">\n".
                "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/uaMyData.php\">\n".
                "<fieldset><legend>".$lng->get("properties")."</legend>\n".
                "<div class=\"labsys_mop_in_fieldset\">\n" );

     $pge->put(
     // userName
                '<label for="userName" class="labsys_mop_input_field_label_top">'.$lng->get('userName').'</label>'."\n".
                '<input tabindex="'.$pge->nextTab++.'" type="text" id="userName" name="USERNAME" class="labsys_mop_input_fullwidth" value="'.returnEditable( $data[ $cfg->get('UserDBField_username') ] ).'" onchange="isDirty=true">'."\n".
     // surName
                '<label for="surName" class="labsys_mop_input_field_label_top">'.$lng->get('surName').'</label>'."\n".
                '<input tabindex="'.$pge->nextTab++.'" type="text" id="surName" name="NAME" class="labsys_mop_input_fullwidth" value="'.returnEditable( $data[ $cfg->get('UserDBField_name') ] ).'" onchange="isDirty=true">'."\n".
     // foreName
                '<label for="name" class="labsys_mop_input_field_label_top">'.$lng->get('foreName').'</label>'."\n".
                '<input tabindex="'.$pge->nextTab++.'" type="text" id="name" name="FORENAME" class="labsys_mop_input_fullwidth" value="'.returnEditable( $data[ $cfg->get('UserDBField_forename') ] ).'" onchange="isDirty=true">'."\n".
     // email
                '<label for="email" class="labsys_mop_input_field_label_top">'.$lng->get('eMail').'</label>'."\n".
                '<input tabindex="'.$pge->nextTab++.'" type="text" id="eMail" name="EMAIL" class="labsys_mop_input_fullwidth" value="'.returnEditable( $data[ $cfg->get('UserDBField_email') ] ).'" onchange="isDirty=true">'."\n"
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
     foreach ( $data as $key => $value ){
        if ( in_array( $key, $doNotListFromUser ) || ( $key[0] == '_' ) ){ /* do nothing */;
        }else{
          $pge->put(
                       // new key
                          '<label for="labsys_mop_'.$key.'" class="labsys_mop_input_field_label_top">'.$key.'</label>'."\n".
                          '<input tabindex="'.$pge->nextTab++.'" type="text" id="labsys_mop_'.$key.'" name="LABSYS_MOP_'.$key.'" class="labsys_mop_input_fullwidth" value="'.htmlentities( $value ).'" onchange="isDirty=true">'."\n"
                       );
        }
     }

     $pge->put( "</div>\n".
                "</fieldset>\n".
                "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("apply")."\" onclick='isDirty=false'>\n".
                "</FORM>"
               );

     // handle withdraw...
     if (isset($_POST['WithDrawMe']) && $_POST['WithDrawMe']==session_id()){
       // update the values
       $userDBC->mkUpdate( "`_unassigned`=0",
           $cfg->get('UserDatabaseTable'),
           $cfg->get('UserDBField_uid')."='".( $usr->isOfKind( IS_DB_USER_ADMIN ) && $usr->isSeeingSomeonesData() ?  $usr->theSeeingUid()  : $usr->uid  )."'"
       );
       $data['_unassigned']=0;
     }

     if ($usr->isOfKind( IS_DB_USER_ADMIN )){
       $data['_unassigned']=1;
       $data['registerFor']=$cfg->get('User_courseID').' ('.$configPrefix.$GLOBALS['url']->get('config').')';
     }

     if (($data['_unassigned']==1&&$data['registerFor']==$cfg->get('User_courseID').' ('.$configPrefix.$GLOBALS['url']->get('config').')')){
       // Show all other registered people...
       $pge->put('<h2 style="margin-top: 3em" class="labsys_mop_h2">'.$data['registerFor']."</h2>\n");

       // unassigned note
       if ( $lng->get("uaUnassignedNote") != "" ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("uaUnassignedNote")."</div>\n" );

       // participants list:
       $result = $userDBC->mkSelect( $cfg->get('UserDBField_name').','.$cfg->get('UserDBField_forename').',`desiredTeamPartner`',
           $cfg->get('UserDatabaseTable'),
           '`_unassigned`=1&&`registerFor`="'.$data['registerFor'].'"',
           '`last_registered` ASC'
       );

       $pge->put("<table class=\"uaOtherParticipantsTable\">\n");
       $pge->put("<tr><th style=\"text-align:right\">#</th><th>".$lng->get('surName').', '.$lng->get('foreName')."</th><th>".$lng->get('team')."</th></tr>");
       $counter=0;
       $maxPlaces = ($cfg->doesExist('maxRegistrations') ? $cfg->get('maxRegistrations') : 0);
       while($nextRegistree=mysql_fetch_assoc( $result )){
         $counter++;
         $pge->put("<tr><td style=\"text-align:right\">".
                   $counter.
                   ($counter<=$maxPlaces ? '<img src="../syspix/freePlace_11x12.gif" width="11" height="12" alt="O">' : '<img src="../syspix/fullPlace_11x12.gif" width="11" height="12" alt="X">').
                   "</td>
                    <td>".htmlentities($nextRegistree[$cfg->get('UserDBField_name')].', '.$nextRegistree[$cfg->get('UserDBField_forename')]).'</td>
                    <td>'.(isset($nextRegistree['desiredTeamPartner']) && $nextRegistree['desiredTeamPartner']!=''? htmlentities($nextRegistree['desiredTeamPartner']):'').'</td>
                    </tr>'."\n");
       }
       $pge->put("</table>\n");

       // withdraw note
       if ( $lng->get("uaWithdrawNote") != "" ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("uaWithdrawNote")."</div>\n" );

       // withdraw button
       $pge->put("<FORM class=\"labsys_mop_std_form\" style=\"text-align:center\" NAME=\"myDataEdit\" METHOD=\"POST\" ACTION=\"#\">");
       $pge->put("<input type=\"hidden\" name=\"WithDrawMe\" value=\"".session_id()."\">\n");
       $pge->put('<input tabindex="'.$pge->nextTab++.'" type="submit" class="labsys_mop_button" style="background-color:#faa" value="'.htmlentities( $lng->get('uaWithdrawButtonText') ).'" '.
           "onClick='return confirmLink(this, \"".$lng->get('uaWithdrawButtonText')."\");'".
           '>'."\n");
       $pge->put("</FORM>");
     }

// focus
     $pge->put(
                '<script language="JavaScript" type="text/javascript">
                <!--
                if (document.myDataEdit) document.myDataEdit.userName.focus();
                //-->
                </script>'
               );
  } // /showing myData

// show!
  require( $cfg->get("SystemPageLayoutFile") );
?>
