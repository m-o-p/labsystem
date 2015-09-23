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

require( "../include/init.inc");
require( "../php/getFirstLastFinal.inc" ); $id = $firstFinal{0}; $num = substr( $firstFinal, 1);
require( "../php/getDBIbyID.inc" ); /* -> $DBI */
// Helper
function getAllAddressesAsArray($parentAddress, &$collection){
  $output = array();
  $output[] = $parentAddress;
  foreach ($collection->getMyVisibleElements() as $nextChildID){
    $output[] = $parentAddress.'.'.$nextChildID;
    if (($nextChildID[0] == 'C') || ($nextChildID[0] == 'c')){
      $output = array_merge($output, getAllAddressesAsArray($parentAddress.'.'.$nextChildID, $GLOBALS['cDBI']->getData2idx(substr($nextChildID,1))));
    }
  }
  return $output;
}

/**
 * Returns the title info for the element specified by $resourceID.
 * @param unknown $resourceFullAddress The ID the title information should be returned for.
 */
function getTitle($resourceFullAddress){
  $pos = strrpos($resourceFullAddress, '.');
  if ($pos != 0){
    $pos++;
  }
  $lastID = substr($resourceFullAddress, $pos);
  
  $title = '';
  if (strtolower($lastID[0]) == 'a') {
  	$title .= $lastID;
  } else {
  	$title .= strip_tags($GLOBALS[strtolower($lastID[0]).'DBI']->getData2idx(substr($lastID,1))->title);
  }
  
  $exploded = explode('.', $resourceFullAddress);
  if (sizeof($exploded) > 1) {
  	$num = "";
	for ($i = 1; $i < count($exploded)-1; $i++) {
  		$enclosingC = $GLOBALS['cDBI']->getData2idx(substr($exploded[$i], 1));
  		$num .= $enclosingC->getParagraph(implode(".",array_slice($exploded, $i+1, 1)));
  		$num .= ".";
	}
	$result = $num . " " . trim($title);
	if (strlen($result) > 28) {
		$title = substr($result, 0, 24)." ...";
	}
  }
  return $title;
}

function startsWith($haystack, $needle)
{
  return !strncmp($haystack, $needle, strlen($needle));
}

function endsWith($haystack, $needle)
{
  $length = strlen($needle);
  if ($length == 0) {
    return true;
  }

  return (substr($haystack, -$length) === $needle);
}

function getTeamsOfLab($labIDX) {
	$usDBi = new LlDBInterfaceUidStatus($labIDX);
	$queryResult = $usDBi->myDBC->mkSelect('DISTINCT current_team', $usDBi->myTable, 'l_idx = '.$labIDX);
	$teams = [];
	while ($row = mysql_fetch_array($queryResult)) {
		$teams[] = $row['current_team'];
	}
	return $teams;
}

// /Helper

$pge->title       = $lng->get( strtolower($id)."TimeTracking Page" );
$pge->matchingMenu= $lng->get( strtolower($id)."TimeTracking" );
$pge->visibleFor  = IS_USER;

if ( !$pge->isVisible() ){ // directly show warning and close.
  require( $cfg->get("SystemPageLayoutFile") );
  exit;
}

$pge->put( EM::manageTop( $id ) );
$pge->put('<div class="labsys_mop_h2">'.$pge->title.'</div>'."\n");

$pge->put('<p>ttlidx: '.$url->get('ttlab').'</p>');

// Collect all visible labs in the order:
//   - Visible labs without schedule.
//     (Scheduled labs that are visible before schedule are listed below.)
//   - Scheduled labs in the order of their schedule.
require('../include/allVisibleLabIDX.inc');

// Now $allVisibleLabIDX contains the indices as specified above.
foreach( $allVisibleLabIDX as $nextIDX ){
  $nextLab = $GLOBALS['lDBI']->getData2idx(substr($nextIDX,1));
  if (($nextLab!==false) && (($nextLab->visibleBefore1stSched===true) || ($nextLab->isInSched===true) || ($nextLab->isAfter1stSched===true))){
    $pge->put('<li>'.$nextIDX.': '.$nextLab->title.'</li>');
  }
}

$labIDX    = $url->get('ttlab');
$userID    = $url->get('userid');
$teamNr	   = $url->get('team') == '' ? NULL : intval($url->get('team'));

$labSched = $GLOBALS['sDBI']->getData2idx($labIDX);
$startTime = $labSched->start;
$stopTime  = $labSched->stop;
$pge->put('<br>Lab: '.$labIDX.
		  '<br>Beginning: '.date('r', $startTime).
		  '<br>End: '.date('r', $stopTime).
		  '<br>Team: '.$teamNr.
          '<br>Teams in Lab: '.implode(', ', getTeamsOfLab($labIDX)));

$pge->put('<p>PearlView:</p>');
require('timeTracking/pearlView.inc');

    

$pge->put( EM::manageBottom( $id ) );

// show!
  require( $cfg->get("SystemPageLayoutFile") );
?>
