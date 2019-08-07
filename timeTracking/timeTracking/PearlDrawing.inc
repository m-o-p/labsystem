<?php

require("Canvas.inc");
require("Lollipop.inc");

/**
 * This class takes some lab progresses and returns an HTML5 canvas script to visualize them in a
 * graph with a thread of pearls. When a team is specified the progress of the team is displayed,
 * otherwise the performance of the average of the teams is displayed.
 *
 * @author Fabian Oehlmann
 *
 */

class PearlDrawing {
  private $canvas;

  private $progresses = [];         //the progresses of all teams
  private $progressAggregate;       //the progress of the average
  private $progressToDraw;          //the progress which will be visualized

  private $secondsPerPixel = 30;
  private $creditsPerPixel;         //graph has fixed width, so this is calculated

  private $offsetXl = 60;           //margin left of the graph
  private $offsetXr = 150;          //margin right of the graph
  private $offsetY = 50;            //margin above and below the graph
  private $graphWidth = 800;        //fixed
  private $graphHeight = 120;       //initialized with the minimum value

  //these are used for the statistics in the legend
  private $totalSecondsTeam;
  private $sumGivenCreditsTeam;
  private $sumPossibleCredits;

  /**
   * Calculates the average team's performance and generates the HTML5 canvas script.
   *
   * @param unknown $progresses Array of Progresses of each team, that participated in the lab. The key is expected to be the team number.
   * @param string $team        (optional) Team, whose progress should be displayed. If not given, the average team will be displayed.
   * @return string             HTML5 canvas script code
   */
  public function __construct($progresses, $team = null) {
  //print_r($progresses);
    $this->progresses = $progresses;
    $this->progressAggregate = new ProgressAggregate($progresses);
    if ($team != null) {
      $this->progressToDraw = $progresses[$team];
      $this->totalSecondsTeam = $this->progresses[$team]->getSumOfIntervalTimes();
      $this->sumGivenCreditsTeam = $this->progresses[$team]->getSumOfGivenCredits();
//       $this->progressAggregate->normalizeToTeam($this->totalSecondsTeam);
    } else {
      $this->progressToDraw = $this->progressAggregate;
      $this->totalSecondsTeam = $this->progressAggregate->getSumOfIntervalTimes();
      $this->sumGivenCreditsTeam = $this->progressAggregate->getSumOfGivenCredits();
    }
    $this->loadProgress($this->progressToDraw);
  }

  /**
   * Generates the HTML5 canvas script and returns it as string
   *
   * @return string The result
   */
  public function generateDrawing() {
    $this->canvas = new Canvas();

    $canvasWidth = $this->graphWidth + $this->offsetXl + $this->offsetXr;
    $canvasHeight = $this->graphHeight + 2 * $this->offsetY;
    $this->canvas->rectangle(0, 0, $canvasWidth, $canvasHeight);
    //$this->drawShadowAverage(); // TODO scale credits on left shadow correctly
    $this->drawShadowUnreachable();
    $this->drawBorder();
    $this->drawTicks();
    $this->drawPearlThread($this->progressToDraw);
    $this->drawPossiblePearls();
    $this->drawTitle();
    $this->drawLegend();

    return $this->canvas->finish($canvasWidth, $canvasHeight);
  }

  /**
   * Increases the graph height to fit the given progress and resets the creditsPerPixel
   *
   * @param unknown $progress The progress in question
   */
  private function loadProgress($progress) {
    $this->graphHeight = max($this->graphHeight,
        $progress->getSumOfIntervalTimes() / $this->secondsPerPixel);

    $this->sumPossibleCredits = $progress->getSumOfPossibleCredits();
    $this->creditsPerPixel = $this->sumPossibleCredits / $this->graphWidth;
  }

  /**
   * Calculates a color with regard to the time spent on a LabNode and either the credits given or
   * the credits possible
   *
   * @param unknown $node     The node in question
   * @param string $possible  (optional) Whether to use the possible credits instead of the given credits
   * @return string           The CSS color string
   */
  private function getColor($node, $possible = false) {
    if ($possible) {
      $alpha = $node->getPossibleCredits() * ($this->totalSecondsTeam / $this->sumPossibleCredits) /
           (0 == $node->timeInterval ? 1 : $node->timeInterval);
    } else {
      $alpha = $node->creditsGiven * ($this->totalSecondsTeam / $this->sumPossibleCredits) /
           (0 == $node->timeInterval ? 1 : $node->timeInterval);
    }
    $alpha /= 2;
    $alpha = min(1.0, $alpha);
    $h = strval($alpha * 120);
    $s = '100%';
    $l = '40%';
    return 'hsl(' . $h . ',' . $s . ',' . $l . ')';
  }

