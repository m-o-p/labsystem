<?php

/**
 * This class describes a Timespan between two timestamps
 * 
 * @author fabian
 *
 */
class Timespan {
  public $start;
  public $end;
  public $resource;
  public $role;

  /**
   * Creates the object
   * 
   * @param unknown $start    When the Timespan has started 
   * @param unknown $end      When it ended
   * @param unknown $resource What lab resource it was spent in
   * @param unknown $role     In what role it was done
   */
  function __construct($start, $end, $resource, $role) {
    $this->start = $start;
    $this->end = $end;
    $this->resource = $resource;
    $this->role = $role;
  }

  /**
   * Difference between end and start
   * 
   * @return number
   */
  function length() {
    return $this->end - $this->start;
  }
}

?>