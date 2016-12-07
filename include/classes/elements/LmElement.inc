<?php
/**
 *  labsystem.m-o-p.de -
 *                  the web based eLearning tool for practical exercises
 *  Copyright (C) 2010  Marc-Oliver Pahl
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
* implementation of the m (multiple choice) element.
*
* This element consists of a question with multiple choices as answer the user can checkmark.
*
* @module     ../include/classes/elements/LmElement.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/

require_once( INCLUDE_DIR."/classes/elements/Element.inc" );
// The following DBI is used in loadUserData4() (connects to the user's answers).
require( INCLUDE_DIR."/classes/elements/LmDBInterfaceAnswers.inc" );

class LmElement extends Element{
  var $question;                    // The question.
  var $answerArray;                 // Array containing the answers.
  protected $reactionArray;         // Array that contains the hints that the system shows on wrong answers and the credit reduction.
  var $answerExplanation;           // Why is the answer correct? Shown after all tries or correct answer.
  var $correctMask;                 // Bitmask: Which answers are correct? (0010 = Third answer is correct).
  protected $possibleCredits;       // The credits one can get when answering this question correctly.
  protected $doNotShuffleAnswers;   // If not set the answers are shuffled to prevent the students from exchanging then in the "stupid" way ("3 + 4 are correct").
                                    // The shuffling happens only once to allow memorizing the answers on future tries.
  protected $onlyOneAnswerPossible; // Shows radio buttons instead of check boxes enabling to check only one item.
  protected $requiresNcorrect;      // The answer is correct when at least N correct checkboxes are set.
  protected $enableReactions;       // Shall it be possible to enter context-sensitive hints?
  protected $showHintForClicked;    // Shall the reaction be shown for the clicked or all wrong answers?
  protected $reshowLastHint;        // If no hint is given for the answer use the last one specified?
  protected $enableDetailedGrading; // Shall it be possible to give detailed credits?
  protected $horizontalLayout;      // Displays the answers horizontally instead of vertically.
  var $visibleFor;                  // Visibility constraints.

  var $answerCount;             // Number of answers.
  var $displayPermutation;      // Array containing the permutated Order of answers.
  var $previousAnswersArray;    // Array containing the bitmasks of previous answers.
  var $triesUp2Now;             // Number of tries the user had.
  var $isCorrectlyAnswered;         // Was the last answer given correct?
  var $isAnswered;                  // Is the last answer correct or the number of tries too high?
  var $answerHistory;               // Who answered when?


  /**
  * The constructor.
  *
  * @param $idx                       This instance's index.
  * @param $question                  The multiple choice question.
  * @param $answerArray               Array containing the answers.
  * @param $reactionArray             Array for the interactive reactions by the system.
  * @param $answerExplanation         Explanation why the answer is correct.
  * @param $correctMask               Bitmask telling which answers are correct.
  * @param $possibleCredits           The amount of credits one can get.
  * @param $doNotShuffleAnswers       Display the answers in the order they are stored.
  * @param $onlyOneAnswerPossible     Only allow one answer (radio buttons).
  * @param $requiresNcorrect          How many of the correct answers have to be clicked to grade this Q as correct?
  * @param $enableReactions           Shall it be possible to enter context-sensitive hints?
  * @param $showHintForClicked        Shall the reaction be shown for the clicked or all wrong answers?
  * @param $reshowLastHint            If no hint is given for the answer use the last one specified?
  * @param $enableDetailedGrading     Shall it be possible to give detailed credits?
  * @param $horizontalLayout          Show the answers horizontally placed.
  * @param $visibleFor                The bitmask of rights a viewer has to match.
  * @param $visibleOnlyInCollection   Is the element visible only inside a collection.
  * @param $history                   This' history.
  * @param $isInColl                  Am I inside a collection? (important for visibility aspects).
  */
  function __construct( $idx,
                        $question,
                        $answerArray,
                        $reactionArray,
                        $answerExplanation,
                        $correctMask,
                        $possibleCredits,
                        $doNotShuffleAnswers,
                        $onlyOneAnswerPossible,
                        $requiresNcorrect,
                        $enableReactions,
                        $showHintForClicked,
                        $reshowLastHint,
                        $enableDetailedGrading,
                        $horizontalLayout,
                        $visibleFor,
                        $visibleOnlyInCollection,
                        $history,
                        $isInColl=false ){
    global $usr, $cfg;
    $this->elementId                = "m";
    $this->idx                      = $idx;
    $this->title                    = trim(strip_tags(preg_replace("(\r\n|\n|\r)", '', // used to remove linebreaks
                                        returnHTML( // remove [HTML] etc.
                                          substr( $question, 0, 50 )
                                        )))
                                      );
    $this->matchingMenu             = '';
    $this->question                 = $question;
    $this->answerArray              = $answerArray;
    $this->reactionArray            = $reactionArray;

    $this->answerCount              = count( $this->answerArray );
    $this->answerExplanation        = $answerExplanation;
    $this->correctMask              = $correctMask;
    $this->possibleCredits          = $possibleCredits;
    $this->doNotShuffleAnswers      = $doNotShuffleAnswers;
    $this->onlyOneAnswerPossible    = $onlyOneAnswerPossible;
    $this->requiresNcorrect         = $requiresNcorrect;
    $this->enableReactions          = $enableReactions;
    $this->showHintForClicked       = $showHintForClicked;
    $this->reshowLastHint           = $reshowLastHint;
    $this->enableDetailedGrading    = $enableDetailedGrading;
    $this->horizontalLayout         = $horizontalLayout;

    $this->visibleFor               = $visibleFor;
    $this->visibleOnlyInCollection  = $visibleOnlyInCollection;
    $this->history                  = $history;

    $this->IamVisible               = (($this->visibleFor == '0') || $usr->isOfKind( $this->visibleFor ))
                                      &&
                                      ( !$this->visibleOnlyInCollection || ($this->visibleOnlyInCollection && $isInColl) );

    $this->mcDBI                    = new LmDBInterfaceAnswers();
    $this->loadUserData4(''); // Fills rest of the structures with neutral values.

    $this->serializationArray       = array( 'idx', 'question', 'answerArray', 'answerExplanation', 'correctMask', 'visibleFor', 'visibleOnlyInCollection', 'history' ); // see Element.inc for details
  }

  // Selectors
  function getQuestion()               { return $this->question; }
  function getAnswerArray()            { return $this->answerArray; }
  function getReactionArray()          { return $this->reactionArray; }
  function getAnswerExplanation()      { return $this->answerExplanation; }
  function getCorrectMask()            { return $this->correctMask; }
  function getPossibleCredits()        { return $this->possibleCredits; }
  function getDoNotShuffleAnswers()    { return $this->doNotShuffleAnswers; }
  function getOnlyOneAnswerPossible()  { return $this->onlyOneAnswerPossible; }
  function getRequiresNcorrect()       { return $this->requiresNcorrect; }
  function getReactionsEnabled()       { return $this->enableReactions; }
  function getHintForKlicked()         { return $this->showHintForClicked; }
  function getReshowLastHint()         { return $this->reshowLastHint; }
  function getDetailedGradingEnabled() { return $this->enableDetailedGrading; }
  function getHorizontalLayout()       { return $this->horizontalLayout; }
  function getVisibleFor()             { return $this->visibleFor; }


  function isAnswered(){
    return $this->isAnswered;
  }
  function isCorrectlyAnswered(){
    return $this->isCorrectlyAnswered;
  }
  
  /**
   * Returns if the given $answer is considered correct.
   * @param unknown $answerMask
   * @return boolean Returns if the given $answer is considered correct.
   */
  function computeIfAnswerConsideredCorrect($answerMask){
  	if ($answerMask == $this->correctMask){
		return TRUE;  		
  	}
  	
  	if (!is_null($this->requiresNcorrect)){
  		// fewer correct answers are considered okay
  		// The algorithm is correctlyChecked-wronglyChecked =
  		// [all answers that should be checked and are checked] - 
  		// [all answers that should not be checked but are checked]
  		$answerNumber = $this->answerCount;
  		$amountCheckedCorrectAnswers = 0;
  		while (--$answerNumber >=0){
  			$isAnswerCorrect = (($this->correctMask & (1<<$answerNumber)) == 1<<$answerNumber);
  			$didUserCheckAnswer = (($answerMask & (1<<$answerNumber)) == (1<<$answerNumber));
  			if ($didUserCheckAnswer){
  				if ($isAnswerCorrect){
  					$amountCheckedCorrectAnswers++; // correctly checked: -1
  				}else{
  					$amountCheckedCorrectAnswers--; // wrongly checked: -1
  				}
  			}
  		}
  		if ($amountCheckedCorrectAnswers>=$this->requiresNcorrect){
  			return TRUE;
  		}
  	}
      			
  	return FALSE;
  }
  
  /**
  * Loads the answer data to the given $uid into this instance's field.
  *
  * @param $uid   The UID to load the data of.
  *
  * @return       The LmDBInterfaceAnswers() or FALSE.
  */
  function loadUserData4( $uid ){
    global $usr, $cfg, $url;
    if ( !empty($uid) ){
      $answerData = $this->mcDBI->getData4( $uid,
                                            $this->idx,
                                            $this->answerCount,
                                            !$usr->isSeeingSomeonesData() || $url->available('hideExampleSolutionInThisTab')
          );

      $this->answerHistory        = $answerData["history"];
      $this->displayPermutation   = $answerData['permutation'];
      $this->previousAnswersArray = $answerData['answers'];
      $this->triesUp2Now          = count( $this->previousAnswersArray )-1; // Last holds the current try.
      // If the answer is considered correct depends on the "requiresNcorrect" parameter
      $this->isCorrectlyAnswered  = ( ( $this->triesUp2Now > 0 ) && 
      								  $this->computeIfAnswerConsideredCorrect( $this->previousAnswersArray[$this->triesUp2Now-1] ) ); 
      $this->isAnswered =  $this->isCorrectlyAnswered || ( $this->triesUp2Now >= $cfg->get("MCAnsweringAttempts") );
    } else { // the example solution
      $this->answerHistory        = '';
      $this->displayPermutation   = range( 0, $this->answerCount-1 ); // identity
      $this->previousAnswersArray  = false;
      $this->triesUp2Now          = 0;
      $this->isCorrectlyAnswered  = false;
      $this->isAnswered           = true;
    }
  }


  /**
   * Returns the credits given by the corrector.
   *
   * Since I did not implement any credits you get one
   * credit less for each answering attempt...
   *
   * @return $cfg->get( 'MCAnsweringAttempts' )-$this->answerCount
   */
   function getGivenCredits(){
     global $cfg;
     if ( $this->isCorrectlyAnswered ) $add = 1; else $add = 0;
     return $this->getPossibleCredits()-$this->getPossibleCredits()/$cfg->get( 'MCAnsweringAttempts' )*$this->triesUp2Now;
   }

  /**
  * Returns a bitmask with the $setBit-th bit set.
  *
  * @param $setBit  The number of the bit that shoud be set.
  *
  * @return         A bitmask with the $setBit-th bit set.
  */
  function mask( $setBit ){
    return (1 << $setBit );
  }

