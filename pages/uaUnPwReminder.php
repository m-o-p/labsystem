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
* @module     ../pages/uaUnPwRemainder.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
require( "../include/init.inc" );

$pge->title        = $lng->get("titleUaUnPwRem");
$pge->matchingMenu = $lng->get("MnuEntryUaUnPwRem");
$pge->visibleFor   = IS_GUEST;

  if ( substr( $url->get('config'), 0, 9 ) != 'useradmin' ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("TxtNotConfigUA")."\n</div>" );
  else{ // showing password fields
     // title
     $pge->put( "<div class=\"labsys_mop_h2\">__PAGETITLE__</div>\n" );
     
     // note
     if ( $lng->get("uaUnPwRemNote") != "" ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("uaUnPwRemNote")."</div>\n" );  
      
     $pge->put( "<FORM class=\"labsys_mop_std_form\" NAME=\"UnPwRemainder\" METHOD=\"POST\" ACTION=\"".$url->link2("../php/uaUnPwRemind.php")."\">\n".
                "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/uaUnPwReminder.php\">\n".
                "<fieldset><legend>".$lng->get("eMail")."</legend>\n".
                "<div class=\"labsys_mop_in_fieldset\">\n" );

     $pge->put( 
     // email address
                '<label for="eMail" class="labsys_mop_input_field_label_top">'.$lng->get('eMail').'</label>'."\n".
                '<input tabindex="'.$pge->nextTab++.'" type="text" id="eMail" name="EMAIL" class="labsys_mop_input_fullwidth" value="user@server.tld" />'."\n"
               );
               
     $pge->put( "</div>\n".
                "</fieldset>\n".
                "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\">\n".            
                "</FORM>"
               );
               
// focus
     $pge->put(
                '<script language="JavaScript" type="text/javascript">
                <!--
                if (document.UnPwRemainder){
                  document.UnPwRemainder.eMail.focus();
                  document.UnPwRemainder.eMail.select();
                }
                //-->
                </script>'
               );
  } // /showing reminder stuff

// show!
  require( $cfg->get("SystemPageLayoutFile") );   
?>