  /**
   * Draws the border and the diagonal of the graph
   */
  private function drawBorder() {
    // diagonal
    $this->canvas->line($this->offsetXl, $this->offsetY, $this->graphWidth + $this->offsetXl,
        $this->totalSecondsTeam / $this->secondsPerPixel + $this->offsetY, 0.5);
    // x-axis
    $this->canvas->line($this->offsetXl, $this->offsetY, $this->graphWidth + $this->offsetXl,
        $this->offsetY);
    // y-axis
    $this->canvas->line($this->offsetXl, $this->offsetY, $this->offsetXl,
        $this->graphHeight + $this->offsetY);
    // x2-axis
    $this->canvas->line($this->offsetXl, $this->graphHeight + $this->offsetY,
        $this->graphWidth + $this->offsetXl, $this->graphHeight + $this->offsetY);
    // y2-axis
    $this->canvas->line($this->graphWidth + $this->offsetXl, $this->offsetY,
        $this->graphWidth + $this->offsetXl, $this->graphHeight + $this->offsetY);
  }

  /**
   * Draws the ticks and the grid of the graph
   */
  private function drawTicks() {
    // y-axis - Time
    // every 60 pixels a small tick
    $smallYTickCount = intval($this->graphHeight / 60);
    for($i = 0; $i <= $smallYTickCount; $i ++) {
      $this->canvas->line($this->offsetXl, $this->offsetY + (60 * $i), $this->offsetXl + 5,
          $this->offsetY + (60 * $i));
      $hours = intval($i * $this->secondsPerPixel / 60);
      $minutes = $i * $this->secondsPerPixel % 60;
      if ($hours > 0) {
        $xTickLabel = $hours . 'h ' . $minutes . 'min';
      } else {
        $xTickLabel = $minutes . 'min';
      }
      $this->canvas->label($xTickLabel, 3, $this->offsetY + (60 * $i) + 3, '0');
    }
    // every hour a big tick
    $bigYTickCount = intval($this->graphHeight * $this->secondsPerPixel / 3600);
    for($i = 1; $i <= $bigYTickCount; $i ++) {
      $this->canvas->line($this->offsetXl, $this->offsetY + ($i * 3600 / $this->secondsPerPixel),
          $this->offsetXl + 10, $this->offsetY + ($i * 3600 / $this->secondsPerPixel));
      $this->canvas->lineDashed($this->offsetXl,
          $this->offsetY + ($i * 3600 / $this->secondsPerPixel), $this->graphWidth + $this->offsetXl,
          $this->offsetY + ($i * 3600 / $this->secondsPerPixel));
    }
    // x-axis - Credits
    $smallXTickWidth = intval($this->graphWidth / 10);
    $creditsPer10 = $this->sumPossibleCredits / 10;
    for($i = 0; $i <= 10; $i ++) {
      if ($i != 5) {
        $this->canvas->line($this->offsetXl + ($smallXTickWidth * $i), $this->offsetY,
            $this->offsetXl + ($smallXTickWidth * $i), $this->offsetY + 5);
      } else {
        $this->canvas->line($this->offsetXl + ($smallXTickWidth * $i), $this->offsetY,
            $this->offsetXl + ($smallXTickWidth * $i), $this->offsetY + 10);
        $this->canvas->lineDashed($this->offsetXl + ($smallXTickWidth * $i), $this->offsetY,
            $this->offsetXl + ($smallXTickWidth * $i), $this->graphHeight + $this->offsetY);
      }
      $this->canvas->label(strval(round($i * $creditsPer10, 2)) . 'cr',
          $this->offsetXl + ($smallXTickWidth * $i), $this->offsetY - 5, '0');
    }
  }

