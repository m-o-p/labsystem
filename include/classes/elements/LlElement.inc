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
* implementation of the l (lab) element.
*
* This element consists of two collections and some visibility logic.
*
* @module     ../include/classes/elements/LiElement.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
require_once( INCLUDE_DIR."/classes/elements/Element.inc" );

// The following DBI connects UID and labstatus (prelabdone, points etc.)
require( INCLUDE_DIR."/classes/elements/LlDBInterfaceUidStatus.inc" );

class LlElement extends Element{
  var $uniqueID;              // Unique ID for export
  var $authors;               // The authors of the course
  var $comment;               // Comment to the lab (e.g. abstract of the course module)

  var $prelabCollectionIdx;   // Index of the prelab collection.
  var $labCollectionIdx;      // Index of the lab collection.
  
  var $noPrelab;              // Is there a prelab? (idx != 1)
  var $noLab;                 // Is there a lab? (idx != 1)
  
  var $visibleBefore1stSched; // Will the element be visible before the first schedule?
  var $visibleDuringSched;    // Will the element be visible while it is on schedule?
  var $visibleAfter1stSched;  // Will the element be visible after the first schedule?
  
  var $usDBI;                 // UidStatusDataBaseInterface.
  var $preLabIsFinished;      // usDBI.Has the user finished the prelab?
  var $preLabAllTeamFinished; // usDBI.Have all Teammembers finished the prelab?
  var $labIsClosed;           // usDBI.Is the lab closed?
  var $labIsCorrected;        // usDBI.Is the lab corrected
  
  var $isBefore1stSched;      // Is currently before 1st schedule.
  var $isInSched;             // Is currently inside a schedule.
  var $isAfter1stSched;       // Is currently after 1st schedule.
    
  var $preLab;                // Instance of prelab collection.
  var $preLabIsVisible2Me;    // Is $prelab visible to me?
  var $lab;                   // Instance of lab collection.
  var $labIsVisible2Me;       // Is $lab visible to me?
  
  var $userIsViewing;         // is a user or a corrector or example solution viewer watching?
  
  /**
  * The constructor.
  *
  * @param $idx                       This instance's index.
  * @param $uniqueID;                 Unique ID for remote synchronisation.
  * @param $authors;                  The authors of the course.
  * @param $comment;                  Comment to the lab (e.g. abstract of the course module).
  * @param $title                     The lab's title.
  * @param $prelabCollectionIdx       Index of the prelab collection.
  * @param $labCollectionIdx          Index of the lab collection.
  * @param $matchingMenu              The title of the menu that gets generated/ is highlighted to the element.
  * @param $visibleBefore1stSched     Is the element visible before the first schedule?
  * @param $visibleDuringSched        Is the element be visible while it is on schedule?
  * @param $visibleAfter1stSched      Is the element be visible after the first schedule?
  * @param $visibleOnlyInCollection   Is the element visible only inside a collection.
  * @param $history                   This' history.
  * @param $isInColl                  Am I inside a collection? (important for visibility aspects).
  */ 
  function LlElement( $idx, $uniqueID, $title, $authors, $comment, $prelabCollectionIdx, $labCollectionIdx, $matchingMenu, $visibleBefore1stSched, $visibleDuringSched, $visibleAfter1stSched, $visibleOnlyInCollection, $history, $isInColl=false ){
    global $CDBI, $usr, $sDBI, $cfg;
    $this->elementId                = "l";
    
    $this->idx                      = $idx;
    $this->uniqueID                 = $uniqueID;
    
    $this->title                    = $title;
    $this->authors                  = $authors;
    $this->comment                  = $comment;
    $this->prelabCollectionIdx      = $prelabCollectionIdx;
    $this->labCollectionIdx         = $labCollectionIdx;
    $this->matchingMenu             = $matchingMenu;
    $this->visibleBefore1stSched    = $visibleBefore1stSched;
    $this->visibleDuringSched       = $visibleDuringSched;
    $this->visibleAfter1stSched     = $visibleAfter1stSched;
    $this->visibleOnlyInCollection  = $visibleOnlyInCollection;
    $this->history                  = $history;

    // no prelab or no lab?
    $this->noPrelab                 = ($this->prelabCollectionIdx == 1);
    $this->noLab                    = ($this->labCollectionIdx == 1);
        
    // data from status database $usDBI
    $this->usDBI                    = new LlDBInterfaceUidStatus( $this->idx );

    if ( $usr->isSeeingSomeonesData() && 
         ( $usr->theSeeingUid() != "all" ) ) $permissions = $this->usDBI->getData4( $usr->theSeeingUid() ); // load user's data
                                        else $permissions = $this->usDBI->getData4( $usr->uid ); // own data
                                        
    $this->preLabIsFinished         = $permissions['preLabFinished'];
    $this->preLabAllTeammFinished   = $permissions['preLabAllTeammFinished'];
    $this->labIsClosed              = $permissions['labClosed'];
    $this->labIsCorrected           = $permissions['labCorrected'];
    
    $this->userIsViewing            = !($usr->isOfKind( IS_CORRECTOR ) || $usr->isOfKind( IS_EX_SOLUTION_VIEWER ) ); // is a user viewing or a corrector or example solution viewer watching?

    if ($this->noPrelab && !$this->preLabIsFinished && $this->userIsViewing) $this->createPrelabFinishedStatus( $usr->currentTeam );
    if ($this->noPrelab) $this->preLabIsFinished = $this->preLabAllTeammFinished = true;
    
    // get current schedule information
    $scheduleData = LsElement::getVisibility( "l", $this->idx );
    $this->isBefore1stSched = $scheduleData["isBefore1stSched"];
    $this->isInSched        = $scheduleData["isInSched"];
    $this->isAfter1stSched  = $scheduleData["isAfter1stSched"];
    
    $visiblePerSchedule             = LsElement::visiblePerSchedule( "l", $this->idx, $this->visibleBefore1stSched, $this->visibleDuringSched, $this->visibleAfter1stSched );
    
    if ( $visiblePerSchedule || !$this->userIsViewing ){
      $this->preLab                   = $CDBI->getData2idx( $this->prelabCollectionIdx, true, true );
      $this->preLabIsVisible2Me       = $this->preLab && $this->preLab->isVisible() && !$this->noPrelab;
      if ( $this->userIsViewing && 
          !$this->preLabIsVisible2Me &&
          !$this->preLabAllTeammFinished ) $this->lab = false; // not visible
                                      else $this->lab = $CDBI->getData2idx( $this->labCollectionIdx, true, true );
        
      $this->labIsVisible2Me          = ( $this->preLabAllTeammFinished || // normal user's chance
                                          !$this->userIsViewing
                                         ) && $this->lab && $this->lab->isVisible() && !$this->noLab;

      // Close inputs?
      // If 1st schedule is up and there is no running schedule: Close all lab questions! (affects only user!)
      // To reenable lab answering: First make a schedule! (Then the link will appear)
      if ( $usr->isOfKind( IS_USER ) && // makes no sense for not logged in users!
           $this->preLabAllTeammFinished &&                                         // are data there?
           !$this->labIsClosed &&
           $this->isAfter1stSched && !$this->isInSched &&
           ($permissions['currentTeam'] != '') // is there a record for this lab?
          ) $this->reallyCloseAllLabInputs( $permissions['currentTeam'] ); // use the teamnumber from the state that time as the team might have changed in between...
          
    }else{ // time consuming loading is not necessary since it is not visible anyway...
      $this->preLab                   = false;
      $this->preLabIsVisible2Me       = false;
      $this->lab                      = false;
      $this->labIsVisible2Me          = false;
    }
    
    $this->IamVisible               = $usr->isOfKind( IS_USER ) && // makes no sense for not logged in users!
                                      ( $this->preLabIsVisible2Me || $this->labIsVisible2Me ) && // is anything visible?
                                      ( !$this->userIsViewing || $visiblePerSchedule ) &&  // is it visible by a schedule?
                                      ( !$this->visibleOnlyInCollection || ($this->visibleOnlyInCollection && $isInColl) ); // in collection restriction fulfilled?
    $this->serializationArray       = array( 'idx', 'uniqueID', 'title', 'authors', 'comment', 'prelabCollectionIdx', 'labCollectionIdx', 'matchingMenu', 'visibleBefore1stSched', 'visibleDuringSched', 'visibleAfter1stSched', 'visibleOnlyInCollection', 'history' );  // see Element.inc for details
  }
  
// The next functions are used for the element's view page. *************************************************

// helper (if this exists some time they should be declared private)  - - - - - - - - - - - - - - - - - - - 
  /**
  * Shows the lab's TableOfContents.
  *
  * This function is non-recursive.
  *
  * @param $fullAddress       The element's full address (p.e. C42.l23)
  * @param $extParagraph      The element's paragraph number (p.e. 2.3). Will be added to the title if given.
  *
  * @return string            The html representation of the TOC.
  */
  function showTOC( $fullAddress, $extParagraph ){
    global $lng;

    $myRet = '<div class="labsys_mop_l_comment">'."\n".
             returnHTML( $this->comment ).
             "</div>\n";
    if ( $this->preLabIsVisible2Me ) $myRet .= $this->preLab->show( $fullAddress.".C".$this->prelabCollectionIdx, ( $lng->get("lPrefixPreLab") != "" ?  $extParagraph.".".$lng->get("lPrefixPreLab")  :  $extParagraph  ) );
    if ( $this->labIsVisible2Me ) $myRet .= $this->lab->show( $fullAddress.".C".$this->labCollectionIdx, ( $lng->get("lPrefixLab") != "" ?  $extParagraph.".".$lng->get("lPrefixLab")  :  $extParagraph  ) );
    return $myRet;
  }
  
  /**
  * Displays all multiple choice questions of the prelab and (if not done) the check button.
  *
  * @param $fullAddress       The element's full address (p.e. l23.C42)
  *
  * @return string            The html representation of all prelab mcs.
  */
  function showAllPrelabMultipleChoices( $fullAddress ){
    global $lng, $pge, $url, $cfg, $usr;
    $myRetVal =  $this->preLab->showAllElementsOfType( "m", $fullAddress.".C".$this->prelabCollectionIdx, $lng->get("lPrefixPreLab") );

    if ( !$this->preLabIsFinished && $this->userIsViewing ) // the "check prelab" button
      $myRetVal .=  "<FORM class=\"labsys_mop_std_form\" NAME=\"checkPrelab\" METHOD=\"POST\" ACTION=\"".$url->link2("../php/executeCommandOnElement.php?address=l".$this->idx)."\">\n".
                    "<input type=\"hidden\" name=\"FUNCTIONNAME\" value=\"checkPreLab()\">\n".  // this function will statically be called by ../php/executeCommandOnElement.php
                    "<input type=\"hidden\" name=\"IDX\" value=\"".$this->idx."\">\n".            // needed by ../php/executeCommandOnElement.php
                    "<input type=\"hidden\" name=\"SESSION_ID\" value=\"".session_id()."\">\n". // to check whether the call is valid
                    "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/view.php?address=".$url->get('address')."\">\n".
                    "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button_fullwidth\" ".
                     "value=\"".$lng->get("checkPrelab")."\">\n".  
                    "</FORM>\n".
                    ( $lng->get("prelabCheckNote") != "" ? "<div class=\"labsys_mop_note\">\n".$lng->get("prelabCheckNote").$cfg->get("MCAnsweringAttempts")."</div>\n" : '');

    return $myRetVal;
  }
  
