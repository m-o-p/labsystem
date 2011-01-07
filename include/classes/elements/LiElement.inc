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
* implementation of the i (input) element.
*
* This element consists of a question the user can answer.
*
* @module     ../include/classes/elements/LiElement.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/

/*
The inputs are arbitrated by a lock. 
Each Team can only open one input at the same time.

This serves two purposes:
-	It prevents the team partners to work non cooperatively by answering the questions not together
-	It helps significantly not to forget saving an answer (as there is no Ajax this is not done automatically!)


How does the lock work now?
Suppose we have computer A and B. 
Both load the same question sheet so both see the "give answer…" buttons.

Suppose A opens a question.
The lock is set to that question now.
As A and B are working independently, B wants to open another question.
B clicks on "give answer…" of a question.
The system notices that B clicked on the button that was only visible since the view of A and B was inconsistent and just reloads the page on B.
B sees now red locks (as A) on all non open questions and the question A opened is open at B now too.
Of course B only sees the last saved answer of the question A answered so there might be no answer visible even though A wrote already something (but did not save).

If someone clicks on a red lock below a question, the system assumes, the person understands that it grabs the lock from another input by this and forces a lock take.
(This might be useful if one does not know anymore, which question one opened.)

Suppose now the case B grabbed the lock but A does not know that (they should cooperate so this should NEVER happen).
A wants to save the answer but the lock is not set to the question anymore.
Instead of discarding the answer what would be the normal case as the team did not coordinate itself, the system saves the answer now anyway but replaces all spaces by "NO_LOCK" to show the team that this behavior was not intended.
 */
require_once( INCLUDE_DIR."/classes/elements/Element.inc" );

// The following DBI is used in loadUserData4() (connects UID and team for one answer).
require( INCLUDE_DIR."/classes/elements/LiDBInterfaceUidTeam.inc" );
// The following DBI is used in loadUserData4() (connects to the user's answers).
require( INCLUDE_DIR."/classes/elements/LiDBInterfaceAnswers.inc" );
// The following DBI is used in loadUserData4() (connects to the user's locks).
require( INCLUDE_DIR."/classes/elements/LiDBInterfaceLocks.inc" );

class LiElement extends Element{
  var $question;              // The question.
  var $exampleSolution;       // Example solution.
  var $possibleCredits;       // Possible credits as float :2.

  var $visibleFor;            // Visibility constraints.
  
  var $userAnswer;            // The user's answer to this question.
  var $answerComment;         // The correctors comment to this question.
  var $givenCredits;          // The correctors given credits.

  var $isClosed;              // Is the user answer closed?
  var $isCorrected;           // Is the user answer corrected?
  var $answerHistory;         // The user answer's history.
  
  var $getDataOfTeam;         // The tem number data should be loaded of.
                              // For each Input the team is set to this beeing the $usr->currentTeam at the moment of first opening.
  
  var $lockIsSetTo;           /* A team can only answer the questions one by one.  Reason: Force them to work together. 
                               * The lock tells, which question is currently in answer mode (it contains its index or false).
                               */
                               
  var $forceReLock;           // This gets set if the user clicked on the red lock so when he is sure he wants to get the lock from 
                              // another input which meant the other input could not be saved anymore (set to student friendly mod: save anyway)
                    
  var $utDBI;                 // new LiDBInterfaceUidTeam();
  var $uaDBI;                 // new LiDBInterfaceAnswers();
  var $lkDBI;                 // new LiDBInterfaceLocks();
 