  /**
   * Draws a Lollipop a LabNode is visualized with in the Progress of a team
   *
   * @param unknown $lol      The Lollipop in question
   * @param string $drawCross Whether to draw the min and max values on the head of the Lollipop
   */
  private function drawLollipop($lol, $drawCross = true) {
    if ($lol->node->isAggregate() && $drawCross) {
      // draw cross
      // timeMin
      $ymin = $lol->toY - (($lol->node->timeInterval - $lol->node->timeMin) / $this->secondsPerPixel);
      $ymin = max($this->offsetY, $ymin);
      $this->canvas->line($lol->toX, $ymin, $lol->toX, $lol->toY, '1', $lol->color);
      // timeMax
      $ymax = $lol->toY + (($lol->node->timeMax - $lol->node->timeInterval) / $this->secondsPerPixel);
      $ymax = min($this->graphHeight + $this->offsetY, $ymax);
      $this->canvas->line($lol->toX, $lol->toY, $lol->toX, $ymax, '1', $lol->color);
      // creditsMin
      $xmin = $lol->toX - (($lol->node->creditsGiven - $lol->node->creditsMin) /
           $this->creditsPerPixel);
      $xmin = max($this->offsetXl, $xmin);
      $this->canvas->line($xmin, $lol->toY, $lol->toX, $lol->toY, '1', $lol->color);
      // creditsMax
      $xmax = $lol->toX + (($lol->node->creditsMax - $lol->node->creditsGiven) /
           $this->creditsPerPixel);
      $xmax = min($this->graphWidth + $this->offsetXl, $xmax);
      $this->canvas->line($lol->toX, $lol->toY, $xmax, $lol->toY, '1', $lol->color);
    }

    $this->canvas->line($lol->fromX, $lol->fromY, $lol->toX, $lol->toY, '2', $lol->color);
    if ($lol->labelFlag) {
      $timeLabelPosX = ($lol->fromX + $lol->toX) * 0.5 - 32;
      $timeLabelPosY = ($lol->fromY + $lol->toY) * 0.5 + 10;
      if ($lol->toY - $lol->fromY > 10) {
        $this->canvas->label($lol->minutes . 'min', $timeLabelPosX, $timeLabelPosY, '0');
      }
    }

    $this->canvas->circle($lol->toX, $lol->toY, 4, 'fill', $lol->color);
  }

  /**
   * Writes the title of a node to the right side of the graph
   *
   * @param unknown $node         The node in question
   * @param unknown $circlePosY   The y-coordinate of the node
   * @param unknown $minLabelPosY The
   */
  private function drawPearlTitle($node, $circlePosY, &$minLabelPosY) {
    $labelPosY = max($circlePosY, $minLabelPosY);
    $this->canvas->label(getTitle($node->resource), $this->graphWidth + $this->offsetXl + 5,
        $labelPosY + 3, '0');
    $minLabelPosY = $labelPosY + 12; // number 12 is question title height
  }

  /**
   * Calculates the coordinates of the next node relative to the coordinates of the last node
   *
   * @param unknown $node     The node in question
   * @param unknown $oldX     X-coordinate of the last node
   * @param unknown $oldY     Y-coordinate of the last node
   * @param unknown $newX     X-coordinate of the node in question to modify
   * @param unknown $newY     Y-coordinate of the node in question to modify
   * @param string $possible  Whether to use the possible credits instead of the given credits
   */
  private function nextPearl($node, &$oldX, &$oldY, &$newX, &$newY, $possible = false) {
    $oldX = $newX;
    $oldY = $newY;
    $credits = $possible ? $node->getPossibleCredits() : $node->creditsGiven;
    $newX += $credits / $this->creditsPerPixel;
    $newY += $node->timeInterval / $this->secondsPerPixel;
  }

  /**
   * Draws the line of nodes in the graph consisting of many Lollipops
   *
   * @param unknown $progress The Progress in question
   */
  private function drawPearlThread($progress) {
    $pearlX = $this->offsetXl;
    $pearlY = $this->offsetY;
    $oldPearlX = 0;
    $oldPearlY = 0;
    $minLabelPosY = $this->offsetY;
    $lollipops = array();
    foreach ($progress->getNodeList() as $id => $node) {
      if ($node->getType() == 'i' || $node->getType() == 'm') {
        $this->nextPearl($node, $oldPearlX, $oldPearlY, $pearlX, $pearlY);
        $color = $progress === $this->progressToDraw ? $this->getColor($node) : '#858585';
        $lollipops[] = new Lollipop($node, $oldPearlX, $oldPearlY, $pearlX, $pearlY,
            intval($node->timeInterval / 60), $color, $progress === end($this->progresses));
        if ($progress === $this->progressToDraw) {
          $this->drawPearlTitle($node, $pearlY, $minLabelPosY);
        }
      }
    }
    // draw lollipops in reverse order for correct "z-index"
    foreach (array_reverse($lollipops) as $lollipop) {
      $this->drawLollipop($lollipop);
    }
  }

