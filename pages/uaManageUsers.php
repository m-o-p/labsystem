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
* This page is to administrate the userDB users.
*  - change their relations to the courses
*  - edit their data
*  - delete them
*
* @module     ../pages/uaManageUsers.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
require( "../include/init.inc" );
require_once( INCLUDE_DIR."/classes/DBInterfaceUser.inc" );

$pge->title        = $lng->get("titleUaManageUsr");
$pge->matchingMenu = $lng->get("MnuEntryUaManageUsr");
$pge->visibleFor   = IS_DB_USER_ADMIN;

  if ( substr( $url->get('config'), 0, 9 ) != 'useradmin' ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("TxtNotConfigUA")."\n</div>" );
  else{ // showing
  // head (create new)
     $pge->put(  "<div class=\"labsys_mop_elements_menu_u\">\n".
                 EB::mkLink( $url->link2( '../pages/uaCreateUsers.php' ) , "<img src=\"../syspix/button_new_13x12.gif\" width=\"13\" height=\"12\" border=\"0\" alt=\"new\" title=\"".$lng->get("explainCreateNew")."\">" ).
                 "</div>\n"
               );
            
  // title
     $pge->put( "<div class=\"labsys_mop_h2\">__PAGETITLE__</div>\n" );
     
  // note
     if ( $lng->get("uaManageUsrNote") != "" ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("uaManageUsrNote")."</div>\n" );  

     
// stop seeing
      if( $usr->isSeeingSomeonesData() ) 
        $pge->put( '<div class="labsys_mop_button_fullwidth">'."\n".
                   '<a href="'.$url->link2( '../php/uaManageUsersExecute.php?function=see&param='.urlencode( '' ).'&redirectTo='.urlencode( $_SERVER['REQUEST_URI'] ) ).'">'.
                   $lng->get("stopSeeingData").
                   '</a>'.
                   '</div>'."\n"
                  );
// /stop seeing

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

    // now the array is [n] => $key but for sorting it has to be $keyExpl => $key
    $sortArrayAdd = array_flip( $courseArray );
    foreach( array_keys( $sortArrayAdd ) as $value ) $sortArrayAdd[ $value ] = $value;
     
  // sorting
      $sortArray = array_merge( DBInterfaceUser::sortableByArray(), Array( $lng->get( 'lastChange' ) => 'labsys_mop_last_change' ) );
     // get array of sorter keys from DBInterface
      $sortArray = array_merge( $sortArray, $sortArrayAdd );
     // fill $sorter with the sorters html code and set $orderBy and $asc
      require( "../pages/sorter.inc" );
    // the sorter
      $pge->put( $sorter );

// DB Query
    $result = $userDBC->mkSelect( '*', 
                                  $cfg->get('UserDatabaseTable'), 
                                  '',
                                  $orderBy.retIfTrue( $asc, ' ASC', ' DESC' )
                                 );
                                 
// Multipageresult-Filtering
      // How many lines were returned?
      $existingElemnts = $userDBC->datasetsIn($result);
      
    // With more than 360 elements more than 8M are used and it gets slow!
    // -> only show result partially!
    // In mysql exists [LIMIT offset, rows] as argument, one could use that. BUT how many totally?
      if ( isset( $_GET['startFrom'] ) &&
           is_numeric ( $_GET['startFrom'] ) &&
           ($_GET['startFrom'] > 0)
          ) $startFrom = $_GET['startFrom']; else $startFrom = 1;
    
      if ( isset( $_GET['frameSize'] ) &&
           is_numeric ( $_GET['frameSize'] ) &&
           ($_GET['frameSize'] > 0)
          ) $frameSize = $_GET['frameSize']; else $frameSize = $cfg->get( 'DefElmntsPerManagePage' );
    
      
      $manageNavigation = '<!-- navigation -->'."\n";
      $manageNavigation .= '<div class="labsys_mop_element_navigation">'."\n";
    
        // back Arrows
        if ( $startFrom > $frameSize ) $manageNavigation .= '<a href="'.$url->link2( '../pages/uaManageUsers.php?'.
                                                                                     'startFrom='.($startFrom-$frameSize).
                                                                                     '&frameSize='.$frameSize.
                                                                                     '&orderBy='.$orderByKey.
                                                                                     '&asc='.retIfTrue( $asc, 'asc', 'desc' ) ).'">&lt;&lt;</a> '."\n";
      
          $j = 1;
          for ( $i=1; $i<$existingElemnts; $i+=$frameSize ){
            $manageNavigation .= '<a href="'.$url->link2( '../pages/uaManageUsers.php?'.
                                                          'startFrom='.$i.
                                                          '&frameSize='.$frameSize.
                                                          '&orderBy='.$orderByKey.
                                                          '&asc='.retIfTrue( $asc, 'asc', 'desc' ) ).
                                 '">'.
                                 retIfTrue( ($startFrom == $i), '<b>' ).
                                 $j++.
                                 retIfTrue( ($startFrom == $i), '</b>' ).
                                 '</a> '."\n";
          }
      
        // forward Arrows
        if ( $startFrom+$frameSize < $i ) $manageNavigation .= '<a href="'.$url->link2( '../pages/uaManageUsers.php?'.
                                                                                        'startFrom='.($startFrom+$frameSize).
                                                                                        '&frameSize='.$frameSize.
                                                                                        '&orderBy='.$orderByKey.
                                                                                        '&asc='.retIfTrue( $asc, 'asc', 'desc' ) ).'">&gt;&gt;</a>'."\n";
    
      $manageNavigation .= '</div>'."\n";
      $manageNavigation .= '<!-- /navigation -->'."\n";
      
      
      $pge->put( $manageNavigation );
// /Multipageresult-Filtering
      
      
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
    $pge->put( "<FORM class=\"labsys_mop_std_form\" NAME=\"myDataEdit\" METHOD=\"POST\" ACTION=\"".$url->link2("../php/uaManageUsersSave.php")."\">\n".
               "<input type=\"hidden\" name=\"SESSION_ID\" value=\"".session_id()."\">\n".
               "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/uaManageUsers.php\">\n"
              );

    $currElNr = 0;
    $stopAt = $startFrom+$frameSize;
    while( $data = mysql_fetch_assoc( $result ) ){
      // skip not wanted
      $currElNr++; if ( $currElNr < $startFrom ) continue; if ( $currElNr >= $stopAt ) break;
      $pge->put( '<div class="labsys_mop_u_row">'."\n" );
      
      // Identifier if uid is present (only selection is shown)
      $pge->put( '<input type="hidden" name="'.$data[ $cfg->get('UserDBField_uid') ].'" value="1">' );
      
      foreach ( $data as $key => $value )
        if( $key[0] == '_' )
          $pge->put( '<input type="checkbox" id="'.$data[ $cfg->get('UserDBField_uid') ].$key.'" name="'.$data[ $cfg->get('UserDBField_uid') ].$key.'" value="'.$value.'" tabindex="'.$pge->nextTab++.'" '.retIfTrue( ($value == 1), 'checked="checked" ' ).'/>'.
                     '<label for="'.$data[ $cfg->get('UserDBField_uid') ].$key.'" class="labsys_mop_input_field_label">'.infoArrow( $key, false ).'</label>' );

      $pge->put( ' '.retIfTrue( $usr->isOfKind( IS_DB_USER_ADMIN ), '<a href="'.$url->link2( '../php/uaManageUsersExecute.php?function=see&param='.urlencode( $data[ $cfg->get('UserDBField_uid') ] ).'&redirectTo='.urlencode( '../pages/uaMyData.php' ) ).'">' ).
                     $data[ $cfg->get('UserDBField_name') ].', '.
                     $data[ $cfg->get('UserDBField_forename') ].' ('.
                     $data[ $cfg->get('UserDBField_username') ].')'.
                     retIfTrue( $usr->isOfKind( IS_DB_USER_ADMIN ), '</a>' )."\n".
// delete button
                 ' <a tabindex="'.$pge->nextTab++.'" '.
                    'href="'.$url->link2( '../php/uaManageUsersExecute.php?function=del&param='.urlencode( $data[ $cfg->get('UserDBField_uid') ] ).'&redirectTo='.urlencode( $_SERVER['REQUEST_URI'] ) )."\" ".
                    "onClick='return confirmLink(this, \"".$data[ $cfg->get('UserDBField_forename') ].' '.$data[ $cfg->get('UserDBField_name') ].': '.$lng->get('confirmDelete').'");'."'".
                 '>'.
                 "<img src=\"../syspix/button_delete_13x12.gif\" width=\"13\" height=\"12\" border=\"0\" alt=\"delete\" title=\"".$lng->get("explainDeleteElemnt")."\">".
                 "</a>\n".
// /delete button                     
// mail2
                 ' <a href="mailto:'.$data[ $cfg->get('UserDBField_email') ].'"><img src="../syspix/button_mail_13x12.gif" width="13" height="12" border="0" title="'.$data[ $cfg->get('UserDBField_email') ].'" alt="'.$data[ $cfg->get('UserDBField_email') ].'"></a>'."\n".
// /mail2
// history (last change)
                 EB::history ( $lng->get('lastChange').': '.
                               date( $lng->get("DateFormat"), 
                                     mktime( substr( $data[ 'labsys_mop_last_change' ], 11, 2),  // hh
                                             substr( $data[ 'labsys_mop_last_change' ], 14, 2), // mm
                                             substr( $data[ 'labsys_mop_last_change' ], 17, 2), // ss
                                             substr( $data[ 'labsys_mop_last_change' ], 5, 2),  // MM
                                             substr( $data[ 'labsys_mop_last_change' ], 8, 2),  // DD
                                             substr( $data[ 'labsys_mop_last_change' ], 0, 4)   // YYYY
                                            ) 
                              ), 'p1', true ).
// /history
                 "</div>\n" );
    }

// Multipageresult-Filtering
      $pge->put( $manageNavigation );
// /Multipageresult-Filtering
    
    
// /form
    $pge->put( "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("apply")."\">\n".            
               "</FORM>"
             );
  } // /showing

// show!
  require( $cfg->get("SystemPageLayoutFile") );
?>