  /**
  * The constructor.
  *
  * @param $idx                       This instance's index.
  * @param $question                  The input's question.
  * @param $exampleSolution           Example solution.
  * @param $possibleCredits           Possible credits as float :2.
  * @param $visibleFor                The bitmask of rights a viewer has to match.
  * @param $visibleOnlyInCollection   Is the element visible only inside a collection.
  * @param $history                   This' history.
  * @param $isInColl                  Am I inside a collection? (important for visibility aspects).
  */ 
  function LiElement( $idx, $question, $exampleSolution, $possibleCredits, $visibleFor, $visibleOnlyInCollection, $history, $isInColl=false ){
    global $usr;
    $this->elementId                = "i";
    $this->idx                      = $idx;
    $this->title                    = preg_replace("(\r\n|\n|\r)", '', // used to remove linebreaks
                                        returnHTML( // remove [HTML] etc.
                                          substr( $question, 0, 50 ) 
                                        ) 
                                      );
    $this->matchingMenu             = "";                         // will be handled by ->getMatchingMenu();
    $this->question                 = $question;
    $this->exampleSolution          = $exampleSolution;
    $this->possibleCredits          = $possibleCredits;
    $this->visibleFor               = $visibleFor;
    $this->visibleOnlyInCollection  = $visibleOnlyInCollection;
    $this->history                  = $history;
    
    $this->IamVisible               = ($this->visibleFor == "0") || $usr->isOfKind( $this->visibleFor )
                                      && ( !$this->visibleOnlyInCollection || ($this->visibleOnlyInCollection && $isInColl) );
    if ( $history ){ // history is false if only menu data is wanted (following not necessary)
      /* The mapping user team is stored separately for each input's answer.
       * Why? The user could change its team and should still be able to see its answers.
       * This could be a problem when a user changes his team during a lab.
       * Then all mappings for this user and this lab have to be deleted in order to let them be recreated
       * with the new teamnumber.
       */
      $this->utDBI = new LiDBInterfaceUidTeam();
      $this->uaDBI = new LiDBInterfaceAnswers();
      $this->lkDBI = new LiDBInterfaceLocks();
      
      $this->forceReLock = (( isset($_POST['forceReLock']) ) && ( $_POST['forceReLock'] == 1 ));
  
      $this->loadUserData4(); // Fills rest of the structures with neutral values.
    }
    
    $this->serializationArray       = array( 'idx', 'question', 'exampleSolution', 'possibleCredits', 'visibleFor', 'visibleOnlyInCollection', 'history' ); // see Element.inc for details
  }
  
  /**
  * Serialize the element for export.
  */
  
  /**
  * DeSerialize the element for import.
  */
  
  /**
  * Loads the answer data to the given $uid into this instance's field.
  *
  * @param $teamNumber  The number of the team data should be loaded of.
  */
  function loadUserData4( $teamNumber=0 ){
    global $usr, $cfg;
    if ( $teamNumber != 0 ){
      $this->getDataOfTeam = $teamNumber;
      $answerData          = $this->uaDBI->getData4( $teamNumber,
                                                     $this->idx,
                                                     $teamNumber != 0 );
      $this->userAnswer    = $answerData["answer"];
      $this->answerComment = $answerData["comment"];
      $this->givenCredits  = $answerData["givenCredits"];
      $this->isClosed      = $answerData["isClosed"];
      $this->isCorrected   = $answerData["isCorrected"];
      $this->answerHistory = $answerData["history"];

      /* A team can only answer the questions one by one. Reason: Force them to work together. 
       * The lock tells, which question is currently in answer mode (p.e. i142).
       */
      $this->lockIsSetTo   = $this->lkDBI->getData4( $this->getDataOfTeam );
    } else {
      $this->userAnswer    = "";
      $this->answerComment = "";
      $this->givenCredits  = 0;
      $this->isClosed      = false;
      $this->isCorrected   = false;
      $this->answerHistory = "";
      
      $this->lockIsSetTo   = false;
    }
  }

  /**
   * Returns the credits given by the corrector.
   *
   * @return $this->givenCredits
   */
   function getGivenCredits(){
     return $this->givenCredits;
   }
   
  /**
   * Returns the credits one cen get for the correct answer.
   *
   * @return $this->possibleCredits
   */
   function getPossibleCredits(){
     return $this->possibleCredits;
   }

  
// The next functions are used for the element's view page. *************************************************

// helper (if this exists some time they should be declared private)  - - - - - - - - - - - - - - - - - - - 
  /**
  * Shows the corrector's comments.
  *
  * @param  $isInCorrection   If true the comment is returned as textarea. Otherwise as division.
  *
  * @return string            The html representation of the correctors comment.
  */
  function showComment( $isInCorrection=false ){
    global $usr, $pge, $cfg;
    if ( $isInCorrection ) 
      return "<textarea tabindex=\"".$pge->nextTab++."\" name=\"COMMENT\" class=\"labsys_mop_textarea\" rows=\"".$cfg->get("DefaultICommentRows")."\">".
             returnEditable( $this->answerComment ).
             "</textarea>\n";
    else if ( $this->answerComment != "" )
      return "<div class=\"labsys_mop_i_comment\">".
             returnHTML( $this->answerComment ).
             "</div>\n";
  }
  
  /**
  * Shows the user's answer.
  *
  * @param  $isGettingAnswered  If true the comment is returned as textarea. Otherwise as division.
  *
  * @return string              The html representation of the correctors comment.
  */
  function showAnswer( $isGettingAnswered=false ){
    global $cfg, $pge;
    if ( $isGettingAnswered ) return  "<textarea  tabindex=\"".$pge->nextTab++."\" ".
                                                  'name="ANSWER" '.
                                                  'class="labsys_mop_textarea" '.
                                                  'rows="'.$cfg->get("DefaultIAnswerRows").'"'.
                                                  '>'.
                                                  $this->userAnswer.
                                                  "</textarea>\n";
                         else if ( $this->userAnswer != "" ) return "<div class=\"labsys_mop_textarea\">".
                                                                    returnHTML( $this->userAnswer ).
                                                                    "</div>\n";
                              else return "";
  }
  