// The next functions are used for the element's view page. *************************************************

// helper (if this exists some time they should be declared private)  - - - - - - - - - - - - - - - - - - -

  /**
   * Returns the last available hint of or before $roundNr for the answer $answerNr.
   * @param unknown $answerNr
   * @param unknown $roundNr
   */
  private function getHint($answerNr, $roundNr){
    for ($i=$roundNr; $i>=0; $i--){
      if (isset($this->reactionArray[$answerNr][$i]) && !empty($this->reactionArray[$answerNr][$i]['hint'])){
        return $this->reactionArray[$answerNr][$i]['hint'];
      }
    }
    return '';
  }

  /**
   * Returns all hints to the current answer matrix.
   */
  private function getHints(){
    $hints = '';
    if ($this->getReactionsEnabled() && !$this->isAnswered() && $this->triesUp2Now > 0){
      // Diplay the reaction hints if available
      for ($answerNumber=0; $answerNumber<count($this->answerArray); $answerNumber++){
        $currentRound = $this->triesUp2Now-1;
        $isAnswerCorrect = (($this->correctMask & (1<<$answerNumber)) == 1<<$answerNumber);
        $didUserCheckAnswer = (isset($this->previousAnswersArray[$currentRound]) && (($this->previousAnswersArray[$currentRound] & (1<<$answerNumber)) == (1<<$answerNumber)));
        if ($isAnswerCorrect == $didUserCheckAnswer){
          // correctly checked, so no hint
          continue;
        }
        // wrongly clicked, so give appropriate hints
        if (($this->showHintForClicked && $didUserCheckAnswer) || !$this->showHintForClicked){
          $hints .= $this->getHint($answerNumber, $currentRound).'';
        }
      }
    }
    return trim($hints);
  }
  /**
  * This function displays the answer and contains some logic.
  *
  * @param $fullAddress   The elements full address (needed for answer control).
  *                       Since there might be many independent multiple choice questions
  *                       on one page the user might tend to answer multiple questions be-
  *                       for saving. But the save button of each mc only saves its answer
  *                       so all the other answers would be lost. To prevent from this I
  *                       use a little trick:
  *                       The checkboxes are disabled until the userpresses the answer button.
  *                       Then he can answer and the button is used to save.
  *
  * @return               The html-code of the answer.
  */
  function showAnswer( $fullAddress ){
    global $usr, $lng, $pge, $url, $cfg;

    $isGettingAnswered = ( isset( $_POST['FULLADDRESS'] ) && ( $_POST['FULLADDRESS'] == $fullAddress ) );
    $isAnswerable      = !$this->isAnswered &&                  // not done
                         !$usr->isSeeingSomeonesData() &&       // my answer
                         ( !isset( $_POST['FULLADDRESS'] ) ||   // no other is getting answered at the moment
                           $isGettingAnswered );                // this is the one that gets currently answered
      /* This is a little tricky:
           To prevent the user from forgetting to save only one mc can be answered at once.
           This is controlled here:
            - The checkboxes are disabled by default.
            - Below the answer possibilities is a button "give answers"
              If pressed the element sends its $fulladdress to itself.
            - If the posted fulladdress == this elements full address:
              - The checkboxes are enabled
              - The button below the answers gets the save button.
       */

    $ret = ""; // The html-code to be returned.

    // If the element is not answered a FORM is needed.
    if ( $isAnswerable ){
      $ret .= "<FORM class=\"labsys_mop_std_form\" NAME=\"multipleChoiceEdit".$this->idx."\" METHOD=\"POST\" ".
               "ACTION=\"".( $isGettingAnswered ?
                                      $url->link2('../php/executeCommandOnElement.php', Array('address' => 'm'.$this->idx) ) :         // save button functionality
                                      $url->link2('../pages/view.php', Array('address' => $url->get('address')) )."#".$fullAddress // give answer button functionality
                                     ).
              "\">\n".
              "<input type=\"hidden\" name=\"FUNCTIONNAME\" value=\"saveUserAnswer\">\n". // this function will statically be called by ../php/executeCommandOnElement.php
              "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/view.php?address=".$url->get('address')."#".$fullAddress."\">\n". // return so self
              "<input type=\"hidden\" name=\"FULLADDRESS\" value=\"".$fullAddress."\">\n".
              "<input type=\"hidden\" name=\"IDX\" value=\"".$this->idx."\">\n";            // needed by ../php/executeCommandOnElement.php
    }

    $ret.=$this->getAnswerFieldsHTML( FALSE, $isGettingAnswered );

    //// The answer(attempt)s will be put inside a table.
    //$ret .= "<table id=\"aTable".$this->idx."\" class=\"labsys_mop_m_answers_table\" cellspacing=\"5px\">\n".
    //        "<tr><!-- fixed layout -->";
    //        // all previous answering attempts
    //        for ( $i=0; $i < $this->triesUp2Now; $i++ ) $ret .= "<td class=\"labsys_mop_m_answers_table_checkbox_cell\"></td>";

    //        // the solution
    //        if ( !$this->isAnswered || !$this->isCorrectlyAnswered ) $ret .= "<td class=\"labsys_mop_m_answers_table_checkbox_cell\"></td>";

    //        // the answers
    //        $ret .= "<td class=\"labsys_mop_m_answers_table_answer_cell\"></td></tr>\n";

    //for ( $i=0; $i < $this->answerCount; $i++){ // iteration over the number of answers
    //  $ret .=  "<tr class=\"labsys_mop_m_answers_table_row\">\n";
    //  $actualAnswer = $this->displayPermutation[$i];

    //  for ( $j=0; $j < $this->triesUp2Now; $j++ ) { // Iteration over the previous answers
    //    $actualMask = $this->previousAnswersArray[$j]; // the answer given there

    //    $ret .=  "\t<td class=\"labsys_mop_m_answers_table_checkbox_cell_".( $actualMask == $this->correctMask ?  "right" :  "wrong"  )."\">".
    //             "<input type=\"checkbox\" ".
    //                    "tabindex=\"".$pge->nextTab++."\" ".
    //                    "title='".$lng->get( 'isCorrect' )."' ".
    //                     ( ($actualMask & LmElement::mask($actualAnswer)) ?
    //                                "checked=\"checked\" " : ''
    //                               ).
    //                     "disabled=\"disabled\" ".
    //              " onchange='isDirty=true'></td>\n";
    //   } // Iteration over the previous answers (end)

    //   if ( !$this->isAnswered ) $actualMask = $this->previousAnswersArray[$this->triesUp2Now]; // the current answer
    //                        else $actualMask = $this->correctMask; // the example solution

    //   if ( !$this->isAnswered || !$this->isCorrectlyAnswered ){
    //   // show current answer or example solution
    //    $ret .=  "\t<td class=\"labsys_mop_m_answers_table_checkbox_cell\">".
    //             "<input type=\"checkbox\" ".
    //                    "id=\"m".$this->idx."a".$actualAnswer."\" ".
    //                    "name=\"ANSWER".$actualAnswer."\" ".
    //                    "value=\"".LmElement::mask($actualAnswer)/*2^$actualAnswer*/."\" ".
    //                    "tabindex=\"".$pge->nextTab++."\" ".
    //                     ( ($actualMask & LmElement::mask($actualAnswer)) ?
    //                                "checked=\"checked\" " : ''
    //                               ).
    //                     ( !$isGettingAnswered ?
    //                                "disabled=\"disabled\" " : ''
    //                               ).
    //              " onchange='isDirty=true'></td>\n";
    //   }

    //   // The answers:
    //   $ret .=  "\t<td class=\"labsys_mop_m_answers_table_answer_cell\">".
    //            "<label for=\"m".$this->idx."a".$actualAnswer."\">".
    //            ( ($this->isAnswered && !($this->correctMask & LmElement::mask($actualAnswer))) ?  "<span class=\"labsys_mop_grayed\">"  : '' ).
    //            returnHTML( $this->answerArray[$actualAnswer] ).
    //            ( ($this->isAnswered && !($this->correctMask & LmElement::mask($actualAnswer))) ?  "</span>"  : '' ).
    //            "</label>".
    //            "</td>\n</tr>\n";
    //} // iteration over the number of answers (end)

    //$ret .= "</table>\n";


    $hints = $this->getHints();

    if(!empty($hints)){
      $ret .= '<div class="mcAnswerHints">'.$hints.'</div>';
    }

     // If the element is not answered a FORM is needed.
     if ( $isAnswerable ){ // Form is needed -> button
       $ret .= "<input tabindex=\"".$pge->nextTab++."\" ".
                      "type=\"submit\" ".
                      "class=\"labsys_mop_m_save_button\" ".
                      "value=\"".( $isGettingAnswered ?
                                            $lng->get("save") :
                                            $lng->get("giveAnswer")
                                           )."\" onclick='isDirty=false'>\n".
               "</FORM>\n";
// This is used bythe C element to show the checkup button...
       $GLOBALS['linkForMCcheckUp'] = $fullAddress;
               if ($isGettingAnswered){
                 $GLOBALS['Logger']->logToDatabase($fullAddress, logActions::mStudentOpen);
               }
     }

     if ( $this->isAnswered && ($this->answerExplanation != "") )
      // answer explanation
      $ret .= "<div class=\"labsys_mop_m_answer_explanation\">".returnHTML( $this->answerExplanation )."</div>\n";

     // answer history
     if ( $this->triesUp2Now > 0) $ret .= EM::viewMultipleChoiceAnswerHistory( $this );

     return $ret;
  }