  /**
   * Draws a shadow left, which depicts the normalized performance of the average team
   */
  private function drawShadowAverage() {
    $progress = $this->progressAggregate;
    $shadowX = $this->offsetXl;
    $shadowY = $this->offsetY;
    $oldShadowX = 0;
    $oldShadowY = 0;
    foreach ($progress->getNodeList() as $id => $node) {
      if ($node->getType() == 'i' || $node->getType() == 'm') {
        $this->nextPearl($node, $oldShadowX, $oldShadowY, $shadowX, $shadowY);
        $this->canvas->shadow(intval($oldShadowX), intval($oldShadowY), intval($shadowX),
            intval($shadowY), intval($this->offsetXl), 'rgba(192,192,192,0.5)');
      }
    }
  }

  /**
   * Draws a shadow right, which depicts how many credits a team can have maximal at a point (unless
   * it has bonus points)
   */
  private function drawShadowUnreachable() {
    $progress = $this->progressToDraw;
    $shadowX = $this->offsetXl;
    $shadowY = $this->offsetY;
    $oldShadowX = 0;
    $oldShadowY = 0;
    foreach ($progress->getNodeList() as $id => $node) {
      if ($node->getType() == 'i' || $node->getType() == 'm') {
        $this->nextPearl($node, $oldShadowX, $oldShadowY, $shadowX, $shadowY, true);
        $this->canvas->shadow(intval($oldShadowX), intval($oldShadowY), intval($shadowX),
            intval($shadowY), intval($this->offsetXl + $this->graphWidth), 'rgba(128,128,128,0.5)');
      }
    }
  }

  /**
   * Draws a thread of pearls on the right axis of the graph, which is colored with the possible
   * credits instead of the given ones
   */
  private function drawPossiblePearls() {
    $progress = $this->progressToDraw;
    $pearlY = $this->offsetY;
    $oldPearlY = 0;
    $lollipops = array();
    foreach ($progress->getNodeList() as $id => $node) {
      if ($node->getType() == 'i' || $node->getType() == 'm') {
        $this->nextPearl($node, $t, $oldPearlY, $t, $pearlY, true);
        $color = $this->getColor($node, true);
        $lollipops[] = new Lollipop($node, $this->offsetXl + $this->graphWidth, $oldPearlY,
            $this->offsetXl + $this->graphWidth, $pearlY, 0, $color);
      }
    }
    // draw lollipops in reverse order for correct "z-index"
    foreach (array_reverse($lollipops) as $lollipop) {
      $this->drawLollipop($lollipop, false);
    }
  }

  /**
   * Writes the title of the Progress which is depicted in the graph on top
   */
  private function drawTitle() {
    $title = $this->progressToDraw->getTitle();
    $this->canvas->title($title);
  }

  /**
   * Writes the legend on the top right corner
   */
  private function drawLegend() {
    $userStr = $this->progressToDraw->getUserString();
    $totalTimeStr = 'Total Time: ' . strval(floor($this->totalSecondsTeam / 3600)) . 'h ' .
         strval(floor(($this->totalSecondsTeam % 3600) / 60)) . 'min';
    $sumPossibleCreditsStr = 'Possible Credits: ' . $this->sumPossibleCredits . 'cr';
    $timePerPossibleCreditStr = 'Time / Possible Credits: ' .
         round(($this->totalSecondsTeam / 60) / $this->sumPossibleCredits, 2) . 'min/cr';
    $sumGivenCreditsStr = 'Given Credits: ' . $this->sumGivenCreditsTeam . 'cr';
    $timePerGivenCreditStr = 'Time / Given Credits: ' .
         round(($this->totalSecondsTeam / 60) / $this->sumGivenCreditsTeam, 2) . 'min/cr';

    $legendStrs = array(
        $userStr,
        $totalTimeStr,
        $sumPossibleCreditsStr,
        $timePerPossibleCreditStr,
        $sumGivenCreditsStr,
        $timePerGivenCreditStr
    );
    $this->canvas->legend($legendStrs, $this->offsetXl + $this->graphWidth - 10,
        $this->offsetY + 10);
  }
}

?>
