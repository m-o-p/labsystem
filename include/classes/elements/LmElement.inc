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
  var $question;              // The question.
  var $answerArray;           // Array containing the answers.
  var $answerExplanation;     // Why is the answer correct?
  var $correctMask;           // Bitmask: Which answers are correct?
  var $visibleFor;            // Visibility constraints.

  var $answerCount;           // Number of answers.
  var $displayPermutation;    // Array containing the permutated Order of answers.
  var $previousAnswersArray;  // Array containing the bitmasks of previous answers.
  var $triesUp2Now;           // Number of tries the user had.
  var $isCorrectlyAnswered;   // Was the last answer given correct?
  var $isAnswered;            // Is the las answer correct or the number of tries too high?
  var $answerHistory;         // Who answered when?

  /**
  * The constructor.
  *
  * @param $idx                       This instance's index.
  * @param $question                  The multiple choice question.
  * @param $answerArray               Array containing the answers.
  * @param $answerExplanation         Explanation why the answer is correct.
  * @param $correctMask               Bitmask telling ehich answers are correct.
  * @param $visibleFor                The bitmask of rights a viewer has to match.
  * @param $visibleOnlyInCollection   Is the element visible only inside a collection.
  * @param $history                   This' history.
  * @param $isInColl                  Am I inside a collection? (important for visibility aspects).
  */
  function LmElement( $idx, $question, $answerArray, $answerExplanation, $correctMask, $visibleFor, $visibleOnlyInCollection, $history, $isInColl=false ){
    global $usr;
    $this->elementId                = "m";
    $this->idx                      = $idx;
    $this->title                    = preg_replace("(\r\n|\n|\r)", '', // used to remove linebreaks
                                        returnHTML( // remove [HTML] etc.
                                          substr( $question, 0, 50 )
                                        )
                                      );
    $this->matchingMenu             = "";
    $this->question                 = $question;
    $this->answerArray              = $answerArray;
    $this->answerCount              = count( $this->answerArray );
    $this->answerExplanation        = $answerExplanation;
    $this->correctMask              = $correctMask;
    $this->visibleFor               = $visibleFor;
    $this->visibleOnlyInCollection  = $visibleOnlyInCollection;
    $this->history                  = $history;

    $this->IamVisible               = ($this->visibleFor == "0") || $usr->isOfKind( $this->visibleFor )
                                      && ( !$this->visibleOnlyInCollection || ($this->visibleOnlyInCollection && $isInColl) );
    if ( $history ){ // history is false if only menu data is wanted (following not necessary)
      $this->mcDBI                    = new LmDBInterfaceAnswers();
      $this->loadUserData4(""); // Fills rest of the structures with neutral values.
    }

    $this->serializationArray       = array( 'idx', 'question', 'answerArray', 'answerExplanation', 'correctMask', 'visibleFor', 'visibleOnlyInCollection', 'history' ); // see Element.inc for details
  }

  /**
  * Loads the answer data to the given $uid into this instance's field.
  *
  * @param $uid   The UID to load the data of.
  *
  * @return       The LmDBInterfaceAnswers() or FALSE.
  */
  function loadUserData4( $uid ){
    global $usr, $cfg;
    if ( $uid != "" ){
      $answerData = $this->mcDBI->getData4( $uid,
                                            $this->idx,
                                            $this->answerCount,
                                            !$usr->isSeeingSomeonesData() );

      $this->answerHistory        = $answerData["history"];
      $this->displayPermutation   = $answerData["permutation"];
      $this->previousAnswersArray = $answerData["answers"];
      $this->triesUp2Now          = count( $this->previousAnswersArray )-1; // Last holds the current try.
      $this->isCorrectlyAnswered  = ( ( $this->triesUp2Now > 0 ) && ( $this->previousAnswersArray[$this->triesUp2Now-1] == $this->correctMask ) ); // no correct answer and no attempt!
      $this->isAnswered =  $this->isCorrectlyAnswered || ( $this->triesUp2Now >= $cfg->get("MCAnsweringAttempts") );
    } else { // the example solution
      $this->answerHistory        = "";
      $this->displayPermutation   = range( 0, $this->answerCount-1 ); // identity
      $this->previousAnswerArray  = false;
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
     return $cfg->get( 'MCAnsweringAttempts' )-$this->triesUp2Now+$add;
   }

  /**
   * Returns the credits one cen get for the correct answer.
   *
   * Since I did not implement any credits you can get a maximum
   * of $cfg->get( 'MCAnsweringAttempts' ) as points.
   *
   * @return $cfg->get( 'MCAnsweringAttempts' )
   */
   function getPossibleCredits(){
     global $cfg;
     return $cfg->get( 'MCAnsweringAttempts' );
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
                                      $url->link2("../php/executeCommandOnElement.php?address=m".$this->idx) :         // save button functionality
                                      $url->link2("../pages/view.php?address=".$url->get('address')."#".$fullAddress) // give answer button functionality
                                     ).
              "\">\n".
              "<input type=\"hidden\" name=\"FUNCTIONNAME\" value=\"saveUserAnswer()\">\n". // this function will statically be called by ../php/executeCommandOnElement.php
              "<input type=\"hidden\" name=\"SESSION_ID\" value=\"".session_id()."\">\n".
              "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/view.php?address=".$url->get('address')."#".$fullAddress."\">\n". // return so self
              "<input type=\"hidden\" name=\"FULLADDRESS\" value=\"".$fullAddress."\">\n".
              "<input type=\"hidden\" name=\"IDX\" value=\"".$this->idx."\">\n";            // needed by ../php/executeCommandOnElement.php
    }

    // The answer(attempt)s will be put inside a table.
    $ret .= "<table id=\"aTable".$this->idx."\" class=\"labsys_mop_m_answers_table\" cellspacing=\"5px\">\n".
            "<tr><!-- fixed layout -->";
            // all previous answering attempts
            for ( $i=0; $i < $this->triesUp2Now; $i++ ) $ret .= "<td class=\"labsys_mop_m_answers_table_checkbox_cell\"></td>";

            // the solution
            if ( !$this->isAnswered || !$this->isCorrectlyAnswered ) $ret .= "<td class=\"labsys_mop_m_answers_table_checkbox_cell\"></td>";

            // the answers
            $ret .= "<td class=\"labsys_mop_m_answers_table_answer_cell\"></td></tr>\n";

    for ( $i=0; $i < $this->answerCount; $i++){ // iteration over the number of answers
      $ret .=  "<tr class=\"labsys_mop_m_answers_table_row\">\n";
      $actualAnswer = $this->displayPermutation[$i];

      for ( $j=0; $j < $this->triesUp2Now; $j++ ) { // Iteration over the previous answers
        $actualMask = $this->previousAnswersArray[$j]; // the answer given there

        $ret .=  "\t<td class=\"labsys_mop_m_answers_table_checkbox_cell_".( $actualMask == $this->correctMask ?  "right" :  "wrong"  )."\">".
                 "<input type=\"checkbox\" ".
                        "tabindex=\"".$pge->nextTab++."\" ".
                        "title='".$lng->get( 'isCorrect' )."' ".
                         ( ($actualMask & LmElement::mask($actualAnswer)) ?
                                    "checked=\"checked\" " : ''
                                   ).
                         "disabled=\"disabled\" ".
                  " onchange='isDirty=true'></td>\n";
       } // Iteration over the previous answers (end)

       if ( !$this->isAnswered ) $actualMask = $this->previousAnswersArray[$this->triesUp2Now]; // the current answer
                            else $actualMask = $this->correctMask; // the example solution

       if ( !$this->isAnswered || !$this->isCorrectlyAnswered ){
       // show current answer or example solution
        $ret .=  "\t<td class=\"labsys_mop_m_answers_table_checkbox_cell\">".
                 "<input type=\"checkbox\" ".
                        "id=\"m".$this->idx."a".$actualAnswer."\" ".
                        "name=\"ANSWER".$actualAnswer."\" ".
                        "value=\"".LmElement::mask($actualAnswer)/*2^$actualAnswer*/."\" ".
                        "tabindex=\"".$pge->nextTab++."\" ".
                         ( ($actualMask & LmElement::mask($actualAnswer)) ?
                                    "checked=\"checked\" " : ''
                                   ).
                         ( !$isGettingAnswered ?
                                    "disabled=\"disabled\" " : ''
                                   ).
                  " onchange='isDirty=true'></td>\n";
       }

       // The answers:
       $ret .=  "\t<td class=\"labsys_mop_m_answers_table_answer_cell\">".
                "<label for=\"m".$this->idx."a".$actualAnswer."\">".
                ( ($this->isAnswered && !($this->correctMask & LmElement::mask($actualAnswer))) ?  "<span class=\"labsys_mop_grayed\">"  : '' ).
                returnHTML( $this->answerArray[$actualAnswer] ).
                ( ($this->isAnswered && !($this->correctMask & LmElement::mask($actualAnswer))) ?  "</span>"  : '' ).
                "</label>".
                "</td>\n</tr>\n";
     } // iteration over the number of answers (end)

     $ret .= "</table>\n";

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
               if ($isGettingAnswered) makeLogEntry( 'm', 'opened', $fullAddress );
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
  function show( $fullAddress, $paragraph="" ){
    global $usr, $lng, $pge, $url;
    if (  !$this->isVisible() ){ /* not enough rights->no content */
      return "<div class=\"labsys_mop_note\">\n".$lng->get("TxtNotEnoughRights")."\n</div>";
      exit;
    }

    if ($this->question == ""){ return "'()"; exit; }


    parseHTML( $this->question ); // Process [HTML].

    $answers = "";

    if ( $usr->isOfKind( IS_EX_SOLUTION_VIEWER ) ){
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
                  require_once( INCLUDE_DIR."/classes/DBInterfaceUser.inc" );
                  $this->mcDBI->getAllUIDs( $this->idx );
                  while ( ($nextUID = $this->mcDBI->getNextData()) ){
                    //information row
                    $userData = $usr->getUserInformation( $nextUID );
                    $answers .= "<div class=\"labsys_mop_m_name\">".$userData["link"]."</div>\n";
                    // answer
                    $this->loadUserData4( $nextUID );
                    $answers .= $this->showAnswer( $fullAddress );
                  }
        }
    }

    return "<div class=\"labsys_mop_elements_m_around\">".
           EM::viewTopMultipleChoice( $this, $fullAddress ).
           "<div class=\"labsys_mop_contentArea\">\n".
             "<a name=\"".$fullAddress."\"></a>\n". // anchor for retrieving after saving
             "<div class=\"labsys_mop_m_question\">".
              "<label for=\"aTable".$this->idx."\">".
               ( $this->isAnswered || $this->isChangedByUser( end($this->previousAnswersArray) ) ? '' : "<img src=\"../syspix/button_edit_13x12.gif\" width=\"13\" height=\"12\" border=\"0\" alt=\"edit\" title=\"".$lng->get('explainUnChanged')."\"> ").
               ( $paragraph != "" ?  $paragraph.". "  : '' ).$this->question.
              "</label>".
             "</div>\n".
               $answers.
           "</div>\n".
           EM::viewBottomMultipleChoice( $this, $fullAddress ).
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

    if ($this->question == ""){ echo "'()"; exit; }

    parseHTML( $this->question ); // Process [HTML].
    $pge->replaceConstants( $this->question ); // Process custon fields

    $epubExporter = LSE_Exporter::getInstance();
    $answerArray = array();

    //echo ('[m-element '.$fullAddress.'] question:'.htmlentities( $this->question ).'<br>');

    // The answers using the current permutation array
    for ( $i=0; $i < $this->answerCount; $i++){ // iteration over the number of answers
      parseHTML( $this->answerArray[ $this->displayPermutation[$i] ] );
      //$pge->replaceConstants( $this->answerArray[ $this->displayPermutation[$i] ] ); // Process custon fields
      $answerArray[] = $this->answerArray[ $this->displayPermutation[$i] ] ;
      // echo( '[m-element] answer: [ ]'.htmlentities( $this->answerArray[ $this->displayPermutation[$i] ] ).'<br>' );
    }

    $epubExporter->save('Lm',
        $fullAddress,
        '',
        array(
            'question'   => ( $paragraph != "" ?  $paragraph.". "  : '' ).$this->question,
            'answerArray' => $answerArray,
        )
    );
  }


