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

if ( !$pge->isVisible() ){ // directly show warning and close.
  require( $cfg->get("SystemPageLayoutFile") );
  exit;
}

  if ( substr( $url->get('config'), -9 ) != 'useradmin' ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("TxtNotConfigUA")."\n</div>" );
  else{ // showing
  // head (create new)
     $pge->put(  "<div class=\"labsys_mop_elements_menu_u\">\n".
                 EB::link2Url( '../pages/uaCreateUsers.php' ).
                 EB::mkLink( $url->link2( '../pages/uaCreateUsers.php' ), "<img src=\"../syspix/button_new_13x12.gif\" width=\"13\" height=\"12\" border=\"0\" alt=\"new\" title=\"".$lng->get("explainCreateNew")."\">").
                 "</div>\n"
               );

  // title
     $pge->put( "<div class=\"labsys_mop_h2\">__PAGETITLE__</div>\n" );

  // note
     if ( $lng->get("uaManageUsrNote") != "" ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("uaManageUsrNote")."</div>\n" );


// stop seeing
      if( $usr->isSeeingSomeonesData() )
        $pge->put( '<div class="labsys_mop_button_fullwidth">'."\n".
                   '<a href="'.$url->link2( '../php/uaManageUsersExecute.php', Array('function' => 'see', 'param' => '', 'redirectTo' => $_SERVER['REQUEST_URI']) ).'">'.
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

// Multipageresult-Filtering Init $_GET as it is used by the sorter...
      if ( $GLOBALS['url']->available('startFrom') &&
           is_numeric ( $GLOBALS['url']->get('startFrom') ) &&
           ($GLOBALS['url']->get('startFrom') > 0)
          ) $startFrom = $GLOBALS['url']->get('startFrom'); else $startFrom = 1;

// new restriction? => set start to 1!
if (isset($_POST['restrictTo'])) $startFrom = 1;

      if ( $GLOBALS['url']->available('frameSize') &&
           is_numeric ( $GLOBALS['url']->get('frameSize') ) &&
           ($GLOBALS['url']->get('frameSize') > 0)
          ) $frameSize = $GLOBALS['url']->get('frameSize'); else $frameSize = $cfg->get( 'DefElmntsPerManagePage' );
// /Multipageresult-Filtering Init $_GET as it is used by the sorter...

$pge->put('<script language="javascript" type="text/javascript">' . "\n" .
          'function showCheckboxes(uid) {' . "\n" .
          '    var checkboxes = document.getElementById("checkboxes_" + uid);' . "\n" .
          '    checkboxes.classList.toggle(\'hidden\');' . "\n" .
          '}' . "\n" .
          '</script>');
//Sorter
  // which courses exist?
    // ask for the couseID fields starting with _
    // list all columns
    $result = $userDBC->query( 'SHOW COLUMNS FROM '.$cfg->get('UserDatabaseTable') );
    $courseArray = Array();
    while( $data = $result->fetch_array() )
      if ( substr( $data[0], 0, 1 ) == '_' ) $courseArray[] = $data[0];

    // now the array is [n] => $key but for sorting it has to be $keyExpl => $key
    $sortArrayAdd = array_flip( $courseArray );
    foreach( array_keys( $sortArrayAdd ) as $value ) $sortArrayAdd[ $value ] = $value;
  // if set a restrict to field is shown by sorter.inc
      $restrictToArray = $sortArrayAdd;
      $restrictToArray = array_merge( array( ""=>"" ), $restrictToArray ); //add empty for no restriction
  // sorting
      $sortArray = array_merge( DBInterfaceUser::sortableByArray(), Array( $lng->get( 'lastChange' ) => 'labsys_mop_last_change' ) );
     // get array of sorter keys from DBInterface
      $sortArray = array_merge( $sortArray, $sortArrayAdd );
     // fill $sorter with the sorters html code and set $orderBy and $asc
      $showSearchField = true;
      require( "../pages/sorter.inc" );
    // the sorter
// /Sorter
      $pge->put( $sorter );

// DB Query
    $where = array();
    if (isset($_POST['restrictTo']) && !empty($_POST['restrictTo']))
        $where[] = '`' . $userDBC->escapeString($_POST['restrictTo']) . '`=1';
    else if (($GLOBALS['url']->available('restrictTo')) && !empty($GLOBALS['url']->get('restrictTo')))
        $where[] = '`' . $userDBC->escapeString($GLOBALS['url']->get('restrictTo')) . '`=1';
    if (!empty($searchFor)) {
        foreach (explode(' ', $searchFor) as $searchTerm) {
            if (empty($searchTerm)) continue;
            $searchTermMysql = $userDBC->escapeString('%' . $searchTerm . '%');
            $where[] = '(' . $cfg->get('UserDBField_name') . ' LIKE \'' . $searchTermMysql . '\' OR ' . $cfg->get('UserDBField_forename') . ' LIKE \'' . $searchTermMysql . '\')';
        }
    }
    $result = $userDBC->mkSelect( '*',
                                  $cfg->get('UserDatabaseTable'),
                                  implode(' AND ', $where),
                                   ( $restrictToKey == '_unassigned' ? 'registerFor, ' : '' ). // if unassigned order as well by courses registered to
                                   $orderBy.( $asc ?  ' ASC' :  ' DESC'  )
                                 );

// EXPORT CSV
   if ($GLOBALS['url']->available('exportCSV')){
     header('Content-type: text/x-csv');

     // Es wird downloaded.pdf benannt
     header('Content-Disposition: attachment; filename="labsystem'.$restrictToKey.'CSV.txt"');
     $doNotListFromUser = Array( $cfg->get('UserDBField_uid'),
                                 $cfg->get('UserDBField_password')
                                );
      $printLegend = true;
      while($data = $result->fetch_assoc()){
        if ($printLegend){
          $printLegend = false;
          foreach( $data as $key => $value ) if ( in_array( $key, $doNotListFromUser ) || ( $key[0] == '_' ) ) ; else echo( $key."\t" );
          echo("\r\n");
        }
        foreach( $data as $key => $value ) if ( in_array( $key, $doNotListFromUser ) || ( $key[0] == '_' ) ) ; else echo( str_replace( "\n", '', $value )."\t" );
        echo("\r\n");
      }
      exit();
    }

// Multipageresult-Filtering
      // How many lines were returned?
      $existingElemnts = $userDBC->datasetsIn($result);

    // With more than 360 elements more than 8M are used and it gets slow!
    // -> only show result partially!
    // In mysql exists [LIMIT offset, rows] as argument, one could use that. BUT how many totally?

      $manageNavigation = '<!-- navigation -->'."\n";
      $manageNavigation .= '<div class="labsys_mop_element_navigation">'."\n";

        // back Arrows
        if ( $startFrom > $frameSize ) $manageNavigation .= '<a href="'.$url->link2( '../pages/uaManageUsers.php',
                                                                                     Array('startFrom' => $startFrom-$frameSize,
                                                                                           'frameSize' => $frameSize) ).'">&lt;&lt;</a> '."\n";

          $j = 1;
          for ( $i=1; $i<$existingElemnts; $i+=$frameSize ){
            $manageNavigation .= '<a href="'.$url->link2( '../pages/uaManageUsers.php',
                                                          Array('startFrom' => $i,
                                                                'frameSize' => $frameSize) ).
                                 '">'.
                                 ( ($startFrom == $i) ?  '<b>'  : '' ).
                                 $j++.
                                 ( ($startFrom == $i) ?  '</b>'  : '' ).
                                 '</a> '."\n";
          }

        // forward Arrows
        if ( $startFrom+$frameSize < $i ) $manageNavigation .= '<a href="'.$url->link2( '../pages/uaManageUsers.php',
                                                                                        Array('startFrom' => $startFrom+$frameSize,
                                                                                              'frameSize' => $frameSize) ).'">&gt;&gt;</a>'."\n";

      $manageNavigation .= '</div>'."\n";
      $manageNavigation .= '<!-- /navigation -->'."\n";

    // If preserved before the links above become unsusable...
      $url->preserve( 'startFrom' );
      $url->preserve( 'frameSize' );
// /Multipageresult-Filtering

      $pge->put( $manageNavigation );

// form
    $pge->put( "<FORM class=\"labsys_mop_std_form\" NAME=\"myDataEdit\" METHOD=\"POST\" ACTION=\"".$url->link2("../php/uaManageUsersSave.php")."\">\n".
               "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"".$url->link2( $_SERVER['PHP_SELF'] )."\">\n"
              );

    $currElNr = 0;
    $stopAt = $startFrom+$frameSize;
    while( $data = $result->fetch_assoc() ){
      // skip not wanted
      $currElNr++; if ( $currElNr < $startFrom ) continue; if ( $currElNr >= $stopAt ) break;
      $uid = $data[ $cfg->get('UserDBField_uid') ];
      $pge->put('<div class="labsys_mop_u_row">'."\n" );
      $courseList = Array();
      $optionsHTML = Array();
      foreach ( $courseArray as $key ) {
        $value = $data[$key];
        $optionsHTML[] = '<label class="labsys_mop_multiselectoption">'.
                         '<input type="checkbox" name="uids[' . $uid . ']['.$key.']"'.( ($value == 1) ?  ' checked'  : '' ).' onchange="isDirty=true">'.
                         '<span>' . $key. '</span></label>' . "\n";
        if ($value == 1) $courseList[] = $key;
      }
      $pge->put( ' '.( $usr->isOfKind( IS_DB_USER_ADMIN ) ? '<a href="'.$url->link2( '../pages/uaMyData.php', Array('seeMe' => $uid) ).'">' : '').
                     $data[ $cfg->get('UserDBField_name') ].', '.
                     $data[ $cfg->get('UserDBField_forename') ].' ('.
                     $data[ $cfg->get('UserDBField_username') ].')'.
                     ( $usr->isOfKind( IS_DB_USER_ADMIN ) ? '</a>' : '' )."\n".
// delete button
                 ' <a tabindex="'.$pge->nextTab++.'" '.
                    'href="'.$url->link2( '../php/uaManageUsersExecute.php', Array('function' => 'del', 'param' => $uid, 'redirectTo' => $url->rawLink2()) ).'" '.
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
                              )."\n".$data['history'], 'p1', true ).
          '<div class="courseList">Current instances: '.implode(', ', $courseList).
// /history
                 ( isset( $data[ '_unassigned' ] ) && ($data[ '_unassigned' ] == 1) ? '<br><b>'.$data[ 'registerFor' ]."</b> ".
                                                                                      ( $data[ 'desiredTeamPartner' ] != '' ? "| <img src=\"../syspix/prelabFin_yes_15x12.gif\" border=\"0\" title=\"".$lng->get("desiredTeamPartner")."\">".$data[ 'desiredTeamPartner' ] : '').
                                                                                      ' | '.$data[ 'reasonToParticipate' ]  : ''));
      $pge->put('; <a onclick="showCheckboxes(\'' . $uid . '\')">add/remove</a></div>'."\n" );
      $pge->put('<div class="labsys_mop_checkboxes hidden" id="checkboxes_' . $uid . '">'."\n" );
      $pge->put(implode($optionsHTML));
      $pge->put("</div>\n");
      $pge->put("</div><div style='clear:left;'></div>\n");
    }

// Multipageresult-Filtering
      $pge->put( $manageNavigation );
// /Multipageresult-Filtering


// /form
    $pge->put( "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("apply")."\" onclick='isDirty=false'>\n".
               '<a href="'.$url->link2( $_SERVER['PHP_SELF'], Array('exportCSV' => 'true') ).'">export.csv</a>'.
               "</FORM>"
             );
  } // /showing

// Clean up url variables
// otherwhise it ends up in the menu etc...
$url->rem( 'orderBy' );
$url->rem( 'asc' );
$url->rem( 'restrictTo' );
$url->rem( 'startFrom' );
$url->rem( 'frameSize' );
$url->rem( 'searchFor' );

// show!
  require( $cfg->get("SystemPageLayoutFile") );
?>
