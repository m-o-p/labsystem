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
* Called by some elements (i, m, l) to call functions with posted data (p.e. save an user's answer).
*
* @module     ../php/getDBIbyID.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $id    The type id of the element the DBI should be loaded of.
*
* @return $DBI  Sets $DBI to the appropriate DBI.
*/
  $prefix = retIfTrue( $id == strtoupper( $id ), "B", "L" ).$id; /* little */
  
  if ( !file_exists ( INCLUDE_DIR."/classes/elements/".$prefix."DBInterface.inc" ) ) 
                                    trigger_error( $id.": ".$lng->get( "elementNotExist" ), E_USER_ERROR );

  if ( !class_exists( $prefix.'DBInterface' ) ) require_once( INCLUDE_DIR."/classes/elements/".$prefix."DBInterface.inc" );
  $DBI = $GLOBALS[ $id.'DBI' ];
?>