// The next functions are used for the element's edit page. *************************************************

// helper (if this exists some time they should be declared private)  - - - - - - - - - - - - - - - - - - -
// These function were originally coded inline into the showEdit method but php doesn't support inline
// methods. So I put them out again.
    /**
    * Returns the answer fields (realized as function not as loop so that $pge->nextTab is set correctly).
    *
    * @param &$answerArray    Array containing the answers.
    * @param &$correctMask    Bitmask telling ehich answers are correct.
    *
    * @return string          The html code of the answer field's inputs.
    */
    function answerFields( &$answerArray, &$correctMask ){
      global $cfg, $pge, $lng;
      $answerFields = "<table class=\"labsys_mop_m_answers_table\">\n";
      for ( $i=0; $i < (count( $answerArray ) + $cfg->get("DefaultMCAnswerPlusAmount")); $i++){
        $answerFields .= "<tr>\n".
                         "\t<td class=\"labsys_mop_m_answers_table_checkbox_cell\">".
                         "<input type=\"checkbox\" ".
                                "name=\"CORRECTMASK".$i."\" ".
                                "value=\"".$i."\" ".
                                "tabindex=\"".$pge->nextTab++."\" ".
                                "title='".$lng->get( 'isCorrect' )."'".
                                 ( ( $correctMask & LmElement::mask($i) ) ?   " checked=\"checked\""  : '' ).
                          " onchange='isDirty=true'></td>\n".
                          "\t<td class=\"labsys_mop_m_answers_table_answer_cell\">".
                          "<textarea tabindex=\"".$pge->nextTab++."\" ".
                                    "id=\"a".$i."\" ".
                                    "name=\"ANSWER".$i."\" ".
                                    "class=\"labsys_mop_textarea\" ".
                                    "rows=\"".$cfg->get("DefaultMCAnswerRows")."\" onchange='isDirty=true'>";

         // fror available answers put them into the text area
         if ( $i < count( $answerArray ) ) $answerFields .= returnEditable( $answerArray[$i] ); // _ -> &#x005F; needed cause otherwise Replacement on $pge->show

         $answerFields .= "</textarea>".
                          "</td></tr>\n";
       }
       $answerFields .= "</table>";
       return $answerFields;
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

     return EM::editTopMultipleChoice( $this, $fullAddress ).
            "<FORM class=\"labsys_mop_std_form\" NAME=\"multipleChoiceEdit\" METHOD=\"POST\" ACTION=\"".$url->link2("../php/executeCommandOnElement.php?address=m")."\">\n".
            "<input type=\"hidden\" name=\"FUNCTIONNAME\" value=\"save()\">\n".           // this function will statically be called by ../php/executeCommandOnElement.php
            "<input type=\"hidden\" name=\"IDX\" value=\"".$this->idx."\">\n".            // needed by ../php/executeCommandOnElement.php
            "<input type=\"hidden\" name=\"SESSION_ID\" value=\"".session_id()."\">\n".   // to check whether the call is valid
            "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/edit.php\">\n".  // index of saved el. will be added on save.php!
            "<input type=\"hidden\" name=\"FULLADDRESS\" value=\"".$fullAddress."\">\n".

          // question input
            "<label for=\"q\" class=\"labsys_mop_input_field_label_top\">".$lng->get("question")."</label>".
            "<textarea tabindex=\"".$pge->nextTab++."\" id=\"q\" name=\"QUESTION\" class=\"labsys_mop_textarea\" rows=\"".$cfg->get("DefaultMCQuestionRows")."\" onchange='isDirty=true'>".
            returnEditable( $this->question ). /* _ -> &#x005F; needed cause otherwise Replacement on $pge->show */
            "</textarea>\n".

          // answers
            "<label for=\"a0\" class=\"labsys_mop_input_field_label_top\">".$lng->get("inputAnswers")."</label>".
            $this->answerFields( $this->answerArray, $this->correctMask ).

          // answer explanation
            "<label for=\"aE\" class=\"labsys_mop_input_field_label_top\">".$lng->get("inputAnswerExpl")."</label>".
            "<textarea tabindex=\"".$pge->nextTab++."\" id=\"aE\" name=\"ANSWER_EXPLANATION\" class=\"labsys_mop_textarea\" rows=\"".$cfg->get("DefaultMCAnswerExplRows")."\" onchange='isDirty=true'>".
            returnEditable( $this->answerExplanation ). /* _ -> &#x005F; needed cause otherwise Replacement on $pge->show */
            "</textarea>\n".

            "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("save")."\" accesskey=\"s\" onclick='isDirty=false'>\n".

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
                    "<label for=\"visOnlyColl\" class=\"labsys_mop_input_field_label\">".$lng->get("visOnlyInColl")."</label>".
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
            '<script language="JavaScript" type="text/javascript">
            <!--
            if (document.multipleChoiceEdit) document.multipleChoiceEdit.q.focus();
            //-->
            </script>'.
            EM::editBottomMultipleChoice( $this, $fullAddress );
  }

  /**
  * Used to save the edited element.
  *
  * This function gets STATICALLY called from ../php/executeCommandOnElement.php?address=[elementID].
  * It handles the $_POSTed data and then redirects to the $_POST['REDIRECTTO'] with the address of the element added.
  */
  /*static*/ function save(){
    global $mDBI, $url, $lng, $usr;

    // check for all necessary values posted
    if ( !isset($_POST['IDX']) ||
         !isset($_POST['REDIRECTTO']) ||
         !isset($_POST['FULLADDRESS']) ||
         !isset($_POST['QUESTION']) ||
         !isset($_POST['ANSWER0']) ||
         !isset($_POST['ANSWER_EXPLANATION'])
       ){
          trigger_error( $lng->get("notAllNecPosted"), E_USER_ERROR );
          exit;
         }

    // Validity (session ID) is checked by the calling script yet.

    // only content editors are allowed to save
    if ( !$usr->isOfKind( IS_CONTENT_EDITOR ) ){
                                                  trigger_error( $lng->get("notAllowed"), E_USER_ERROR );
                                                  exit;
                                                }

    // create answerArray + compute correctMask
    $toDoArray = "\$answerArray = Array(";
    $correctMask=0;
    $j=0;
    for ( $i = 0; isset( $_POST['ANSWER'.$i] ); $i++ )
      if ($_POST['ANSWER'.$i] != ""){
        if ( $j > 0 ) $toDoArray .= ", ";
        $toDoArray .= "\"".addslashes($_POST['ANSWER'.$i])."\"";
        if ( isset( $_POST[ "CORRECTMASK".$i] ) ) $correctMask += (1 << $j);
        $j++;
      }
    eval( $toDoArray." );" );
    $answerArray = array_map( "stripslashes", $answerArray );

    // compute user rights:
    $userRights=0;
    for ($i=1; $i<=MAX_USER_ROLE; $i=$i<<1) if ( isset( $_POST[ "UR_".$i] ) ) $userRights += $_POST["UR_".$i];

    $newMultipleChoice = new LmElement( $_POST['IDX'],
                                        $_POST['QUESTION'],
                                        $answerArray,
                                        $_POST['ANSWER_EXPLANATION'],
                                        $correctMask,
                                        $userRights,
                                        ($_POST["VISIBLE_ONLY_IN_COLLECTION"] == "1"),
                                        ""
                                       );

    // if $_POST["SAVEASNEW"] is set the element is saved as new element
    if ( isset($_POST["SAVEASNEW"]) && ($_POST["SAVEASNEW"] == "1") ){
                                      $newMultipleChoice->idx = $mDBI->setData( $newMultipleChoice, true );
                                      $url->put( "address=m".$newMultipleChoice->idx ); // the address of the new element
                                     }
    else{
                                      $mDBI->setData( $newMultipleChoice );
                                      $url->put( "address=".$_POST['FULLADDRESS'] );    // the address remains
    }

    // some user information
    $url->put( "sysinfo=".$lng->get("DataHasBeenSaved")." ".$lng->get("mNumber")." ".$newMultipleChoice->idx );
    makeLogEntry( 'm', 'created', $newMultipleChoice->idx );

    // One could also display the page here but that way it is left to the normal mechanism...
    header( "Location: ".$url->rawLink2( $_POST['REDIRECTTO'] ) );
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
    $sum = 0; for ($i = 0; $i < $this->answerCount; $i++) if ( isset( $_POST['ANSWER'.$i] ) ) $sum += $_POST['ANSWER'.$i];

    // load the dataset
    $this->loadUserData4( $usr->uid );

    // set this at the end
    $this->previousAnswersArray[$this->triesUp2Now] = $sum;

    // store the new dataset
    $this->mcDBI->setData4( $usr->uid, $this->idx, $this->displayPermutation, $this->previousAnswersArray );

    makeLogEntry( 'm', 'saved', $this->idx );

    // One could also display the page here but that way it is left to the normal mechanism...
    header( "Location: ".$url->rawLink2( $_POST['REDIRECTTO'] ) );
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

    // if it is not answered it will not be correct as the not answered status sets a bit...
    if ( $this->isAnswered ){  // enough checks yet or correct...
      makeLogEntry( 'm', 'check: is answered ('.$this->triesUp2Now.'/'.$cfg->get('MCAnsweringAttempts').')', 'm'.$this->idx );
      return true;
    }

    if ( !$this->isChangedByUser(end($this->previousAnswersArray)) ){
      // not answered yet -> do nothing
      return false;
    }

    $this->previousAnswersArray[] = $this->markAsUnChanged(0); // insert empty answer at the end
    $this->mcDBI->setData4( $usr->uid, $this->idx, $this->displayPermutation, $this->previousAnswersArray, "-check-" );

    $this->mcDBI = $this->loadUserData4( $usr->uid ); // Load data with applied changes.
    makeLogEntry( 'm', 'check: '.($this->isAnswered ? 'correct' : 'wrong').' ('.$this->triesUp2Now.'/'.$cfg->get('MCAnsweringAttempts').')', 'm'.$this->idx );
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
                infoArrow( $lng->get("visOnlyInColl") ).
                $lng->get("visOnlyInColl")."<br>\n";

    // rights
    for ($i=1; $i<=MAX_USER_ROLE; $i=$i<<1){
      $lgnd .= "<input type=\"radio\" disabled>".infoArrow( $lng->get("visAlways"), true )."<input type=\"radio\" disabled>".infoArrow( $lng->get("visOnlyInColl"), true )." | ";
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
              infoArrow( $lng->get("visOnlyInColl"), $disabled ).
            ( $disabled ?  "<span class=\"labsys_mop_grayed\">"  : '' ).
            " | ".
            ( $disabled ?  "</span>"  : '' ).
              showRightsHorizontal( $prefix, $this->visibleFor, $disabled ).
            " ".
              cutHTMLText( $this->idx.": ".returnEditable($this->question), $cfg->get("maxCharMngRowTitleShort") );
  }
}
?>
