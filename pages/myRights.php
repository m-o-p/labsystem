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
* This page is the interface to restrict one's rights temporarily.
* This makes sense to disable some edit functionality (and buttons)...
*
* @module     ../pages/myRights.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* HTML W3 VALIDATED!
*/
require( "../include/init.inc" );

$pge->title        = $lng->get("titleMyRightsPge");
$pge->matchingMenu = $lng->get("MnuEntryMyRights");
$pge->visibleFor   = IS_USER;

// title
  $pge->put( "<div class=\"labsys_mop_h2\">__PAGETITLE__</div>\n" );

// note
  if ( $lng->get("myRightsNote") != "" ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("myRightsNote")."</div>\n" );

require_once( INCLUDE_DIR."/classes/DBInterfaceUserRights.inc" );
$urDBI = new DBInterfaceUserRights();
$data = $urDBI->getData4( $_SESSION["uid"] );

$pge->put( "<FORM class=\"labsys_mop_std_form\" NAME=\"myRightsEdit\" METHOD=\"POST\" ACTION=\"".$url->link2("../php/changeMyRights.php")."\">\n".
           "<input type=\"hidden\" name=\"SESSION_ID\" value=\"".session_id()."\">\n".
           "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/myRights.php\">\n".
           "<fieldset><legend>".$lng->get("rights")."</legend>\n".
           "<div class=\"labsys_mop_in_fieldset\">\n" );
// user's possible rights      
  for ($i=2; $i<=MAX_USER_ROLE; $i=$i<<1)
    if ( $usr->isOfKind( $i, $usr->userRights ) ||  // is the user having this right
                                                    // only SU in not licensed mode (-> wont b
                                                    // able to change cause script checks!
         $usr->isOfKind( $i, $data['rights'] )      // is the user able to have this right
        )
      $pge->put( rightsBox( "UR_".$i, $i, $usr->userRights, false )."<label for=\"UR_".$i."\" class=\"labsys_mop_input_field_label\">".$lng->get("Explain_UR_".$i)."</label><br>\n" );
    
$pge->put( "</div>\n".
           "</fieldset>\n".
           "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("apply")."\" onclick='isDirty=false'>\n".            
           "</FORM>"
          );
  
// show!
  require( $cfg->get("SystemPageLayoutFile") );
?>
