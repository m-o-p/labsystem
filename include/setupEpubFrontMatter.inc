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

/*
 * Sets the cover image and the imprint of an ePub up.
 * Used by LlElement, getEPubCover, and accessibleLabs.
 *
 * The following config variables can be set:
 *   courseLogo   e.g. courseLogo = "../pix/myLogo.jpg"
 *                This logo is rendered onto the cover page of the ePub.
 *                Per default the labsystem logo is used.
 *   imprintID    e.g. imprintID = "p2"
 *                This text is shown as imprint.
 *                Per default the text specified below is used.
 */
// cover:
$epubConfig['coverImage'] = ( $cfg->doesExist('courseLogo') && ($cfg->get('courseLogo') != '') ? $cfg->get('courseLogo') : '../syspix/labsyslogo_443x40.gif' );
// imprint:
if ($cfg->doesExist('imprintID') && $cfg->get('imprintID')!=''){
  $imprintID = $cfg->get('imprintID');
  $id = $imprintID{0}; $num = substr( $imprintID, 1);
  require( "../php/getDBIbyID.inc" ); /* -> $DBI */
  if ( !$imprint = $DBI->getData2idx( $num ) ){
    trigger_error( $lng->get(strtolower( $id )."Number").$num." ".$lng->get("doesNotExist"), E_USER_ERROR );
    exit;
  }
} else {
  $imprint = new LpElement( 1, "Imprint", "to be filled next", "", 0, 0, "" );
  $imprint->contents = '[HTML]
<p>
  __SYSTEMTITLE__<br>
  Downloaded at: __CURRENT_TIMESTAMP__<br>
  Downloaded by: __FORENAME__ __NAME__ (__USERNAME__)<br>
  All Copyrights Reserved.
</p>
  ';
}
$epubConfig['imprint'] = $imprint->getePubContents();
?>