  /**
  * Displays all inputs of the lab and (if not closed) the check button.
  *
  * @param $fullAddress       The element's full address (p.e. l23.C42)
  * @param $numPrefix         Numerical prefix that is put before the title (p.e. paragraph number).
  *
  * @return string            The html representation of all prelab mcs.
  */
  function showAllLabInputs( $fullAddress, $extParagraph="" ){
    global $lng, $pge, $url, $cfg, $usr;
    
    $observedUID = $usr->uid;
    if ( $usr->isSeeingSomeonesData() && ( $usr->theSeeingUid() != "all" ) )
      $observedUID = $usr->theSeeingUid();
       
    $myRetVal  = "";
    if ( $this->labIsClosed ) $myRetVal .= $this->labStatusUID( $observedUID )."<div>&nbsp;</div>\n";

    if ( !$this->labIsClosed && $this->userIsViewing )
       $myRetVal .= "<FORM class=\"labsys_mop_std_form\" NAME=\"closeLab\" METHOD=\"POST\" ACTION=\"".
                    $url->link2("../php/executeCommandOnElement.php?address=l".$this->idx).
                    "\">\n".
                    "<input type=\"hidden\" name=\"FUNCTIONNAME\" value=\"closeLabInputs()\">\n". // this function will statically be called by ../php/executeCommandOnElement.php
                    "<input type=\"hidden\" name=\"IDX\" value=\"".$this->idx."\">\n".            // needed by ../php/executeCommandOnElement.php
                    "<input type=\"hidden\" name=\"SESSION_ID\" value=\"".session_id()."\">\n".   // to check whether the call is valid
                    "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/view.php?address=".$url->get('address')."\">\n".
                    "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button_fullwidth\" ".
                           "value=\"".$lng->get("closeLab")."\">\n".  
                    "</FORM>\n".
                    ( $lng->get("labCloseNote")!="" ? "<div class=\"labsys_mop_note\">\n".$lng->get("labCloseNote")."</div>\n" : '');

   // The following is a slightly modified ( add amount of questions and their possibleCredits) of LcElement::showAllElementsOfType();   
    $parameter = Array( 'addr' => $fullAddress.".C".$this->labCollectionIdx, 
                        'paragraph' => $extParagraph, 
                        'myRetVal' => '', 
                        'count' => 0, 
                        'sumPossibleCredits' => 0,
                        'alreadyInSum' => Array() ); // some inputs might be more than one time inside the collection...
                                                     // p.e. I use an input for data that are reshown on a later page for comparison
    $functionCode = "
    \$elementAddr = ( \$parameter['addr'] != '' ?  \$parameter['addr'].'.'  : '' ).\$value;
    \$elParagraph = ( \$parameter['paragraph'] != '' ?  \$parameter['paragraph'].'.'  : '' ).\$this->getParagraph( \$value );

    if ( \$element->elementId == 'i'){
      \$parameter['myRetVal'] .= \$element->show( \$elementAddr, \$elParagraph ); // If you pout this into the parantheses only the first appearance of an input is shown !
      if ( !(in_array( \$value, \$parameter['alreadyInSum'] )) ){
        array_push( \$parameter['alreadyInSum'], \$value ); // no double count
        \$parameter['count']++;
        \$parameter['sumPossibleCredits'] += \$element->possibleCredits;
      }
    }elseif ( (\$element->elementId == 'c') || (\$element->elementId == 'C') ){
      // recursion
      \$newParam = Array( 'addr' => \$elementAddr,
                          'paragraph' => \$elParagraph, 
                          'myRetVal' => '', 
                          'count' => 0, 
                          'sumPossibleCredits' => 0,
                          'alreadyInSum' => \$parameter['alreadyInSum'] ); // no double count
      \$element->extMap( \$functionCode, \$newParam );
      \$parameter['alreadyInSum'] = \$newParam['alreadyInSum'];
      \$parameter['count'] += \$newParam['count'];
      \$parameter['sumPossibleCredits'] += \$newParam['sumPossibleCredits'];
      if ( \$newParam['myRetVal'] != '' ) 
        \$parameter['myRetVal'] .=  \"<div class=\\\"labsys_mop_contentArea\\\">\\n\".
                                    // header inclusive link to the enclosing element (this)
                                    '<div class=\"labsys_mop_h2\">'.( \$elParagraph != '' ?  \$elParagraph.'. '  : '' ).\$element->title.' '.EB::link2Element( \$elementAddr ).\"</div>\\n\".
                                    \"</div>\\n\".
                                    \$newParam['myRetVal'];
    }";
    
    $this->lab->extMap( $functionCode, $parameter );
    
    return "<div class=\"labsys_mop_l_all_inputs_stats\">\n".
           "[".$parameter['count']." ".$lng->get("inputs")."; ".$parameter['sumPossibleCredits']." ".$lng->get("Credits")."]".
           "</div>\n".
           $myRetVal.
           $parameter['myRetVal'];
  }
  
  /**
  * Returns the html representation of the lab all user's status table.
  *
  * @param $orderBy       The $this->usDBI->sortableByArray()'s sorter entry the rows should be ordered by.
  *
  * @return string        The html code of the status table.
  */
  function labStatusTable( $orderBy="" ){
    global $pge, $lng, $usr;
    
    $sortArray = $this->usDBI->sortableByArray();
    require( "../pages/sorter.inc" ); // -> $asc, $orderBy
    $this->usDBI->getAllData( $orderBy, $asc );
    
    return $sorter.$this->showTableResults( $this->labIsCorrected || $usr->isOfKind( IS_CORRECTOR ) );
  }
  
  /**
  * Returns the html representation of one user's status table (row) for this lab.
  *
  * @param $uid     The user's uid.
  *
  * @return string  Html representation of one user's status table (row) for this lab.
  */
  function labStatusUID( $uid ){
    $this->usDBI->getAllUidData( $uid );

    return $this->showTableResults();
  }
  
  /**
  * Returns the html representation of one user's status table for ALL labs.
  *
  * @param $uid     The user's uid.
  * @param $compact Leave the details (points of each lab) out?
  *
  * @return string  Html representation of one user's status table for ALL labs.
  */
  function labStatusUIDALLLabs( $uid, $compact=false ){
    $this->usDBI->getAllUidDataALLLabs( $uid );
    
    return $this->showTableResults( true, $compact );
  }
   
  /**
  * Called by showTableResults to show the percentage bar below the percent in the last row.
  * The percentage is shown via bar length and color
  * (via $cfg->percentageFailed = red -> 100% green)
  *
  * @param $percentage       The value to be shown out of [0..1].
  * @param $percentageFailed The value the bar gets red if below.
  *
  * @return string      The bar's html-code.
  */
  function percentBar( $percentage, $percentageFailed ){
    // $p2 = $percentage - $percentageFailed mapped on [0..1] for coloring.
    // The color varies from #ff0000 ( $p2 =   0% ) (red) 
    //                  over #ffff99 ( $p2 =  50% ) (yellow)
    //                    to #00ff00 ( $p2 = 100% ) (green)
    $p2 = ( $percentage - $percentageFailed )/
          ( 1 - $percentageFailed );
        if ( $p2 < 0 ) $p2 = 0;
    elseif ( $p2 > 1 ) $p2 = 1;
    $move = $p2 * 511;
    if ( $move < 255 ){ // move to yellow
      $moveRed   = 0;
      $moveGreen = $move;
    }else{ // move to green
      $moveRed   = $move-256;
      $moveGreen = 255;
    }
    
    $color = substr( '0'.dechex( 255-$moveRed ), -2 ).substr( '0'.dechex( $moveGreen ), -2 ).'00';
    
    return "<div class=\"labsys_mop_l_status_percentBarDiv\">".
           "<img src=\"../syspix/empty.gif\" alt='".round( $percentage*100, 2 )."%' style=\"color: #".$color."; background-color: #".$color."; width: ".round($percentage*100)."%\"></img>".
           "</div>\n";
  }
  
