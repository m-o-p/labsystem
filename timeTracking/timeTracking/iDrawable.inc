<?php

/**
 * The interface a Progress object must fullfill in order to be able to be drawn in a PearlDrawing
 * 
 * @author Fabian Oehlmann
 *
 */
interface iDrawable {

  /**
   * Contains the i- or m-elements as LabNodes with the intervals to be drawn 
   */
  public function getNodeList();

  /**
   * What title the curve has (appears on top of the graph)
   */
  public function getTitle();

  /**
   * Whose attention was tracked (appears in the legend of the graph)
   */
  public function getUserString();
}

?>