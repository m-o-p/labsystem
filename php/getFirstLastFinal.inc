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
* Extracts $firstFinal and $lastFinal from the address. (Needed by edit.php and view.php).
*
* @module     ../php/getFirstLastFinal.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $_GET['address'] Address to extract $firstFinal and $lastFinal from.
*
* @return $firstFinal
* @return $lastFinal
*/

if ( !isset($_GET['address']) ) trigger_error( $lng->get( "noAddressPosted" ), E_USER_ERROR );

// there might be possible SQL injections...
if ( $_GET['address']!=Ddbc::escapeString($_GET['address']) ) trigger_error( $lng->get( "elementNotExist" ), E_USER_ERROR );

$firstFinal = array_shift ( explode ( ".", $url->get('address') ) );
$lastFinal  = array_pop   ( explode ( ".", $url->get('address') ) );
?>