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
       !($usr->isOfKind( IS_SCHEDULER ) && $id=="s" ) // or in case of a schedule element schedulers are allowed to delete.
      ) || ($num == 1)                                // prototype 1 can't be deleted
    ) $text = $lng->get("notAllowed");

else{
      if ( !isset($_GET["isConfirmed"]) ){ // not confirmed via script -> do it via page
        header("Location: ".$url->rawLink2( "../pages/confirm.php?text=".urlencode( $lastFinal.$lng->get("confirmDelete") )."&redirectTo=".urlencode( $_SERVER["REQUEST_URI"] ) ) );
        exit;
      }
      require( "../php/getDBIbyID.inc" ); // -> $DBI
      if ( !$DBI->deleteData( $num ) ) $text = $DBI->reportErrors();
      else{
        $text = $lastFinal.": ".$lng->get( "deleted" );
        makeLogEntry( 'edit', 'deleted', $lastFinal );
      }
}

header("Location: ".$url->rawLink2( "../pages/manage.php?address=".$id."&sysalert=".urlencode( $text ) ) );
?>