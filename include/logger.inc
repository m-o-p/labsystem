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
* This module contains the log file functions.
*
* @module     ../include/logger.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2010
* @version    1.0
*/
if ( !isset($configPrefix) ) $configPrefix='';
$theLogFile = '/srv/www/log/'.$configPrefix.$url->get('config').'_'.date("Y-m-d").'.log';

/**
* Returns $consequent, if the test is true. Otherwise $alternative.
* (p.e. return 'foo'.retIfTrue( ($foo>$bar), ' is bigger', ' is smaller or equal' );
*
* The big disadvantage to if is that test and consequent are passed by value and therefore evaluated.
* ONLY works for logged in people!
*
* @param $actor       Entity causing the logged event
* @param $operation   A keyword describing the action
* @param $alternative The alternative.
* @return             The consequent or the alternative.
*/
function makeLogEntry( $group, $operation, $resource='' ){ 
global $theLogFile, $doLogging;

  if (!isset($doLogging) || !$doLogging || !isset($_SESSION)) return; // only log for authenticated users

	if ( !(
          $logFile = fopen( $theLogFile, 'a+' )     // w ^= write and create (if not exist)
                                                      ) )
   // alert file open error
    $url->put( 'sysalert='.urlencode( $lng->get('errorOpeningFile').' ('.$theLogFile.')' ) );

  $logEntry = date("Y-m-d\tH:i:s")."\t".
              $_SESSION["uid"]."\t".
              $_SESSION["surName"]."\t".
              $_SESSION["foreName"]."\t".
              $_SESSION["userName"]."\t".
              $_SESSION["currentTeam"]."\t".
              $_SESSION["userRights"]."\t".
              $group."\t". /* system, prelab, lab */
              $operation."\t". /* login, save, open */
              $resource."\r\n";
  // timestamp actorUID actorClearname team action resource
  //echo( $logEntry.'<br>' );

	if ( !(
          fwrite( $logFile, $logEntry )
                                                      ) )
     // alert that it didn't work
    $url->put( 'sysalert='.urlencode( $lng->get('errorWritingFile').' ('.$theLogFile.')' ) );
  
  fclose($logFile);
}
?>