  /**
  * Called by the methods above. Creates the table based on the last $this->usDBI->getAllData query.
  *
  * @param $showSummation   Should the points be summated and a summary be added as last row?
  * @param $onlySummation   Only show the summation.
  *
  * @return string          The html code of the query result's table.
  */
  function showTableResults( $showSummation=false, $onlySummation=false ){
    global $usr, $lng, $url, $pge, $cfg, $lDBI;
    
   // The different learning units might are given only by index.
   // So for the human being I put the name (besides the index that 
   // appears at the link (first row #)).
   // Since it has to be queried from the DB I cache the values for
   // later use (on demand not precaching for performance).
    $labNameCache = array();
   // The additional row will only be displayed for new labs.
    $lastLabIndex = -1;

   // to output one's name only once (summary page) 
    $previousMembersname = "";
    
    if ($onlySummation) $showSummation=true;
    
    $myRetVal  = "";
    if ( $lng->get("labStatusNote") != "" )
      $myRetVal .= "<div class=\"labsys_mop_note\">\n".$lng->get("labStatusNote")."</div>\n";
   
   // 8 rows; if changed change below at the lab's name row!
    $myRetVal .= "<table class=\"labsys_mop_l_status_table\">\n".
                 "<tr class=\"labsys_mop_l_status_table_row_light\">".
                     "<td class=\"labsys_mop_l_status_table_cell_number\">#</td>".
                     "<td class=\"labsys_mop_l_status_table_cell_team\">".$lng->get("team")."</td>".
                     "<td class=\"labsys_mop_l_status_table_cell_name\"></td>".
                     "<td class=\"labsys_mop_l_status_table_cell_lab_prelab_credits\"><i>m</i>-".$lng->get( 'Credits' )."</td>".
                     "<td class=\"labsys_mop_l_status_table_cell_prelab_fin\"></td>".
                     "<td class=\"labsys_mop_l_status_table_cell_lab_closed\"></td>".
                     "<td class=\"labsys_mop_l_status_table_cell_lab_corr\"></td>".
                     "<td class=\"labsys_mop_l_status_table_cell_lab_credits\"><i>i</i>-".$lng->get( 'Credits' )."</td>".
                 "</tr>";
    
    if ( $usr->isOfKind(IS_USER_RIGHTS_EDITOR) ) // user->team mapping possible
      require_once( INCLUDE_DIR."/classes/DBInterfaceUserRights.inc" );

    $i = 0;
   // If $showSummation is set the credits are summed up and a row is added at the table's bottom with that summary.
    if ( $showSummation ){
      $sumGivenCreditsPreLab = 0; 
      $sumPossibleCreditsPreLab = 0;
      $summandsPreLab = 0;
      
      $sumGivenCreditsLab = 0; 
      $sumPossibleCreditsLab = 0;
      $summandsLab = 0;
    }
    
    
    while( $labStatus = $this->usDBI->getNextData() ){
      if ( $showSummation ){
        if ( $labStatus['preLabFinished'] &&
             ($labStatus['preLabPossibleCredits'] > 0)
            ){
                              $sumGivenCreditsPreLab += $labStatus['preLabGivenCredits']; 
                              $sumPossibleCreditsPreLab += $labStatus['preLabPossibleCredits'];
                              $summandsPreLab++;
        }
        if ( $labStatus['labCorrected'] &&
             ($labStatus['labPossibleCredits'] > 0)
            ){
                              $sumGivenCreditsLab += $labStatus['labGivenCredits']; 
                              $sumPossibleCreditsLab += $labStatus['labPossibleCredits'];
                              $summandsLab++;
              }
        }
      
      if (!$onlySummation){ // onlyShowSummation
       // Get lab's human readible name:
        $labName = '';
        if ( $labStatus['lIdx'] != $lastLabIndex ){
          $lastLabIndex = $labStatus['lIdx'];
         // Cachemiss?
          if ( !array_key_exists( $labStatus['lIdx'], $labNameCache ) ){
            $element = $lDBI->getMenuData2idx( $labStatus['lIdx'] );
            $labNameCache += array( $labStatus['lIdx'] => $element->title );
          }
            $labName = '<span class="labsys_mop_grayed"> &nbsp;'.$labNameCache[ $labStatus['lIdx'] ].'</span>';
        }
        
        $userData = $usr->getUserInformation( $labStatus['uid'] );
        
        $myRetVal .= "<tr class=\"labsys_mop_l_status_table_row_".( ++$i%2 == 0 ?  "light" :  "dark"  )."\">".
                     "<td class=\"labsys_mop_l_status_table_cell_number\">".
                        '<a href="'.$url->link2( '../pages/view.php?address=l'.$labStatus['lIdx'] ).'">'.$i.'</a>'.
                     "</td>".
                     "<td class=\"labsys_mop_l_status_table_cell_team\">".
                        $labStatus['currentTeam'];

       // user->team mapping possible
        if ( $usr->isOfKind(IS_USER_RIGHTS_EDITOR) &&
             ( $labStatus['lIdx'] == $this->idx ) // functionality only available for the current lab
            ){
          $urDBI = new DBInterfaceUserRights();
          $ur = $urDBI->getData4( $labStatus['uid'] );
          $myRetVal .= " <a href=\"".$url->link2( "../php/executeOnLab.php?address=l".$this->idx."&function=reMapUidTeam&param=".urlencode( "\"".$labStatus['uid']."\", ".$ur['currentTeam'] )."&redirectTo=".urlencode( "../pages/view.php?address=".$url->get('address') )  )."\">".
                       "<img src=\"../syspix/reMapUsr_26x12.gif\" width=\"26\" height=\"12\" border=\"0\" title=\"".$lng->get("reMapUsrTo")." ".$ur['currentTeam']."\">".
                       "</a>";
        }

        $myRetVal .= "</td>".
                  // member's name
                     "<td class=\"labsys_mop_l_status_table_cell_name\">".
                     ( $usr->isOfKind( IS_CORRECTOR ) ? "<a href=\"".$url->link2("../php/executeOnLab.php?address=l".$this->idx."&function=setSeeingUID&param=".urlencode( '"'.$labStatus['uid'].'"' )."&redirectTo=".urlencode($_SERVER['REQUEST_URI']))."\">" : '').
                     ( ($previousMembersname != $userData["name"].", ".$userData["foreName"]) ? $userData["name"].", ".$userData["foreName"]." " : '').
                     ( $usr->isOfKind( IS_CORRECTOR ) ?  "</a>"  : '' ).
                     EB::history( $userData["name"].", ".$userData["foreName"]." (".$lng->get("team")." ".$labStatus['currentTeam'].")".":<br>\n".$labStatus['history'], "l".$this->idx, true ).
                     $labName."\n";

        $previousMembersname = $userData["name"].", ".$userData["foreName"]; // to output one's name only once (summary page)
        
       // show which are corrected
        if ( $labStatus['labClosed'] && !$labStatus['labCorrected'] && ($labStatus['whichCorrected'] != "") &&
             ( ($labStatus['uid'] == $usr->uid) || $usr->isOfKind( IS_CORRECTOR ) )
            ){
          $myRetVal .= "<div class=\"labsys_mop_l_status_whichCorrected\">\n";
          $correctedString = $labStatus['whichCorrected'];
          for( $qn=0; $qn < strlen( $correctedString ); $qn++){
            $myRetVal .= "<img src=\"../syspix/labCorrected_".
                         ( $correctedString[$qn] == "t" ?  "yes" :  "no"  ).
                         "_12x12.gif\" border=\"0\" title=\"".($qn+1)." ".$lng->get("corrected")."\">";
          }
          $myRetVal .= "</div>\n";
        }
  
        $myRetVal .= "</td>".
                     
                  // prelab credits and percentage bar
                     "<td class=\"labsys_mop_l_status_table_cell_lab_prelab_credits\">";
                      if (  ( ($labStatus['preLabFinished'] && $labStatus['uid'] == $usr->uid) || $usr->isOfKind( IS_CORRECTOR) ) && 
                            $labStatus['preLabPossibleCredits'] > 0
                           ){
                              $percentage = $labStatus['preLabGivenCredits']/$labStatus['preLabPossibleCredits'];
                              $myRetVal .= round($percentage*100, 0)."%\n".
                                           $this->percentBar( $percentage, $cfg->get("percentageFailedPreLab") ).
                                           '<span class="labsys_mop_grayed">'.$labStatus['preLabGivenCredits'].'/ '.$labStatus['preLabPossibleCredits']."</span>\n";
                             }
        $myRetVal .= "</td>".

                  // all teammembers finished prelab?
                     "<td class=\"labsys_mop_l_status_table_cell_prelab_fin\">".
                     " <img src=\"../syspix/prelabFin_".
                     ( $labStatus['preLabAllTeammFinished'] ?  "yes" :  "no"  ).
                     "_15x12.gif\" border=\"0\" title=\"".$lng->get("exAllowedToAnswer")."\">".
                     "</td>".
                     
                     "<td class=\"labsys_mop_l_status_table_cell_lab_closed\">".
                  // reopen lab
                     ( $open = ($usr->isOfKind( IS_CORRECTOR ) && 
                                         ( $labStatus['lIdx'] == $this->idx ) && // functionality only available for the current lab
                                         $labStatus['labClosed'] && 
                                         $this->isInSched
                                        ) ?
                                "<a href=\"".$url->link2( "../php/executeOnLab.php?address=l".$this->idx."&function=reOpenAllLabInputs&param=".$labStatus['currentTeam']."&redirectTo=".urlencode( "../pages/view.php?address=".$url->get('address') )  )."\">" 
                                        : '' ).
                  // close lab
                     ( !$open && ($open = ($usr->isOfKind( IS_CORRECTOR ) && 
                                                    ( $labStatus['lIdx'] == $this->idx ) && // functionality only available for the current lab
                                                    !$labStatus['labClosed'] /* && 
                                                    !$this->isInSched always allow closing */)
                                                   ) ?
                                "<a href=\"".$url->link2( "../php/executeOnLab.php?address=l".$this->idx."&function=closeAllLabInputs&param=".$labStatus['currentTeam']."&redirectTo=".urlencode( "../pages/view.php?address=".$url->get('address') )  )."\">" 
                                                   : '' ).
                     " <img src=\"../syspix/button_lock_".
                     ( $labStatus['labClosed'] ?  "closed" :  "open"  ).
                     "_13x12.gif\" border=\"0\" title=\"".$lng->get("exLabClosed")."\">".
                     ( $open ? "</a>" : '' ).
                     "</td>".
                     
                  // is corrected?
                     "<td class=\"labsys_mop_l_status_table_cell_lab_corr\">".
                     " <img src=\"../syspix/labCorrected_".
                     ( $labStatus['labCorrected'] ?  "yes" :  "no"  ).
                     "_12x12.gif\" border=\"0\" title=\"".$lng->get("exLabCorrected")."\">".
                     "</td>".

                  // credits and percentage bar
                     "<td class=\"labsys_mop_l_status_table_cell_lab_credits\">";

                     if ( ( ($labStatus['labCorrected'] && $labStatus['uid'] == $usr->uid) || $usr->isOfKind( IS_CORRECTOR) ) && 
                          $labStatus['labPossibleCredits'] != 0 
                         ){ 
                            $percentage = $labStatus['labGivenCredits']/$labStatus['labPossibleCredits'];
                            $myRetVal .= round($percentage*100, 0)."%\n".
                                         $this->percentBar( $percentage, $cfg->get("percentageFailedLab") ).
                                         '<span class="labsys_mop_grayed">'.$labStatus['labGivenCredits']."/ ".$labStatus['labPossibleCredits']."</span>\n";
                          }
                          
                     $myRetVal .= "</td></tr>";
     } // /onlyShowSummation
     else $usersTeam = $labStatus['currentTeam'];
   } // /while
   // $showSummation? Show summary:
    if ( $showSummation || $usr->isOfKind( IS_CORRECTOR ) ){
      $myRetVal .=  "<tr class=\"labsys_mop_l_status_table_row_summary\">".
                     "<td class=\"labsys_mop_l_status_table_cell_number\"></td>".
                     "<td class=\"labsys_mop_l_status_table_cell_team\">";
      if ( $onlySummation && isset( $usersTeam ) ) $myRetVal .= $usersTeam; 
/* CSV Export of summarised results */ if ($GLOBALS['url']->available('exportCSV')) echo( ( isset($usersTeam) ? $usersTeam : '' )."\t" );
      $myRetVal .=  "</td>".
                     "<td class=\"labsys_mop_l_status_table_cell_name\">";
                     
    // add entry to correct all users
    if( $usr->isOfKind( IS_CORRECTOR ) && 
        ( !$usr->isSeeingSomeonesData() || ( $usr->theSeeingUid() != "all" ) ) )
      $myRetVal .=      "<a href=\"".$url->link2("../php/executeOnLab.php?address=l".$this->idx."&function=setSeeingUID&param=".urlencode( '"all"' )."&redirectTo=".urlencode($_SERVER['REQUEST_URI']))."\">". //urlencode("../pages/view.php?address=l".$this->idx.".labStatus")
                        $lng->get("showAllUsersAnswers").
                        "</a>";
                        
      $myRetVal .=   "</td>".
                     "<td class=\"labsys_mop_l_status_table_cell_lab_prelab_credits\">";
      if ( isset( $sumPossibleCreditsPreLab ) && 
           ($sumPossibleCreditsPreLab > 0) &&
           ( ($summandsPreLab > 3) || $onlySummation ) // otherwise you see the points of one user...
          ){
        $percentage = $sumGivenCreditsPreLab/$sumPossibleCreditsPreLab;
            $myRetVal  .= round($percentage*100, 2)."%\n";
            $myRetVal  .= $this->percentBar( $percentage, $cfg->get("percentageFailedPreLab") );
            $myRetVal  .= '<span class="labsys_mop_grayed">'.
                          round( $sumGivenCreditsPreLab/ $summandsPreLab, 2 )."/ ".
                          round( $sumPossibleCreditsPreLab/ $summandsPreLab, 2 ).
                          "</span>\n";
/* CSV Export of summarised results */ if ($GLOBALS['url']->available('exportCSV')) echo( round($percentage*100, 2)."%\t" );
      }
      $myRetVal .=   "</td>".
                     "<td class=\"labsys_mop_l_status_table_cell_prelab_fin\"></td>".
                     "<td class=\"labsys_mop_l_status_table_cell_lab_closed\"></td>".
                     "<td class=\"labsys_mop_l_status_table_cell_lab_corr\"></td>".
                     "<td class=\"labsys_mop_l_status_table_cell_lab_credits\">";
      if ( isset( $sumPossibleCreditsLab ) && 
           ($sumPossibleCreditsLab > 0) &&
           ( ($summandsLab > 3) || $onlySummation ) // otherwise you see the points of one user...
          ){
        $percentage = $sumGivenCreditsLab/$sumPossibleCreditsLab;
            $myRetVal  .= round($percentage*100, 2)."%\n";
            $myRetVal  .= $this->percentBar( $percentage, $cfg->get("percentageFailedLab") );
            $myRetVal  .= '<span class="labsys_mop_grayed">'.
                          round( $sumGivenCreditsLab/ $summandsLab, 2 )."/ ".
                          round( $sumPossibleCreditsLab/ $summandsLab, 2 ).
                          "</span>\n";
/* CSV Export of summarised results */ if ($GLOBALS['url']->available('exportCSV')) echo( round($percentage*100, 2)."%" );
      }

      $myRetVal .=   "</td></tr>\n";
    }
    
    $myRetVal .= "</table>\n";

    return $myRetVal;
  }
  