  /**
  * Shows the credits.
  *
  * @param  $isInCorrection   If true the credits are an input.
  * @param  $possiblePtsOnly  If set only the possible points are shown (to be placed below the example solution).
  *
  * @return string            The html representation of the correctors comment.
  */
  function showCredits( $isInCorrection=false, $possiblePtsOnly=false ){
    global $lng, $pge, $usr;
    return "<div class=\"labsys_mop_i_credits".
          // if we are viewing someone the color of the credits division tells whether the input is marked as corrected
          retIfTrue( $usr->isOfKind( IS_CORRECTOR ) && !$possiblePtsOnly,
                     retIfTrue( $this->isCorrected, "_green", "_red") ).
          "\">".
            // the "is corrected" checkbox
            retIfTrue( $isInCorrection, 
                       "<label for=\"IS_CORRECTED\" class=\"labsys_mop_input_field_label_left\">".$lng->get("inputIsCorrected")."</label> ".
                       "<input type=\"checkbox\" name=\"IS_CORRECTED\" value=\"1\" ".
                        "tabindex=\"".$pge->nextTab++."\" ".
                        "checked=\"checked\"". // so it will be checked by default
                       ">"
                      ).
            // the credits
            "[".
            retIfTrue( $isInCorrection, 
                       // show input
                       "<input tabindex=\"".$pge->nextTab++."\" type=\"text\" name=\"CREDITS\" class=\"labsys_mop_input\" maxlength=\"5\" value=\"".
                       // on first entering set the points to full points
                       retIfTrue( $this->isCorrected || /* pre corrected: */$this->givenCredits>0 || $this->answerComment!='', $this->givenCredits, $this->possibleCredits ).
                       "\">"."/ ",
                       retIfTrue( !$possiblePtsOnly,
                                  retIfTrue( ($this->isCorrected || $usr->isOfKind( IS_CORRECTOR )), 
                                              // show given credits
                                              $this->givenCredits,
                                              '0')."/ "
                                 )
                      ).
            $this->possibleCredits." ".$lng->get("Credits")."]".
          "</div>\n";
  }
  
