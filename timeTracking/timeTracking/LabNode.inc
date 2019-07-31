<?php

require_once("RoleTypes.inc");
require_once("AO.inc");
require_once("Timespan.inc");

/**
 * A class with helper functions for lab-elements and pages. It serves as store and getter for
 * times tracked on an element and the given/possible credits on i- and m-elements.
 * 
 * @author Fabian Oehlmann
 *
 */
class LabNode {
  public $resource;
  public $lastPotClose;
  public $timeInterval = 0;
  public $creditsGiven = 0;

  private $timeSpans = array();

  /**
   * Creates the LabNode
   * 
   * @param unknown $resource The address of the element (e.g. l3.C42.c50.p4)
   */
  public function __construct($resource) {
    $this->resource = $resource;
  }

  /**
   * Gives the Timespans which were tracked on this element.
   * 
   * @return multitype: Array of Timespans
   */
  public function getTimespans() {
    return $this->timeSpans;
  }

  /**
   * Adds a Timespan that was tracked to the element. Timespans, which have zero length are not
   * saved, unless they were in the student role. This is to determine whether an i- or m-element
   * was opened. Depending on the tracking logic the attention might not be tracked otherwise, if
   * another element in the collection also fires an open-event immediately afterwards.
   * 
   * @param unknown $start  Timestamp of when it started
   * @param unknown $end    Timestamp of when it ended
   * @param unknown $role   The role in which the Timespan was recorded
   */
  public function addTimespan($start, $end, $role) {
    if ($start < $end || $role == RoleTypes::student) {
      $this->timeSpans[] = new Timespan($start, $end, $this->resource, $role);
    }
  }

  /**
   * Returns the type of the element (i.e. l, C, c, p, m, i, or s)
   */
  public function getType() {
    return substr(end(explode('.', $this->resource)), 0, 1);
  }

  /**
   * Gets how many credits can be obtained on this element.
   * 
   * @return number The amount of credits (zero on non-i- and non-m-elements)
   */
  public function getPossibleCredits() {
    if ($this->getType() == 'i') {
      $credits = $GLOBALS['iDBI']->getData2idx($this->getIdx())->getPossibleCredits();
    } else if ($this->getType() == 'm') {
      $credits = $GLOBALS['mDBI']->getData2idx($this->getIdx())->getPossibleCredits();
    } else
      $credits = 0;
    return $credits;
  }

  /**
   * Gets the amount of credits which were given to a team (in case of i-elements) or user (in case
   * of m-elements) on this element.
   * 
   * @param unknown $userOrTeam The user or team in question
   * @return number             The amount of credits
   */
  public function getGivenCredits($userOrTeam) {
    if ($this->getType() == 'i') {
      $element = $GLOBALS['iDBI']->getData2idx($this->getIdx());
      $element->loadUserData4($userOrTeam);
      $credits = $element->getGivenCredits();
    } else if ($this->getType() == 'm') {
      $element = $GLOBALS['mDBI']->getData2idx($this->getIdx());
      $element->loadUserData4($userOrTeam);
      $credits = $element->getGivenCredits();
    } else {
      $credits = 0;
    }
    return $credits;
  }

  /**
   * Whether the LabNode is an aggregate node, which contains max and min values for the given
   * credits and time tracked
   * 
   * @return boolean
   */
  public function isAggregate() {
    return false;
  }

  /**
   * Returns the index of the element
   */
  private function getIdx() {
    return substr(end(explode('.', $this->resource)), 1);
  }
}

?>