// /helper - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  /**
  * Returns the element's HTML-representation in the way the VIEWER should see it.
  *
  * @param $fullAddress   The element's full address (p.e. C23.c42.p13)
  * @param $paragraph     The element's paragraph number (p.e. 2.3). Will be added to the title if given.
  * @return string        The html code of the element's view view.
  */
  function show( $fullAddress, $paragraph='' ){
    global $usr, $lng, $pge, $url, $cfg;
    if (  !$this->isVisible() ){ /* not enough rights->no content */
      return "<div class=\"labsys_mop_note\">\n".$lng->get("TxtNotEnoughRights")."\n</div>";
      exit;
    }

    $GLOBALS['Logger']->logToDatabase($fullAddress, logActions::mOpen);

    if (empty($this->question)){ return "'()"; exit; }

    $answers = "";

    if ( $usr->isOfKind( IS_EX_SOLUTION_VIEWER ) ){
      if ($url->available('hideExampleSolutionInThisTab')){
        $this->loadUserData4( $usr->uid ); // empty or my answer
      }
      $answers = $this->showAnswer( $fullAddress ); // example solution
    }

    if ( !$usr->isOfKind( IS_CORRECTOR ) ){    // Load the dynamic data from the working table.
      // "normal" usr
      if ( $usr->isOfKind( IS_EX_SOLUTION_VIEWER ) ) ; // example solution
                                                else $this->loadUserData4( $usr->uid ); // loads the dynamic data from the working table.
      $answers = $this->showAnswer( $fullAddress );
    }else{
      // corrector
      if ( $usr->isSeeingSomeonesData() )
        if ( $usr->theSeeingUid() != "all" ){
                  $this->loadUserData4( $usr->theSeeingUid() );
                  $answers = $this->showAnswer( $fullAddress );
        }else{  // seeing all
                  $this->mcDBI->getAllUIDs( $this->idx );
                  while ( ($nextUID = $this->mcDBI->getNextData()) ){
                    //information row
                    $userData = $usr->getGlobalUserData( $nextUID );
                    $answers .= "<div class=\"labsys_mop_m_name\">".$userData['foreName'].' '.$userData['name']."</div>\n";
                    // answer
                    $this->loadUserData4( $nextUID );
                    $answers .= $this->showAnswer( $fullAddress );
                  }
        }
    }

    return '<div class="labsys_mop_elements_m_around">'.
           ($this->isAnswered ? '<div class="'.($this->isCorrectlyAnswered ? 'correct' : ($this->triesUp2Now < $cfg->get('MCAnsweringAttempts') ? 'wrongButAnswerable' : 'wrong' ) ).'">' : '' ).
           EM::viewTopMultipleChoice( $this, $fullAddress ).
           "<div class=\"labsys_mop_contentArea\">\n".
             "<a name=\"".$fullAddress."\"></a>\n". // anchor for retrieving after saving
             "<div class=\"labsys_mop_m_question\">".
              "<label for=\"aTable".$this->idx."\">".
               ( $this->isAnswered || $this->isChangedByUser( end($this->previousAnswersArray) ) ? '' : "<img src=\"../syspix/button_edit_13x12.gif\" width=\"13\" height=\"12\" border=\"0\" alt=\"edit\" title=\"".$lng->get('explainUnChanged')."\"> ").
               '<div class="withLinkAnchor">'.( !empty($paragraph) ?  $paragraph.'. '  : '' ).returnHTML($this->question).'<a href="#'.$fullAddress.'" class="link-anchor">¶</a></div>'.
              "</label>".
             "</div>\n".
               $answers.
           "</div>\n".
           EM::viewBottomMultipleChoice( $this, $fullAddress ).
           ($this->isAnswered ? '</div>' : '' ).
           "</div>";
  }


  /**
  * Produces the ePub version of this element.
  * Called by LcElement.inc
  *
  * @param $fullAddress   The element's full address (p.e. C23.c42.p13)
  * @param $paragraph     The element's paragraph number (p.e. 2.3). Will be added to the title if given.
  */
  function showEPub( $fullAddress, $paragraph="" ){
    global $usr, $lng, $url, $pge;
    if (  !$this->isVisible() ){ /* not enough rights->no content */
      return "<div class=\"labsys_mop_note\">\n".$lng->get("TxtNotEnoughRights")."\n</div>";
      exit;
    }

    if (empty($this->question)){ echo "'()"; exit; }

    $epubExporter = LSE_Exporter::getInstance();
    $answerArray = array();

    //echo ('[m-element '.$fullAddress.'] question:'.htmlentities( $this->question ).'<br>');

    // The answers using the current permutation array
    for ( $i=0; $i < $this->answerCount; $i++){ // iteration over the number of answers
      $this->answerArray[ $this->displayPermutation[$i] ] = returnHTML( $this->answerArray[ $this->displayPermutation[$i] ] );
      $answerArray[] = $this->answerArray[ $this->displayPermutation[$i] ] ;
    }

    $epubExporter->save('Lm',
        $fullAddress,
        '',
        array(
            'question'   => ( $paragraph != "" ?  $paragraph.". "  : '' ).returnHTML( $this->question ),
            'answerArray' => $answerArray,
        )
    );
  }


