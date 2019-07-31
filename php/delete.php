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
* @module     ../php/delete.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $_GET['address']  The address of the element to be deleted.
*/
require( "../include/init.inc" );

require( "../php/getFirstLastFinal.inc" ); $id = $lastFinal{0}; $num = substr( $lastFinal, 1);

if ( ( !$usr->isOfKind( IS_CONTENT_EDITOR ) &&       // only content editors
       !($id=="s" && $usr->isOfKind( IS_SCHEDULER )) // or in case of a schedule element schedulers are allowed to delete.
      ) || ($num == 1)                                // prototype 1 can't be deleted
    ) $text = $lng->get("notAllowed");

else{
      if ( !$GLOBALS['url']->available("isConfirmed") ){ // not confirmed via script -> do it via page
        header("Location: ".$url->rawLink2( '../pages/confirm.php', Array('text' => $lastFinal.$lng->get("confirmDelete"), 'redirectTo' => $_SERVER["REQUEST_URI"]) ) );
        exit;
      }
      if ($id=='s' || $id=='l' ){
      	// regular delete that works for all element classes
      	// we only want to delete s elements as for the others it makes no sense.
	      require( "../php/getDBIbyID.inc" ); // -> $DBI
	      $ret = $DBI->deleteData( $num );
	      if ($ret === false) {
	      	$text = 'Mysql Error: ' . $userDBC->link->error;
	      }else{
	        $text = $lastFinal.": ".$lng->get( "deleted" );
	        makeLogEntry( 'edit', 'deleted', $lastFinal );
	        $url->put('sysalert', $text);
	        $destinationAddress=$id.'1';
	      }
      }else{
      	// For non-s elements: remove them from the next enclosing collection:
      	$fulladdress = $url->get('address');
      	// Remove the entity to be deleted:
      	$remainingAddr = substr($fulladdress, 0, strpos($fulladdress, $id.$num));
      	// Identify last c:
      	$posLastC = strrpos($remainingAddr, 'c');
      	if (!$posLastC){
      		$posLastC = strrpos($remainingAddr, 'C');
      	}
      	if ($posLastC){
      		$destinationAddress = substr($remainingAddr,0,strpos($remainingAddr, '.', $posLastC));
  			$url->put('deleteChild', $id.$num);
      	}else{
      		$destinationAddress = $fulladdress; // do nothing.
      	}
      }
}


header("Location: ".$url->rawLink2( '../pages/view.php', Array('address' => $destinationAddress) ).'#'.$destinationAddress );
?>