  /**
  * Returns the legend to the TableResults.
  *
  * @return string  The legend's html code.
  */
  function getTableResultsLegend(){
    global $usr, $lng;

    $myRetVal = "";
   // explain reOpen/ close the lab for a team
    if( $usr->isOfKind( IS_CORRECTOR ) ){
      $myRetVal .= "<div class=\"labsys_mop_note\">\n".
                     "<img src=\"../syspix/button_lock_".
                     ( $this->isInSched ?  "closed" :  "open"  ).
                     "_13x12.gif\" border=\"0\" title=\"".$lng->get("exLabClosed")."\">: ".
                     $lng->get("youCanClickOn").
                     " <img src=\"../syspix/button_lock_".
                     ( $this->isInSched ?  "closed" :  "open"  ).
                     "_13x12.gif\" border=\"0\" title=\"".$lng->get("exLabClosed")."\"> ".
                     ( $this->isInSched ?  $lng->get("toReOpenTheLab") :  $lng->get("toCloseTheLab" ) ).
                   "</div>\n";
    }
    
   // explain user->team mapping possible
    if ( $usr->isOfKind(IS_USER_RIGHTS_EDITOR) ){
        $myRetVal .= "<div class=\"labsys_mop_note\">\n".
                       "<img src=\"../syspix/reMapUsr_26x12.gif\" width=\"26\" height=\"12\" border=\"0\">: ".
                       $lng->get("reMapIconExplain").
                     "</div>\n";
    }
                      
   // Legend
    $myRetVal .= "<h4>".$lng->get("legend")."</h4>\n".
    
                 "<div class=\"labsys_mop_note\">\n".
                 "<img src=\"../syspix/prelabFin_no_15x12.gif\" border=\"0\" title=\"".$lng->get("exAllowedToAnswer")."\">: ".
                 $lng->get("prelabAllnoFExplain")."<br>\n".
                 "<img src=\"../syspix/prelabFin_yes_15x12.gif\" border=\"0\" title=\"".$lng->get("exAllowedToAnswer")."\">: ".
                 $lng->get("prelabAllFinExplain")."<br>\n".
                 "</div>\n".
                 
                 "<div class=\"labsys_mop_note\">\n".
                 "<img src=\"../syspix/button_lock_open_13x12.gif\" border=\"0\" title=\"".$lng->get("exLabClosed")."\">: ".
                 $lng->get("labOpenExplain")."<br>\n".
                 "<img src=\"../syspix/button_lock_closed_13x12.gif\" border=\"0\" title=\"".$lng->get("exLabClosed")."\">: ".
                 $lng->get("labClosedExplain")."<br>\n".
                 "</div>\n".
                 
                 "<div class=\"labsys_mop_note\">\n".
                 "<img src=\"../syspix/labCorrected_no_12x12.gif\" border=\"0\" title=\"".$lng->get("exLabCorrected")."\">: ".
                 $lng->get("labNoCorrExplain")."<br>\n".
                 "<img src=\"../syspix/labCorrected_yes_12x12.gif\" border=\"0\" title=\"".$lng->get("exLabCorrected")."\">: ".
                 $lng->get("labCorrExplain")."<br>\n".
                 "</div>\n";

    return $myRetVal;
  }
  
  /**
  * Produces the ePub version of the lab.
  * Called via show?address="l[idx].ePub"
  *
  * @param $fullAddress       The element's full address (p.e. C42.l23)
  * @param $extParagraph      The element's paragraph number (p.e. 2.3). Will be added to the title if given.
  */
  function showEPub( $fullAddress, $extParagraph ){
    global $lng;
    // Uses Grandt PHPePUB (https://github.com/Grandt/PHPePub)
    $pluginFile = '../plugins/Grandt-PHPePub/EPub.php';
    if (!file_exists( $pluginFile )){
          trigger_error( 'ePub Plugin "'.$pluginFile.'" missing.', E_USER_ERROR );
          exit;
         }
    include_once( $pluginFile );
    // The meta information of the ePub.
    
    // The title page.
    echo('ePub to '.
          '[l-Element '.$fullAddress.'] Title: '.$this->title.'<br>'.
          '[l-Element] Authors: '.$this->authors.'<br>'.
          '[l-Element] Abstract: '.$this->comment.'<br>'.
          '[l-Element] Language: '.$lng->get('Content-Language')
          // maybe create the TOC on the fly? At each C element...
          //"[l-Element] <!-- start of collection entity --><div class=\"labsys_mop_c_inner_coll_entity\">\n".
          //$this->showTOC( $fullAddress, $extParagraph ). // we need a ePub Version here.
          //"<!-- stop of collection entity --></div>\n"
         );
    // The content.
    echo( '<hr><hr>prelab<hr><hr><br>' );
    if ( $this->preLabIsVisible2Me )
      $this->preLab->show( $fullAddress.".C".$this->prelabCollectionIdx.".epub", ( $lng->get("lPrefixPreLab") != "" ?  $extParagraph.".".$lng->get("lPrefixPreLab")  :  $extParagraph  ) );
    echo( '<hr><hr>lab<hr><hr><br>' );
    if ( $this->labIsVisible2Me )
      $this->lab->show( $fullAddress.".C".$this->labCollectionIdx.".epub", ( $lng->get("lPrefixLab") != "" ?  $extParagraph.".".$lng->get("lPrefixLab")  :  $extParagraph  ) );
    exit;
  }
  
  /**
  * Shows the members lab status.
  *
  * @return string  The labstatus' html code.
  */
  function showLabStatus(){
    global $lng, $pge, $usr, $url;

    // The status table
    $myRetVal = $this->labStatusTable( "" );
    
    // link to all lab's status
    $myRetVal .= "<div class=\"labsys_mop_note\">".
                 "<a href=\"".$url->link2( "../pages/view.php?address=l".$this->idx.".ALLlabStatus" )."\">".$lng->get("statusOfAllLabs")."</a>".
                 "</div>";
                 
    
   // if seeing someone "stop seeing" button
    if( $usr->isSeeingSomeonesData() ) 
      $myRetVal .=  "<div class=\"labsys_mop_button_fullwidth\">\n".
                    "<a href=\"".$url->link2("../php/executeOnLab.php?address=l".$this->idx."&function=setSeeingUID&param=".urlencode( '""' )."&redirectTo=".urlencode("../pages/view.php?address=l".$this->idx.".labStatus"))."\">".
                    $lng->get("stopSeeingData").
                    "</a>".
                    "</div>\n";
   
   // update/ reCalculate button
    if ( $usr->isOfKind( IS_CORRECTOR ) )
      $myRetVal .= "<FORM class=\"labsys_mop_std_form\" NAME=\"reFresh\" METHOD=\"POST\" ACTION=\"".
                      $url->link2("../php/executeCommandOnElement.php?address=l".$this->idx).
                      "\">\n".
                      "<input type=\"hidden\" name=\"SESSION_ID\" value=\"".session_id()."\">\n".
                      "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/view.php?address=".$url->get('address')."\">\n".
                      "<input type=\"hidden\" name=\"IDX\" value=\"".$this->idx."\">\n".            // needed by ../php/executeCommandOnElement.php
                      "<input type=\"hidden\" name=\"FUNCTIONNAME\" value=\"updateStatus()\">\n".
                      "<input tabindex=\"".$pge->nextTab++."\" 
                               type=\"submit\" 
                               class=\"labsys_mop_button_fullwidth\" 
                               value=\"".$lng->get("updateStatus")."\">\n".  
                      "</FORM>\n";
                      
    $myRetVal .= $this->getTableResultsLegend();
                      
    return $myRetVal;
  }
  
  /**
  * Shows the members lab status.
  *
  * @param $compact Don't show the details (points of each lab)?
  * @return string  The html code of the user's (or all observed user's) status tables.
  */
  function showStatusALLLabs( $compact = false ){
    global $lng, $pge, $usr, $cfg, $url;

    $myRetVal = "";
    
   // additional note
    if ( $lng->get("NoteStatusALLLabs") != "" )
      $myRetVal .= "<div class=\"labsys_mop_note\">\n".$lng->get("NoteStatusALLLabs")."</div>\n";
      
   // link to switch mode to compact (no detailed points of each labs).
    $myRetVal .= "<div class=\"labsys_mop_note\">\n".
                 '<a href="'.$url->link2( '../pages/view.php?address=l'.$this->idx.'.ALLlabStatus'.( !$compact ?  'Compact' : '' ) ).'">'.
                 $lng->get('SwitchTo').' '.( $compact ?  $lng->get('StandardView') :  $lng->get('CompactView' ) ).
                 '</a>'.( $compact && $usr->isSeeingSomeonesData() && ($usr->theSeeingUid() == 'all') ? ' | <a href="'.$url->link2( $_SERVER['PHP_SELF'].'?address='.$url->get('address').'&exportCSV=true' ).'">exportCSV.txt</a>' : '' ).
                 "</div>\n";
      
    $observedUID = $usr->uid;
    if ( $usr->isSeeingSomeonesData() ) $observedUID = $usr->theSeeingUid();

   // The status table
    if ( $observedUID != "all" ){ // single user
      $myRetVal .= "<div class=\"labsys_mop_l_status_userhead\">";
      if ( $usr->isSeeingSomeonesData() )
        $myRetVal .= $usr->theSeeingDescr();
      else
        $myRetVal .= $usr->surName.', '.$usr->foreName;
      $myRetVal .= '</div>'.
                   $this->labStatusUIDALLLabs( $observedUID, $compact );
    }else{ // all users
/* CSV Export of summarised results */ if ($GLOBALS['url']->available('exportCSV')) {
     header('Content-type: text/x-csv');
     header('Content-Disposition: attachment; filename="labsystem_result_summary'.$GLOBALS['cfg']->get('User_courseID').'CSV.txt"');
     echo( "name\tforename\tteam\tprelabCredits\tlabCredits\r\n");
}
              /* the first entry is the default */
              require_once( INCLUDE_DIR."/classes/DBInterfaceUser.inc" );
              require_once( INCLUDE_DIR."/classes/DBInterfaceUserRights.inc" );
              $sortArray = array_merge ( DBInterfaceUserRights::sortableByArray(), DBInterfaceUser::sortableByArray() );

              /* sorting */ require( "../pages/sorter.inc" );
                            $myRetVal .= $sorter;
            
              /* Since the data have two sources, the external users and the internal user rights db
                 we have to distinguish! */
              if ( ( $orderBy == $cfg->get("UserDBField_name") ) || 
                   ( $orderBy == $cfg->get("UserDBField_forename") ) || 
                   ( $orderBy == $cfg->get("UserDBField_username") ) ){
                      /* $uDBI is the source */
                      $uidDBI = new DBInterfaceUser(); 
                      $hasName = true;
              }else{
                      /* $urDBI is source */
                      $uidDBI = new DBInterfaceUserRights();  
                      $hasName = false;
              }

              $uidDBI->getAllData( $orderBy, $asc );
              while( $userData = $uidDBI->getNextData() ){
                $myRetVal .= "<div class=\"labsys_mop_l_status_userhead\">";
                if ( $hasName ) $myRetVal .= $userData['name'].", ".$userData['foreName'];
                else{
                        $nameData = $usr->getUserInformation( $userData["uid"] );
                        $myRetVal .= $nameData["name"].", ".$nameData["foreName"];
/* CSV Export of summarised results */ if ($GLOBALS['url']->available('exportCSV')) echo( "\r\n".$nameData["name"]."\t".$nameData["foreName"]."\t" );
                     }
                $myRetVal .= "</div>";
                $myRetVal .= $this->labStatusUIDALLLabs( $userData["uid"], $compact )."<div>&nbsp;</div>\n";
              }
    }
    
    $myRetVal .= $this->getTableResultsLegend();
/* CSV Export of summarised results */ if ($GLOBALS['url']->available('exportCSV')) exit;
    return $myRetVal;
  }

// /helper - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  
  /**
  * Returns the element's HTML-representation in the way the VIEWER should see it.
  *
  * @param $_GET['noExSol'] If set the example solution is hidden.
  * @param $fullAddress     The element's full address (p.e. C23.c42.p13)
  * @param $extParagraph    The element's paragraph number (p.e. 2.3). Will be added to the title if given.
  *
  * @return string          The html code of the element's view view.
  */ 
  function show( $fullAddress, $extParagraph="" ){
    global $usr, $lng, $pge, $url, $cfg;
    if (  !$this->isVisible() && 
          !(substr($fullAddress, strrpos($fullAddress, '.')+1, strrpos($fullAddress, '.')+13) == 'ALLlabStatus') // the Labstatus should always be visible
         ){ /* not enough rights->no content */
      return "<div class=\"labsys_mop_note\">\n".$lng->get("TxtNotEnoughRights")."\n</div>";
      exit;
    }

   // Address splitting.
    $parentAddr = $fullAddress;
    $this->splitAddress( $parentAddr, $myAddrArray );
                                     $me        = $myAddrArray[0];
    if ( count( $myAddrArray ) > 1 ) $first     = $myAddrArray[1]; else $first = "";
    if ( count( $myAddrArray ) > 1 ) $last      = $myAddrArray[ count( $myAddrArray )-1 ]; else $last = "";
    if ( count( $myAddrArray ) > 1 ) $scndLast  = $myAddrArray[ count( $myAddrArray )-2 ]; else $scndLast = "";

    $myRetVal = "";
    
   // index
    if     ( $last == "" )   $myRetVal .= $this->showTOC( $parentAddr.$me, $extParagraph );
   // ePub
    elseif ( $last == "epub" &&
             $this->isVisible() )             $this->showEPub( $parentAddr.$me, $extParagraph );
   // all preLab questions
    elseif ( $last == "allPrelabQ" &&
             $scndLast == $me &&
             $this->preLabIsVisible2Me )      $myRetVal .= $this->showAllPrelabMultipleChoices( "l".$this->idx );
   // preLab status
    elseif ( $last == "preLabStatus" &&
             $scndLast == $me &&
             $this->preLabIsVisible2Me )      $myRetVal .= $this->showPrelabStatus( $extParagraph );
   // all lab questions
    elseif ( $last == "allLabQ" &&
             $scndLast == $me &&
             $this->labIsVisible2Me )         $myRetVal .= $this->showAllLabInputs( "l".$this->idx );
   // lab's status
    elseif ( $last == "labStatus" &&
             $scndLast == $me &&
             $this->isVisible() )             $myRetVal .= $this->showLabStatus( $extParagraph );
   // ALL lab's status
    elseif ( $last == "ALLlabStatus" &&
             $scndLast == $me ) {
                                              $myRetVal .= $this->showStatusALLLabs();
                                              $this->title = $lng->get('MnuEntryMyStatistics');
                                              
                                 }
   // ALL lab's status compact
    elseif ( $last == "ALLlabStatusCompact" &&
             $scndLast == $me ) {
                                              $myRetVal .= $this->showStatusALLLabs( true );
                                              $this->title = $lng->get('MnuEntryMyStatistics');
                                 }
   // all visible contents of the lab on one page
    elseif ( $last == "all" &&
             $scndLast == $me ){          
                                              $myRetVal .= "<!-- start of collection entity --><div class=\"labsys_mop_c_inner_coll_entity\">\n".
                                                           $this->showTOC( $parentAddr.$me, $extParagraph ).
                                                           "<!-- stop of collection entity --></div>\n";
                                            if ( $this->preLabIsVisible2Me )
                                              $myRetVal .= $this->preLab->show( $parentAddr.$me.".C".$this->prelabCollectionIdx.".all", ( $lng->get("lPrefixPreLab") != "" ?  $extParagraph.".".$lng->get("lPrefixPreLab")  :  $extParagraph  ) );
                                            if ( $this->labIsVisible2Me )
                                              $myRetVal .= $this->lab->show( $parentAddr.$me.".C".$this->labCollectionIdx.".all", ( $lng->get("lPrefixLab") != "" ?  $extParagraph.".".$lng->get("lPrefixLab")  :  $extParagraph  ) );
                                            $_GET['address'] = $parentAddr.$me;
                              }
                                              
   // preLab
    elseif ( $this->preLabIsVisible2Me && 
             ($first == "C".$this->prelabCollectionIdx) ) 
                                              $myRetVal .= $this->preLab->show( $fullAddress, ( $lng->get("lPrefixPreLab") != "" ?  $extParagraph.".".$lng->get("lPrefixPreLab")  :  $extParagraph  ) );
   // lab                                  
    elseif ( $this->labIsVisible2Me && 
             ($first == "C".$this->labCollectionIdx) ) 
                                              $myRetVal .= $this->lab->show( $fullAddress, ( $lng->get("lPrefixLab") != "" ?  $extParagraph.".".$lng->get("lPrefixLab") :  $extParagraph  ) );
                                              
    else { return "l".$this->idx.": ".$lng->get("elNotFoundVisible")." (".$fullAddress.") "; exit; }
        
    return EM::viewTopLab( $this, $parentAddr.$me ).
           "<div class=\"labsys_mop_h2\">".( $extParagraph != "" ?  $extParagraph.". "  : '' ).$this->title."</div>\n".
           ( ($this->authors != '') ? '<div class="labsys_mop_l_authors">'.returnHTML($this->authors).'</div>' : '' ).
           $myRetVal.
           EM::viewBottomLab( $this, $parentAddr.$me );
  }
  