// The next functions are used for the element's edit page. *************************************************

// helper (if this exists some time they should be declared private)  - - - - - - - - - - - - - - - - - - -
// These function were originally coded inline into the showEdit method but php doesn't support inline
// methods. So I put them out again.
    /**
     * Returns the checkmarks for question # $questionNumber.
     * @param unknown $questionNumber
     * @param unknown $isEditable
     */
    private function getCheckMarksHTML( $questionNumber, $isGettingEdited=FALSE, $isGettingAnswered=FALSE ){
      global $pge,$usr,$url,$cfg;
      $returnValue = '';
      for ($i=0; $i<=$this->triesUp2Now; $i++){
      	// Shall the background be colored as feedback of the correctness of the answer?
      	$colorMe = !$isGettingEdited && ($i < $this->triesUp2Now);
      	// Shall each checkbox be individually colored for detailed feedback?
      	$individualColors = $colorMe && $this->isAnswered;
      	$colorExampleSolution = FALSE;
      	
      	// Show example solution?
      	$showExampleSolution = ($i == $cfg->get('MCAnsweringAttempts')) || 		 // The last is always empty... 
      						   $isGettingEdited || 
      						   (($i == $this->triesUp2Now) && !$isGettingEdited && $usr->isOfKind(IS_EX_SOLUTION_VIEWER) && !$url->available('hideExampleSolutionInThisTab')) ||
      						   (($i == $this->triesUp2Now) && $this->isCorrectlyAnswered);
      	if ($showExampleSolution) {
      		$colorExampleSolution = ($i == $this->triesUp2Now) && !$isGettingEdited && $usr->isOfKind(IS_EX_SOLUTION_VIEWER) && !$url->available('hideExampleSolutionInThisTab');
      		$this->previousAnswersArray[$i] = $this->getCorrectMask(); // show solution.
      	}

        $returnValue .= '<div class="checkMark">'.
                        // Feedback at the end:
                        ($colorMe && $individualColors ? '<div class="'.(($this->previousAnswersArray[$i] & $this->mask($questionNumber)) == ($this->getCorrectMask() & $this->mask($questionNumber)) ? 'right' : 'wrong').'">' : '').
                        // Feedback in between:
                        ($colorMe && !$individualColors ? '<div class="'.($this->previousAnswersArray[$i] == $this->getCorrectMask() ? 'right' : 'wrong').'">' : '').
                        // Example Solution
                        ($colorExampleSolution ? '<div class="'.($this->mask($questionNumber) == ($this->getCorrectMask() & $this->mask($questionNumber)) ? 'exampleSolutionCorrect' : 'exampleSolutionWrong').'">' : '').
                        '<input type="'.($this->getOnlyOneAnswerPossible() & !$isGettingEdited ? 'radio' : 'checkbox').'"'.
                        ((isset($this->previousAnswersArray[$i]) && (($this->previousAnswersArray[$i] & (1<<$questionNumber)) == (1<<$questionNumber))) ||
                         ($showExampleSolution && (($this->correctMask & $this->mask($questionNumber)) == $this->mask($questionNumber))
                              ) ? ' checked="checked"' : '').
                        ' name="mcCheckMark_q'.$this->idx.($this->getOnlyOneAnswerPossible() && !$showExampleSolution ? ($i == $this->triesUp2Now ? '':'_old'.$i).'" value="'.(1<<$questionNumber) : '_a'.$questionNumber).'"'.
                        ' id="mcCheckMark_q'.$this->idx.'_a'.$questionNumber.'"'.
                        ( $i<$this->triesUp2Now || !($isGettingEdited || $isGettingAnswered) ? ' disabled="disabled"' : ' onchange="isDirty=true" tabindex="'.$pge->nextTab++.'"').
                        '></div>'.
                        ($colorMe || $colorExampleSolution ? '</div>' : '' ).PHP_EOL;
        if ($this->previousAnswersArray[$i] == $this->getCorrectMask()){ break; }
      }
      return $returnValue;
    }

    /**
     * Returns the answer to the $questionNumber. If $isGettingEdited the hints and credit deductions are shown.
     * @param integer $questionNumber
     * @param boolean $isGettingEdited
     */
    private function getAnswerHTML( $answerNumber, $isGettingEdited=FALSE ){
      global $cfg, $pge;
      $returnValue = '<div class="answers">'.
                     ( $isGettingEdited ? '<textarea tabindex="'.$pge->nextTab++.
                                                  '" name="mcAnswer_q'.$this->idx.'_a'.$answerNumber.'"'.
                                                  ' class="labsys_mop_textarea" rows="'.$cfg->get('DefaultMCQuestionRows').'"'.
                                                  ' onchange="isDirty=true">' :
                                          '<label for="mcCheckMark_q'.$this->idx.'_a'.$answerNumber.'">').
                     (!isset($this->getAnswerArray()[$answerNumber]) ? '' :
                         ($isGettingEdited ? returnEditable($this->getAnswerArray()[$answerNumber]) : returnHTML($this->getAnswerArray()[$answerNumber]) )).
                     ( $isGettingEdited ? '</textarea>' :
                                          '</label>').PHP_EOL;
      if ($isGettingEdited && ($this->getReactionsEnabled() || $this->getDetailedGradingEnabled())){
        // Show hints that are shown when the question is answered wrongly and the credit reduction that happens when the link is shown.
        for ($tryNo = 0; $tryNo < $cfg->get("MCAnsweringAttempts"); $tryNo++){
          $returnValue .= '<div class="answerReaction"'.($this->horizontalLayout ? '': ' style="width:'.(1/$cfg->get("MCAnsweringAttempts")*100-2).'%"').'>'.
                          ($tryNo+1).
                          (!$this->getReactionsEnabled() || ($tryNo >= $cfg->get("MCAnsweringAttempts")-1) ? '':
                                                          '<textarea tabindex="'.$pge->nextTab++.
                                                                  '" name="mcAnswer_reaction_q'.$this->idx.'_a'.$answerNumber.'_r'.$tryNo.
                                                                  '" class="labsys_mop_textarea" rows="1"'.
                                                                  ' onchange="isDirty=true">'.
                                                                  (isset($this->reactionArray[$answerNumber][$tryNo]) ? $this->reactionArray[$answerNumber][$tryNo]['hint'] :
                                                                                                                     '' ).
                                                          '</textarea>'.PHP_EOL
                              ).
                          (!$this->getDetailedGradingEnabled() ? '':
                                                          '<input tabindex="'.$pge->nextTab++.
                                                               '" type="text" name="mcAnswer_reaction_credits_q'.$this->idx.'_a'.$answerNumber.'_r'.$tryNo.'" '.
                                                               'class="labsys_mop_input" maxlength="255" value="'.(isset($this->reactionArray[$answerNumber][$tryNo]) ? $this->reactionArray[$answerNumber][$tryNo]['creditReduction'] : 0).
                                                               '" onchange="isDirty=true">'.PHP_EOL
                              ).
                          '</div>';
        }
      }
      return $returnValue.'</div>';
    }

    /**
    * Returns the answer fields (realized as function not as loop so that $pge->nextTab is set correctly).
    *
    * @return string          The html code of the answer field's inputs.
    */
    private function getAnswerFieldsHTML( $isGettingEdited=FALSE, $isGettingAnswered=FALSE ){
      global $cfg, $pge, $lng, $usr;
      $returnValue =  '<div class="labsys_mop_m_answers">';
      // How many answer fields?
      for ( $i=0; $i < (count( $this->getAnswerArray() ) + ($isGettingEdited ? $cfg->get("DefaultMCAnswerPlusAmount") : 0)); $i++){
        $returnValue .= '<div class="answer'.($this->horizontalLayout ? 'Horizontal" style="width:'.(1/$cfg->get("MCAnsweringAttempts")*100-11).'%"': '"').'>'.PHP_EOL.
                        $this->getCheckMarksHTML(($isGettingEdited || $this->doNotShuffleAnswers ? $i : $this->displayPermutation[$i]), $isGettingEdited, $isGettingAnswered).
                        $this->getAnswerHTML(($isGettingEdited || $this->doNotShuffleAnswers ? $i : $this->displayPermutation[$i]), $isGettingEdited).
                        '<div class="clearBoth"></div></div>'.PHP_EOL;
      }
      $returnValue .= '</div>';

      return $returnValue;





//       $answerFields = "<table class=\"labsys_mop_m_answers_table\">\n";



//       for ( $i=0; $i < (count( $this->getAnswerArray() ) + $cfg->get("DefaultMCAnswerPlusAmount")); $i++){
//         $answerFields .= "<tr>\n".
//                          "\t<td class=\"labsys_mop_m_answers_table_checkbox_cell\">".
//                          '<input type="'.( $this->onlyOneAnswerPossible ? 'radio' : 'checkbox' ).'" '.
//                                 "name=\"CORRECTMASK".( $this->onlyOneAnswerPossible ? $this->idx : $i )."\" ".
//                                 "value=\"".$i."\" ".
//                                 "tabindex=\"".$pge->nextTab++."\" ".
//                                 "title='".$lng->get( 'isCorrect' )."'".
//                                  ( ( $this->getCorrectMask() & LmElement::mask($i) ) ?   " checked=\"checked\""  : '' ).
//                           " onchange='isDirty=true'></td>\n".
//                           "\t<td class=\"labsys_mop_m_answers_table_answer_cell\">".
//                           "<textarea tabindex=\"".$pge->nextTab++."\" ".
//                                     "id=\"a".$i."\" ".
//                                     "name=\"ANSWER".$i."\" ".
//                                     "class=\"labsys_mop_textarea\" ".
//                                     "rows=\"".$cfg->get("DefaultMCAnswerRows")."\" onchange='isDirty=true'>";

//          // fror available answers put them into the text area
//          if ( $i < count( $this->getAnswerArray() ) ) $answerFields .= returnEditable( $this->getAnswerArray()[$i] ); // _ -> &#x005F; needed cause otherwise Replacement on $pge->show

//          $answerFields .= "</textarea>".
//                           "</td></tr>\n";
//        }
//        $answerFields .= "</table>";
//        return $answerFields;
     }
