<?php

require_once("iDrawable.inc");
require_once("LabNode.inc");

/**
 * The Progress class gets from the logger the times tracked for a team or user. The base class is
 * abstract and contains logic, which converts the logged events to tracked Timespans.
 * 
 * @author Fabian Oehlmann
 *
 */
abstract class Progress implements iDrawable {
  const timeoutRegisterStr = '+60 minutes'; // After what time a tracked attention should timeout
  const timeoutAddStr = '+0 minutes';       // How much time should be added instead of the timed out attention
  
  private $nodeList;
  private $logger;
  protected $labIDX;
  protected $lDBI;
  private $schedule;

  /**
   * Gets the relevant addresses of the Lab or PreLab and creates an array of according LabNodes and
   * queries the events from the logger. Then it performs the attention tracking and calculates the
   * intervals spent between questions. 
   * 
   * @param unknown $Logger The logger
   * @param unknown $labIDX The lab in question
   */
  public function __construct($Logger, $labIDX) {
    $this->labIDX = $labIDX;
    $this->logger = $Logger;
    $this->lDBI = $GLOBALS['lDBI']->getData2idx($labIDX);
    $this->schedule = $GLOBALS['sDBI']->getSchedule4('l', $labIDX);
    
    $labAddresses = $this->labAddresses();
    $this->nodeList = $this->createNodes($labAddresses, $labIDX);
    
    $eventList = $this->getEventlist();
    $this->expandEntries($eventList, $labAddresses);
    
    $this->runAttentionTracking($eventList, $this->nodeList);
    $this->calculateIntervals($this->nodeList);
  }

  /**
   * (non-PHPdoc)
   * @see iDrawable::getNodeList()
   */
  public function getNodeList() {
    return $this->nodeList;
  }

  /**
   * The sum of credits which is possible to attain in this Lab or PreLab
   * 
   * @return number
   */
  public function getSumOfPossibleCredits() {
    $progSumPossibleCredits = 0;
    foreach ($this->nodeList as $node) {
      if ($node->getType() == 'i' || $node->getType() == 'm') {
        $progSumPossibleCredits += $node->getPossibleCredits();
      }
    }
    return $progSumPossibleCredits;
  }

  /**
   * The sum of credits which was given in this Lab or PreLab
   * 
   * @return number
   */
  public function getSumOfGivenCredits() {
    $progSumGivenCredits = 0;
    foreach ($this->nodeList as $node) {
      if ($node->getType() == 'i' || $node->getType() == 'm') {
        $progSumGivenCredits += $node->creditsGiven;
      }
    }
    return $progSumGivenCredits;
  }

  /**
   * The total sum of times spent between answering the questions
   * 
   * @return number
   */
  public function getSumOfIntervalTimes() {
    $progTotalSeconds = 0;
    foreach ($this->nodeList as $node) {
      if ($node->getType() == 'i' || $node->getType() == 'm') {
        $progTotalSeconds += $node->timeInterval;
      }
    }
    return $progTotalSeconds;
  }

  /**
   * (non-PHPdoc)
   * @see iDrawable::getUserString()
   */
  abstract public function getUserString();

  /**
   * (non-PHPdoc)
   * @see iDrawable::getTitle()
   */
  abstract public function getTitle();

  /**
   * The userid or team number of who did the Lab or Prelab
   */
  abstract protected function getUser();

  /**
   * The user restriction used in the SQL query
   */
  abstract protected function userRestriction();

  /**
   * The relevant addresses of the lab (i.e. the ones from the Lab or Prelab)
   */
  abstract protected function labAddresses();

