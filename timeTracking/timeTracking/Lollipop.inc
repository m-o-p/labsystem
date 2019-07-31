<?php

/**
 * Lollipops are the elements a pearl thread in the graph consists of. The start at the last node
 * and have their head where the team/user is in the graph after adding the time tracked for the
 * node and the credits given for the question.
 * 
 * @author Fabian Oehlmann
 *
 */
class Lollipop {
  public $node;

  public $fromX;
  public $fromY;

  public $toX;
  public $toY;

  public $minutes;
  public $color;
  public $labelFlag;

  public function __construct($node, $fromX, $fromY, $toX, $toY, $minutes, $color = '#000000', $labelFlag = FALSE) {
    $this->node = $node;

    $this->fromX = $fromX;
    $this->fromY = $fromY;

    $this->toX = $toX;
    $this->toY = $toY;

    $this->minutes = $minutes;
    $this->color = $color;
    $this->labelFlag = $labelFlag;
  }
}

?>