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
* This page is to create userDB users.
*
* @module     ../pages/uaCreateUsers.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
require( "../include/init.inc" );
require_once( INCLUDE_DIR."/classes/DBInterfaceUser.inc" );

$pge->title        = $lng->get("titleUaCreateUsr");
$pge->matchingMenu = $lng->get("MnuEntryUaCreateUsr");
$pge->visibleFor   = IS_DB_USER_ADMIN;

if ( !$pge->isVisible() ){ // directly show warning and close.
  require( $cfg->get("SystemPageLayoutFile") );
  exit;
}

  if ( substr( $url->get('config'), -9 ) != 'useradmin' ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("TxtNotConfigUA")."\n</div>" );
  else{ // showing
  // title
     $pge->put( "<div class=\"labsys_mop_h2\">__PAGETITLE__</div>\n" );
     
  // note
     if ( $lng->get("uaCreateUsrNote") != "" ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("uaCreateUsrNote")."</div>\n" );  

  // new Interface to the userDB
    $userDBC = new DBConnection($cfg->get('UserDatabaseHost'), 
                                $cfg->get('UserDatabaseUserName'), 
                                $cfg->get('UserDatabasePassWord'), 
                                $cfg->get('UserDatabaseName'));

  // which courses exist?
    // ask for the couseID fields starting with _                         
    // list all columns
    $result = $userDBC->query( 'SHOW COLUMNS FROM '.$cfg->get('UserDatabaseTable') );
    $courseArray = Array();
    while( $data = mysql_fetch_array( $result ) )
      if ( substr( $data[0], 0, 1 ) == '_' ) array_push( $courseArray, $data[0] );

// legend
      $pge->put( "<div class=\"labsys_mop_u_row\">\n".
                 "<div class=\"labsys_mop_h3\">".$lng->get("legend")."</div>\n" );
    
      for ( $i=0; $i<count( $courseArray ); $i++){
          for ($j=1; $j<=$i; $j++) // empty boxes
            $pge->put( '<input type="checkbox" disabled>'.infoArrow( '', true )/* ."\n" saves space! */ );
          $pge->put( '<input type="checkbox" id="course_'.$i.'" name="LEGEND'.$courseArray[ $i ].'" value="0" tabindex="'.$pge->nextTab++.'" checked="checked">'.
                     '<label for="course_'.$i.'" class="labsys_mop_input_field_label">'.infoArrow( $courseArray[ $i ], false ).'</label>'/*."\n" removed to save space... */.
                     $courseArray[ $i ]."<br />\n" );
        }
      $pge->put( "</div>\n" );
// /legend

// form
    $pge->put( "<FORM class=\"labsys_mop_std_form\" NAME=\"createUser\" METHOD=\"POST\" ACTION=\"".$url->link2("../php/uaCreateUsers.php")."\">\n".
               "<input type=\"hidden\" name=\"SESSION_ID\" value=\"".session_id()."\">\n".
               "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/uaManageUsers.php\">\n"
              );
                    
    $pge->put( '<div class="labsys_mop_u_row">'."\n" );
    for ( $i=0; $i<count( $courseArray ); $i++)
      $pge->put( '<input type="checkbox" id="'.$courseArray[ $i ].'" name="'.$courseArray[ $i ].'" value="1" tabindex="'.$pge->nextTab++.'"'./* check this' id to enable those users to edit their data! */( $courseArray[ $i ] == $cfg->get( 'User_courseID' ) ?  ' checked="checked"' : '' ).' onchange="isDirty=true">'.
                 '<label for="'.$courseArray[ $i ].'" class="labsys_mop_input_field_label">'.infoArrow( $courseArray[ $i ], false ).'</label>'/*."\n" removed to save space... */
                );

      $pge->put( ' '.$lng->get( 'uaNewUsrsSubscript' )."<br />\n".
                 '<label for="mailAddresses" class="labsys_mop_input_field_label_top">'.$lng->get( 'mailaddressesNewUsr' ).'</label>'.
                 "<textarea tabindex=\"".$pge->nextTab++."\" id=\"mailAddresses\" name=\"MAILADDRESSES\" class=\"labsys_mop_textarea\" rows=\"".$cfg->get("uaCreateUsrsRows")."\" onchange='isDirty=true'>".
                 "</textarea>\n".
                 "</div>\n" );

// /form
    $pge->put( "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get('titleUaCreateUsr')."\" onclick='isDirty=false'>\n".            
               "</FORM>"
             );
} // /showing
// show!
  require( $cfg->get("SystemPageLayoutFile") );
?>
