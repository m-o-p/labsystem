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
* CHANGE YOUR PASSWORD.
*
* @module     ../pages/uaMyPW.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
require( "../include/init.inc" );

$pge->title        = $lng->get("titleUaMyPW");
$pge->matchingMenu = $lng->get("MnuEntryUaMyPW");
$pge->visibleFor   = IS_USER;

if ( !$pge->isVisible() ){ // directly show warning and close.
  require( $cfg->get("SystemPageLayoutFile") );
  exit;
}

  if ( substr( $url->get('config'), -9 ) != 'useradmin' ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("TxtNotConfigUA")."\n</div>" );
  else{ // showing password fields
     // title
     $pge->put( "<div class=\"labsys_mop_h2\">__PAGETITLE__</div>\n" );
      
     // note
     if ( $lng->get("uaMyPwNote") != "" ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("uaMyPwNote")."</div>\n" );  

     $pge->put( "<FORM class=\"labsys_mop_std_form\" NAME=\"myPWEdit\" METHOD=\"POST\" ACTION=\"".$url->link2("../php/uaMyPWSave.php")."\">\n".
                "<input type=\"hidden\" name=\"SESSION_ID\" value=\"".session_id()."\">\n".
                "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/uaMyPW.php\">\n".
                "<fieldset><legend>".$lng->get("passWord")."</legend>\n".
                "<div class=\"labsys_mop_in_fieldset\">\n" );

     $pge->put( 
     // new password input
                '<label for="newPW" class="labsys_mop_input_field_label_top">'.$lng->get('newPW').'</label>'."\n".
                '<input tabindex="'.$pge->nextTab++.'" type="password" id="newPW" name="NEWPW" class="labsys_mop_input_fullwidth" value="" onchange="isDirty=true">'."\n".
     // reType
                '<label for="newPWreType" class="labsys_mop_input_field_label_top">'.$lng->get('newPWreType').'</label>'."\n".
                '<input tabindex="'.$pge->nextTab++.'" type="password" id="newPWreType" name="NEWPWRETYPE" class="labsys_mop_input_fullwidth" value="" onchange="isDirty=true">'."\n"
               );
               
     $pge->put( "</div>\n".
                "</fieldset>\n".
                "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("apply")."\" onclick='isDirty=false'>\n".            
                "</FORM>"
               );
               
// focus
     $pge->put(
                '<script language="JavaScript" type="text/javascript">
                <!--
                if (document.myPWEdit) document.myPWEdit.newPW.focus();
                //-->
                </script>'
               );
  } // /showing myData

// show!
  require( $cfg->get("SystemPageLayoutFile") );   
?>