  /**
  * Shows the user's answer. Includes some msic!
  *                       Since there might be many independent inputs on one page the
  *                       corrector is prevented from forgetting to save the same way
  *                       as the user is with the multiple choice questions.
  *                       The inputs are disabled until the userpresses the answer button.
  *                       Then he can answer and the button is used to save.
  *
  * @param $fullAddress   The elements full address.
  *
  * @return string        The html representation of the user's answer.
  */
  function showAnswerData( $fullAddress ){
    global $usr, $url, $pge, $cfg, $lng;
    
    /* The element can have 5 states:
     *  - answerable (user did not close the input)
     *    + answer not editable
     *    + give answer button is shown
     *    corrector: -
     *  - getting answered (user opened the input)
     *    user:
     *    + answer editable (textarea)
     *    + save button shown
     *    corrector: -
     *  - corectable (user closed the input)
     *    + answer not editable
     *    corrector:
     *    + comment/ points not editable
     *    + correct button shown
     *  - in correction (corrector opened the input)
     *    user:
     *    + answer not editable
     *    + comment/ points not visible
     *    corrector:
     *    + comment/ points editable
     *    + save button shown
     *  - corrected (corrector set the isCorrected bit)
     *    user:
     *    + answer not editable
     *    + comment/ points visible
     *    corrector:
     *    + same as correctable
     */

         $isInCorrection  = ( $usr->isOfKind( IS_CORRECTOR ) && 
                              $this->isClosed && 
                              isset( $_POST['FULLADDRESS'] ) && ( $_POST['FULLADDRESS'] == $fullAddress ) &&
                              $_POST['TEAM'] == $this->getDataOfTeam
                             );
      if ($isInCorrection) makeLogEntry( 'i', 'getting corrected for team '.$this->getDataOfTeam, $this->idx );
                             
      $isGettingAnswered  = ( $this->lockIsSetTo && ( $this->lockIsSetTo == $this->idx ) );
      if ($isGettingAnswered) makeLogEntry( 'i', 'getting answered', $this->idx );

      $ret =   "<a name=\"".$fullAddress.".".$this->getDataOfTeam."\"></a>\n". // anchor for retrieving after saving
               "<FORM class=\"labsys_mop_std_form\" NAME=\"InputAnswer".$this->idx."\" METHOD=\"POST\" ".
               "ACTION=\"".retIfTrue( ($usr->isOfKind( IS_CORRECTOR ) && $this->isClosed && !$isInCorrection),
                                      // link for the corrector to unlock... (-> see description above method)
                                      $url->link2("../pages/view.php?address=".$url->get('address')."#".$fullAddress.".".$this->getDataOfTeam),
                                      // normal handling -> pass method to executeCommandOnElement.php
                                      $url->link2("../php/executeCommandOnElement.php?address=i")
                                     ).
                "\">\n".
                
                retIfTrue(  $isInCorrection,
                            retIfTrue( $isInCorrection,
                                       // the corrector has opened this for correction
                                       "<input type=\"hidden\" name=\"FUNCTIONNAME\" value=\"saveCorrectorStuff()\">\n"
                                      ),
                            retIfTrue( $isGettingAnswered,
                                       // user set lock to this
                                       "<input type=\"hidden\" name=\"FUNCTIONNAME\" value=\"saveUserAnswer()\">\n",
                                       // user may set a lock to this
                                       "<input type=\"hidden\" name=\"FUNCTIONNAME\" value=\"setUserAnswerLock()\">\n"
                                      )
                ).
              "<input type=\"hidden\" name=\"SESSION_ID\" value=\"".session_id()."\">\n".
              "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/view.php?address=".$url->get('address')."#".$fullAddress.".".$this->getDataOfTeam."\">\n". // return so self
              "<input type=\"hidden\" name=\"FULLADDRESS\" value=\"".$fullAddress."\">\n".
              "<input type=\"hidden\" name=\"IDX\" value=\"".$this->idx."\">\n".            // needed by ../php/executeCommandOnElement.php
              "<input type=\"hidden\" name=\"TEAM\" value=\"".$this->getDataOfTeam."\">\n".
              
            // answer
              $this->showAnswer( $isGettingAnswered && !$usr->isOfKind( IS_CORRECTOR ) ); // correctors never edit answers
              
    // comment
      if ( $this->isCorrected || $usr->isOfKind( IS_CORRECTOR ) ) $ret .= $this->showComment( $isInCorrection );
      
    // credits           
      $ret .= $this->showCredits( $isInCorrection );

    // button
      if ( $usr->isOfKind( IS_CORRECTOR ) )
        if ( !$this->isClosed ) ; 
        else if ( $isInCorrection ) $ret .= "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_i_save_button\" ".
                                                   "value=\"".$lng->get("save")."\" accesskey=\"s\">\n";
                               else if ( !isset( $_POST['FULLADDRESS'] ) )  $ret .= "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_i_save_button\" ".
                                                                                           "value=\"".$lng->get("correct")."\">\n";
                                    else ; // another is opened yet
      else if ( $this->isClosed ) ;
           else  if ( $isGettingAnswered ) $ret .= "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_i_save_button\" ".
                                                          "value=\"".$lng->get("save")."\">\n";
                                      else if ( !$this->lockIsSetTo )
                                           $ret .= "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_i_save_button\" ".
                                                          "value=\"".$lng->get("giveAnswer")."\">\n";
                                           else // lock is set and is not set to this
                                           $ret .= "<input tabindex=\"".$pge->nextTab++."\" type=\"image\" ".
                                                          "src=\"../syspix/button_lock_closed_red_7x10.gif\" width=\"7\" height=\"10\" alt=\"".$lng->get("LockSetButForce").' '.$this->lockIsSetTo."\">\r\n".
                                                // here the users gets indicated that another question is locked, so clicking on the red lock forces acquiring from another question.
                                                          '<input type="hidden" name="forceReLock" value="1">'."\r\n";
              
            
      $ret .= "</FORM>\n".
    
              retIfTrue(  ( $isGettingAnswered || $isInCorrection ),
                          '<script language="JavaScript" type="text/javascript">'."\n".
                          '<!--'."\n".
                          'if (document.InputAnswer'.$this->idx.') document.InputAnswer'.$this->idx.'.'.retIfTrue( $isGettingAnswered, "ANSWER", "COMMENT" ).'.focus();'.
                          '//-->'."\n".
                          '</script>'."\n" ).
                          
                        // answer history
              retIfTrue( ($this->answerHistory != ""), EM::viewInputAnswerHistory( $this ) );
              
    return $ret;                          
  }
// /helper - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  /**
  * Returns the element's HTML-representation in the way the VIEWER should see it.
  *
  * @param $_GET['noExSol'] If set the example solution is hidden.
  * @param $fullAddress     The element's full address (p.e. C23.c42.p13)
  * @param $paragraph       The element's paragraph number (p.e. 2.3). Will be added to the title if given.
  * @return string          The html code of the element's view view.
  */ 
  function show( $fullAddress, $paragraph="" ){
    global $usr, $lng, $pge;
    if (  !$this->isVisible() ){ // not enough rights->no content
      return "<div class=\"labsys_mop_note\">\n".$lng->get("TxtNotEnoughRights")."\n</div>";
      exit;
    }

    if ($this->question == "") return "'()";
    
    $answers = "";
    // Load the dynamic data from the working table.
    if ( !$usr->isOfKind( IS_CORRECTOR ) && !$usr->isOfKind( IS_EX_SOLUTION_VIEWER ) ){ // "normal" user
      $this->loadUserData4( $this->utDBI->getData4( $usr->uid, $this->idx, $usr->currentTeam ) );    // create dataset tith user's team
      $answers = $this->showAnswerData( $fullAddress );
    }
    
    if ( $usr->isSeeingSomeonesData() ) // correcting someone
      if ( $usr->theSeeingUid() != "all" ){
                                $this->loadUserData4( $this->utDBI->getData4( $usr->theSeeingUid(), $this->idx, 0 ) ); // ( 0 = don't create the record )
                                $answers = $this->showAnswerData( $fullAddress );
      }else{ // crosscorrecting all
                                $this->uaDBI->getAllTeams( $this->idx/*, "team"*/ ); // as is unsorted/ order of answering
                                while ( ($nextTeam = $this->uaDBI->getNextData()) ){
                                  $this->loadUserData4( $nextTeam );
                                  $answers .= "<div class=\"labsys_mop_i_teamnr\">".$lng->get("team")." ".$nextTeam."</div>\n".
                                              $this->showAnswerData( $fullAddress );
                                }
      }
      
  
    parseHTML( $this->question ); // Process [HTML].
    parseHTML( $this->exampleSolution );
    
    return "<a name=\"".$fullAddress."\"></a>\n". // anchor for retrieving after saving
           "<div class=\"labsys_mop_elements_i_around\">".
           EM::viewTopInput( $this, $fullAddress ).
           "<div class=\"labsys_mop_contentArea\">\n".
             "<div class=\"labsys_mop_i_question\">".
               retIfTrue( ($paragraph!=""), $paragraph.". " ).$this->question.
             "</div>\n".
             
           // example solution
             retIfTrue( $usr->isOfKind( IS_EX_SOLUTION_VIEWER ) && ($this->exampleSolution != "") && !isset( $_GET['noExSol'] ), 
                                   "<div class=\"labsys_mop_i_example_solution\">".
                                   '<!-- example solution -->'."\r\n".
                                   $this->exampleSolution.
                                   '<!-- /example solution -->'."\r\n".
                                   "</div>\n"
                       ).
                        
           // answer + eventually comment + credits
             $answers.
             retIfTrue( ($answers == ""), $this->showCredits( false, true ) ).
           "</div>\n".
           EM::viewBottomInput( $this, $fullAddress ).
           "</div>";
  }
  
// The next functions are used for the element's edit page. *************************************************

