<?php

require_once("LabNode.inc");

/**
 * This class extends the LabNode for aggregate information. The only difference is that it contains
 * additionally min and max values for the time tracked and the credits given.
 *
 * @author fabian
 *
 */
class LabNodeAggregate extends LabNode {
  public $timeMin = 0;
  public $timeMax = 0;

  public $creditsMin = 0;
  public $creditsMax = 0;

  /**
   * (non-PHPdoc)
   * @see LabNode::isAggregate()
   */
  public function isAggregate() {
    return true;
  }
}

?>