  /**
   * Retrieves the relevant events from the logger
   * 
   * @return multitype:unknown
   */
  private function getEventList() {
    $startTime = $this->schedule['start'];
    $stopTime = $this->schedule['stop'];
    
    $where = $this->userRestriction();
    $where .= ' AND UNIX_TIMESTAMP(timestamp) > "' . $startTime .
         '" AND UNIX_TIMESTAMP(timestamp) < "' . $stopTime . '"';
    
    $query = $this->logger->myDBC->mkSelect("*, UNIX_TIMESTAMP(timestamp) as timestampInt", 
        $this->logger->myTable, $where);
    $eventList = array();
    while ($rep = $query->fetch_array()) {
      if ($rep)
        $eventList[] = $rep;
    }
    return $eventList;
  }
  
  /**
   * Classifies the events by the role in which the event occured and whether it's an opening or
   * closing event
   * 
   * @param unknown $event
   * @return multitype:number
   */
  private static function determineRoleAndAO($event) {
    $action = $event['action'];
    if ($action == \logActions::logout) {
      return [
      RoleTypes::viewer,
      AO::end
      ];
    }
    if ($action == \logActions::login) {
      return [
      RoleTypes::viewer,
      AO::start
      ];
    }
    if ($action == \logActions::cOpen || $action == \logActions::pOpen ||
    $action == \logActions::iOpen || $action == \logActions::mOpen ||
    $action == \logActions::lOpen) {
      return [
      RoleTypes::viewer,
      AO::start
      ];
    } elseif ($action == \logActions::cClose || $action == \logActions::pClose ||
        $action == \logActions::iClose || $action == \logActions::mClose ||
        $action == \logActions::lClose) {
      return [
      RoleTypes::viewer,
      AO::end
      ];
    } elseif ($action == \logActions::cEditorOpen || $action == \logActions::pEditorOpen ||
        $action == \logActions::iEditorOpen || $action == \logActions::mEditorOpen ||
        $action == \logActions::lEditorOpen) {
      return [
      RoleTypes::editor,
      AO::start
      ];
    } elseif ($action == \logActions::cEditorSave || $action == \logActions::pEditorSave ||
        $action == \logActions::iEditorSave || $action == \logActions::mEditorSave ||
        $action == \logActions::lEditorSave) {
      return [
      RoleTypes::editor,
      AO::end
      ];
    } elseif ($action == \logActions::iStudentOpen || $action == \logActions::mStudentOpen) {
      return [
      RoleTypes::student,
      AO::start
      ];
    } elseif ($action == \logActions::iStudentSave || $action == \logActions::iStudentCancel ||
        $action == \logActions::mStudentSave || $action == \logActions::mCheck) {
      return [
      RoleTypes::student,
      AO::end
      ];
    } elseif ($action == \logActions::iCorrectorOpen) {
      return [
      RoleTypes::corrector,
      AO::start
      ];
    } elseif ($action == \logActions::iCorrectorSave || $action == \logActions::iCorrectorCancel ||
        $action == \logActions::iCorrectorQuick) {
      return [
      RoleTypes::corrector,
      AO::end
      ];
    }
  }

  /**
   * Creates the LabNodes from the lab addresses
   * 
   * @param unknown $allResources The list of lab addresses or resource names
   * @return LabNode              The resulting list of LabNodes
   */
  private function createNodes($allResources) {
    foreach ($allResources as $resource) {
      $newNode = new LabNode($resource);
      if ($newNode->getType() == 'i' || $newNode->getType() == 'm') {
        $newNode->creditsGiven = $newNode->getGivenCredits($this->getUser());
      }
      $resourceList[$resource] = $newNode;
    }
    return $resourceList;
  }
  
  /**
   * Expands entries with a '~' character to have a full resource name
   * 
   * @param unknown $eventList      The list of relevant events
   * @param unknown $resourceNames  The addresses in the relevant part of the lab
   */
  private function expandEntries(&$eventList, $resourceNames) {
    foreach ($eventList as &$event) {
      $id = $event['resourceID'];
      if (strpos($id, '~')) {
        $needle = end(explode('~', $id));
        foreach ($resourceNames as $name) {
          if (strpos($name, $needle))
            $event['resourceID'] = $name;
        }
      }
    }
  }