  /**
  * Returns the element's HTML-representation in the way the EDITOR should see it.
  *
  * @param $_GET["saveAsNew"] If $_GET["saveAsNew"] is set the save as new checkbox on the edit page should be set
  * @param $fullAddress       The element's full address (p.e. C23.c42.p13)
  * @return string            The html code of the element's edit view.
  */  
  function showEdit( $fullAddress ){
    global $lng, $url, $pge, $cfg;
    
     return EM::editTopInput( $this, $fullAddress ).
            "<FORM class=\"labsys_mop_std_form\" NAME=\"InputEdit\" METHOD=\"POST\" ACTION=\"".$url->link2("../php/executeCommandOnElement.php?address=i")."\">\n".
            "<input type=\"hidden\" name=\"FUNCTIONNAME\" value=\"save()\">\n".           // this function will statically be called by ../php/executeCommandOnElement.php
            "<input type=\"hidden\" name=\"SESSION_ID\" value=\"".session_id()."\">\n".
            "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/edit.php\">\n". /* index of saved el. will be added on save.php! */
            "<input type=\"hidden\" name=\"FULLADDRESS\" value=\"".$fullAddress."\">\n".
            "<input type=\"hidden\" name=\"IDX\" value=\"".$this->idx."\">\n".            // needed by ../php/executeCommandOnElement.php
            
          // question
            "<label for=\"question\" class=\"labsys_mop_input_field_label_top\">".$lng->get("question")."</label>".
            "<textarea tabindex=\"".$pge->nextTab++."\" id=\"question\" name=\"QUESTION\" class=\"labsys_mop_textarea\" rows=\"".$cfg->get("DefaultIQuestionRows")."\">".
            returnEditable( $this->question ). /* _ -> &#x005F; needed cause otherwise Replacement on $pge->show */
            "</textarea>\n".

          // example solution
            "<label for=\"exSol\" class=\"labsys_mop_input_field_label_top\">".$lng->get("inputExSolutionExpl")."</label>".
            "<textarea tabindex=\"".$pge->nextTab++."\" id=\"exSol\" name=\"EXAMPLE_SOLUTION\" class=\"labsys_mop_textarea\" rows=\"".$cfg->get("DefaultIExSolutionRows")."\">".
            returnEditable( $this->exampleSolution ). /* _ -> &#x005F; needed cause otherwise Replacement on $pge->show */
            "</textarea>\n".

          // credits
            "<div class=\"labsys_mop_i_credits\">".
            "<label for=\"pC\" class=\"labsys_mop_input_field_label\">".$lng->get("Credits")."</label> ".
            "<input tabindex=\"".$pge->nextTab++."\" type=\"text\" id=\"pC\" name=\"POSSIBLE_CREDITS\" class=\"labsys_mop_input\" maxlength=\"5\" value=\"".$this->possibleCredits."\">".
            "</div>\n".
                          
            "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("save")."\" accesskey=\"s\">\n".

          // element properties
            "<fieldset><legend>".$lng->get("properties")."</legend>\n".
              "<table width=\"100%\"><tr><td valign=\"top\" width=\"40%\">\n".
                  "<fieldset><legend>".$lng->get("visibility")."</legend>\n".
                    "<div>".
                      "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" id=\"visAlways\" name=\"VISIBLE_ONLY_IN_COLLECTION\" value=\"0\"".retIfTrue( !$this->visibleOnlyInCollection, " checked=\"checked\"").">".
                      "<label for=\"visAlways\" class=\"labsys_mop_input_field_label\">".$lng->get("visAlways")."</label>".
                    "</div>\n".
                    "<div>".
                      "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\"  id=\"visOnlyColl\"name=\"VISIBLE_ONLY_IN_COLLECTION\" value=\"1\"".retIfTrue( $this->visibleOnlyInCollection, " checked=\"checked\"").">".
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
            "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" id=\"saveAsNew\" name=\"SAVEASNEW\" value=\"1\"".retIfTrue( ( isSet( $_GET["saveAsNew"] ) || ($this->idx == 1) ),  " checked=\"checked\"" ).">".
            "<label for=\"saveAsNew\" class=\"labsys_mop_input_field_label\">".$lng->get("saveAsNew")."</label>".
            "</fieldset>\n".

            "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("save")."\">\n".            

            "</FORM>\n".
            '<script language="JavaScript" type="text/javascript">
            <!--
            if (document.InputEdit) document.InputEdit.question.focus();
            //-->
            </script>'.
            EM::editBottomInput( $this, $fullAddress );
  }
  
