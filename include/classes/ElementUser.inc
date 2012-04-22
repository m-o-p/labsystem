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
* This class holds the values of the user element and formats it for the output.
* It is a partial implementation of the elements in classes/elements/.
* It should not be displayable so it doesn't derive from classes/elements/Element.inc.
*/

/**
* @module     ../include/classes/ElementUser.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
class ElementUser {
  var $uid;
  var $userName;
  var $foreName;
  var $surName;
  var $currentTeam;
  var $userRights;
  var $mailAddress;
  var $history;

  function ElementUser( $uid, $userName, $foreName, $surName, $currentTeam, $userRights, $mailAddress, $history ){
    $this->uid                      = $uid;
    $this->userName                 = $userName;
    $this->foreName                 = $foreName;
    $this->surName                  = $surName;
    $this->currentTeam              = $currentTeam;
    $this->userRights               = $userRights;
    $this->mailAddress              = $mailAddress;
    $this->history                  = $history;
  }

  /**
  * Checks if the $usr->userRights or $ownRole are fulfilled.
  * @param $role          The bitmask (role) to check for.
  * @param $ownRole       If given, the $role is checked against this. (p.e. in helper.inc::rightsBox() ).
  * @return BOOL          True, if the rights are fulfilled false otherwise.
  */
  function isOfKind( $role, $ownRole="" ){
    if (!is_numeric( $role ) && !defined ( $role )){
      trigger_error( $role.": is not defined (ElementUser::isOfKind)", E_USER_WARNING );
      return false; // user role is not defined
    }

    if ( $role == 0 ) { // too riscious to return true here (the user_roles might not be included) 0 must be checked explicitly
                            trigger_error( $role.": ElementUser::isOfKind: checked for 0", E_USER_WARNING );
                            return false;
                        }
    if ($ownRole == "") $ownRole = $this->userRights;
    return ( (intval($ownRole) & intval($role)) == $role ); // checks whether all bits of $role are set in $ownRole
  }

  /**
  * Shows the relevant details in one row (for manageUsers).
  * @param $prefix        "$prefix_" is added to the Identifier (for form processing).
  * @param $disabled      Should the inputs be disabled?
  * @return               The html code of the row.
  */
  function showPropertyRow( $prefix, $disabled=false ){
    global $pge;
    return  "<div class=\"labsys_mop_u_row\">\n".
            showRightsHorizontal( 'ID'.$this->uid, $this->userRights, $disabled ).
            " <input type=\"text\" name=\"".$this->uid."_team\" tabindex=\"".$pge->nextTab++."\" class=\"labsys_mop_input_team\" value=\"".$this->currentTeam."\" onchange='isDirty=true'>\n".
            " ".$this->surName.", ".$this->foreName." (".$this->userName.")\n".
            " <a href=\"mailto:".$this->mailAddress."\"><img src=\"../syspix/button_mail_13x12.gif\" width=\"13\" height=\"12\" border=\"0\" title=\"".$this->mailAddress."\" alt=\"".$this->mailAddress."\"></a>\n".
            EB::history( $this->surName.", ".$this->foreName.":<br>\n".$this->history, "p1", true )."\n".
            "</div>\n";
  }

  /**
  * Shows the legend of the property rows (for manageUsers).
  * @return               The html code of the legend.
  */
  function showPropertyLegend(){
    global $lng, $pge;
    $lgnd = "<div class=\"labsys_mop_u_row\">\n".
            "<div class=\"labsys_mop_h3\">".$lng->get("legend")."</div>\n";
    for ($i=1; $i<=MAX_USER_ROLE; $i=$i<<1){
      for ($j=1; $j<$i; $j=$j<<1)
        $lgnd .= '<input type="checkbox" disabled>'.infoArrow( '', true )/* ."\n" saves space! */;
      $lgnd .= rightsBox( "legend_".$i, $i, $i )." ".$lng->get("Explain_UR_".$i)."<br>\n";
    }
    // team
    for ($j=1; $j<=MAX_USER_ROLE; $j=$j<<1)
      $lgnd .= '<input type="checkbox" disabled>'.infoArrow( '', true )/* ."\n" saves space! */;
    $lgnd .= "<input type=\"text\" name=\"legend\" tabindex=\"".$pge->nextTab++."\" class=\"labsys_mop_input_team\" value=\"".$lng->get("team")."\"><br>\n";
    $lgnd .= "</div>\n";
    return $lgnd;
  }
}
?>