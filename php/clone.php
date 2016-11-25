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
* Deletes the $lastFinal element from the database.
*
* @module     ../php/clone.php
* @author     Marc-Oliver Pahl
* @version    1.0
*
* @param $_GET['address']  The address of the element to be cloned.
*/
require( "../include/init.inc" );

require( "../php/getFirstLastFinal.inc" ); $id = $lastFinal{0}; $num = substr( $lastFinal, 1);

if ( !$usr->isOfKind( IS_CONTENT_EDITOR ) ){
	trigger_error( $lng->get("notAllowed"), E_USER_ERROR );
	exit;
}else{
      require( "../php/getDBIbyID.inc" ); // -> $DBI
      $element = $DBI->getData2idx( $num, true );
	  $newNum = $element->cloneAndReturnNewIdx();
	  header("Location: ".$url->rawLink2( '../pages/edit.php', Array('address' => $id.$newNum, 'sysinfo' => $id.$num.'->'.$id.$newNum) ));
}
?>