  /**
  * Used to save the edited element.
  *
  * This function gets STATICALLY called from ../php/executeCommandOnElement.php?address=[elementID].
  * It handles the $_POSTed data and then redirects to the $_POST['REDIRECTTO'] with the address of the element added.
  */  
  /*static*/  function save(){
    global $iDBI, $url, $lng, $usr;
    
    // check for all necessary values posted
    if ( !isset($_POST['IDX']) || 
         !isset($_POST['REDIRECTTO']) || 
         !isset($_POST['FULLADDRESS']) || 
         !isset($_POST['QUESTION']) || 
         !isset($_POST['EXAMPLE_SOLUTION']) || 
         !isset($_POST['POSSIBLE_CREDITS'])
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

    // compute user rights:
    $userRights=0;
    for ($i=1; $i<=MAX_USER_ROLE; $i=$i<<1) if ( isset( $_POST[ "UR_".$i] ) ) $userRights += $_POST["UR_".$i];

    $newInput = new LiElement(  $_POST['IDX'],
                                $_POST['QUESTION'], 
                                $_POST['EXAMPLE_SOLUTION'],
                                $_POST['POSSIBLE_CREDITS'],
                                $userRights, 
                                ($_POST["VISIBLE_ONLY_IN_COLLECTION"] == "1"),
                                ""
                               );
                               
    // if $_POST["SAVEASNEW"] is set the element is saved as new element
    if ( isset($_POST["SAVEASNEW"]) && ($_POST["SAVEASNEW"] == "1") ){
                                      $newInput->idx = $iDBI->newInput( $newInput );
                                      $url->put( "address=i".$newInput->idx );        // the address of the new element
                                     }
    else{
                                      $iDBI->setData( $newInput );
                                      $url->put( "address=".$_POST['FULLADDRESS'] );  // the address remains
    }
    
    // some user information
    $url->put( "sysinfo=".urlencode( $lng->get("DataHasBeenSaved")." ".$lng->get("iNumber")." ".$newInput->idx ) );
    makeLogEntry( 'edit', 'saved', 'i'.$newInput->idx );
      
    // One could also display the page here but that way it is left to the normal mechanism...
    header( "Location: ".$url->rawLink2( $_POST['REDIRECTTO'] ) );
  }
  
  
    
  /**
  * Acquires the lock for $this->idx and $this->getDataOfTeam.
  *
  *@param $fullAddress  The elements full address (only passed for logging purposes).
  */
  function acquireLock4( $fullAddress ){
    $this->lkDBI->setData4( $this->getDataOfTeam, $this->idx, $fullAddress );
  }

  /**
  * Removes all locks for $this->getDataOfTeam.
  */
  function removeLock(){
    if ($this->lockIsSetTo) $this->lkDBI->remLock4( $this->getDataOfTeam );
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
         !isset($_POST['ANSWER']) ||
         !isset($_POST['FULLADDRESS']) ||
         !isset($_POST['TEAM'])
       ){
          trigger_error( $lng->get("notAllNecPosted"), E_USER_ERROR );
          exit;
         }
       
    // Validity (session ID) is checked by the calling script yet.
       
    $this->loadUserData4( $_POST['TEAM'] ); // loads the dynamic data from the working table.
    
    if ( $this->lockIsSetTo == $this->idx ){
      $this->uaDBI->setData4( $_POST['TEAM'], $this->idx, $_POST['ANSWER'] );
      makeLogEntry( 'i', 'saved', $this->idx );
      $this->removeLock();
    } else{ // You are only allowed to answer one question at the same time. 
           // Since you can move the lock this ensures the desired behavior.
           // In case the lock is not at this question anymore, the data is discarded and the error is shown...
            $url->put( "sysalert=".urlencode( $lng->get("NotAllowedSaveLockAt")." ".$this->lockIsSetTo ) );
            
           // As this case should never happen, we save here anyway not to lose data:
           // To abondon parallel answering we replace all spaces by NO_LOCK so the user has to remove this later again.
            $_POST['ANSWER'] = preg_replace( '/\s/', 'NO_LOCK', $_POST['ANSWER'] );
            $this->uaDBI->setData4( $_POST['TEAM'], $this->idx, $_POST['ANSWER'] );
    }
    // One could also display the page here but that way it is left to the normal mechanism...
    header( "Location: ".$url->rawLink2( $_POST['REDIRECTTO'] ) );
  }
  
