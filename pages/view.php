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
* View the $firstFinal element from the database.
*
* @module     ../pages/view.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $_GET['address'] Address of element to be shown.
*/
require( "../include/init.inc" );
require( "../php/getFirstLastFinal.inc" ); $id = $firstFinal{0}; $num = substr( $firstFinal, 1);
require( "../php/getDBIbyID.inc" ); /* -> $DBI */

if ( !$element = $DBI->getData2idx( $num ) )
      trigger_error( $lng->get(strtolower( $id )."Number").$num." ".$lng->get("doesNotExist"), E_USER_ERROR );

$pge->title        = $element->title;
$pge->matchingMenu = $element->getMatchingMenu();



$pge->put( $element->show( $url->get("address"), "" ) );

makeLogEntry( 'view', 'show', $url->get("address") );
  
require( $cfg->get("SystemPageLayoutFile") );
?>
