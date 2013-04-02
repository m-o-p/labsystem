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
* This class is instantiated to make a working database connection.
*
* @module     ../include/menu.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/

require_once( INCLUDE_DIR."/classes/DBConnection.inc" );
require_once( INCLUDE_DIR."/configuration.inc" );

class Wdbc extends DBConnection{
  /**
  * @return           A mysql connection identifier or false
  */
  function Wdbc(){
    global $cfg;

    return $this->DBConnection( $cfg->get("WorkingDatabaseHost"),
                                $cfg->get("WorkingDatabaseUserName"),
                                $cfg->get("WorkingDatabasePassWord"),
                                $cfg->get("WorkingDatabaseName"));
  }
}
