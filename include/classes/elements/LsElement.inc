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
* implementation of the s (schedule) element.
*/

/**
* This element consists of a schedule a scheduler can make for c and l elements.
*
* @module     ../include/classes/elements/LsElement.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
require_once( INCLUDE_DIR."/classes/elements/Element.inc" );

class LsElement extends Element{
  var $id;      // This' scheduled elements id.
  var $num;     // This' scheduled elements idx.
  var $start;   // This schedules start time (TIMESTAMP (int) format).
  var $stop;    // This schedules stop  time (TIMESTAMP (int) format).
  var $comment; // A comment to this schedule.
 
  /**
  * The constructor.
  *
  * @param $idx       This instance's index.
  * @param $id        The scheduled element's id.
  * @param $num       The scheduled element's index.
  * @param $start     This' start time (TIMESTAMP (int) format).
  * @param $stop      This' stop time (TIMESTAMP (int) format).
  * @param $comment   A comment to the schedule (added to legend and hover).
  * @param $history   This' history.
  * @param $isInColl  Am I inside a collection? (with other elements important for visibility aspects).
  *                   Only passed because it is given. Has no use in this implementation.
  */
  function LsElement( $idx, $id, $num, $start, $stop, $comment, $history, $isInColl=false ){
    global $usr;
    $this->elementId                = "s";
    $this->idx                      = $idx;
    $this->title                    = $this->elementId." ".$idx;
    $this->matchingMenu             = "schedule";
    $this->id                       = $id;
    $this->num                      = $num;
    $this->start                    = $start;
    $this->stop                     = $stop;
    $this->comment                  = $comment;
    $this->history                  = $history;
    
    $this->IamVisible               = true; // schedules are always visible
    
    $this->serializationArray       = array( 'idx', 'id', 'num', 'start', 'stop', 'comment', 'history' ); // see Element.inc for details
  }

  /**
  * Returns the schedule information to a specified element.
  * The element is specified by $id & $num (p.e. c1).
  *
  * @param $id                An element's id.
  * @param $num               An element's number.
  *
  * @return Array(  "isBefore1stSched"  => bool,
  *                 "isInSched"         => bool,
  *                 "isAfter1stSched"   => bool,
  *                 "notFound"          => bool
  *               );
  */  
  /*static*/ function getVisibility( $id, $num ){
    global $sDBI;
    
    $data = $sDBI->getSchedule4( strtolower($id), $num );
    if ( $data["notFound"] ) $data = $sDBI->getSchedule4( strtoupper($id), $num ); // c and C element are the same...
        
    return $data;
  }
  
  /**
  * Called statically from an element that is schedule dependent to determined which state it has according to its schedules.
  *
  * @param $id                      The element's identifier (p.e. l).
  * @param $num                     The element's iindex (p.e. 23).
  * @param $visibleBefore1stSched   Is the element that asks visible before its 1st schedule? (elements property).
  * @param $visibleDuringSched      Is the element that asks visible while it is scheduled? (elements property).
  * @param $visibleAfter1stSched    Is the element that asks visible after its 1st schedule? (elements property).
  *
  * @return bool                    Is the element visible now?
  */
  /*static*/ function visiblePerSchedule( $id, $num, $visibleBefore1stSched, $visibleDuringSched, $visibleAfter1stSched ){
    global $sDBI, $usr;
    if ( $visibleBefore1stSched && $visibleDuringSched && $visibleAfter1stSched) return true;     // always visible
    if ( !$visibleBefore1stSched && !$visibleDuringSched && !$visibleAfter1stSched) return false; // never visible
    
    $data = LSElement::getVisibility( strtolower($id), $num );
        
    return ( ($data["isBefore1stSched"] && $visibleBefore1stSched) ||
             ($data["isInSched"] && $visibleDuringSched) ||
             ($data["isAfter1stSched"] && $visibleAfter1stSched) );
  }

 
// The next functions are used for the element's view page. *************************************************

