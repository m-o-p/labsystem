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
* Lists all property rows (editable) of elements type.
*
* @module     ../pages/manage.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
require( "../include/init.inc");
require( "../php/getFirstLastFinal.inc" ); $id = $firstFinal{0}; $num = substr( $firstFinal, 1);
require( "../php/getDBIbyID.inc" ); /* -> $DBI */

$pge->title       = $lng->get( strtolower($id)."ManageTitle" );
$pge->matchingMenu= $lng->get( strtolower($id)."ManageMnuEntry" );
$pge->visibleFor  = IS_CONTENT_EDITOR;
if ( $id=="s" ) $pge->visibleFor  = IS_SCHEDULER; // schedules are an exception

$pge->put( EM::manageTop( $id ) );
$pge->put('<div class="labsys_mop_h2">'.$pge->title.'</div>'."\n");

// additional note
  if ( $lng->get( strtolower($id)."ManageNote" ) != "" ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get( strtolower($id)."ManageNote" )."</div>\n" );
  

// sorting
 // get array of sorter keys from DBInterface
  $sortArray = $GLOBALS[ $id."DBI" ]->sortableByArray();
 // fill $sorter with the sorters html code and set $orderBy and $asc
  require( "../pages/sorter.inc" );
// the sorter
  $pge->put( $sorter );

// The legend needs an existing instance ($this->elementId) of the current object so it is shown below when an instance exists.
  $legendShown = false;

// iterate over all elements ordered by $orderBy, $asc
  $DBI->getAllData( $orderBy, $asc ); 

  $existingElemnts = $DBI->allSize();
// With more than 360 elements more than 8M are used and it gets slow!
// -> only show result partially!
// In mysql exists [LIMIT offset, rows] as argument, one could use that. BUT how many totally?
  if ( $url->available('startFrom') &&
       is_numeric ( $url->get('startFrom') ) &&
       ($url->get('startFrom') > 0)
      ) $startFrom = $url->get('startFrom'); else $startFrom = 1;

  if ( $url->available('frameSize') &&
       is_numeric ( $url->get('frameSize') ) &&
       ($url->get('frameSize') > 0)
      ) $frameSize = $url->get('frameSize'); else $frameSize = $cfg->get( 'DefElmntsPerManagePage' );

  
  $manageNavigation = '<!-- navigation -->'."\n";
  $manageNavigation .= '<div class="labsys_mop_element_navigation">'."\n";

    // back Arrows
    if ( $startFrom > $frameSize ) $manageNavigation .= '<a href="'.$url->link2( '../pages/manage.php?address='.$id.
                                                                                 '&startFrom='.($startFrom-$frameSize).
                                                                                 '&frameSize='.$frameSize ).'">&lt;&lt;</a> '."\n";
  
      $j = 1;
      for ( $i=1; $i<=$existingElemnts; $i+=$frameSize ){
        $manageNavigation .= '<a href="'.$url->link2( '../pages/manage.php?address='.$id.
                                                      '&startFrom='.$i.
                                                      '&frameSize='.$frameSize ).
                             '">'.
                             ( ($startFrom == $i) ?  '<b>'  : '' ).
                             $j++.
                             ( ($startFrom == $i) ?  '</b>'  : '' ).
                             '</a> '."\n";
      }
  
    // forward Arrows
    if ( $startFrom+$frameSize < $i ) $manageNavigation .= '<a href="'.$url->link2( '../pages/manage.php?address='.$id.
                                                                                    '&startFrom='.($startFrom+$frameSize).
                                                                                    '&frameSize='.$frameSize ).'">&gt;&gt;</a>'."\n";

  $manageNavigation .= '</div>'."\n";
  $manageNavigation .= '<!-- /navigation -->'."\n";
  
  $pge->put( $manageNavigation );
  
  $currElNr = 0;
  $stopAt = $startFrom+$frameSize;
  while ( $element = $DBI->getNextData() ){ 
    $currElNr++; if ( $currElNr < $startFrom ) continue; if ( $currElNr >= $stopAt ) break;
    if ( !$legendShown ){ /* I need an instance to call the showPropertyLegend() method.
                             A static call would not do the job since a variable is needed ( $this must be callable). */
                         // show the legend
                          $pge->put( $element->showPropertyLegend() ); 
                         // open the form
                          $pge->put('<FORM NAME="manage" METHOD="POST" ACTION="../php/saveAllElement.php?address='.$id.'"><div>'."\n");
                          $legendShown = true;
                        }
   // show the property row
    $pge->put( $element->showPropertyRow( $element->idx ) );
  }
  
  $pge->put( $manageNavigation );
  
// saving
/* The names of the inputs are prepared for saving all properties but there is no save function
 * implemented since I didn't find it helpful. If necessary you can easily implement one for each element
 * or even the prototype and create a script ../php/saveAllElement.php (like ../php/saveElement.php).
 *
 * The advantage: You could set the property row properties for all elements at once on this page and save then.
 *
  $pge->put("<input type=\"hidden\" name=\"REDIRECTTO\" value=\"".urlencode($_SERVER['REQUEST_URI'])."\">".
            "<input type=\"hidden\" name=\"SESSION_ID\" value=\"".session_id()."\">".
            "<input TABINDEX=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("save")."\" accesskey=\"s\">" );
*/

// close the form
  $pge->put("</div></FORM>");

// the bottom menu
  $pge->put( EM::manageBottom( $id ) );

// show!
  require( $cfg->get("SystemPageLayoutFile") );
?>
