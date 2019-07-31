<?php

require_once("Progress.inc");

/**
 * This class determines the Progress of a user in a Prelab
 * 
 * @author Fabian Oehlmann
 *
 */
class ProgressPrelab extends Progress {
  private $userID;

  /**
   * Creates the object
   * 
   * @param unknown $Logger
   * @param unknown $labIDX
   * @param unknown $userID
   */
  public function __construct($Logger, $labIDX, $userID) {
    $this->userID = $userID;
    Progress::__construct($Logger, $labIDX);
  }

  /**
   * (non-PHPdoc)
   * @see Progress::getUserString()
   */
  public function getUserString() {
    return 'User ID: ' . $this->userID;
  }

  /**
   * (non-PHPdoc)
   * @see Progress::getTitle()
   */
  public function getTitle() {
    return getTitle('l' . $this->labIDX . '.C' . $this->lDBI->prelabCollectionIdx);
  }

  /**
   * (non-PHPdoc)
   * @see Progress::getUser()
   */
  protected function getUser() {
    return $this->userID;
  }

  /**
   * (non-PHPdoc)
   * @see Progress::userRestriction()
   */
  protected function userRestriction() {
    $where = ' userID = "' . $this->userID . '"';
    $where .= ' AND (resourceID LIKE "%C' . $this->lDBI->prelabCollectionIdx . '%"';
    $where .= ' OR resourceID = "l' . $this->labIDX . '.allPrelabQ"';
    $where .= ' OR resourceID = "l' . $this->labIDX . '.labStatus")';
    return $where;
  }

  /**
   * (non-PHPdoc)
   * @see Progress::labAddresses()
   */
  protected function labAddresses() {
    $preLab = [];
    $preLab[] = 'l' . $this->labIDX;
    $preLab[] = 'l' . $this->labIDX . '.allPrelabQ';
    $preLab[] = 'l' . $this->labIDX . '.labStatus';
    if (! $this->lDBI->noPrelab) {
      $preLab = array_merge($preLab,
          getAllAddressesAsArray('l' . $this->labIDX . '.C' . $this->lDBI->prelabCollectionIdx,
              $this->lDBI->preLab));
    } else {
      $preLab = [];
    }
    return $preLab;
  }
}

?>