// /helper - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  /**
  * Returns the element's HTML-representation in the way the EDITOR should see it.
  *
  * @param $_GET["saveAsNew"] If $_GET["saveAsNew"] is set the save as new checkbox on the edit page should be set
  * @param $fullAddress       The element's full address (p.e. C23.c42.p13)
  * @return string            The html code of the element's edit view.
  */
  function showEdit( $fullAddress ){
    global $lng, $url, $pge, $cfg;
    $GLOBALS['Logger']->logToDatabase($fullAddress, logActions::mEditorOpen);
    $answerNoptions = '';
    $selected = (is_null($this->requiresNcorrect) ? -1 : $this->requiresNcorrect);
    for ($i=-1; $i < $this->answerCount+$cfg->get('DefaultMCAnswerPlusAmount'); $i++){
    	$answerNoptions .= '<option value="'.$i.'"'.($i == $selected ? 'selected="selected"' : '').'>'.($i>=0 ? $i : '-').'</option>';
    }

     return EM::editTopMultipleChoice( $this, $fullAddress ).
            "<FORM class=\"labsys_mop_std_form\" NAME=\"multipleChoiceEdit\" METHOD=\"POST\" ACTION=\"".$url->link2('../php/executeCommandOnElement.php', Array('address' => 'm'))."\">\n".
            "<input type=\"hidden\" name=\"FUNCTIONNAME\" value=\"save\">\n".           // this function will statically be called by ../php/executeCommandOnElement.php
            "<input type=\"hidden\" name=\"IDX\" value=\"".$this->idx."\">\n".            // needed by ../php/executeCommandOnElement.php
            "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/edit.php\">\n".  // index of saved el. will be added on save.php!
            "<input type=\"hidden\" name=\"FULLADDRESS\" value=\"".$fullAddress."\">\n".

          // question input
            "<label for=\"q\" class=\"labsys_mop_input_field_label_top\">".$lng->get("question")."</label>".
            "<textarea tabindex=\"".$pge->nextTab++."\" id=\"q\" name=\"QUESTION\" class=\"labsys_mop_textarea\" rows=\"".$cfg->get("DefaultMCQuestionRows")."\" onchange='isDirty=true'>".
            returnEditable( $this->question ). /* _ -> &#x005F; needed cause otherwise Replacement on $pge->show */
            "</textarea>\n".

            '<input tabindex="'.$pge->nextTab++.
            '" type="text" name="mcCredits_q'.$this->idx.'" '.
            'class="labsys_mop_input" maxlength="255" value="'.$this->getPossibleCredits().
            '" onchange="isDirty=true">'.PHP_EOL.

          // answers
            "<label for=\"a0\" class=\"labsys_mop_input_field_label_top\">".$lng->get("inputAnswers")."</label>".
            $this->getAnswerFieldsHTML(TRUE).

          // answer explanation
            "<label for=\"aE\" class=\"labsys_mop_input_field_label_top\">".$lng->get("inputAnswerExpl")."</label>".
            "<textarea tabindex=\"".$pge->nextTab++."\" id=\"aE\" name=\"ANSWER_EXPLANATION\" class=\"labsys_mop_textarea\" rows=\"".$cfg->get("DefaultMCAnswerExplRows")."\" onchange='isDirty=true'>".
            returnEditable( $this->answerExplanation ). /* _ -> &#x005F; needed cause otherwise Replacement on $pge->show */
            "</textarea>\n".

            "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("save")."\" accesskey=\"s\" onclick='isDirty=false'>\n".

          // element options
            "<fieldset><legend>".$lng->get("options")."</legend>\n".

            '<div>'.
            '<input type="hidden" name="DO_NOT_SHUFFLE_ANSWERS" value="0" /><input tabindex="'.$pge->nextTab++.'" type="checkbox" id="doNotShuffleAnswers" name="DO_NOT_SHUFFLE_ANSWERS" value="1"'.( $this->doNotShuffleAnswers ? ' checked="checked"' : '').'>'.
            '<label for="doNotShuffleAnswers" class="labsys_mop_input_field_label">'.$lng->get('doNotShuffleAnswers').'</label>'.
            '</div>'.PHP_EOL.

            '<div>'.
            '<input type="hidden" name="ONLY_ONE_ANSWER_POSSIBLE" value="0" /><input tabindex="'.$pge->nextTab++.'" type="checkbox" id="onlyOneAnswerPossible" name="ONLY_ONE_ANSWER_POSSIBLE" value="1"'.( $this->onlyOneAnswerPossible ? ' checked="checked"' : '').'>'.
            '<label for="onlyOneAnswerPossible" class="labsys_mop_input_field_label">'.$lng->get('onlyOneAnswerPossible').'</label>'.
            '</div>'.PHP_EOL.
            
            '<div>'.
            '<select tabindex="'.$pge->nextTab++.'" name="CORRECT_WHEN_N_CORRECT_SELECTED" id="correctWhenNcorrectSelected" size="1">'.
            $answerNoptions.
			'</select>'.
            '<label for="CORRECT_WHEN_N_CORRECT_SELECTED" class="labsys_mop_input_field_label">'.$lng->get('correctWhenNCorrectSelected').'</label>'.
            '</div>'.PHP_EOL.

            '<div>'.
            '<input type="hidden" name="REACTIONS" value="0" /><input tabindex="'.$pge->nextTab++.'" type="checkbox" id="reactions" name="REACTIONS" value="1"'.( $this->getReactionsEnabled() ? ' checked="checked"' : '').'>'.
            '<label for="reactions" class="labsys_mop_input_field_label">'.$lng->get('reactionsEnable').'</label>'.
            '</div>'.PHP_EOL.

            '<div class="subOptions">'.
            '<div>'.
            '<div><input'.( $this->getReactionsEnabled() ? '' : ' disabled="disabled"').' tabindex="'.$pge->nextTab++.'" type="radio" id="hintForClicked" name="HINT_FOR_CLICKED" value="1"'.( $this->getHintForKlicked() ? ' checked="checked"' : '').'>'.
            '<label for="hintForClicked" class="labsys_mop_input_field_label">'.$lng->get('hintForClicked').'</label></div>'.
            '<div><input'.( $this->getReactionsEnabled() ? '' : ' disabled="disabled"').' tabindex="'.$pge->nextTab++.'" type="radio" id="hintForWrongClicked" name="HINT_FOR_CLICKED" value="0"'.( !$this->getHintForKlicked() ? ' checked="checked"' : '').'>'.
            '<label for="hintForWrongClicked" class="labsys_mop_input_field_label">'.$lng->get('hintForWrongClicked').'</label></div>'.
            '</div>'.PHP_EOL.

            '<div>'.
            '<input type="hidden" name="RESHOW_LAST_HINT" value="0" /><input'.( $this->getReactionsEnabled() ? '' : ' disabled="disabled"').' tabindex="'.$pge->nextTab++.'" type="checkbox" id="reshowLastHint" name="RESHOW_LAST_HINT" value="1"'.( $this->getReshowLastHint() ? ' checked="checked"' : '').'>'.
            '<label for="reshowLastHint" class="labsys_mop_input_field_label">'.$lng->get('reshowLastHint').'</label>'.
            '</div>'.
            '</div>'.PHP_EOL.

            '<div>'.
            '<input type="hidden" name="DETAILED_CREDITS" value="0" /><input tabindex="'.$pge->nextTab++.'" type="checkbox" id="detailedCredits" name="DETAILED_CREDITS" value="1"'.( $this->getDetailedGradingEnabled() ? ' checked="checked"' : '').'>'.
            '<label for="detailedCredits" class="labsys_mop_input_field_label">'.$lng->get('detailedCreditsEnable').'</label>'.
            '</div>'.PHP_EOL.

            '<div>'.
            '<input type="hidden" name="HORIZONTAL_LAYOUT" value="0" /><input tabindex="'.$pge->nextTab++.'" type="checkbox" id="horizontalLayout" name="HORIZONTAL_LAYOUT" value="1"'.( $this->horizontalLayout ? ' checked="checked"' : '').'>'.
            '<label for="horizontalLayout" class="labsys_mop_input_field_label">'.$lng->get('horizontalLayout').'</label>'.
            '</div>'.PHP_EOL.

            "</fieldset>\n".

          // element properties
            "<fieldset><legend>".$lng->get("properties")."</legend>\n".
              "<table width=\"100%\"><tr><td valign=\"top\" width=\"40%\">\n".
                "<fieldset><legend>".$lng->get("visibility")."</legend>\n".
                  "<div>".
                    "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" id=\"visAlways\" name=\"VISIBLE_ONLY_IN_COLLECTION\" value=\"0\"".( !$this->visibleOnlyInCollection ?  " checked=\"checked\"" : '' )." onchange='isDirty=true'>".
                    "<label for=\"visAlways\" class=\"labsys_mop_input_field_label\">".$lng->get("visAlways")."</label>".
                  "</div>\n".
                  "<div>".
                    "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\"  id=\"visOnlyColl\"name=\"VISIBLE_ONLY_IN_COLLECTION\" value=\"1\"".( $this->visibleOnlyInCollection ?  " checked=\"checked\"" : '' )." onchange='isDirty=true'>".
                    "<label for=\"visOnlyColl\" class=\"labsys_mop_input_field_label\">".$lng->get("visOnlyInLab")."</label>".
                  "</div>\n".
                "</fieldset>\n".
              "</td><td valign=\"top\" width=\"60%\">\n".
                "<fieldset><legend>".$lng->get("rights")."</legend>\n".
                "<div class=\"labsys_mop_input_field_note\">".$lng->get("visibleFor")."</div>\n".
                "<div class=\"labsys_mop_in_fieldset\">\n".
                showRightsVertical( "UR", $this->visibleFor, false ).
                "</div>\n".
                "</fieldset>\n".
              "</td></tr></table>\n".
            "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" id=\"saveAsNew\" name=\"SAVEASNEW\" value=\"1\"".( ( $GLOBALS['url']->available("saveAsNew") || ($this->idx == 1) ) ?   " checked=\"checked\""  : '' )." onchange='isDirty=true'>".
            "<label for=\"saveAsNew\" class=\"labsys_mop_input_field_label\">".$lng->get("saveAsNew")."</label>".
            "</fieldset>\n".

            "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("save")."\" onclick='isDirty=false'>\n".

            "</FORM>\n".
            // preview

            '<script language="JavaScript" type="text/javascript">
            <!--
            if (document.multipleChoiceEdit) document.multipleChoiceEdit.q.focus();
            //-->
            </script>'.
            EM::editBottomMultipleChoice( $this, $fullAddress ).
            $this->show($fullAddress);
  }

  /**
  * Used to save the edited element.
  *
  * This function gets STATICALLY called from ../php/executeCommandOnElement.php?address=[elementID].
  * It handles the $_POSTed data and then redirects to the $_POST['REDIRECTTO'] with the address of the element added.
  */
  /*static*/ function save(){
    global $mDBI, $url, $lng, $usr, $cfg;

    // check for all necessary values posted
    if ( !isset($_POST['IDX']) ||
         !isset($_POST['REDIRECTTO']) ||
         !isset($_POST['FULLADDRESS']) ||
         !isset($_POST['QUESTION']) ||
         !isset($_POST['mcAnswer_q'.$this->idx.'_a0']) ||
         !isset($_POST['ANSWER_EXPLANATION'])
       ){
          trigger_error( $lng->get("notAllNecPosted"), E_USER_ERROR );
          exit;
         }

    // only content editors are allowed to save
    if ( !$usr->isOfKind( IS_CONTENT_EDITOR ) ){
                                                  trigger_error( $lng->get("notAllowed"), E_USER_ERROR );
                                                  exit;
                                                }

    // create answerArray + compute correctMask
    $answerArray = Array();
    $correctMask=0;
    $reactionArray=Array();
    for ( $answerNumber = 0; isset( $_POST['mcAnswer_q'.$this->idx.'_a'.$answerNumber] ); $answerNumber++ ){
      $currentAnswer = trim($_POST['mcAnswer_q'.$this->idx.'_a'.$answerNumber]);
      if (empty($currentAnswer)){
        continue; // Skip empty answers...
      }
      $answerArray[] = getTidiedHTML($currentAnswer);
      if (isset($_POST['mcCheckMark_q'.$this->idx.'_a'.$answerNumber])){
        $correctMask += $this->mask($answerNumber);
      }
      for( $tryNo=0;
            isset($_POST['mcAnswer_reaction_q'.$this->idx.'_a'.$answerNumber.'_r'.$tryNo]) ||
            isset($_POST['mcAnswer_reaction_credits_q'.$this->idx.'_a'.$answerNumber.'_r'.$tryNo]);
            $tryNo++){
        $reactionArray[$answerNumber][]=Array('hint' => (isset($_POST['mcAnswer_reaction_q'.$this->idx.'_a'.$answerNumber.'_r'.$tryNo])?
                                            getTidiedHTML($_POST['mcAnswer_reaction_q'.$this->idx.'_a'.$answerNumber.'_r'.$tryNo]) :
                                            (isset($this->reactionArray[$answerNumber][$tryNo]['hint']) ? $this->reactionArray[$answerNumber][$tryNo]['hint'] : '') // Preserve existing value
                                            ),
                    'creditReduction' => (isset($_POST['mcAnswer_reaction_credits_q'.$this->idx.'_a'.$answerNumber.'_r'.$tryNo]) &&
                                          is_numeric($_POST['mcAnswer_reaction_credits_q'.$this->idx.'_a'.$answerNumber.'_r'.$tryNo]) ?
                                            $_POST['mcAnswer_reaction_credits_q'.$this->idx.'_a'.$answerNumber.'_r'.$tryNo] :
                                            (isset($this->reactionArray[$answerNumber][$tryNo]['creditReduction']) ? $this->reactionArray[$answerNumber][$tryNo]['creditReduction'] : 0) // Preserve existing value
                                            ));
      }
    }
    if (isset($_POST['mcCheckMark_q'.$this->idx])){
      $correctMask = $_POST['mcCheckMark_q'.$this->idx];
    }

//     print_r($reactionArray);
//     echo('<br><br>');
//     print_r($_POST);
//     exit;

    // compute user rights:
    $userRights=0;
    for ($i=1; $i<=MAX_USER_ROLE; $i=$i<<1) if ( isset( $_POST[ "UR_".$i] ) ) $userRights += $_POST["UR_".$i];

    // Here it will be necessary to update THIS element in a good way!
    // Update this element
    $newMultipleChoice = new LmElement( $_POST['IDX'],
                                        getTidiedHTML($_POST['QUESTION']),
                                        $answerArray,
                                        (empty($reactionArray) ? $this->reactionArray : $reactionArray), // preserve data
                                        getTidiedHTML($_POST['ANSWER_EXPLANATION']),
                                        $correctMask,
                                        (is_numeric($_POST['mcCredits_q'.$this->idx]) ? $_POST['mcCredits_q'.$this->idx] : $cfg->get('MCAnsweringAttempts')),
                                        $_POST['DO_NOT_SHUFFLE_ANSWERS'] == 1,
                                        $_POST['ONLY_ONE_ANSWER_POSSIBLE'] == 1,
    									( $_POST['CORRECT_WHEN_N_CORRECT_SELECTED'] < 0 ? NULL : $_POST['CORRECT_WHEN_N_CORRECT_SELECTED']),
                                        $_POST['REACTIONS'] == 1,
                                        $_POST['HINT_FOR_CLICKED'] == 1,
                                        $_POST['RESHOW_LAST_HINT'] == 1,
                                        $_POST['DETAILED_CREDITS'] == 1,
                                        $_POST['HORIZONTAL_LAYOUT'] == 1,
                                        $userRights,
                                        ($_POST["VISIBLE_ONLY_IN_COLLECTION"] == "1"),
                                        $this->history
                                       );

    // if $_POST["SAVEASNEW"] is set the element is saved as new element
    if ( isset($_POST["SAVEASNEW"]) && ($_POST["SAVEASNEW"] == "1") ){
                                      $newMultipleChoice->idx = $mDBI->setData( $newMultipleChoice, true );
                                      $url->put( 'address', 'm'.$newMultipleChoice->idx ); // the address of the new element
                                      $GLOBALS['Logger']->logToDatabase('m'.$newMultipleChoice->idx, logActions::mEditorSave);
                                     }
    else{
                                      $mDBI->setData( $newMultipleChoice );
                                      $url->put( 'address', $_POST['FULLADDRESS'] );    // the address remains
                                      $GLOBALS['Logger']->logToDatabase('m'.$this->idx, logActions::mEditorSave);
    }

    // some user information
    $url->put( 'sysinfo', $lng->get("DataHasBeenSaved")." ".$lng->get("mNumber")." ".$newMultipleChoice->idx );

    // One could also display the page here but that way it is left to the normal mechanism...
    header( "Location: ".$url->rewriteExistingUrl( $_POST['REDIRECTTO'] ) );
  }

  /**
  * Used to save the users answer.
  *
  * This function gets STATICALLY called from ../php/executeCommandOnElement.php?address=[elementID].
  * It handles the $_POSTed data and then redirects to the $_POST['REDIRECTTO'] with the address of the element added.
  */
  /*static*/ function saveUserAnswer(){
    global $url, $lng, $usr;

    // check for all necessary values posted
    if ( !isset($_POST['IDX']) ||
         !isset($_POST['REDIRECTTO']) ||
         !isset($_POST['FULLADDRESS'])
       ){
          trigger_error( $lng->get("notAllNecPosted"), E_USER_ERROR );
          exit;
         }

    // Validity (session ID) is checked by the calling script yet.

    // compute bitmask (sum of posted values).
    $answerMask=0;
    for ( $answerNumber = 0; $answerNumber<count($this->answerArray); $answerNumber++ ){
      if (isset($_POST['mcCheckMark_q'.$this->idx.'_a'.$answerNumber])){
        $answerMask += $this->mask($answerNumber);
      }
    }
    if (isset($_POST['mcCheckMark_q'.$this->idx])){
      $answerMask = $_POST['mcCheckMark_q'.$this->idx];
    }


    // load the dataset
    $this->loadUserData4( $usr->uid );

    // set this at the end
    $this->previousAnswersArray[$this->triesUp2Now] = $answerMask;

    // store the new dataset
    $this->mcDBI->setData4( $usr->uid, $this->idx, $this->displayPermutation, $this->previousAnswersArray );

    $GLOBALS['Logger']->logToDatabase($_POST['FULLADDRESS'], logActions::mStudentSave);

    // One could also display the page here but that way it is left to the normal mechanism...
    header( "Location: ".$url->rewriteExistingUrl( $_POST['REDIRECTTO'] ) );
  }

  /**
  * Called to check the user's answer.
  *
  * This function gets STATICALLY called from ../php/executeCommandOnElement.php?address=[elementID].
  * It handles the $_POSTed data and then redirects to the $_POST['REDIRECTTO'] with the address of the element added.
  *
  * @return bool  true if the answer is correct or there are no more tries left (called by lab).
  */
  /*static*/ function checkUserAnswer(){
    global $usr, $cfg;

    $GLOBALS['Logger']->logToDatabase('l'.$_POST['IDX'].'~m'.$this->idx, logActions::mCheck);
    $GLOBALS['Logger']->logToDatabase('l'.$_POST['IDX'].'~m'.$this->idx, ($this->isCorrectlyAnswered?logActions::mIsCorrectAnswer:logActions::mIsWrongAnswer));

    // if it is not answered it will not be correct as the not answered status sets a bit...
    if ( $this->isAnswered ){  // enough checks yet or correct...
      return true;
    }

    if ( !$this->isChangedByUser(end($this->previousAnswersArray)) ){
      // not answered yet -> do nothing
      return false;
    }

    $this->previousAnswersArray[] = $this->markAsUnChanged(0); // insert empty answer at the end
    $this->mcDBI->setData4( $usr->uid, $this->idx, $this->displayPermutation, $this->previousAnswersArray, "-check-" );

    $this->mcDBI = $this->loadUserData4( $usr->uid ); // Load data with applied changes.
    return $this->isAnswered;
  }

  /**
  * Marks the answer as UNanswered. Used to mark if the user already saved
  * an answer.
  * Only questions answered once get checked on the checkup.
  * $input The value to mark
  * return The marked value
  */
  public static function markAsUnChanged( $input ){
    return $input | (1 << 60);
  }

  /**
   * Is the question marked as answered?
   * $input The value to check
   * return If the question is marked as answered
   */
  private function isChangedByUser( $input ){
    return !($input & (1 << 60));
  }

