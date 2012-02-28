<?php
/**
 *  labsystem.m-o-p.de - 
 *                  the web based eLearning tool for practical exercises
 *  Copyright (C) 2011  Marc-Oliver Pahl
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
* Edit page for the main menu ini-file.
*
* @module     ../pages/accessableLabs.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2011
* @version    1.0
*/
require( "../include/init.inc" );

$pge->matchingMenu = $lng->get('MnuEntryCourseContent');
$pge->title        = $cfg->get('SystemTitle').' '.$lng->get('MnuEntryCourseContent');
$returnEpub = $url->available( 'ePub' );

  // head (create new)
     if ( !$returnEpub && $usr->isOfKind( IS_CONTENT_EDITOR ) ) $pge->put(  "<div class=\"labsys_mop_elements_menu_p\">\n".
           EB::mkLink(  $url->link2( '../pages/accessableLabs.php?ePub=ePub' ),
                        "<img src=\"../syspix/button_epub_12x12.gif\" width=\"12\" height=\"12\" border=\"0\" alt=\"link to\" title=\"".$lng->get("explainLink2epub")."\">" ).
           EB::link2Url( '../pages/accessableLabs.php' )."</div>\n" );
     if ($returnEpub){
       echo("initializing ePub<br>");
       //TODO: Call functions to tell ePub export that multiple labs come now.
       echo("Adding foreword page...<br>");
       //TODO: Add forword.
     }
  // title
     if (!$returnEpub){$pge->put( "<div class=\"labsys_mop_h2\">__PAGETITLE__</div>\n" );}
     
  // note
     if ( !$returnEpub && $lng->get("AccessableLabsNote") != "" ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("AccessableLabsNote")."</div>\n" ); 

     $accessableLabs = array();
     $alreadyAdded = array(); // for not adding dups...
  // Labs that are visible without a schedule
    //SELECT `idx` FROM `labs` WHERE `visible_before_first_sched`=1
    $lDBI->queryResult = $lDBI->myDBC->mkSelect( '*', $lDBI->myTable, "idx!=1 && `visible_before_first_sched`=1" );
    while( $nextElement=$lDBI->getNextData() ){
      $accessableLabs[] = $nextElement;
      $alreadyAdded[] = $nextElement->idx;
    }

  // Currently scheduled in order of schedule
    //SELECT `num` FROM `schedules` WHERE 1 GROUP BY num ORDER BY `start`
    $sDBI->queryResult = $sDBI->myDBC->mkSelect( 'num', $sDBI->myTable, 'idx!=1', '`start`', 'num' );
    while( $nextData=mysql_fetch_array($sDBI->queryResult) ){
      if (  !in_array( $nextData['num'], $alreadyAdded ) && ($nextLab = $lDBI->getData2idx( $nextData['num'] )) ) // do not add twice...
        $accessableLabs[] = $nextLab;
    }

     $counter = 0;
     $charCounter = 97;
     if (!$returnEpub){$pge->put('<table align="center" width="80%" cellspacing="10">');}
     foreach ( $accessableLabs as $value ){
       if ($returnEpub){
         $extParagraph = (string)( $value->visibleBefore1stSched ? chr ($charCounter++) : $counter++ );
         echo( $extParagraph.' '.$value->title.' ('.$value->elementId.$value->idx.')<br>');
         //echo( $value->showTOC( $value->elementId.$value->idx, $extParagraph ) );
         // to be done: $value->showEPub( $value->elementId.$value->idx, ( $value->visibleBefore1stSched ? chr ($charCounter++) : $counter++ ) );
       }else{
         $pge->put('
<tr>
  <td width="75" class="labIndexNumber">
'.( $value->visibleBefore1stSched ? chr ($charCounter++) : $counter++ ).'
  </td>
  <td class="labIndexText">
    <b><a href="../pages/view.php?address=l'.$value->idx.'&amp;__LINKQUERY__" target="_top">'.$value->title.'</a></b> - '.$value->comment.'
    </td>
  </tr>
'        );
       }
     }
     if (!$returnEpub){$pge->put('</table>');}
  
if (!$returnEpub){require( $cfg->get("SystemPageLayoutFile") );}
else{
  echo("creating ePub");
}
?>