// helper (if this exists some time they should be declared private)  - - - - - - - - - - - - - - - - - - - 
// These function were originally coded inline into the show method but php doesn't support inline
// methods. So I put them out again.
  /**
  * @return string  The html code of the first table row (table-layout: fixed;).
  **/
  function emptyRow(){
    $ret  = "<tr>";
    for ( $i = 0; $i<31; $i++ ) $ret .= "<td></td>";
    $ret .= "</tr>\n";
    return $ret;
  }
  
  /**
  * @return string  The html code of an empty row (spacer).
  */
  function spacerRow(){
    return "<tr><td colspan=\"31\">&nbsp;</td></tr>\n";
  }
  
  /**
  * @param $givenDate TIMESTAMP containing fitting month and year.
  *                   If also the day is given and the current date() is in that month the appropriate cell is highlighted.
  *
  * @return string    The html code of two table rows containing:
  *                     - cells filled with the daynames of the $givenDate's month.
  *                     - cells filled with the daynumbers of the $givenDate's month.
  */
  function monthRow( $givenDate ){
    $nameOfMonth = date("F Y", $givenDate);

    // How many days has the month?
      $nextMonth = (date("n", $givenDate) % 12)+1;
      $nextMonthsYear = date("Y", $givenDate)+floor(date("n", $givenDate) / 12);
    $daysInMonth = date("d", mktime(0, 0, 0, $nextMonth, 0, $nextMonthsYear ) );
    
    // Is today in this month?
    $today = 0;
    if ( ( date("Y", $givenDate) == date("Y") ) && ( date("n", $givenDate) == date("n") ) ) $today = date("j");
    
    $header  = "<tr><td colspan=\"31\" class=\"labsys_mop_s_table_name_month\">".$nameOfMonth."</td></tr>";

    $dayNames    = "<tr>";
    $dayNumbers  = "<tr>";

    for ( $i=1; $i <= $daysInMonth; $i++ ){
      $dayNames   .= "<td class=\"labsys_mop_s_table_day_name\">".date( "D", mktime( 0, 0, 0, date("n", $givenDate), $i, date("Y", $givenDate) ) )."</td>";
      $dayNumbers .= "<td class=\"labsys_mop_s_table_".( $i == $today ?  "to"  : '' )."day\">".$i."</td>"; 
                                        }

    // There might be empty fields at the end (p.e. februrary) to fill the 31 columns.
    for ( $i=$daysInMonth; $i <= 31; $i++ ){ $dayNames .= "<td></td>"; $dayNumbers .= "<td></td>"; }

    $dayNames   .= "</tr>";
    $dayNumbers .= "</tr>";

    return $header.$dayNames.$dayNumbers;
  }

  /**
  * @param $startDayOfMonth   The day/ cell {1..31} the schedule row should start.
  * @param $stopDayOfMonth    The day/ cell {1..31} the schedule row should stop.
  * @param $daysInMonth       The number of days {1..31} the month the row is for has.
  * @param $label             What should be put inside the bar?
  */    
  function scheduledRow( $startDayOfMonth, $stopDayOfMonth, $daysInMonth, $label ){
    $ret  = "<tr>";

    for ( $i=1; $i < $startDayOfMonth; $i++ ) // before
      $ret .= "<td class=\"labsys_mop_s_table_no_schedule_cell\">&nbsp;</td>";

    $ret .= "<td colspan=\"".($stopDayOfMonth - $startDayOfMonth + 1)."\" class=\"labsys_mop_s_table_schedule_cell\">".$label."</td>";

    for ( $i=$stopDayOfMonth+1; $i <= $daysInMonth; $i++ )  // after
      $ret .= "<td class=\"labsys_mop_s_table_no_schedule_cell\">&nbsp;</td>";
    
    for ( $i=$daysInMonth; $i <= 31; $i++ ) // fill till 31
      $ret .= "<td></td>";

    $ret .= "</tr>";

    return $ret;
  }

  /**
  * Adds the html code for one schedule to &$arrayOtherSchedules.
  *
  * @param $dateStart             TIMESTAMP containing the start date.
  * @param $dateStop              TIMESTAMP containing the stop date.
  * @param $label                 What should be put inside the bar?
  * @param &$arrayOtherSchedules  An array consisting of the following pairs:
  *                               ( mktime( 0, 0, 0, month, 1, year ), "<tr>...</tr>\n ..." )
  */
  function addSchedule( $dateStart, $dateStop, $label, &$arrayOtherSchedules ){
    $stopMonth  = date( "m", $dateStop  ); // with leading 0
    $stopYear   = date( "Y", $dateStop  );
    
    $currentMonth = date( "m", $dateStart ); // with leading 0
    $currentYear  = date( "Y", $dateStart );
    
    $startDayInCurrentMonth = date( "j", $dateStart );

    // add the necessary rows for each month in between    
    while( intval( $currentYear.$currentMonth ) <= intval( $stopYear.$stopMonth ) ){
     $nextMonth = substr( "0".(($currentMonth % 12)+1), -2); // with leading 0
     $nextMonthsYear = $currentYear+floor($currentMonth / 12);
     $currentMonthDays = date("d", mktime(0, 0, 0, $nextMonth, 0, $nextMonthsYear ) );
    
     if ( intval( $currentYear.$currentMonth ) == intval( $stopYear.$stopMonth ) ) 
      $stopDayInCurrentMonth = date( "j", $dateStop  ); 
     else 
      $stopDayInCurrentMonth = $currentMonthDays;
      
     $key = "key".$currentYear.$currentMonth; // this is the array key to add values to
     $value = $this->scheduledRow( $startDayInCurrentMonth, $stopDayInCurrentMonth, $currentMonthDays, $label );
    
     if ( array_key_exists( $key, $arrayOtherSchedules) ) // $key exists yet -> just add
      $arrayOtherSchedules[$key] .= $value; 
     else // $key doesn't exist -> create new header ( monthRow() ) and add yourself
      $arrayOtherSchedules = array_merge( $arrayOtherSchedules, Array( $key => $this->spacerRow().$this->monthRow( mktime( 0, 0, 0, $currentMonth, 1, $currentYear ) ).$value ) );         // init for next
     
     $startDayInCurrentMonth = 1;
     $currentMonth = $nextMonth;
     $currentYear  = $nextMonthsYear;
    }
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
    global $usr, $lng, $pge, $sDBI, $url, $cfg;
    if (  !$this->isVisible() ){ // not enough rights->no content
      return "<div class=\"labsys_mop_note\">\n".$lng->get("TxtNotEnoughRights")."\n</div>";
      exit;
    }
    
    $schedules = Array();
    $legend = Array();
    
    // walk thru all schedules and add them
    $sDBI->getAllData( "start" );
    while( $element = $sDBI->getNextData() ){
      if ( $element->idx == 1 ) continue; // prototype

      // Get more information of the element.
      if ( !array_key_exists( "s".$element->idx, $legend) ){ // new element? -> add to legend
        $id = $element->id;
        require( "../php/getDBIbyID.inc" ); // -> $DBI
        $element2 = $DBI->getMenuData2idx( $element->num );
        $legend = array_merge( $legend, 
                               Array( "s".$element->idx =>  "<a href=\"".$url->link2( "../pages/view.php?address=".$element->id.$element->num )."\">".$element2->title."</a>".
                                                            " (".date( $lng->get("DateFormat"),  $element->start ).
                                                            " - ".date( $lng->get("DateFormat"), $element->stop ).")".
                                                            ( $usr->isOfKind( IS_SCHEDULER ) ? " ".EB::edit( "s".$element->idx ).EB::delete( "s".$element->idx ) : '' ).
                                                            " ".EB::history( $element->history, "s".$element->idx, false ).
                                                            ( $element->comment != "" ?  "\n<br />".$element->comment."\n"  : '' )
                                     ) 
                              );
      }

      // Add it to the schedule table      
      $this->addSchedule( $element->start,
                          $element->stop, 
                          "<a href=\"".$url->link2( "../pages/view.php?address=".$element->id.$element->num )."\" ".
                              "title=\""."s".$element->idx.': '.$element2->title.( $element->comment != "" ?  "\n".$element->comment  : '' )."\">".
                          $element2->title."</a>",
                          $schedules,
                          $legend
                         );
    }
    
    // sort schedules by key (by date)
    ksort( $schedules );
    
    // sort legend by id
    //natsort( $legend ); // this doesn't work correctly with my php version - without further sorting: by date -> o.k.
    
    // create html code
    $myCalendar = "";
    foreach( $schedules as $value ) $myCalendar .= $value;
    $myLegend = "";
    foreach( $legend as $key => $value ) $myLegend .= "<li>".$key.": ".$value."</li>\n";
    
    // return the whole element
    return EM::viewTopSchedule( $this, $fullAddress ).
           "<div class=\"labsys_mop_contentArea\">\n".
           "<div class=\"labsys_mop_h2\">".$lng->get("titleSchedules")." - ".$cfg->get("SystemTitle")."</div>".
           ( $lng->get("showScheduleNote") != '' ? "<div class=\"labsys_mop_note\">\n".$lng->get("showScheduleNote")."</div>\n" : '' ). // note
           "<table class=\"labsys_mop_s_table\">\n".
           $this->emptyRow().
           $myCalendar.
           "</table>".
           "<ul class=\"labsys_mop_s_legend\">".
           $myLegend.
           "</ul>".
           "</div>\n".
           EM::viewBottomSchedule( $this, $fullAddress );
  }
  
  
