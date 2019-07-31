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
* This module is the database interface for the m element.
* It connects to the data database's table "multiple_choices".
*
* @module     ../include/elements/LmDBInterface.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
if ( isset( $GLOBALS["mDBI"] ) ) return; /* require_once doesn't work well */

require_once( INCLUDE_DIR."/classes/Ddbc.inc" );
require( INCLUDE_DIR."/classes/elements/LmElement.inc" );

class LmDBInterface {
  var $myDBC;
  var $myTable;

  function __construct() {
    global $dDBI;
    $this->myDBC   = $dDBI;
    $this->myTable = "multiple_choices";
  }

  /**
   * Creates a new element of the required type using the database query result given as parameter.
   * @param unknown $data The result of the database query.
   * @return LcElement The new element filled with the data from the DB.
   */
  function createElementFromDbResult( &$data, $isInColl=false ){
    global $cfg;
    $newElement = new LmElement(  $data['idx'],                                         // $idx
                                  $this->myDBC->unEscapeString($data['question']),                      // $question
                                  unserialize( $this->myDBC->unEscapeString($data['answer_array']) ),   // $answerArray
                                  (isset($data['reaction_array']) && !empty($data['reaction_array']) ? unserialize( $this->myDBC->unEscapeString($data['reaction_array']) ) : Array()),   // $reactionArray
                                  $this->myDBC->unEscapeString($data['answer_explanation']),            // $answerExplanation
                                  $data['correct_mask'],                                // $correctMask
                                  (isset($data['possible_credits']) && ($data['possible_credits']!=-1) ? round($this->myDBC->unEscapeString( $data['possible_credits'] )/100, 2) : $cfg->get('MCAnsweringAttempts') ), // $possibleCredits
                                  (isset($data['do_not_shuffle_answers']) ? $data['do_not_shuffle_answers'] == 1 : FALSE),
                                  (isset($data['only_one_answer_possible']) ? $data['only_one_answer_possible'] == 1 : FALSE),
                                  (isset($data['requires_n_correct']) ? $data['requires_n_correct'] : NULL),
                                  (isset($data['reactions']) ? $data['reactions'] == 1 : FALSE), //-1 is the default...
                                  (isset($data['show_hint_for_clicked']) ? $data['show_hint_for_clicked'] == 1 : FALSE), //-1 is the default...
                                  (isset($data['reshow_last_hint']) ? $data['reshow_last_hint'] == 1 : FALSE), //-1 is the default...
                                  (isset($data['detailed_grading']) ? $data['detailed_grading'] == 1 : FALSE),
                                  (isset($data['horizontal_layout']) ? $data['horizontal_layout'] == 1 : FALSE),
                                  $data['visible_for'],                                 // $visibleFor
                                  ($data['visible_only_in_collection'] == "1"),         // $visibleOnlyInCollection
                                  $data['history'],                                     // $history
                                  $isInColl                                             // $isInColl
                                );
    return $newElement;
  }

  /**
  * Returns the LmElement specified by $idx.
  * Calling this method is the normal way to get an instance of the fitting element.
  * This might seem strange but since constructors can't be overloaded it was the better
  * (more flexible) alternative to let the constructor create the object by values and not
  * just by an index like it is done here.
  *
  * @param $idx               The element's index (also idx in the database).
  * @param $isInColl          The element might be restricted to be visible only inside a collection.
  *                           Since this is kind of the elements constructor this argument is just
  *                           passed to the real constructor.
  *
  * @return LmElement
  */
  function getData2idx( $idx, $isInColl=false ){
    global $cfg,$url,$lng,$usr;
    $result = $this->myDBC->mkSelect( "*", $this->myTable, "idx='".$idx."'" );
    if ( !($data = $result->fetch_array()) ){
      //trigger_error( $this->myDBC->link->error. "mc with idx not found: ".$idx, E_USER_WARNING );
      return false;
      exit;
    }else{
      // load old version...
      $myId = 'm';
      if ($usr->isOfKind( IS_CONTENT_EDITOR ) && $cfg->doesExist('versioning') && $cfg->get('versioning')=='1' &&
      $url->available('historicVersion') &&
      strpos($url->get('historicVersion'), $myId.$idx.'-') !== FALSE){
        $historyIdx = substr($url->get('historicVersion'), strpos($url->get('historicVersion'),'-')+1);
        $backupIdx = $data['idx'];
        $backupHistory = $data['history'];
        $result = $this->myDBC->mkSelect( "*", 'bak_'.$this->myTable, "idx='".$historyIdx."'" );
        if ( !($data = $result->fetch_array()) ){
          trigger_error( $this->myDBC->link->error. 'Version '.$historyIdx.' of '.$myId.$idx.' not found.', E_USER_WARNING );
          return false;
          exit;
        }
        $data['idx'] = $backupIdx;
        $data['history'] = preg_replace('/^(@@'.$historyIdx.' )(.*)/m','$1<span class="selectedVersion">$2</span>',$backupHistory);
      }

      return $this->createElementFromDbResult($data,$isInColl);
    }
  }

