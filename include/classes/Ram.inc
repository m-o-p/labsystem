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
* As the name says this class implements a RandomAccessMemory.
* You can read and write the PREDEFINED (on initialization) cells.
*/

/**
* @module     ../include/classes/Ram.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
require_once( INCLUDE_DIR."/classes/Rom.inc" );

class Ram extends Rom{
  function Ram( $dataArray ){
    $this->Rom( $dataArray ); // call the extended classes constructor
  }
  
  /**
  * This method sets the value of the data cell specified by $stringID.
  * @param $stringID  Identifies the data cell.
  * @param $value     The value to be set.
  * @return           Triggers an error when the datacell is non existent.
  */
  function set( $stringID, $value ){
    if ( isset($this->theDataArray[ $stringID ]) )
      $this->theDataArray[ $stringID ] = $value;
    else{
          trigger_error( $stringID." has no value in memory", E_USER_WARNING );
          exit;
         }
  }
}
?>