// The next functions are used for the element's edit page. *************************************************

// helper (if this exists some time they should be declared private)  - - - - - - - - - - - - - - - - - - - 
// These function were originally coded inline into the showEdit method but php doesn't support inline
// methods. So I put them out again.
    /**
    * Creates the html code of a selectors options. It contains the numerical values to select a date.
    *
    * @param $first_N     The first number the input should contain.
    * @param $last_N      The last number the input should contain.
    * @param $selected_N  The number that should currently be selected.
    *
    * @return string  The html code of a selectors options.
    */
    function makeOptions( $first_N, $last_N, $selected_N ){
      $ret = "";
      
      $first_N = intval( $first_N );
      $last_N = intval( $last_N );
      $selected_N = intval( $selected_N );
      
      for ( $i=$first_N; $i<=$last_N; $i++ ){
        $ret .= "<option value=\"".$i."\"".
                ( ($i == $selected_N ) ?  " selected=\"selected\"" : '' ).
                ">".$i."</option>\n";
      }
      return $ret;
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
    global $lng, $url, $pge, $cfg, $usr;
    
    // dropdown list for collections and labs:
    $dropDownOptionsCollLab = "";
    // labs
    $id = "l";
    require( "../php/getDBIbyID.inc" ); // -> $DBI
    $DBI->getAllData( "idx", true );    // sorted by idx ascending
    while ( $element = $DBI->getNextData() ){
      $dropDownOptionsCollLab.= "<option value=\"l".$element->idx."\"".
                                ( ("l".$element->idx == $this->id.$this->num) ?  " selected=\"selected\"" : '' ).
                                ">l".$element->idx.": ".$element->title."</option>\n";
    }
    // collections
    $id = "c";
    require( "../php/getDBIbyID.inc" ); // -> $DBI
    $DBI->getAllData( "idx", true );    // sorted by idx ascending
    while ( $element = $DBI->getNextData() ){
      $dropDownOptionsCollLab.= "<option value=\"C".$element->idx."\"".
                                ( ("C".$element->idx == $this->id.$this->num) ?  " selected=\"selected\"" : '' ).
                                ">C".$element->idx.": ".$element->title."</option>\n";
      $dropDownOptionsCollLab.= "<option value=\"c".$element->idx."\"".
                                ( ("c".$element->idx == $this->id.$this->num) ?  " selected=\"selected\"" : '' ).
                                ">c".$element->idx.": ".$element->title."</option>\n";
    }
    
    return EM::editTopSchedule( $this, $fullAddress ).
          "<FORM class=\"labsys_mop_std_form\" NAME=\"ScheduleEdit\" METHOD=\"POST\" ACTION=\"".$url->link2("../php/executeCommandOnElement.php?address=s")."\">\n".
          "<input type=\"hidden\" name=\"FUNCTIONNAME\" value=\"save()\">\n".           // this function will statically be called by ../php/executeCommandOnElement.php
          "<input type=\"hidden\" name=\"SESSION_ID\" value=\"".session_id()."\">\n".   // to check whether the call is valid
          "<input type=\"hidden\" name=\"REDIRECTTO\" value=\"../pages/edit.php\">\n".  // index of saved el. will be added on save.php!
          "<input type=\"hidden\" name=\"FULLADDRESS\" value=\"".$fullAddress."\">\n".
          "<input type=\"hidden\" name=\"IDX\" value=\"".$this->idx."\">\n".            // needed by ../php/executeCommandOnElement.php
          
        // selectors for the element the schedule is for
          "<fieldset><legend>".$lng->get("scheduleFor")."</legend>\n".
            "<select tabindex=\"".$pge->nextTab++."\" id=\"collLab\" name=\"COLL_LAB\" size=\"1\" class=\"labsys_mop_input\" onchange='isDirty=true'>\n".
            $dropDownOptionsCollLab.
            "</select>".
          "</fieldset>\n".
          
        // the start date selectors
          "<fieldset><legend>".$lng->get("startDate")."</legend>\n".
            "<label for=\"sY\" class=\"labsys_mop_input_field_label_left\">".$lng->get("year")." </label>".
            "<select tabindex=\"".$pge->nextTab++."\" id=\"sY\" name=\"START_YEAR\" size=\"1\" class=\"labsys_mop_input\" onchange='isDirty=true'>\n".
            $this->makeOptions(  date( "Y", mktime(0, 0, 0, 1,   1,   date( "Y", $this->start )-1) ), // one year before schedule
                                 date( "Y", mktime(0, 0, 0, 1,   1,   date("Y")+1) ), // one year after now
                                 date( "Y", $this->start )
                               ).
            "</select> \n".
            
           "<label for=\"sM\" class=\"labsys_mop_input_field_label_left\">".$lng->get("month")." </label>".
            "<select tabindex=\"".$pge->nextTab++."\" id=\"sM\" name=\"START_MONTH\" size=\"1\" class=\"labsys_mop_input\" onchange='isDirty=true'>\n".
            $this->makeOptions( 1, 
                                12,
                                date( "m", $this->start )
                               ).
            "</select> \n".
    
           "<label for=\"sD\" class=\"labsys_mop_input_field_label_left\">".$lng->get("day")." </label>".
            "<select tabindex=\"".$pge->nextTab++."\" id=\"sD\" name=\"START_DAY\" size=\"1\" class=\"labsys_mop_input\" onchange='isDirty=true'>\n".
            $this->makeOptions( 1, 
                                31,
                                date( "d", $this->start )
                               ).
            "</select> \n".
            
           "<label for=\"sH\" class=\"labsys_mop_input_field_label_left\">".$lng->get("hour")." </label>".
            "<select tabindex=\"".$pge->nextTab++."\" id=\"sH\" name=\"START_HOUR\" size=\"1\" class=\"labsys_mop_input\" onchange='isDirty=true'>\n".
            $this->makeOptions( 0, 
                                23,
                                date( "H", $this->start )
                               ).
            "</select> \n".
            
           "<label for=\"sMin\" class=\"labsys_mop_input_field_label_left\">".$lng->get("minute")." </label>".
            "<select tabindex=\"".$pge->nextTab++."\" id=\"sMin\" name=\"START_MINUTE\" size=\"1\" class=\"labsys_mop_input\" onchange='isDirty=true'>\n".
            $this->makeOptions( 0, 
                                59,
                                date( "i", $this->start )
                               ).
            "</select> \n".
            $lng->get("currentValue")." ".date( $lng->get("DateFormat"), $this->start )."\n".
          "</fieldset>\n".
          
        // the start date selectors
          "<fieldset><legend>".$lng->get("stopDate")."</legend>\n".
            "<label for=\"stopY\" class=\"labsys_mop_input_field_label_left\">".$lng->get("year")." </label>".
            "<select tabindex=\"".$pge->nextTab++."\" id=\"stopY\" name=\"STOP_YEAR\" size=\"1\" class=\"labsys_mop_input\" onchange='isDirty=true'>\n".
            $this->makeOptions( date( "Y", mktime(0, 0, 0, 1,   1,   date( "Y", $this->stop )-1) ), // one year before schedule
                                date( "Y", mktime(0, 0, 0, 1,   1,   date("Y")+1) ), // one year after now
                                date( "Y", $this->stop )
                               ).
            "</select> \n".
            
           "<label for=\"stopM\" class=\"labsys_mop_input_field_label_left\">".$lng->get("month")." </label>".
            "<select tabindex=\"".$pge->nextTab++."\" id=\"stopM\" name=\"STOP_MONTH\" size=\"1\" class=\"labsys_mop_input\" onchange='isDirty=true'>\n".
            $this->makeOptions( 1, 
                                12,
                                date( "m", $this->stop )
                               ).
            "</select> \n".
    
           "<label for=\"stopD\" class=\"labsys_mop_input_field_label_left\">".$lng->get("day")." </label>".
            "<select tabindex=\"".$pge->nextTab++."\" id=\"stopD\" name=\"STOP_DAY\" size=\"1\" class=\"labsys_mop_input\" onchange='isDirty=true'>\n".
            $this->makeOptions( 1, 
                                31,
                                date( "d", $this->stop )
                               ).
            "</select> \n".
            
           "<label for=\"stopH\" class=\"labsys_mop_input_field_label_left\">".$lng->get("hour")." </label>".
            "<select tabindex=\"".$pge->nextTab++."\" id=\"stopH\" name=\"STOP_HOUR\" size=\"1\" class=\"labsys_mop_input\" onchange='isDirty=true'>\n".
            $this->makeOptions( 0, 
                                23,
                                date( "H", $this->stop )
                               ).
            "</select> \n".
            
           "<label for=\"stopMin\" class=\"labsys_mop_input_field_label_left\">".$lng->get("minute")." </label>".
            "<select tabindex=\"".$pge->nextTab++."\" id=\"stopMin\" name=\"STOP_MINUTE\" size=\"1\" class=\"labsys_mop_input\" onchange='isDirty=true'>\n".
            $this->makeOptions( 0, 
                                59,
                                date( "i", $this->stop )
                               ).
            "</select> \n".
            $lng->get("currentValue")." ".date( $lng->get("DateFormat"), $this->stop )."\n".
          "</fieldset>\n".

        // comment
          "<fieldset><legend>".$lng->get("comment")."</legend>\n".
            "<textarea tabindex=\"".$pge->nextTab++."\" ".
                      "name=\"COMMENT\" ".
                      "class=\"labsys_mop_textarea\" ".
                      "rows=\"".$cfg->get("DefaultSCommentRows")."\" onchange='isDirty=true'>".
            $this->comment.
            "</textarea>".
          "</fieldset>\n".
                    
        // the save button
          "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("save")."\" accesskey=\"s\" onclick='isDirty=false'>\n".
          
        // element properties
          "<fieldset><legend>".$lng->get("properties")."</legend>\n".
          "<input tabindex=\"".$pge->nextTab++."\" type=\"checkbox\" id=\"saveAsNew\" name=\"SAVEASNEW\" value=\"1\"".( ( $GLOBALS['url']->available("saveAsNew") || ($this->idx == 1) ) ?   " checked=\"checked\""  : '' )." onchange='isDirty=true'>".
          "<label for=\"saveAsNew\" class=\"labsys_mop_input_field_label\">".$lng->get("saveAsNew")."</label>".
          "</fieldset>\n".
    
          "<input tabindex=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("save")."\" onclick='isDirty=false'>\n".            
    
          "</FORM>\n".
          '<script language="JavaScript" type="text/javascript">
          <!--
          if (document.ScheduleEdit) document.ScheduleEdit.collLab.focus();
          //-->
          </script>'.
          EM::editBottomSchedule( $this, $fullAddress ).
        // show all schedules (calendar)
          $this->show( "s1" );
  }
  
  /**
  * This function gets STATICALLY called from ../php/executeCommandOnElement.php?address=[elementID].
  * It handles the $_POSTed data and then redirects to the $_POST['REDIRECTTO'] with the address of the element added.
  */  
  /*static*/ function save(){
    global $sDBI, $url, $lng, $usr;
    
    // check for all necessary values posted
    if ( !isset($_POST['IDX']) || 
         !isset($_POST['REDIRECTTO']) || 
         !isset($_POST['FULLADDRESS']) || 
         !isset($_POST['COLL_LAB']) || 
         !isset($_POST['START_YEAR']) || !isset($_POST['START_MONTH']) || !isset($_POST['START_DAY']) || !isset($_POST['START_HOUR']) || !isset($_POST['START_MINUTE']) || 
         !isset($_POST['STOP_YEAR']) || !isset($_POST['STOP_MONTH']) || !isset($_POST['STOP_DAY']) || !isset($_POST['STOP_HOUR']) || !isset($_POST['STOP_MINUTE']) ||
         !isset($_POST['COMMENT'])
       ){
          trigger_error( $lng->get("notAllNecPosted"), E_USER_ERROR );
          exit;
         }
       
    // Validity (session ID) is checked by the calling script yet.
    
    // only sdchedulers are allowed to make schedules
    if ( !$usr->isOfKind( IS_SCHEDULER ) ){
                                            trigger_error( $lng->get("notAllowed"), E_USER_ERROR );
                                            exit;
                                           }

    // create a new schedule element with the given values
    $newSchedule = new LsElement( $_POST['IDX'],
                                  $_POST['COLL_LAB']{0}, 
                                  substr( $_POST['COLL_LAB'], 1 ),
                                  mktime( $_POST['START_HOUR'], $_POST['START_MINUTE'], 0, $_POST['START_MONTH'], $_POST['START_DAY'], $_POST['START_YEAR'] ),
                                  mktime( $_POST['STOP_HOUR'], $_POST['STOP_MINUTE'], 59, $_POST['STOP_MONTH'], $_POST['STOP_DAY'], $_POST['STOP_YEAR'] ),
                                  /* STOP_SECONDS = 59 => a following schedule can start withou interruption
                                                       => the shortest schedule is 1 minute. */
                                  $_POST['COMMENT'],
                                  ""
                                 );
                                 
    // if $_POST["SAVEASNEW"] is set the element is saved as new element
    if ( isset($_POST["SAVEASNEW"]) && ($_POST["SAVEASNEW"] == "1") ){
                                      $newSchedule->idx = $sDBI->setData( $newSchedule, true );
                                      $url->put( "address=s".$newSchedule->idx );     // the address of the new element
                                     }
    else{ // just save the element
                                      $sDBI->setData( $newSchedule );
                                      $url->put( "address=".$_POST['FULLADDRESS'] );  // the address remains
    }
    
    // some user information
    $url->put( "sysinfo=".urlencode( $lng->get("DataHasBeenSaved")." ".$lng->get("sNumber")." ".$newSchedule->idx ) );
    
    // One could also display the page here but that way it is left to the normal mechanism...
    header( "Location: ".$url->rawLink2( $_POST['REDIRECTTO'] ) );
  }
  

// The next functions are used for the element's manage page. ***********************************************

  /* This element behaves abnormal!!! So if you want to see, what the following functions
   * are intended to do look at another element! 
   */
     
  /**
  * Instead of the field's legend all schedules are shown here for orientation.
  */
  function showPropertyLegend(){
    return $this->show( "s1" );
  }
  
  /**
  * This function is called to display the element's properties.
  * The s implementation has no editable fields.
  *
  * @param $prefix    If given this is put before the element's title (p.e. 1.2.3 title).
  * @param $disabled  Should the checkboxes, radio buttons etc. be disabled?
  * @return string    The html-code of the elements properties horizontally arranged.
  */
  function getPropertyRow( $prefix, $disabled=false ){
    global $lng, $usr, $cfg;
    return $this->idx.": ".date( $lng->get("DateFormat"), $this->start )." - ".date( $lng->get("DateFormat"), $this->stop );
  }
}
?>