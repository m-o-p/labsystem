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
* manage.php for elements. This for managing the user rights.
*
* @module     ../pages/manageUsers.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* Why does no delete function exist?
*  If you want to delete a user you should not remove its rights (since
*  the user will get the $cfg[DefaultAuthUserRights] rights at next login.
*  You have to remover the users qualification to log in in the user
*  authentication source.
*
* HTML W3 VALIDATED!
*/
require( "../include/init.inc" );
require_once( INCLUDE_DIR."/classes/DBInterfaceUser.inc" );
require_once( INCLUDE_DIR."/classes/DBInterfaceUserRights.inc" );

$pge->title       = $lng->get("TitleUserRightsPage");
$pge->matchingMenu= $lng->get( "MnuEntryUserRights" );
$pge->visibleFor  = IS_USER_RIGHTS_EDITOR;

if ( !$pge->isVisible() ){ // directly show warning and close.
  require( $cfg->get("SystemPageLayoutFile") );
  exit;
}

$pge->put( EM::userManageTop() );

$pge->put('<div class="labsys_mop_h2">'.$pge->title.'</div>'."\n");

// additional note
  if ( $lng->get("NoteUserRightsPage") != "" ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("NoteUserRightsPage")."</div>\n" );

// sorting
 // get array of sorter keys from DBInterfaces
  $sortArray = array_merge ( DBInterfaceUser::sortableByArray(), DBInterfaceUserRights::sortableByArray() );
 // fill $sorter with the sorters html code and set $orderBy and $asc
  $showSearchField = true;
  require( "../pages/sorter.inc" );
// the sorter
  $pge->put( $sorter );

$usr->validateAllUsers();

$pge->put('<FORM NAME="userRights" METHOD="POST" ACTION="'.$url->link2("../php/saveUserRights.php").'"><div>'."\n");

// The search only works when the user database is selected as primary search. So when searching for something, this will become the case independent of selected other orderings.
if ( !empty($searchFor) ){
	$orderBy = $cfg->get("UserDBField_name");
}

// Since the data have two sources, the external users and the internal user rights db
//   we have to distinguish!
  if ( ( $orderBy == $cfg->get("UserDBField_name") ) ||
       ( $orderBy == $cfg->get("UserDBField_forename") ) ||
       ( $orderBy == $cfg->get("UserDBField_username") )
  		){
          /* $uDBI is the source */
          $master = $uDBI;
          $slave  = $urDBI;
  }else{
          /* $urDBI is source */
          $master = $urDBI;
          $slave  = $uDBI;
  }

  $where = array();
  if (!empty($searchFor)) {
      foreach (explode(' ', $searchFor) as $searchTerm) {
          if (empty($searchTerm)) continue;
          $searchTermMysql = $master->myDBC->escapeString('%' . $searchTerm . '%');
          $where[] = '(' . $cfg->get('UserDBField_name') . ' LIKE \'' . $searchTermMysql . '\' OR ' . $cfg->get('UserDBField_forename') . ' LIKE \'' . $searchTermMysql . '\' OR ' . $cfg->get('UserDBField_username') . ' LIKE \'' . $searchTermMysql . '\')';
      }
  }
  $master->getAllData( $orderBy, $asc, implode(' AND ', $where) );

  $existingElemnts = $master->allSize();
// With more than 360 elements more than 8M are used and it gets slow!
// -> only show result partially!
// In mysql exists [LIMIT offset, rows] as argument, one could use that. BUT how many totally?
  if ( $url->available('startFrom') &&
       is_numeric ( $GLOBALS['url']->get('startFrom') ) &&
       ($GLOBALS['url']->get('startFrom') > 0)
      ) $startFrom = $GLOBALS['url']->get('startFrom'); else $startFrom = 1;

  if ( $url->available('frameSize') &&
       is_numeric ( $GLOBALS['url']->get('frameSize') ) &&
       ($GLOBALS['url']->get('frameSize') > 0)
      ) $frameSize = $GLOBALS['url']->get('frameSize'); else $frameSize = $cfg->get( 'DefElmntsPerManagePage' );


  $manageNavigation = '<!-- navigation -->'."\n";
  $manageNavigation .= '<div class="labsys_mop_element_navigation">'."\n";

    // back Arrows
    if ( $startFrom > $frameSize ) $manageNavigation .= '<a href="'.$url->link2( '../pages/manageUsers.php',
                                                                                 Array('startFrom' => $startFrom-$frameSize,
                                                                                       'frameSize' => $frameSize,
                                                                                       'orderBy' => $orderByKey,
                                                                                       'asc' => ( $asc ? 'asc' : 'desc' )) ).'">&lt;&lt;</a> '."\n";

      $j = 1;
      for ( $i=1; $i<=$existingElemnts; $i+=$frameSize ){
        $manageNavigation .= '<a href="'.$url->link2( '../pages/manageUsers.php',
                                                      Array('startFrom' => $i,
                                                            'frameSize' => $frameSize,
                                                            'orderBy' => $orderByKey,
                                                            'asc' => ( $asc ?  'asc' :  'desc'  )) ).
                             '">'.
                             ( ($startFrom == $i) ?  '<b>'  : '' ).
                             $j++.
                             ( ($startFrom == $i) ?  '</b>'  : '' ).
                             '</a> '."\n";
      }

    // forward Arrows
    if ( $startFrom+$frameSize < $i ) $manageNavigation .= '<a href="'.$url->link2( '../pages/manageUsers.php',
                                                                                    Array('startFrom' => ($startFrom+$frameSize),
                                                                                          'frameSize' => $frameSize,
                                                                                          'orderBy' => $orderByKey,
                                                                                          'asc' => ( $asc ?  'asc' :  'desc'  )) ).'">&gt;&gt;</a>'."\n";

  $manageNavigation .= '</div>'."\n";
  $manageNavigation .= '<!-- /navigation -->'."\n";


  $pge->put( $manageNavigation );

// legend
  $pge->put( ElementUser::showPropertyLegend() );

  $currElNr = 0;
  $stopAt = $startFrom+$frameSize;
  while( $masterData = $master->getNextData() ){
    // skip not wanted
    $currElNr++; if ( $currElNr < $startFrom ) continue; if ( $currElNr >= $stopAt ) break;
    $slaveData = $slave->getData4( $masterData["uid"] );

    // If a user does not exist anymore in the UserDB don't show it.
    if ( !$slaveData ) continue;

    $userData = array_merge( $masterData, $slaveData );
    $user = new ElementUser( $userData["uid"], $userData["userName"], $userData["foreName"], $userData["name"], $userData["currentTeam"], $userData["rights"], $userData["rights"], $userData["eMail"], $userData["history"] );
    $pge->put( '<input type="hidden" name="'.$userData[ 'uid' ].'_present" value="1">'."\n" ); // necessary to identify available users
    $pge->put( $user->showPropertyRow( $userData["uid"] ) );
  }

  $pge->put( $manageNavigation );

//saving
  $pge->put('
              <input type="hidden" name="REDIRECTTO" value="'.$url->link2( $_SERVER['PHP_SELF'], isset($startFrom) ? Array('startFrom' => $startFrom, 'frameSize' => $frameSize) : NULL ).'">
              <input TABINDEX="'.$pge->nextTab++.'" type="submit" class="labsys_mop_input" value="'.$lng->get("save").'"  accesskey="s" onclick="isDirty=false">

              </div></FORM>
  ');

// the bottom menu
  $pge->put( EM::userManageBottom() );

// show!
  require( $cfg->get("SystemPageLayoutFile") );
?>