  function getMenuData2idx($idx, $isInColl=false ){
    return $this->getData2idx($idx, $isInColl);
  }

  /**
   * Creates the database set string from the element.
   * @param unknown $element
   * @return string
   */
  function createSetStringFromElement( &$eM ){
    return    'question="'.              $this->myDBC->escapeString( $eM->getQuestion() ).'", '.
              'answer_array="'.          $this->myDBC->escapeString( serialize($eM->getAnswerArray()) ).'", '.
              'reaction_array="'.        $this->myDBC->escapeString( serialize($eM->getReactionArray() )).'",'.
              'answer_explanation="'.    $this->myDBC->escapeString( $eM->getAnswerExplanation() ).'", '.
              'correct_mask="'.          $eM->getCorrectMask().'", '.
              'possible_credits="'.      (!is_numeric($eM->getPossibleCredits()) ? 0 : round($eM->getPossibleCredits()*100,0)).'", '.
              'do_not_shuffle_answers='. ($eM->getDoNotShuffleAnswers() ? 1 : 0).', '.
              'only_one_answer_possible='.($eM->getOnlyOneAnswerPossible() ? 1 : 0).', '.
              'requires_n_correct='.     (is_null($eM->getRequiresNcorrect())? 'NULL' : $eM->getRequiresNcorrect()).', '.
              'reactions='.              ($eM->getReactionsEnabled() ? 1 : 0).', '.
              'show_hint_for_clicked='.  ($eM->getHintForKlicked() ? 1 : 0).', '.
              'reshow_last_hint='.       ($eM->getReshowLastHint() ? 1 : 0).', '.
              'detailed_grading='.       ($eM->getDetailedGradingEnabled() ? 1 : 0).', '.
              'horizontal_layout='.      ($eM->getHorizontalLayout() ? 1 : 0).', '.
              'visible_for="'.           $eM->getVisibleFor().'", '.
              'visible_only_in_collection="'.( $eM->visibleOnlyInCollection ?  1 :  0 ).'", ';
  }

  /**
  * !$createNew: Stores the data into the database. Position specified by $eM->idx.
  *  $createNew: Creates a new dataset and returns its index in the database.
  *
  * @param $eM  The LmElement to store.
  * @param $createNew  Shall a new element be created in the DB?
  * @return !$createNew: possible error
  *          $createNew: int The idx of the newly created element.
  */
  function setData( &$eM, $createNew = false ){
    global $usr, $cfg, $lng;

    $toSet = $this->createSetStringFromElement($eM);

    $element = $eM;
    $element->history = trim( $element->history[0] == ']' ?
                          substr( $element->history, 1 ) : // import... -> no stamping please
                          date("Y-m-d H:i:s", time()).' '.$usr->foreName.' '.$usr->surName.( $createNew ? '' : "\n".$element->history)
                        );
    // versioning
    if ($cfg->doesExist('versioning') && $cfg->get('versioning')=='1'){
      $versionedIdx = $this->myDBC->mkInsertReturnIDX( $toSet.
          "history='".$this->myDBC->escapeString( $element->history )."'",
          'bak_'.$this->myTable );
      $element->history = '@@'.$versionedIdx.' '.$element->history;
    }

    if ( $createNew )
      return $this->myDBC->mkInsertReturnIDX( $toSet.
                                              "history='".$this->myDBC->escapeString( $element->history )."'",
                                              $this->myTable );
    else
      return $this->myDBC->mkUpdIns( $toSet.
          'history=\''.$this->myDBC->escapeString($element->history)."'",
                                     $this->myTable,
                                     "idx=".$element->idx );
  }

  /**
  * Returns an Array( description => database key, ... ).
  * While database key is one of the keys the data can be sorted by getAllData( $orderBy ...
  * The first entry is the default soring key.
  * This is used for the order by selector on top of the manage page.
  *
  * @return array An array of pairs ( description, database key ).
  */
  function sortableByArray(){
    global $lng;
    return Array( $lng->get("index")            => "idx",
                  $lng->get("question")         => "question",
                  $lng->get("rights")           => "visible_for",
                  $lng->get("onlyInCollection") => "visible_only_in_collection"
                 );
  }
}
$GLOBALS["mDBI"] = new LmDBInterface();