  /**
   * The actual attention tracking logic.
   * 
   * Goes linearly through the list of events. If an opening event occurs it is assumed that the
   * participant's attention is on that element. If a closing event occurs the attention tracking
   * stops for that element. Furthemore, the attention tracking stops after a timeout is reached or
   * a different element is opened.
   * 
   * @param unknown $eventlist  The list of events
   * @param unknown $nodelist   The list of LabNodes to work with
   */
  private function runAttentionTracking($eventlist, $nodelist) {
    $lastOpenEvent = NULL;
    foreach ($eventlist as $event) {
      list($curRole, $curAO) = $this->determineRoleAndAO($event);
      
      if ($curRole != RoleTypes::viewer && $curRole != RoleTypes::student) { // only work with student perspective
        continue;
      }
      if (! array_key_exists($event['resourceID'], $nodelist)) { // ignore events for addresses that are not available
        continue;
      }
      
      $timeout = strtotime(Progress::timeoutRegisterStr, $lastOpenEvent['timestampInt']);
      if ($lastOpenEvent && $timeout < $event['timestampInt']) {
        // last open thing timed out
        $lastPotCloseEvent = $nodelist[$event['resourceID']]->lastPotClose;
        
        if ($lastPotCloseEvent && $lastPotCloseEvent['timestampInt'] < $timeout &&
             $lastOpenEvent['timestampInt'] < $lastPotCloseEvent['timestampInt']) {
          // close with potclose
          list($lastRole, $lastAo) = $this->determineRoleAndAO($lastPotCloseEvent);
          $nodelist[$lastOpenEvent['resourceID']]->addTimespan($lastOpenEvent['timestampInt'], 
              $lastPotCloseEvent['timestampInt'], $lastRole);
        } else {
          // close with timeout
          list($lastRole, $lastAo) = $this->determineRoleAndAO($lastOpenEvent);
          $nodelist[$lastOpenEvent['resourceID']]->addTimespan($lastOpenEvent['timestampInt'], 
              strtotime(Progress::timeoutAddStr, $lastOpenEvent['timestampInt']), $lastRole);
        }
        $lastOpenEvent = NULL;
      }
      
      if ($curAO == AO::start) {
        if (! $lastOpenEvent) {
          $lastOpenEvent = $event;
          continue;
        }
        if ($lastOpenEvent['resourceID'] != $event['resourceID'] ||
             $lastOpenEvent['action'] != $event['action']) {
          list($lastRole, $lastAo) = $this->determineRoleAndAO($lastOpenEvent);
          $nodelist[$lastOpenEvent['resourceID']]->addTimespan($lastOpenEvent['timestampInt'], 
              $event['timestampInt'], $lastRole);
          $nodelist[$lastOpenEvent['resourceID']]->lastPotClose = NULL;
          $lastOpenEvent = $event;
        }
      }
      
      if ($curAO == AO::end) {
        $nodelist[$event['resourceID']]->lastPotClose = $event;
      }
    }
  }
  
  /**
   * Calculates the time intervals tracked between question elements. The Timespans are sorted by
   * their beginning and then processed. Timespans are summed up until a Timespan was recorded in a
   * student role, which is when a i- or m-element was worked with. The until then summed time is
   * then added to the time spent on answering this question. Time spent after answering the last
   * question is not used, as it apparently did not contribute to giving answers anymore.
   * 
   * @param unknown $nodeList The list of nodes containing the Timespans
   */
  private function calculateIntervals($nodeList) {
    $timeSpanList = [];
    foreach ($nodeList as $id => $node) {
      $timeSpanList = array_merge($timeSpanList, $node->getTimespans());
    }
    usort($timeSpanList, function ($a, $b) {
      return $a->start - $b->start;
    });
    
    $sum = 0;
    foreach ($timeSpanList as $ts) {
      if ($ts->role != RoleTypes::student) {
        $sum += $ts->length();
      } else {
        $nodeList[$ts->resource]->timeInterval += $sum + $ts->length();
        $sum = 0;
      }
    }
  }

}

?>
