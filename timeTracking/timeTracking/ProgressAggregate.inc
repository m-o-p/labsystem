<?php

require_once("LabNodeAggregate.inc");

/**
 * The class calculates the progress of the average of the progresses of different teams
 * 
 * @author Fabian Oehlmann
 *
 */
class ProgressAggregate implements iDrawable {
  private $nodeList = array();
  private $progresses;

  /**
   * Create the Progress of the average team.
   * 
   * @param unknown $progresses An array containing the progresses of the single teams
   */
  public function __construct($progresses) {
    $this->progresses = $progresses;
    $this->generateNodeList(reset($progresses)->getNodeList());

    $this->aggregateProgress();
  }

  /**
   * (non-PHPdoc)
   * @see iDrawable::getNodeList()
   */
  public function getNodeList() {
    return $this->nodeList;
  }

  /**
   * (non-PHPdoc)
   * @see iDrawable::getUserString()
   */
  public function getUserString() {
    return 'Team: Aggregate';
  }

  /**
   * (non-PHPdoc)
   * @see iDrawable::getTitle()
   */
  public function getTitle() {
    return reset($this->progresses)->getTitle();
  }

  /**
   * Normalizes the time spent on each LabNode to the total time spent by one team
   * 
   * @param unknown $totalTimeOfTeam  The total time to normalize to
   */
  public function normalizeToTeam($totalTimeOfTeam) {
    $totalTimeOfAggregate = 0;
    foreach ($this->nodeList as $node) {
      $totalTimeOfAggregate += $node->timeInterval;
    }
    foreach ($this->nodeList as $node) {
      $node->timeInterval *= $totalTimeOfTeam / $totalTimeOfAggregate;
    }
  }

  /**
   * Returns the sum of the time spent on all questions
   * 
   * @return number
   */
  public function getSumOfIntervalTimes() {
    $progTotalSeconds = 0;
    foreach ($this->nodeList as $node) {
      $progTotalSeconds += $node->timeInterval;
    }
    return $progTotalSeconds;
  }

  /**
   * Returns the sum of the credits given on all questions
   * 
   * @return number
   */
  public function getSumOfGivenCredits() {
    $progSumGivenCredits = 0;
    foreach ($this->nodeList as $node) {
      $progSumGivenCredits += $node->creditsGiven;
    }
    return $progSumGivenCredits;
  }

  /**
   * Returns the sum of all credits theoretically attainable on all questions
   * 
   * @return number
   */
  public function getSumOfPossibleCredits() {
    $progSumPossibleCredits = 0;
    foreach ($this->nodeList as $node) {
      $progSumPossibleCredits += $node->getPossibleCredits();
    }
    return $progSumPossibleCredits;
  }

  /**
   * Makes a list of LabNodes containing only Nodes for question elements
   * 
   * @param unknown $sourceNodeList The list of nodes of one of the single teams
   */
  private function generateNodeList($sourceNodeList) {
    foreach ($sourceNodeList as $id => $node) {
      if ($node->getType() == 'i' || $node->getType() == 'm') {
        $this->nodeList[] = new LabNodeAggregate($id);
      }
    }
  }

  /**
   * Performs the aggregation from all of the single teams' values
   */
  private function aggregateProgress() {
    foreach ($this->nodeList as $id => $node) {
      $this->setTime($node);
      $this->setCredits($node);
    }
  }

  /**
   * Aggregates the time of the interval spent on one LabNode from the single teams
   * 
   * @param unknown $node
   */
  private function setTime($node) {
    $intervalTimes = array_map(
        function ($teamProg) use($node) {
          return $teamProg->getNodeList()[$node->resource]->timeInterval;
        }, $this->progresses);
        $node->timeMin = min($intervalTimes);
        $node->timeMax = max($intervalTimes);
        $node->timeInterval = array_sum($intervalTimes) / count($intervalTimes);
  }

  /**
   * Aggregates the credits given on one LabNode from the single teams
   * 
   * @param unknown $node
   */
  private function setCredits($node) {
    $credits = array_map(
        function ($teamProg) use($node) {
          return $teamProg->getNodeList()[$node->resource]->creditsGiven;
        }, $this->progresses);
        $node->creditsMin = min($credits);
        $node->creditsMax = max($credits);
        $node->creditsGiven = array_sum($credits) / count($credits);
  }
}

?>