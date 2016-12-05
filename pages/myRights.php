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

if ( !$pge->isVisible() ){ // directly show warning and close.
  require( $cfg->get("SystemPageLayoutFile") );
  exit;
}

// title
  $pge->put( "<div class=\"labsys_mop_h2\">__PAGETITLE__</div>\n" );

// note
  if ( $lng->get("myRightsNote") != "" ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("myRightsNote")."</div>\n" );

$notRemovableRights = intval(IS_USER);
$newUsrRights = $notRemovableRights;

$pge->put( '<FORM class="labsys_mop_std_form" NAME="myRightsEdit" METHOD="POST" ACTION="#">'.PHP_EOL.
           "<fieldset><legend>".$lng->get("rights").' ('.$usr->userRights.'/'.$usr->userMaximumRights.')</legend>'.PHP_EOL.
           "<div class=\"labsys_mop_in_fieldset\">\n" );
// user's possible rights
  for ($i=1; $i<=MAX_USER_ROLE; $i=$i<<1){
    if ( $usr->isOfKind( $i, $usr->userMaximumRights ) ){ // does the user have this right?
    	if (isset($_POST['UR_'.$i]) && $_POST['UR_'.$i]==$i ){
    		$newUsrRights |= $i;
    	}
      $pge->put( rightsBox( "UR_".$i, $i, (!empty($_POST) ? $newUsrRights : $usr->userRights), ($i & $notRemovableRights == $i) )."<label for=\"UR_".$i."\" class=\"labsys_mop_input_field_label\">".$lng->get("Explain_UR_".$i)." ($i)</label><br>\n" );
    }
  }
  if (!empty($_POST) && ($newUsrRights != $usr->userRights)){
  	if (!isset($_POST['thisTabOnly'])){
  		$usr->saveCurrentRights($newUsrRights);
  		makeLogEntry( 'system', 'user rights changed to '.$newUsrRights.'/'.$usr->userMaximumRights );
  		$url->rem('myRights');
  	}else{
  		//only in this tab...
  		$url->put('myRights', $newUsrRights);
  		$usr->userRights=$newUsrRights; // validity checked above already!
  	}
  }
    
$pge->put( "</div>\n".
           "</fieldset>\n".
           "<input tabindex=\"".++$pge->nextTab."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("apply")."\" onclick='isDirty=false'>".PHP_EOL.
		   '<input type="checkbox" id="thisTabOnly" name="thisTabOnly" value="1" tabindex="'.$pge->nextTab++.'"'.( isset($_POST['thisTabOnly'])?' checked="checked"':'').' onchange="isDirty=true"/>'.PHP_EOL.
		   '<label for="thisTabOnly" class="labsys_mop_input_field_label">'.infoArrow( $lng->get('changeRightsInThisTabOnly'), false ).' '.$lng->get('changeRightsInThisTabOnly').'</label>'.PHP_EOL.
		   "</FORM>"
          );

// show!
  require( $cfg->get("SystemPageLayoutFile") );
?>