  /**
  * Used to save the corrector's comment and given points.
  *
  * This function gets STATICALLY called from ../php/executeCommandOnElement.php?address=[elementID].
  * It handles the $_POSTed data and then redirects to the $_POST['REDIRECTTO'] with the address of the element added.
  */  
  /*static*/ function saveCorrectorStuff(){
    global $url, $lng, $usr;
    
    // check for all necessary values posted
    if ( !isset($_POST['IDX']) || 
         !isset($_POST['REDIRECTTO']) || 
         !isset($_POST['CREDITS']) ||
         !isset($_POST['FULLADDRESS']) ||
         !isset($_POST['TEAM'])
       ){
          trigger_error( $lng->get("notAllNecPosted"), E_USER_ERROR );
          exit;
         }
    
    // Validity (session ID) is checked by the calling script yet.
       
    // only correctors are allowed to save
    if ( !$usr->isOfKind( IS_CORRECTOR ) ){
                                             trigger_error( $lng->get("notAllowed"), E_USER_ERROR );
                                             exit;
                                           }
       
    $this->loadUserData4( $_POST['TEAM'] ); // loads the dynamic data from the working table.

    $this->uaDBI->setData4( $_POST['TEAM'], $this->idx, $this->userAnswer, $_POST['CREDITS'], retIfTrue( isset($_POST['COMMENT']), $_POST['COMMENT'], $this->answerComment), true, isset($_POST['IS_CORRECTED']) );

    makeLogEntry( 'i', 'correction saved', $this->idx );
    // One could also display the page here but that way it is left to the normal mechanism...
    header( "Location: ".$url->rawLink2( $_POST['REDIRECTTO'] ) );
  }
  
