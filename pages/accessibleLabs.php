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
 * @module     ../pages/accessibleLabs.php
 * @author     Marc-Oliver Pahl
 */
require( "../include/init.inc" );

$pge->matchingMenu = $lng->get('MnuEntryCourseContent');
$pge->title        = $cfg->get('SystemTitle').' '.$lng->get('MnuEntryCourseContent');
$allowEpub         = $cfg->doesExist('allowEpubDownload') && ($cfg->get('allowEpubDownload') == "1");
$returnEpub        = $url->available( 'ePub' ) && $allowEpub;

if ($returnEpub){
  $pge->visibleFor  = IS_USER;
  if (!$usr->isOfKind( $pge->visibleFor )){
    // not logged in but requesting ePub -> show login
    require( $cfg->get("SystemPageLayoutFile") );
    exit;
  } else {
    require_once(INCLUDE_DIR . '/../plugins/LSE/Exporter.php');
  }
}

// head (create new)
if ( !$returnEpub && $usr->isOfKind( IS_USER )) $pge->put(  "<div class=\"labsys_mop_elements_menu_p\">\n".
($usr->isOfKind( IS_CONTENT_EDITOR ) ? EB::link2Url( '../pages/accessibleLabs.php' ) : '').
($allowEpub ? EB::mkLink(  $url->link2( '../pages/accessibleLabs.php?ePub=ePub' ),
                                        "<img src=\"../syspix/button_epub_12x12.gif\" width=\"12\" height=\"12\" border=\"0\" alt=\"link to\" title=\"".$lng->get("explainLink2epub")."\">" )
              : '' )."</div>\n" );
if ($returnEpub){
  $GLOBALS['Logger']->logToDatabase('accessibleLabs', logActions::ePubLoad);
  // echo("initializing ePub<br>");
  //TODO: Call functions to tell ePub export that multiple labs come now.
  $epubExporter = LSE_Exporter::getInstance();
  $epubConfig = array(
    'title'                 => $cfg->get('SystemTitle'),
    'authors'               => ($cfg->doesExist('ePubMultiAuthors') && $cfg->get('ePubMultiAuthors') != "" ? $cfg->get('ePubMultiAuthors') : 'labsystem.sf.net' ),
    'lang'                  => $lng->get('Content-Language'),
    'isMultiChapterEnabled' => TRUE,
    'identifier'            => ($cfg->doesExist('ePubIdentifier') && $cfg->get('ePubIdentifier') != "" ?
                                                                           $cfg->get('ePubIdentifier') : $cfg->get('SystemTitle').'-labsystem.sf.net' ),
    'description'           => ($cfg->doesExist('ePubDescription') && $cfg->get('ePubDescription') != "" ?
                                                                            $cfg->get('ePubDescription') : 'labsystem.sf.net ePub to the course '.$cfg->get('SystemTitle') ),
    'publisher'             => ($cfg->doesExist('ePubPublisher') && $cfg->get('ePubPublisher') != "" ?
                                                                          $cfg->get('ePubPublisher') : 'labsystem.sf.net' ),
    'publisherUrl'          => ($cfg->doesExist('ePubPublisherURL') && $cfg->get('ePubPublisherURL') != "" ?
                                                                             $cfg->get('ePubPublisherURL') : 'http://labsystem.sf.net' ),
    'rights'                => ($cfg->doesExist('ePubCopyrightMeta') && $cfg->get('ePubCopyrightMeta') != "" ?
                                                                              $cfg->get('ePubCopyrightMeta') : 'All rights reserved' ),
    'sourceUrl'             => ($cfg->doesExist('ePubSourceURL') && $cfg->get('ePubSourceURL') != "" ?
                                                                          $cfg->get('ePubSourceURL') : 'http://'.$_SERVER['SERVER_NAME'] )
  );

  // set cover and imprint up:
  require( '../include/setupEpubFrontMatter.inc');
}
// title
if (!$returnEpub){
  $pge->put( "<div class=\"labsys_mop_h2\">__PAGETITLE__</div>\n" );
}

// note
if ( !$returnEpub && $lng->doesExist("AccessibleLabsNote") && $lng->get("AccessibleLabsNote") != "" ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get("AccessibleLabsNote")."</div>\n" );

// Collect all visible labs in the order:
//   - Visible labs without schedule.
//     (Scheduled labs that are visible before schedule are listed below.)
//   - Scheduled labs in the order of their schedule.
require('../include/allVisibleLabIDX.inc');
// Now $allVisibleLabIDX contains the indices as specified above.

$accessibleLabs = array();
foreach( $allVisibleLabIDX as $nextIDX ){
  if ( ($nextIDX[0] == 'l') && ($nextLab = $lDBI->getData2idx( substr($nextIDX, 1) )) ){
  $accessibleLabs[] = $nextLab;
  }
}

$counter = 0;
$charCounter = 97;
if (!$returnEpub){
  $pge->put('<table align="center" width="80%" cellspacing="10">');
}

// possibly preface
if ($cfg->doesExist('prefaceID') && $cfg->get('prefaceID')!=''){
  $prefaceID = $cfg->get('prefaceID');
  $id = $prefaceID{0}; $num = substr( $prefaceID, 1);
  require( "../php/getDBIbyID.inc" ); /* -> $DBI */
  if ( !$preface = $DBI->getData2idx( $num ) ){
    trigger_error( $lng->get(strtolower( $id )."Number").$num." ".$lng->get("doesNotExist"), E_USER_WARNING );
  }else{
    if ($returnEpub){
      $epubConfig['preface'] = $preface->getePubContents();
    }else{
      $pge->put('
      <tr>
        <td width="75" class="labIndexNumber">
        </td>
        <td class="labIndexText">
          <i><a href="../pages/view.php?address='.$prefaceID.'&amp;__LINKQUERY__" target="_top">'.$preface->title.'</a></i>
        </td>
      </tr>
      ');
    }
  }
}

foreach ( $accessibleLabs as $value ){
  if ($returnEpub){
    $extParagraph = (string)( $value->visibleBefore1stSched ? chr ($charCounter) : $counter );
    if ($value->isVisible()) {
      $value->showEPub( $value->elementId.$value->idx, ( $value->visibleBefore1stSched ? chr ($charCounter++) : $counter++ ).'');
    }
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
');
  }
}
if (!$returnEpub && $allowEpub && $usr->isOfKind( IS_USER )){
  $pge->put('
    <tr>
      <td width="75" class="labIndexNumber">
      </td>
      <td class="labIndexText">
        <a href="'.$url->link2( '../pages/accessibleLabs.php?ePub=ePub' ).'">

        <img src="'.$url->link2( '../pages/getEPubCover.php' ).'" width="75px" style="float: left; padding-right: 1em;" />
        '.( $lng->doesExist('explainLink2epub') && $lng->get('explainLink2epub') != "" ? $lng->get('explainLink2epub') : 'get the ePub...' ).'
        <div style="clear: left;"></div>

        </a>
      </td>
    </tr>
  ');
}

if (!$returnEpub){
  $pge->put('</table>');
  require( $cfg->get("SystemPageLayoutFile") );
}
else{
  $epubExporter->setOptions($epubConfig);
  $epubExporter->render();
  // echo("creating ePub");
}
?>