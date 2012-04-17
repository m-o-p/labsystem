<?php
/*
 * Sets the cover image and the imprint of an ePub up.
 * Used by LlElement, getEPubCover, and accessibleLabs.
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
  $imprint->contents = '
[HTML]
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