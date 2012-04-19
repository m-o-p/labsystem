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
 * Creates the arry $allVisibleLabIDX containing all vivible labs (visibleBeforeSchedule)
 * and all elements that are scheduled in the order they are scheduled.
 *
 * Gets required by:
 *   include/menu.inc
 *   pages/accessibleLabs.php
 */
$allVisibleLabIDX = array();
// First the scheduled labs are collected as they should be displayed in the
// order they are scheduled.
// Then labs visible before schedule are collected and added before the others.
// This also shows labs that are visible before schedule in their schedule order.
// In ilab2 we offer to start exercises early. Still they should be in the normal
// schedule and be displayed in that order.
$otherVisibleLabs = array();
// Currently scheduled in order of schedule
//SELECT `num` FROM `schedules` WHERE 1 GROUP BY num ORDER BY `start`
$sDBI->queryResult = $sDBI->myDBC->mkSelect( 'id, num', $sDBI->myTable, 'idx!=1', '`start`', 'num' );
while( $nextData=mysql_fetch_array($sDBI->queryResult) ){
  $otherVisibleLabs[] = $nextData['id'].$nextData['num'];
}

// Labs that are visible without a schedule and are not added yet
// get pushed to the front.
//SELECT `idx` FROM `labs` WHERE `visible_before_first_sched`=1
$lDBI->queryResult = $lDBI->myDBC->mkSelect( 'idx', $lDBI->myTable, "idx!=1 && `visible_before_first_sched`=1" );
while( $nextData=mysql_fetch_array($lDBI->queryResult) ){
  $addtionalWithoutSchedule = array();
  if (  !in_array( 'l'.$nextData['idx'], $otherVisibleLabs ) ){
    // do not add twice...
    $allVisibleLabIDX[] = 'l'.$nextData['idx'];
  }
}

foreach( $otherVisibleLabs as $nextIDX ){
  $allVisibleLabIDX[] = $nextIDX;
}
?>