  /**
  * Tries to set the answer lock.
  *
  * This function gets STATICALLY called from ../php/executeCommandOnElement.php?address=[elementID].
  * It handles the $_POSTed data and then redirects to the $_POST['REDIRECTTO'] with the address of the element added.
  */  
  /*static*/ function setUserAnswerLock(){ 
    global $url, $lng;
    if ( !isset($_POST['IDX']) || 
         !isset($_POST['REDIRECTTO']) || 
         !isset($_POST['FULLADDRESS']) ||
         !isset($_POST['TEAM'])   
       ){
          trigger_error( $lng->get("notAllNecPosted"), E_USER_ERROR );
          exit;
         }

    // Validity (session ID) is checked by the calling script yet.
     
    $this->loadUserData4( $_POST['TEAM'] ); // loads the dynamic data from the working table.
     
    if ($this->isClosed)
      $url->put( "sysalert=".urlencode( $lng->get("InputAlreadyClosed") ) );
    else{
      $reForceLock = ( $this->lockIsSetTo && $this->forceReLock ); // lock is set yet and user clicked on lock -> place a note but set it
      if ( $reForceLock )
        $url->put( "sysalert=".urlencode( $lng->get("LockTakenFrom")." ".$this->lockIsSetTo ) );

      if (!$this->lockIsSetTo || $reForceLock){
        $this->acquireLock4( $_POST['FULLADDRESS'] );
        makeLogEntry( 'i', 'open', $this->idx );
      }
    }
         
    // One could also display the page here but that way it is left to the normal mechanism...
    header( "Location: ".$url->rawLink2( $_POST['REDIRECTTO'] ) );
  }
  
  /**
  * Closes the input.
  *
  * Gets called by the l (lab) element.
  *
  * @param $teamNumber  The number of the team to open the input of.
  */ 
  function closeInput( $teamNumber ){
    global $usr, $lng;
    $this->loadUserData4( $teamNumber );
    $this->removeLock();
    if (!$this->isClosed) $this->uaDBI->setData4( $teamNumber, $this->idx, $this->userAnswer, $this->givenCredits, $this->answerComment, true, false, $lng->get("closesInput")  );
    $this->isClosed = true;
    $this->isCorrected = false;
  }
  
  /**
  * ReOpens the input.
  *
  * Gets called by the l (lab) element.
  *
  * @param $teamNumber  The number of the team to open the input of.
  */ 
  function reOpenInput( $teamNumber ){
    global $usr, $lng;
    $this->loadUserData4( $teamNumber );
    if ($this->isClosed) $this->uaDBI->setData4( $teamNumber, $this->idx, $this->userAnswer, $this->givenCredits, $this->answerComment, false, false, $lng->get("reOpensInput")  );
    $this->isClosed = false;
    $this->isCorrected = false;
  }

  /**
  * Creates the mapping $UID->$team.
  * Normally the mapping should exist but if the user never viewed the question 
  * (only one teammember opened for answering) it might be missing.
  *
  * @param $uid   The user's unique identifier.
  * @param $team  The team to be mapped to.
  */
  function mapInput( $uid, $team ){
    global $lng;
    if ( $this->utDBI->getData4( $uid, $this->idx, 0 ) != $team ) // changes?
      $this->utDBI->setData4( $uid, $this->idx, $team, $lng->get("userReMapped") );
    $this->loadUserData4( $team ); // necessary to create an answer
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
    return  
              "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" ".retIfTrue( !$disabled, "name=\"".$prefix."_VISIBLE_ONLY_IN_COLLECTION\" " )."value=\"0\"".retIfTrue( !$this->visibleOnlyInCollection, " checked=\"checked\"").retIfTrue( $disabled, " disabled=\"disabled\"").">".
              infoArrow( $lng->get("visAlways"), $disabled ).
              "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" ".retIfTrue( !$disabled, "name=\"".$prefix."_VISIBLE_ONLY_IN_COLLECTION\" " )."value=\"1\"".retIfTrue( $this->visibleOnlyInCollection, " checked=\"checked\"").retIfTrue( $disabled, " disabled=\"disabled\"").">".
              infoArrow( $lng->get("visOnlyInColl"), $disabled ).
            retIfTrue( $disabled, "<span class=\"labsys_mop_grayed\">" ).
            " | ".
            retIfTrue( $disabled, "</span>" ).
              showRightsHorizontal( $prefix, $this->visibleFor, $disabled ).
            " ".
              cutHTMLText( $this->idx.": ".returnEditable($this->question), $cfg->get("maxCharMngRowTitleShort") );
  }
}
?>
