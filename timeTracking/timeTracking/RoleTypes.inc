<?php

/**
 * This class provides mnemonics for the classification of events, i.e. in which role an event has
 * occured.
 * 
 * @author Fabian Oehlmann
 *
 */
abstract class RoleTypes {
  const activity = 0;
  const viewer = 1;
  const editor = 2;
  const student = 3;
  const corrector = 4;
}

?>