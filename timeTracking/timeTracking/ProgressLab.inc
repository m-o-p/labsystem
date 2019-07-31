<?php

require("Progress.inc");

/**
 * This class determines the Progress of a team in a Lab
 * 
 * @author Fabian Oehlmann
 *
 */
class ProgressLab extends Progress {
  private $teamNr;

  /**
   * Creates the object
   * 
   * @param unknown $Logger
   * @param unknown $labIDX
   * @param unknown $teamNr
   */
  public function __construct($Logger, $labIDX, $teamNr) {
    $this->teamNr = $teamNr;
    Progress::__construct($Logger, $labIDX);
  }

  /**
   * (non-PHPdoc)
   * @see Progress::getUserString()
   */
  public function getUserString() {
    return 'Team: ' . $this->teamNr;
  }

  /**
   * (non-PHPdoc)
   * @see Progress::getTitle()
   */
  public function getTitle() {
    return getTitle('l' . $this->labIDX . '.C' . $this->lDBI->labCollectionIdx);
  }

  /**
   * (non-PHPdoc)
   * @see Progress::getUser()
   */
  protected function getUser() {
    return $this->teamNr;
  }

  /**
   * (non-PHPdoc)
   * @see Progress::userRestriction()
   */
  protected function userRestriction() {
    $where = ' teamNr = "' . $this->teamNr . '"';
    $where .= ' AND (resourceID LIKE "%C' . $this->lDBI->labCollectionIdx . '%"';
    $where .= ' OR resourceID = "l' . $this->labIDX . '.allLabQ"';
    $where .= ' OR resourceID = "l' . $this->labIDX . '.labStatus")';
    return $where;
  }

  /**
   * (non-PHPdoc)
   * @see Progress::labAddresses()
   */
  protected function labAddresses() {
    $lab = [];
    $lab[] = 'l' . $this->labIDX;
    $lab[] = 'l' . $this->labIDX . '.allLabQ';
    $lab[] = 'l' . $this->labIDX . '.labStatus';
    if (! $this->lDBI->noLab)
      $lab = getAllAddressesAsArray('l' . $this->labIDX . '.C' . $this->lDBI->labCollectionIdx, 
          $this->lDBI->lab);
    else
      $lab = [];
    return $lab;
  }
}

?>