  /**
  * Shows the lab's Menu.
  *
  * @param $linkFullAddress   Since the Menu might be part of another Collection the $fullAddress might have another
  *                           collection as root ( p.e. C7.C4.C[this->idx] ).
  * @param &$menu2Populate    The instance of the Menu.inc-class the entries should get in.
  * @param $prefix            If given this is put before the menu entry.
  * @param $unfold            Should the submenu-entries be visible or just the title-row?
  *
  * @return                   The html code of the menu.
  * 
  * This function is non-recursive.
  */
  function getMenu( $linkFullAddress, &$menu2Populate, $prefix="", $unfold=false ){
    global $lng, $url;
    
    if (!$this->isVisible()) return;
    
    $fullAddress = $url->get('address'); // to determine if active
    
    // lab's menu entry
    if ( !$unfold )
      $ret = $menu2Populate->entry( $prefix.$this->getMatchingMenu(), "../pages/view.php?address=".$linkFullAddress, ($linkFullAddress == $fullAddress) );
    else{
      $ret = $menu2Populate->startSubmenu( $prefix.$this->getMatchingMenu(), "../pages/view.php?address=".$linkFullAddress, ($linkFullAddress == $fullAddress) );
    
   // Address splitting.
    $parentAddr = $fullAddress;
    $this->splitAddress( $parentAddr, $myAddrArray );
                                     $me        = $myAddrArray[0];
    if ( count( $myAddrArray ) > 1 ) $first     = $myAddrArray[1]; else $first = "";
    if ( count( $myAddrArray ) > 1 ) $last      = $myAddrArray[ count( $myAddrArray )-1 ]; else $last = "";
    if ( count( $myAddrArray ) > 1 ) $scndLast  = $myAddrArray[ count( $myAddrArray )-2 ]; else $scndLast = "";
     
     // prelab's menu
      if ($this->preLabIsVisible2Me){
        // Is the menu active and should unfold?
        $unfold = ($first == "C".$this->prelabCollectionIdx) ||
                  ($last == "allPrelabQ" );
                  
        $ret .= $this->preLab->getMenu( $linkFullAddress.".C".$this->prelabCollectionIdx, 
                                        $menu2Populate, 
                                        $lng->get("lPrefixPreLab"),
                                        $unfold ); // unfold if active
       // additional menus
       // close/ all questions
        $ret .= $menu2Populate->entry( ( $this->preLabIsFinished || !$this->userIsViewing ?
                                                  $lng->get("mnuEntryPrelabFin") :
                                                  $lng->get("mnuEntryPrelab2Fin")
                                       ),
                                       "../pages/view.php?address=".$linkFullAddress.".allPrelabQ", 
                                       ( $last == "allPrelabQ" ) );
                                       
       if ( $unfold ){ // unfold if active
         // prelab status
          $ret .= $menu2Populate->entry( $lng->get("mnuEntryPreLabStats"), 
                                         "../pages/view.php?address=".$linkFullAddress.".labStatus&orderBy=".urlencode( $lng->get("orderPrelabFin") ), 
                                         ( $last == "labStatus" ) );
        }
      }
      
     // lab's menu
      if ($this->labIsVisible2Me){
        // Is the menu active and should unfold?
        $unfold = ($first == "C".$this->labCollectionIdx) ||
                  ($last == "allLabQ" );

        $ret .= $this->lab->getMenu( $linkFullAddress.".C".$this->labCollectionIdx, 
                                     $menu2Populate, 
                                     $lng->get("lPrefixLab"),
                                     $unfold );
       // additional menus
       // close lab/ all lab questions
        $ret .= $menu2Populate->entry( ( $this->labIsClosed || !$this->userIsViewing ?
                                                  $lng->get("mnuEntryLabFin") : 
                                                  $lng->get("mnuEntryLab2Fin")
                                       ),
                                       "../pages/view.php?address=".$linkFullAddress.".allLabQ", 
                                       ( $last == "allLabQ" ) );
       if ( $unfold ){ // unfold if active

       }
     }
         // lab status
          $ret .= $menu2Populate->entry( $lng->get("mnuEntryLabStatus"),
                                         "../pages/view.php?address=".$linkFullAddress.".labStatus", 
                                         ( $last == "labStatus" ) );
       // /submenu
    $ret .= $menu2Populate->stopSubmenu();
    }
    return $ret;
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
    
    // dropdown list for collections:
    $dropDownOptionsPreLab = '<option value="0">0: '.$lng->get( 'explainCreateNew' ).'</option>';
    $dropDownOptionsLab    = '<option value="0">0: '.$lng->get( 'explainCreateNew' ).'</option>';

    $id = "C";
    require( "../php/getDBIbyID.inc" ); // -> $DBI
    $DBI->getAllData( "idx", true ); // sorted by idx ascending
    while ( $element = $DBI->getNextData() ){
      $dropDownOptionsPreLab .= "<option value=\"".$element->idx."\"".
                                ( ($element->idx == $this->prelabCollectionIdx) ?  " selected=\"selected\"" : '' ).
                                ">".$element->idx.": ".( ($element->idx == 1) ?  '-' :  $element->title )."</option>\n";
      $dropDownOptionsLab    .= "<option value=\"".$element->idx."\"".
                                ( ($element->idx == $this->labCollectionIdx) ?  " selected=\"selected\"" : '' ).
                                ">".$element->idx.": ".( ($element->idx == 1) ?  '-' :  $element->title )."</option>\n";
    }
    
     return EM::editTopLab( $this, $fullAddress ).
            "<FORM class=\"labsys_mop_std_form\" NAME=\"labEdit\" METHOD=\"POST\" ACTION=\"".$url->link2("../php/executeCommandOnElement.php?address=l")."\">\n".
            "<input type=\"hidden\" name=\"FUNCTIONNAME\" value=\"save()\">\n".           // this function will statically be called by ../php/executeCommandOnElement.php
            "<input type=\"hidden\" name=\"SESSION_ID\" value=\"".session_id()."\">\n".   // to check whether the call is valid            "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/edit.php\">\n". // index of saved el. will be added on save.php!
            "<input type=\"hidden\" name=\"FULLADDRESS\" value=\"".$fullAddress."\">\n".
            "<input type=\"hidden\" name=\"IDX\" value=\"".$this->idx."\">\n".            // needed by ../php/executeCommandOnElement.php^
            "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/edit.php\">\n". /* index of saved el. will be added on save.php! */
            
           // title
            "<label for=\"title\" class=\"labsys_mop_input_field_label_top\">".$lng->get("inputTitle")."</label>".
            "<input tabindex=\"".$pge->nextTab++."\" type=\"text\" id=\"title\" name=\"TITLE\" class=\"labsys_mop_input_fullwidth\" maxlength=\"255\" value=\"".$this->title."\" onchange='isDirty=true'>\n".

           // prelab selector
            "<fieldset><legend>".$lng->get("inputPreLab")."</legend>\n".
              "<select tabindex=\"".$pge->nextTab++."\" name=\"PRELAB\" size=\"1\" class=\"labsys_mop_input\" onchange='isDirty=true'>\n".
              $dropDownOptionsPreLab.
              "</select>".
            "</fieldset>\n".
            
           // lab selector
            "<fieldset><legend>".$lng->get("inputLab")."</legend>\n".
              "<select tabindex=\"".$pge->nextTab++."\" name=\"LAB\" size=\"1\" class=\"labsys_mop_input\" onchange='isDirty=true'>\n".
              $dropDownOptionsLab.
              "</select>".
            "</fieldset>\n".
            
           // meta data
            "<fieldset><legend>".$lng->get("metaData")."</legend>\n".            
              // authors
              "<label for=\"authors\" class=\"labsys_mop_input_field_label_top\">".$lng->get('authors')."</label>".
              "<input tabindex=\"".$pge->nextTab++."\" type=\"text\" id=\"authors\" name=\"AUTHORS\" class=\"labsys_mop_input_fullwidth\" maxlength=\"255\" value=\"".$this->authors."\" onchange='isDirty=true'>\n".
                            
              // abstract
              "<label for=\"comment\" class=\"labsys_mop_input_field_label_top\">".$lng->get('abstract')."<br>\n".$lng->get('abstractNote')."</label>".
              "<textarea tabindex=\"".$pge->nextTab++."\" id=\"comment\" name=\"COMMENT\" class=\"labsys_mop_textarea\" rows=\"".$cfg->get("DefaultLCommentRows")."\" onchange='isDirty=true'>".
              returnEditable( $this->comment ).    // Process [HTML]. (../include/helper.inc).
              "</textarea>\n".             
            "</fieldset>\n".
                                             
           // save
            "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("save")."\" accesskey=\"s\" onclick='isDirty=false'>\n".
           
           // element properties
            "<fieldset><legend>".$lng->get("properties")."</legend>\n".
              "<label for=\"mM\" class=\"labsys_mop_input_field_label_top\">".$lng->get("inputMatchingMnu")."</label>".
              "<input tabindex=\"".$pge->nextTab++."\" type=\"text\" id=\"mM\" name=\"MATCHINGMENU\" class=\"labsys_mop_input_fullwidth\" maxlength=\"255\" value=\"".$this->matchingMenu."\" onchange='isDirty=true'>\n".

              "<table width=\"100%\"><tr><td valign=\"top\" width=\"33%\">\n".
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
              "</td><td valign=\"top\" width=\"67%\">\n".
                 // schedule
                  "<fieldset><legend>".$lng->get("schedule")."</legend>\n".
                    "<div>".
                      "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" id=\"visBef1stSched\" name=\"VISIBLE_BEFORE_FIRST_SCHED\" value=\"1\"".( $this->visibleBefore1stSched ?  " checked=\"checked\"" : '' )." onchange='isDirty=true'>".
                      "<label for=\"visBef1stSched\" class=\"labsys_mop_input_field_label\">".$lng->get("visibleBef1stSched")."</label>".
                    "</div>\n".
                    "<div>".
                      "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" id=\"visDuringSched\" name=\"VISIBLE_DURING_SCHED\" value=\"1\"".( $this->visibleDuringSched ?  " checked=\"checked\"" : '' )." onchange='isDirty=true'>".
                      "<label for=\"visDuringSched\" class=\"labsys_mop_input_field_label\">".$lng->get("visibleDuringSched")."</label>".
                    "</div>\n".
                    "<div>".
                      "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" id=\"visAft1stSched\" name=\"VISIBLE_AFTER_FIRST_SCHED\" value=\"1\"".( $this->visibleAfter1stSched ?  " checked=\"checked\"" : '' )." onchange='isDirty=true'>".
                      "<label for=\"visAft1stSched\" class=\"labsys_mop_input_field_label\">".$lng->get("visibleAftr1stSched")."</label>".
                    "</div>\n".
                  "</fieldset>\n".
              "</td></tr></table>\n".
            "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" id=\"saveAsNew\" name=\"SAVEASNEW\" value=\"1\"".( ( $GLOBALS['url']->available("saveAsNew") || ($this->idx == 1) ) ?   " checked=\"checked\""  : '' )." onchange='isDirty=true'>".
            "<label for=\"saveAsNew\" class=\"labsys_mop_input_field_label\">".$lng->get("saveAsNew")."</label>".
            "</fieldset>\n".
            
           // export/ import
            "<fieldset><legend>".$lng->get("exportImport")."</legend>\n".
              // unique ID
              "<label for=\"uniqueID\" class=\"labsys_mop_input_field_label_top\">".$lng->get('uniqueID')."<br>\r\n".$lng->get('uniqueIDnote')."</label>".
              "<input tabindex=\"".$pge->nextTab++."\" type=\"text\" id=\"uniqueID\" name=\"UNIQUEID\" class=\"labsys_mop_input_fullwidth\" maxlength=\"255\" value=\"".
              $this->uniqueID.
              "\" onchange='isDirty=true'>\n".
            "</fieldset>\n".

            "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("save")."\" onclick='isDirty=false'>\n".            

            "</FORM>\n".
            '<script language="JavaScript" type="text/javascript">
            <!--
            if (document.labEdit) document.labEdit.title.focus();
            //-->
            </script>'.
            EM::editBottomLab( $this, $fullAddress );
  }
  
