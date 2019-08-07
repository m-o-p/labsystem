<?php

require("ProgressLab.inc");
require("ProgressPrelab.inc");
require("ProgressAggregate.inc");
require("PearlDrawing.inc");

function printIntervals($pge, $url, $nodeList) {
  $totalSum = 0;
  $pge->put('Intervals between i- or m-Elements:');
  foreach ($nodeList as $id => $node) {
    if ($node->getType() == 'i' || $node->getType() == 'm') {
      $totalSum += $node->timeInterval;
      $pge->put(
          '<li><a href="' . $url->link2('/Labsystem/pages/view.php?address=' . $id) . '">' .
               getTitle($id) . '</a>: ' . strval(floor($node->timeInterval / 3600)) . ' h ' .
               strval(floor(($node->timeInterval % 3600) / 60)) . ' min ' .
               strval($node->timeInterval % 60) . ' sec</li>');
    }
  }
  $pge->put(
      '<br>Total Sum: ' . strval(floor($totalSum / 3600)) . ' h ' .
           strval(floor(($totalSum % 3600) / 60)) . ' min ' . strval($totalSum % 60) . ' sec<br>');
}

// @formatter:off
// Lab Teams

/* Draw Lab/Teams */
$progressByTeam = [];
foreach (getTeamsOfLab($labIDX) as $labTeamNr) {
  $progressByTeam[$labTeamNr] = new ProgressLab($Logger, $labIDX, $labTeamNr);
}
// printIntervals($pge, $url, $nodeList);
$drawing = new PearlDrawing($progressByTeam, $teamNr);

/*
 * Draw Prelab/Users
 * $users = [ //ws2013
 * "96ae12d25ea7f9cf171496318a4fb54e", "f111113a3a20a3189dc8da44d024e167", //team 1
 * "d4578ce87cc23badf4179d6da81c80bb", "9483b6e66a105c986619ee48592143a5", //team 2
 * "7d160c7210876f93b26be3eac2c507e5", "90cd9f2e6d2af39b4b3b6014ad1b908b", //team 3
 * "6b5fed9dc13312f1a6919815886f49fe", "6887e763c99aeba0174ad877ce504091"  //team 4 ];
 * $prog = new ProgressPrelab($Logger, $labIDX, $users[3]);
 * $drawing->loadProgress($prog);
 * // printIntervals($pge, $url, $prog->getNodeList());
 */

$pge->put($drawing->generateDrawing());

?>
