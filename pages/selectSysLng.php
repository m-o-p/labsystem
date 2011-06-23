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

$pge->matchingMenu = $lng->get('MnuEntrySysLng');
$pge->title        = $lng->get('TitleSysLngSelect');

     $pge->put( '<div class="labsys_mop_contentArea">' );
  // head (create new)
     if ( $usr->isOfKind( IS_CONTENT_EDITOR ) ) $pge->put(  "<div class=\"labsys_mop_elements_menu_p\">\n".EB::link2Url( '../pages/selectSysLng.php' )."</div>\n" );
  // title
     $pge->put( "<div class=\"labsys_mop_h2\">__PAGETITLE__</div>\n" );
     
  // note
     if ( $lng->get("NoteChooseSysLng") != "" ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("NoteChooseSysLng")."</div>\n" ); 

  //select
      // remove current language line from url:
      if ($url->available( 'lng' )) $url->rem( 'lng='.$runningSystemLanguage );
      $pge->put('<p>'.$lng->get('AvailableLng').': ');
      $subDirs = scandir( $cfg->get("SystemResourcePath") );
      foreach( $subDirs as $entry ){
        if ( $entry == '.' || $entry == '..' ) continue; // skip those
        $parts = preg_split( '/[\s.]/', $entry );
        if( isset($parts[1]) && ($parts[1] == 'lng')) {
          $url->put('lng='.$parts[0]);
          $pge->put('<a href="'.$url->link2('../pages/selectSysLng.php').'">'.( $parts[0] == $runningSystemLanguage ? '[<b>'.$parts[0].'</b>]' : $parts[0]).'</a> ');
          $url->rem('lng='.$parts[0]);
        };
      }
      $pge->put("</p>\n");
      // add running language to url
      $url->put('lng='.$runningSystemLanguage);
      
  // note: create own
     if ( $lng->get("NoteCreateOwnLng") != "" ) $pge->put(  "<div class=\"labsys_mop_note\">\n".$lng->get("NoteCreateOwnLng").
                                                            '<pre>'.returnEditable( file_get_contents($languageFile) ).'</pre>'.
                                                            "</div>\n" ); 
    
     $pge->put( '</div>' );
  
require( $cfg->get("SystemPageLayoutFile") );
?>