  /**
  * Used to save the edited element.
  *
  * This function gets STATICALLY called from ../php/executeCommandOnElement.php?address=[elementID].
  * It handles the $_POSTed data and then redirects to the $_POST['REDIRECTTO'] with the address of the element added.
  */  
  /*static*/ function save(){
    global $lDBI, $url, $lng, $usr;
    if ( !isset($_POST['IDX']) || 
         !isset($_POST['REDIRECTTO']) || 
         !isset($_POST['FULLADDRESS']) || 
         !isset($_POST['UNIQUEID']) || 
         !isset($_POST['TITLE']) || 
         !isset($_POST['AUTHORS']) || 
         !isset($_POST['COMMENT']) || 
         !isset($_POST['PRELAB']) || 
         !isset($_POST['LAB'])     
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

    if ( $_POST['PRELAB'] == 0 ) $_POST['PRELAB'] = createNew( 'C' ); // create a new collection
    if ( $_POST['LAB'] == 0 ) $_POST['LAB'] = createNew( 'C' ); // create a new collection
    
    $newLab = new LlElement(  $_POST['IDX'],
                              $_POST['UNIQUEID'],
                              $_POST['TITLE'], 
                              $_POST['AUTHORS'],
                              $_POST['COMMENT'], 
                              $_POST['PRELAB'],
                              $_POST['LAB'],
                              $_POST['MATCHINGMENU'], 
                              ( isset( $_POST["VISIBLE_BEFORE_FIRST_SCHED"] ) && $_POST["VISIBLE_BEFORE_FIRST_SCHED"] == 1 ), 
                              ( isset( $_POST["VISIBLE_DURING_SCHED"] ) && $_POST["VISIBLE_DURING_SCHED"] == 1 ),
                              ( isset( $_POST["VISIBLE_AFTER_FIRST_SCHED"] ) && $_POST["VISIBLE_AFTER_FIRST_SCHED"] == 1 ),
                              ( $_POST["VISIBLE_ONLY_IN_COLLECTION"] == "1" ),
                              ""
                             );

    // if $_POST["SAVEASNEW"] is set the element is saved as new element
    if ( isset($_POST["SAVEASNEW"]) && ($_POST["SAVEASNEW"] == "1") ){
                                      $newLab->idx = $lDBI->setData( $newLab, true );
                                      $url->put( "address=l".$newLab->idx );
                                     }
    else{
                                      $lDBI->setData( $newLab );
                                      $url->put( "address=".$_POST['FULLADDRESS'] );
    }
    
    // some user information
    $url->put( "sysinfo=".$lng->get("DataHasBeenSaved")." ".$lng->get("lNumber")." ".$newLab->idx );

    // One could also display the page here but that way it is left to the normal mechanism...
    header( "Location: ".$url->rawLink2( $_POST['REDIRECTTO'] ) );
  }
  
// The next functions are used for the element's manage page. ***********************************************

  /**
  * Displays the element's property legend on the manage page.
  * All entries of getPropertyRow() should be explained here.
  */
  function showPropertyLegend(){ /* Shows the legend to the property rows */
    global $lng, $pge;
    
    $lgnd = "<div class=\"labsys_mop_".$this->elementId."_row\">\n".
            "<div class=\"labsys_mop_h3\">".$lng->get("legend")."</div>\n".
           // visible only inside a collection?
            "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" name=\"legend1_VISIBLE_ONLY_IN_COLLECTION\" value=\"0\" checked=\"checked\">".
            infoArrow( $lng->get("visAlways") ).
            $lng->get("visAlways")."<br />\n".
            
            "<input type=\"radio\" name=\"legend2_VISIBLE_ONLY_IN_COLLECTION\" value=\"0\" disabled=\"disabled\">".
            infoArrow( $lng->get("visAlways"), true ).
            "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" name=\"legend2_VISIBLE_ONLY_IN_COLLECTION\" value=\"1\" checked=\"checked\">".
            infoArrow( $lng->get("visOnlyInColl") ).
            $lng->get("visOnlyInColl")."<br />\n".
            
            "<input type=\"radio\" name=\"legend2a_VISIBLE_ONLY_IN_COLLECTION\" value=\"0\" disabled=\"disabled\">".
            infoArrow( $lng->get("visAlways"), true ).
            "<input type=\"radio\" name=\"legend2a_VISIBLE_ONLY_IN_COLLECTION\" value=\"1\" disabled=\"disabled\">".
            infoArrow( $lng->get("visOnlyInColl"), true ).
          " | ".
            "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"legend3_VISIBLE_BEFORE_FIRST_SCHED\" value=\"1\" checked=\"checked\">".
            infoArrow( $lng->get("visibleBef1stSched") ).
            $lng->get("visibleBef1stSched")."<br />\n".
            
            "<input type=\"radio\" name=\"legend2a_VISIBLE_ONLY_IN_COLLECTION\" value=\"0\" disabled=\"disabled\">".
            infoArrow( $lng->get("visAlways"), true ).
            "<input type=\"radio\" name=\"legend2a_VISIBLE_ONLY_IN_COLLECTION\" value=\"1\" disabled=\"disabled\">".
            infoArrow( $lng->get("visOnlyInColl"), true ).
          " | ".
           // schedule issues
            "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"legend4_VISIBLE_BEFORE_FIRST_SCHED\" value=\"1\" disabled=\"disabled\">".
            infoArrow( $lng->get("visibleBef1stSched"), true ).
            "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"legend4_VISIBLE_DURING_SCHED\" value=\"1\" checked=\"checked\">".
            infoArrow( $lng->get("visibleDuringSched") ).
            $lng->get("visibleDuringSched")."<br />\n".
            
            "<input type=\"radio\" name=\"legend2a_VISIBLE_ONLY_IN_COLLECTION\" value=\"0\" disabled=\"disabled\">".
            infoArrow( $lng->get("visAlways"), true ).
            "<input type=\"radio\" name=\"legend2a_VISIBLE_ONLY_IN_COLLECTION\" value=\"1\" disabled=\"disabled\">".
            infoArrow( $lng->get("visOnlyInColl"), true ).
          " | ".
            "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"legend5_VISIBLE_BEFORE_FIRST_SCHED\" value=\"1\" disabled=\"disabled\">".
            infoArrow( $lng->get("visibleBef1stSched"), true ).
            "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"legend5_VISIBLE_DURING_SCHED\" value=\"1\" disabled=\"disabled\">".
            infoArrow( $lng->get("visibleDuringSched"), true ).
            "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"legend5_VISIBLE_AFTER_FIRST_SCHED\" value=\"1\" checked=\"checked\">".
            infoArrow( $lng->get("visibleAftr1stSched") ).
            $lng->get("visibleAftr1stSched")."\n".
            "</div>\n";
    return $lgnd;
  }
  
  /**
  * This function is called to display the element's properties.
  * @param $prefix    If given this is put before the element's title (p.e. 1.2.3 title).
  * @param $disabled  Should the checkboxes, radio buttons etc. be disabled?
  * @return string    The html-code of the elements properties horizontally arranged.
  */
  function getPropertyRow( $prefix, $disabled=false ){
    global $pge, $lng;
    
    return    "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" ".( !$disabled ?  "name=\"".$prefix."_VISIBLE_ONLY_IN_COLLECTION\" "  : '' )."value=\"0\"".( !$this->visibleOnlyInCollection ?  " checked=\"checked\"" : '' ).( $disabled ?  " disabled=\"disabled\"" : '' )." onchange='isDirty=true'>".
              infoArrow( $lng->get("visAlways"), $disabled ).
              "<input tabindex=\"".$pge->nextTab++."\" type=\"radio\" ".( !$disabled ?  "name=\"".$prefix."_VISIBLE_ONLY_IN_COLLECTION\" "  : '' )."value=\"1\"".( $this->visibleOnlyInCollection ?  " checked=\"checked\"" : '' ).( $disabled ?  " disabled=\"disabled\"" : '' )." onchange='isDirty=true'>".
              infoArrow( $lng->get("visOnlyInColl"), $disabled ).
            ( $disabled ?  "<span class=\"labsys_mop_grayed\">"  : '' ).
            " | ".
           // schedule issues
            ( $disabled ?  "</span>"  : '' ).
              "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"".$prefix."_VISIBLE_BEFORE_FIRST_SCHED\" value=\"1\"".( $this->visibleBefore1stSched ?  " checked=\"checked\"" : '' ).( $disabled ?  " disabled=\"disabled\"" : '' )." onchange='isDirty=true'>".
              infoArrow( $lng->get("visibleBef1stSched"), $disabled ).
              "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"".$prefix."_VISIBLE_DURING_SCHED\" value=\"1\"".( $this->visibleDuringSched ?  " checked=\"checked\"" : '' ).( $disabled ?  " disabled=\"disabled\"" : '' )." onchange='isDirty=true'>".
              infoArrow( $lng->get("visibleDuringSched"), $disabled ).
              "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" name=\"".$prefix."_VISIBLE_AFTER_FIRST_SCHED\" value=\"1\"".( $this->visibleAfter1stSched ?  " checked=\"checked\"" : '' ).( $disabled ?  " disabled=\"disabled\"" : '' )." onchange='isDirty=true'>".
              infoArrow( $lng->get("visibleAftr1stSched"), $disabled ).
            " ".
              $this->title;
  }
  