// The next functions are used for the element's manage page. ***********************************************

  /**
  * Displays the element's property legend on the manage page.
  * All entries of getPropertyRow() should be explained here.
  */
  function showPropertyLegend(){
    global $lng, $pge;
    $lgnd = "<div class=\"labsys_mop_".$this->elementId."_row\">\n".
            "<div class=\"labsys_mop_h3\">".$lng->get("legend")."</div>\n".
          // visible only in collection?
            "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" name=\"legend_VISIBLE_ONLY_IN_COLLECTION\" value=\"0\" checked=\"checked\">".
                infoArrow( $lng->get("visAlways") ).
                $lng->get("visAlways")."<br>\n".

             "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" name=\"legend2_VISIBLE_ONLY_IN_COLLECTION\" value=\"0\" disabled>".
                infoArrow( $lng->get("visAlways"), false ).
                "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" name=\"legend2_VISIBLE_ONLY_IN_COLLECTION\" value=\"1\" checked=\"checked\">".
                infoArrow( $lng->get("visOnlyInLab") ).
                $lng->get("visOnlyInLab")."<br>\n";

    // rights
    for ($i=1; $i<=MAX_USER_ROLE; $i=$i<<1){
      $lgnd .= "<input type=\"radio\" disabled>".infoArrow( $lng->get("visAlways"), true )."<input type=\"radio\" disabled>".infoArrow( $lng->get("visOnlyInLab"), true )." | ";
      for ($j=1; $j<$i; $j=$j<<1) $lgnd .= '<input type="checkbox" disabled>'.infoArrow( $lng->get("Explain_UR_".$j), true )/* ."\n" saves space! */;
      $lgnd .= rightsBox( "legend_".$i, $i, $i )." ".$lng->get("Explain_UR_".$i)."<br>\n";
    }

    $lgnd .= "</div>\n";

    return $lgnd;
  }

  /**
  * This function is called to display the element's properties.
  * @param $prefix    If given this is put before the element's title (p.e. 1.2.3 title).
  * @param $disabled  Should the checkboxes, radio buttons etc. be disabled?
  * @return string    The html-code of the elements properties horizontally arranged.
  */
  function getPropertyRow( $prefix, $disabled=false ){
    global $pge, $lng, $cfg;

    return    "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" ".( !$disabled ?  "name=\"".$prefix."_VISIBLE_ONLY_IN_COLLECTION\" "  : '' )."value=\"0\"".( !$this->visibleOnlyInCollection ?  " checked=\"checked\"" : '' ).( $disabled ?  " disabled=\"disabled\"" : '' )." onchange='isDirty=true'>".
              infoArrow( $lng->get("visAlways"), $disabled ).
              "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" ".( !$disabled ?  "name=\"".$prefix."_VISIBLE_ONLY_IN_COLLECTION\" "  : '' )."value=\"1\"".( $this->visibleOnlyInCollection ?  " checked=\"checked\"" : '' ).( $disabled ?  " disabled=\"disabled\"" : '' )." onchange='isDirty=true'>".
              infoArrow( $lng->get("visOnlyInLab"), $disabled ).
            ( $disabled ?  "<span class=\"labsys_mop_grayed\">"  : '' ).
            " | ".
            ( $disabled ?  "</span>"  : '' ).
              showRightsHorizontal( $prefix, $this->visibleFor, $disabled ).
            " ".
              cutHTMLText( $this->title, $cfg->get("maxCharMngRowTitleShort") );
  }
}
?>
