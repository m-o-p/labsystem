<?php
class LmUserAnswer{
  private $answerArray;
  private $correctMask;
  private $displayPermutation;
  private $history;

  /**
   * Creates a new answer.
   * @param unknown $answerCount The amount of answers in the corresponding MC for creating the permutation array;
   * @param unknown $correctMask The correct answer as bit mask.
   */
  function __construct( $answerCount, $correctMask ){
    $this->answerArray = array();
    $this->correctMask = $correctMask;
    $this->displayPermutation = shuffle(range( 0, $answerCount-1 ));
    $this->history = getHistoryEntryLine('*');
  }

  function __construct( $answerArray, $correctMask, $displayPermutation, $history){
    $this->answerArray = $answerArray;
    $this->correctMask = $correctMask;
    $this->displayPermutation = $displayPermutation;
    $this->history = $history;
  }

  function getAnswerBitMask( $i ){
    return (isset($this->answerArray[$i]) ? $this->answerArray[$i] : 0);
  }
  function addAnswer($answerBitMask){
    if (is_numeric($answerBitMask)){
      $this->answerArray[] = $answerBitMask;
      $this->history = getHistoryEntryLine('*'.count($this->answerArray)).$this->history;
    }
  }

  function getDisplayPermutation(){
    return $this->displayPermutation;
  }

  function getHistory(){
    return $this->history;
  }

  function getTriesUp2Now(){
    return count($this->answerArray);
  }

  function isCorrectlyAnswered(){
    return (array_pop($this->answerArray) & $this->correctMask);
  }

  function isAnswered(){
    global $cfg;
    return $this->isCorrectlyAnswered() || $this->getTriesUp2Now() >= $cfg->get('MCAnsweringAttempts');
  }
}
?>