  /**
  * This function is called to display the elements export data on the import_export-page /pages/import_export_labs.php.
  * @param $prefix    If given this is put before the element's title (p.e. 1.2.3 title).
  * @param $isExport  Show export or import row?
  * @param $disabled  Should the checkboxes, radio buttons etc. be disabled?
  * @return string    The html-code of the elements properties horizontally arranged.
  */
  function getExportImportRow( $prefix, $isExport=true, $disabled=false ){
    global $pge, $lng;
    
    if ( $isExport )
      return    "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" ".( !$disabled ?  "name=\"".$this->idx."\" "  : '' )."value=\"EXPORT\"".( $disabled ?  " disabled=\"disabled\"" : '' )." onchange='isDirty=true'>".
              ' <img src="../syspix/button_export2disk_30x12.gif" width="30" height="12" border="0" alt="next" title="export"> '.
                $prefix.$this->title.' '.EB::delete( 'l'.$this->idx );
    else
      return  '<img src="../syspix/button_importFromDisk_30x12.gif" width="30" height="12" border="0" alt="next" title="export"> '.
              "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" ".( !$disabled ?  "name=\"".$this->idx."\" "  : '' )."value=\"IMPORT\"".( $disabled ?  " disabled=\"disabled\"" : '' )." onchange='isDirty=true'> ".
              $prefix.$this->title;
  }
  
