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
* As the name says this class implements a ReadOnlyMemory.
* You can read the predifined valuees out.
*/

/**
* @module     ../include/classes/Rom.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
class Rom{
  var $theDataArray;
  function Rom( $dataArray ){
    $this->theDataArray = $dataArray;
  }
    
  /**
  * This method returns the value of the data cell specified by $stringID.
  * @param $stringID  Identifies the data cell.
  * @return           The data cells value or an error message.
  */
  function get( $stringID ){
    if ( isset($this->theDataArray[ $stringID ]) )
      return($this->theDataArray[ $stringID ]);
    else 
      return( 'String "'.$stringID.'" missing!' ); // only use constants here $lng uses this!
  }
  
  /**
  * This method echos the value of the data cell specified by $stringID or an error.
  * @param $stringID  Identifies the data cell.
  */
  function say( $stringID ){
    if ( isset($this->theDataArray[ $stringID ]) )
      echo($this->theDataArray[ $stringID ]);
    else 
      echo( 'String "'.$stringID.'" missing!' ); // only use constants here $lng uses this!
  }
  
  /**
  * Does a stinrId exist?
  * @param $stringID  Identifieer to check.
  * @return BOOL      Does it exist?
  */
  function doesExist( $stringID ){
    return isset( $this->theDataArray[ $stringID ] );
  }
}
?>