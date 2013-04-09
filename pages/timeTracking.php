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
  return $GLOBALS[strtolower($lastID[0]).'DBI']->getMenuData2idx(substr($lastID,1))->title;
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

$pge->put('<p>Hello world!</p>');

    // Collect all visible labs in the order:
    //   - Visible labs without schedule.
    //     (Scheduled labs that are visible before schedule are listed below.)
    //   - Scheduled labs in the order of their schedule.
    require('../include/allVisibleLabIDX.inc');

    // Now $allVisibleLabIDX contains the indices as specified above.
    foreach( $allVisibleLabIDX as $nextIDX ){
      $nextLab = $GLOBALS['lDBI']->getData2idx(substr($nextIDX,1));
      if (($nextLab!==false) && (($nextLab->visibleBefore1stSched===true) || ($nextLab->isInSched===true) || ($nextLab->isAfter1stSched===true))){
        $pge->put('<li>'.$nextLab->title);
      }
    }

    $labIDX    = 5;
    $userID    = 'something';
    $startTime = '2013-04-02 16:21:29';
    $stopTime  = '2013-04-05 16:21:29';

// Collect buckets. Each full address becomes a bucket.
    $currentLab = $GLOBALS['lDBI']->getData2idx($labIDX);
    $preLabBuckets = false;
    $labBuckets = false;

    $allBuckets = array();
    if (!$currentLab->noPrelab) {
      foreach (getAllAddressesAsArray('l'.$labIDX.'.C'.$currentLab->prelabCollectionIdx, $currentLab->preLab) as $value){
        $allBuckets[$value] = array();
      }
    }

    if (!$currentLab->noLab) {
      foreach (getAllAddressesAsArray('l'.$labIDX.'.C'.$currentLab->labCollectionIdx, $currentLab->lab) as $value){
        $allBuckets[$value] = array();
      }
    }

    $allBuckets['system'] = array();

// Buckets are there. The keys are the element addresses.

    // The logger pushed all potential closes (35) from the children to the parents.
    // 1) Unnecessary ones should be deleted from the DB.
    // 2) The parents should push potential close events to their children as "last resort"

// Sort all events to their matching bucket:
    $result = $Logger->myDBC->mkSelect("*, UNIX_TIMESTAMP(timestamp) as timestampInt", $Logger->myTable, 'resourceID LIKE "l'.$labIDX.'%" OR resourceID="system"'); // TODO: Add date and user restrictions.
    $labFragmentID = 'l'.$labIDX.'~'; // The ~occurs on check events when the context is unknown (only the lab is known).
    while($resArray = mysql_fetch_array($result)){
      if (startswith($resArray['resourceID'], $labFragmentID)){
        foreach( $allBuckets as $key=>$value){
          if(endswith($key, substr($resArray['resourceID'], strlen($labFragmentID)))){
            $allBuckets[$key][] = $resArray;
          }
        }
      }else{
        if (array_key_exists($resArray['resourceID'], $allBuckets)){
          $allBuckets[$resArray['resourceID']][] = $resArray;
        }else{
          $allBuckets['l'.$labIDX][] = $resArray;
        }
      }
    }

// Generate some output:
    // Invariant: $allBuckets contains all resourceIDs as keys sorted by occurrence.
    //                        The values are arrays of the events for the resource sorted by time.
    // getTitle($resourceFullAddress) can be used to get the title of a resource.
    // $url->link2('/pages/view.php?address='.$key) links to the resource.
    //
    // Traversing could be:
    //
    //     foreach( $allBuckets as $key=>$value){
    //       $pge->put('<li><a href="'.$url->link2('/pages/view.php?address='.$key).'">'.$key.'</a>: '.getTitle($key).'<ul>');
    //       foreach ($value as $logEntry){
    //         $pge->put('<li>'.date('r', $logEntry['timestampInt']).': '.$logEntry['action']."\t -&gt; ".$logEntry['idx'].': '.$logEntry['resourceID'].': '.$logEntry['referrerID'].': '.$logEntry['teamNr']);
    //       }
    //       $pge->put('</ul>');
    //     }

    require('timeTracking/listView.inc');

$pge->put( EM::manageBottom( $id ) );

// show!
  require( $cfg->get("SystemPageLayoutFile") );
?>