// The next functions are adfditional functions of the element. *********************************************
  /**
  * Called by USER (so $this->labIsClosed etc. are set correctly) to check if his prelab answers are correct.
  *
  * Calls the prelab collection's allMultipleChoicesAnswered().
  * Sets the stats in $usDBI.
  */
  function checkPreLab(){ // called to check the answers here
    global $url, $lng, $usr;
    if ( !isset($_POST['REDIRECTTO'])
       ){
          trigger_error( $lng->get("notAllNecPosted"), E_USER_ERROR );
          exit;
         }
       
    // Validity (session ID) is checked by the calling script yet.
    
    if( $this->preLabIsVisible2Me && !$this->preLabIsFinished ){
   // Are all multiple choice questions answered (->LmElement)?
    $parameter = Array( 'allPreLabAnswered' => true,
                        'givenCredits' => 0,
                        'possibleCredits' => 0 );
    
    $this->preLab->extMap(  "if ( \$element->elementId == 'm' ){ ".
                              "\$element->loadUserData4( '".$usr->uid."' ); ".
                              "if ( \$parameter[ 'allPreLabAnswered' ] &= \$element->checkUserAnswer() ){ ". // check the answer (if not closed this adds an attempt!).
                                "\$parameter[ 'givenCredits' ] += \$element->getGivenCredits();".
                              "}".
                              "\$parameter[ 'possibleCredits' ] += \$element->getPossibleCredits();".
                            "}".
                            "else if ( (\$element->elementId == 'C') || (\$element->elementId == 'c') ) \$element->extMap( \$functionCode, \$parameter );", // recurse
                            $parameter );
                            
      makeLogEntry( 'l', 'prelab checked '.( $parameter[ 'allPreLabAnswered' ] ? '(right)' : '(wrong)' ), $this->idx );

      if( $parameter[ 'allPreLabAnswered' ] ){

         // create my dataset
          $this->usDBI->setData4( $usr->uid,                        // $uid
                                  $usr->currentTeam,                // $currentTeam
                                  true,                             // $prelabFinished
                                  $parameter[ 'givenCredits' ],     // $prelabGivenCredits
                                  $parameter[ 'possibleCredits' ],  // $prelabPossibleCredits
                                  false,                            // $prelabAllTeammembersFinished
                                  false,                            // $labClosed
                                  false,                            // $labCorrected
                                  "",                               // $whichCorrected
                                  0,                                // $givenCredits
                                  0,                                // $possibleCredits
                                  $lng->get("preLabUserFinished")   // $history
                                 );
         makeLogEntry( 'l', 'prelab finished', $this->idx );

         // now CHECK whether ALL teammates (identified by DBInterfaceUserRights.inc->currentTeam) finished the prelab:
          require_once( INCLUDE_DIR."/classes/DBInterfaceUserRights.inc" );
          $urDBI = new DBInterfaceUserRights();
          
          $urDBI->getAllData( "", true, "`currentTeam` = '".$usr->currentTeam."' && `uid` != '".$usr->uid."'" ); // Only check others.
          $preLabAllTeammatesFin = true;
          while ( ($dataArray = $urDBI->getNextData()) ){
            $uid          = $dataArray['uid'];
            $permissions  = $this->usDBI->getData4( $uid );
            $preLabAllTeammatesFin &= $permissions['preLabFinished'];
          }
          
          if ( $preLabAllTeammatesFin ){ // all have finished so 
            // update their prelab status
            $this->usDBI->setPrelabFinished( $usr->currentTeam );
            
            makeLogEntry( 'l', 'prelab finished team '.$usr->currentTeam, $this->idx );
            
            if ( $this->noLab ) // If we have no lab it gets closed when all teammembers finished the prelab
              $this->usDBI->closeLab( $usr->currentTeam, $lng->get("labClosed") );

            // map all inputs to them
            // If there come new inputs after the mapping here, the user will be mapped to
            // his $usr->currentTeam on entering the question.
            // The mapping can be renewed/ changed by user editors on the labstate page.
            $this->usDBI->getAllTeamData( $usr->currentTeam );
            while( $labStatusData = $this->usDBI->getNextData() ) 
              $this->mapAllLabInputs( $labStatusData["uid"], $labStatusData["currentTeam"] );
          }
      }
    }

    // One could also display the page here but that way it is left to the normal mechanism...
    header( "Location: ".$url->rawLink2( $_POST['REDIRECTTO'] ) );
  }
  
  /**
  * Called if no prelab is present to create the status table entries for 
  * all teammembers of the current users.
  * Otherwise that would be done by finishing the prelab...
  *
  * @param $teamNumber      The Team the status should be generated for.
  *
  */
  function createPrelabFinishedStatus( $teamNumber ){
    global $usr, $lng;
         // now CHECK whether ALL teammates (identified by DBInterfaceUserRights.inc->currentTeam) finished the prelab:
          require_once( INCLUDE_DIR."/classes/DBInterfaceUserRights.inc" );
          $urDBI = new DBInterfaceUserRights();
          $urDBI->getAllData( "", true, "currentTeam='".$teamNumber."'" );
          while ( ($dataArray = $urDBI->getNextData()) ){
            $uid          = $dataArray['uid'];
           // create dataset
            $this->usDBI->setData4( $uid, $teamNumber, true, 0, 0, true, false, false, "", 0, 0, $lng->get("userReMapped")." (".$teamNumber.")" );
          }
  }
  
  /**
  * Called by ../php/executeOnLab.php.
  * Acquires the lock not taking care of any existing locks.
  *
  * @param $i_idx         The index of the input the lock should get acquired on.
  * @param $fullAddress   The element's full address (p.e. l23.C42.i17)
  */
  function forceLockOn( $i_idx, $fullAddress ){
    global $url;
    
    $element = $GLOBALS[ 'iDBI' ]->getData2idx( $i_idx );
    $element->acquireLock4( $fullAddress );
    
    // One could also display the page here but that way it is left to the normal mechanism...
    header( "Location: ".$url->rawLink2( stripslashes( $url->get('redirectTo') ) ) );
  }
  
  /**
  * Closes all lab inputs. The function below has this function's name
  * because it gets called via url...
  *
  * @param $team  The teamnumber.
  */
  function reallyCloseAllLabInputs( $team ){
    global $lng, $usr;
    makeLogEntry( 'l', 'closing all i', 'l'.$this->idx );
   // close all lab inputs:
   // also reMap all inputs  (necessary for users that never entered a question (done together).
    $parameter = Array(); //teamUIDs
    $this->usDBI->getAllTeamData( $team );
    while( ($labStatus = $this->usDBI->getNextData()) ) array_push( $parameter, $labStatus['uid'] );

    $this->lab->extMap( "if ( \$element->elementId == 'i' ){
                           \$element->closeInput( ".$team." ); // close the input.
                           foreach( \$parameter as \$uidVal ) \$element->mapInput( \$uidVal, ".$team." ); // map UID->Team.
                        }".
                        "else if ( (\$element->elementId == 'C') || (\$element->elementId == 'c') ) \$element->extMap( \$functionCode, \$parameter );", // recurse
                        $parameter );

    if ( !$labStatus['labClosed'] ) // Status changed?
      $this->usDBI->closeLab( $team, $lng->get("labClosed") );
  }
  
  /**
  * Called by ../php/executeOnLab.php.
  * Closes all inputs in $lab and changes the status in $usDBI.
  *
  * @param $team  The teamnumber.
  */
  function closeAllLabInputs( $team ){
    global $url;
    $this->reallyCloseAllLabInputs( $team );

    makeLogEntry( 'l', 'all inputs closed for team '.$team, $this->idx );
    
    // One could also display the page here but that way it is left to the normal mechanism...
    header( "Location: ".$url->rawLink2( stripslashes( $url->get('redirectTo') ) ) );
  }
  
  /**
  * Called by user to close all lab answers (ablility to correct then).
  */
  function closeLabInputs(){
    global $url, $lng, $usr;
    if ( !isset($_POST['REDIRECTTO'])
       ){
          trigger_error( $lng->get("notAllNecPosted"), E_USER_ERROR );
          exit;
        }

    // Validity (session ID) is checked by the calling script yet.
    
    if ( !$this->labIsVisible2Me ){
                                    trigger_error( $lng->get("notAllowed"), E_USER_ERROR );
                                    exit;
                                   }

    $this->closeAllLabInputs( $usr->currentTeam );

    header( "Location: ".$url->rawLink2( $_POST['REDIRECTTO'] ) );
  }
  
  /**
  * Called by ../php/executeOnLab.php.
  * ReOpens all inputs in $lab and changes the status in $usDBI.
  *
  * @param $team  The teamnumber.
  */
  function reOpenAllLabInputs( $team ){
    global $usr, $lng, $url;
    
   // reOpen all lab inputs
    $dummy = false;
    $this->lab->extMap( "if ( \$element->elementId == 'i' ) \$element->reOpenInput( ".$team." );". // reOpen the input.
                        "else if ( (\$element->elementId == 'C') || (\$element->elementId == 'c') ) \$element->extMap( \$functionCode, \$parameter );", // recurse
                        $dummy );
                            
    $this->usDBI->reOpenLab( $team, $lng->get("labReOpened") );
    
    makeLogEntry( 'l', 'all inputs reopened for team '.$team, $this->idx );
    
    // One could also display the page here but that way it is left to the normal mechanism...
    header( "Location: ".$url->rawLink2( stripslashes( $url->get('redirectTo') ) ) );
  }

  /**
  * Creates the mappings in the $this->usDBI ($uid => $team).
  *
  * What for? The user get assign to the team on entering the question!?
  *   It is necessary to make sure that all mappings UID, i_idx -> team exist!
  *
  * @param $uid   The user's unique identifier.
  * @param $team  The teamnumber.
  */
  function mapAllLabInputs( $uid, $team ){
   // remap all lab inputs:
    $dummy = false;
    $this->lab->extMap( "if ( \$element->elementId == 'i' ) \$element->mapInput( '".$uid."', '".$team."' );". // reMap UID->Team.
                        "else if ( (\$element->elementId == 'C') || (\$element->elementId == 'c') ) \$element->extMap( \$functionCode, \$parameter );", // recurse
                        $dummy );
   
    $labStatus = $this->usDBI->getData4( $uid );
    if ( $labStatus['labClosed'] ) // if the lab is closed the input has to be closed too.
      $this->reallyCloseAllLabInputs( $labStatus['currentTeam'] );
      
    makeLogEntry( 'l', 'all inputs remapped for team '.$team, $this->idx );
  }
  
  /**
  * Called by ../php/executeOnLab.php.
  * Remaps all inputs and the status record for this lab and the given user ($uid) to $team.
  *
  * @param $uid   The user's unique identifier.
  * @param $team  The teamnumber.
  */
  function reMapUidTeam( $uid, $team ){
    global $lng, $url;
    // update all input assignments
    $this->mapAllLabInputs( $uid, $team );
    
    // update the status record
    $labStatus = $this->usDBI->getData4( $uid );
    if ( $labStatus["currentTeam"] != $team ) // changes?
      $this->usDBI->setData4( $uid,                                 // $uid
                              $team,                                // $currentTeam
                              $labStatus['preLabFinished'],         // $prelabFinished
                              $labStatus['preLabGivenCredits'],     // $prelabGivenCredits
                              $labStatus['preLabPossibleCredits'],  // $prelabPossibleCredits
                              $labStatus['preLabAllTeammFinished'], // $prelabAllTeammembersFinished
                              $labStatus['labClosed'],              // $labClosed
                              $labStatus['labCorrected'],           // $labCorrected
                              $labStatus['whichCorrected'],         // $whichCorrected
                              $labStatus['labGivenCredits'],        // $givenCredits
                              $labStatus['labPossibleCredits'],     // $possibleCredits
                              $lng->get("userReMapped")." (".$team.")" );          // $history
    
    // recheck the prelab... as new team
    $_POST['REDIRECTTO'] = $url->get('redirectTo');
    $_POST['IDX'] = $this->idx;
    $this->updateStatus(); // links automatically
    // One could also display the page here but that way it is left to the normal mechanism...
    //header( "Location: ".$url->rawLink2( stripslashes( $url->get('redirectTo') ) ) );
  }
  
  /**
  * Called by ../php/executeOnLab.php.
  * Sets the observed $uid (for correctors);
  *
  * @param $uid   The user's unique identifier.
  */
  function setSeeingUID( $uid ){
    global $usr, $url;
    $usr->seesDataOf( $uid );

    // One could also display the page here but that way it is left to the normal mechanism...
    header( "Location: ".$url->rawLink2( stripslashes( $url->get('redirectTo') ) ) );
  }
  
  /**
  * ReCalculates the user's
  *  - given points
  *  - possible points
  *  - is corrected state ( all checkmarks set? )
  */
  function updateStatus(){

    // If you encounter problems with the statistic or want to recalculate it you have
    // to perform both following steps
    
   
    // create StatusEntry for Teams
//    for ($teamNumber=1; $teamNumber<9; $teamNumber++)
//      $this->createPrelabFinishedStatus( $teamNumber );


    // calculate prelab stats for EXISTING entries
    $this->updateStatus2();
  
    
    global $url, $lng, $usr;
    if ( !isset($_POST['IDX']) || 
         !isset($_POST['REDIRECTTO'])     
       ){
          trigger_error( $lng->get("notAllNecPosted"), E_USER_ERROR );
          exit;
         }
      
    // Validity (session ID) is checked by the calling script yet.
   
    $is2bClosed = $this->isAfter1stSched && !$this->isInSched; // should the lab be closed?

// 1) compute everything on the inputs
//    This is extremly performant because it touches each input only once!
//    The previous "normal" iteration took about three times longer...
   // iterate over statistics
    $parameter = Array(); // all teams
    $this->usDBI->getAllData( "current_team" );
    while( ($labStatus = $this->usDBI->getNextData()) )
      if ( !(in_array( $labStatus['currentTeam'], $parameter ) ) )
        array_push( $parameter, $labStatus['currentTeam'] );
      array_push( $parameter, 'alreadyInSum' ); // takes the indices of the inputs so that none is handled twice...

   // flip keys and values
    $parameter = array_flip( $parameter );

   // fill with empty containers
    foreach ( array_keys($parameter) as $val ) 
      if ( $val != 'alreadyInSum' ) $parameter[$val] = Array( 'areAllClosed' => true,
                                                              'areAllCorrected' => true,
                                                              'correctedString' => "",
                                                              'sumGivenCredits' => 0,
                                                              'sumPossibleCredits' => 0 );

    $parameter['alreadyInSum'] = Array(); // some inputs might be more than one time inside the collection...
                                          // p.e. I use an input for data that are reshown on a later page for comparison

   // check all lab inputs and close them if necessary
      $this->lab->extMap( "if ( (\$element->elementId == 'i') && !(in_array( \$element->idx, \$parameter['alreadyInSum'] )) ){ 
                              array_push( \$parameter['alreadyInSum'], \$element->idx ); // no double count
                              foreach( \$parameter as \$mKey => \$mValue ){
                                if ( \$mKey == 'alreadyInSum' ) continue; // jump over
                                \$element->loadUserData4( \$mKey );
                                if ( !\$element->isClosed && ".( $is2bClosed ?  'true' :  'false'  )." ) \$element->closeInput( \$mKey ); // if necessary close labinput
                                \$parameter[\$mKey]['areAllClosed']       &= \$element->isClosed;
                                \$parameter[\$mKey]['areAllCorrected']    &= \$element->isCorrected;
                                \$parameter[\$mKey]['correctedString']    .= ( \$element->isCorrected ?  't' :  'f'  );
                                if ( \$element->isCorrected ) // only add points of corrected
                                  \$parameter[\$mKey]['sumGivenCredits']  += \$element->getGivenCredits();
                                \$parameter[\$mKey]['sumPossibleCredits'] += \$element->getPossibleCredits();
                              }
                          }".
                          "else if ( (\$element->elementId == 'C') || (\$element->elementId == 'c') ) \$element->extMap( \$functionCode, \$parameter );", // recurse
                          $parameter );

    // iterate over statistics
    $this->usDBI->getAllData( "current_team" );
    while( ($labStatus = $this->usDBI->getNextData()) ){
       // changes?
        if ( ($labStatus['labClosed']          != $parameter[$labStatus['currentTeam']]['areAllClosed']) || // might happen after remapping...
             ($labStatus['labCorrected']       != $parameter[$labStatus['currentTeam']]['areAllCorrected']) ||
             ($labStatus['labGivenCredits']    != $parameter[$labStatus['currentTeam']]['sumGivenCredits']) ||
             ($labStatus['labPossibleCredits'] != $parameter[$labStatus['currentTeam']]['sumPossibleCredits']) ||
             (!$labStatus['labCorrected'] && 
                 ($labStatus['whichCorrected'] != $parameter[$labStatus['currentTeam']]['correctedString'])
             )
            ) // yes -> save them.
              $this->usDBI->setData4( $labStatus['uid'], 
                                      $teamJustChecked = $labStatus['currentTeam'], 
                                      true, 
                                      $labStatus['preLabGivenCredits'],     // $prelabGivenCredits
                                      $labStatus['preLabPossibleCredits'],  // $prelabPossibleCredits
                                      true, 
                                      $parameter[$labStatus['currentTeam']]['areAllClosed'], 
                                      $parameter[$labStatus['currentTeam']]['areAllCorrected'], 
                                      ( $labStatus['labCorrected'] ? '' : $parameter[$labStatus['currentTeam']]['correctedString']),
                                      $parameter[$labStatus['currentTeam']]['sumGivenCredits'], 
                                      $parameter[$labStatus['currentTeam']]['sumPossibleCredits'],
                                      $lng->get("statusUpdated") ); // create my dataset
    }

    header( "Location: ".$url->rawLink2( $_POST['REDIRECTTO'] ) );
  }
  
  function updateStatus2(){
    /* calculates the Prelab points for EXISTING entries! */
    global $url, $lng, $usr;
    if ( !isset($_POST['IDX']) || 
         !isset($_POST['REDIRECTTO'])     
       ){
          trigger_error( $lng->get("notAllNecPosted"), E_USER_ERROR );
          exit;
         }
      
    // Validity (session ID) is checked by the calling script yet.
   
    $is2bClosed = $this->isAfter1stSched && !$this->isInSched; // should the lab be closed?

    // iterate over EXISTING statistics
    $this->usDBI->getAllData( "current_team" );

    while( ($labStatus = $this->usDBI->getNextData()) ){

    $parameter2 = Array( 'allPreLabAnswered' => true,
                        'givenCredits' => 0,
                        'possibleCredits' => 0 );
    

    if ( true )// echo('processing '.$labStatus['uid'].'<br />');
      $this->preLab->extMap(  "if ( \$element->elementId == 'm' ){ ".
                              "\$element->loadUserData4( '".$labStatus['uid']."' ); ".
//                              "echo( '".$labStatus['uid']." - m'.\$element->idx.' - '.\$element->getGivenCredits().'/ '.\$element->getPossibleCredits().'<br>');".
                              "if ( \$parameter[ 'allPreLabAnswered' ] &= \$element->checkUserAnswer() ){ ". // check the answer (if not closed this adds an attempt!).
                                "\$parameter[ 'givenCredits' ] += \$element->getGivenCredits();".
                              "}".
                              "\$parameter[ 'possibleCredits' ] += \$element->getPossibleCredits();".
                            "}".
                            "else if ( (\$element->elementId == 'C') || (\$element->elementId == 'c') ) \$element->extMap( \$functionCode, \$parameter );", // recurse
                            $parameter2 );


//echo( $parameter2['givenCredits'].'/ '.$parameter2['possibleCredits'].'<br>' );
       // changes?
        if ( true// $labStatus['preLabPossibleCredits'] == 0
            ) // yes -> save them.
              $this->usDBI->setData4( $labStatus['uid'], 
                                      $labStatus['currentTeam'], 
                                      $labStatus['preLabFinished'], 
                                     $parameter2['givenCredits'], // $labStatus['preLabGivenCredits'],     // $prelabGivenCredits
                                     $parameter2['possibleCredits'], // $labStatus['preLabPossibleCredits'],  // $prelabPossibleCredits
                                      $labStatus['preLabAllTeammFinished'], 
                                      $labStatus['labClosed'], 
                                      $labStatus['labCorrected'], 
                                      $labStatus['whichCorrected'],
                                      $labStatus['labGivenCredits'], 
                                      $labStatus['labPossibleCredits'],
                                      $lng->get("statusUpdated") ); // create my dataset
    }

    header( "Location: ".$url->rawLink2( $_POST['REDIRECTTO'] ) );
  }
}
?>