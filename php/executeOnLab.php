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
* Called some l-element pages to call a function.
*  -> $allowedFunctions
*
* ../php/executeCommandOnElement.php can't be used since the function 
* must be called via url and not via post (no form).
*
* @module     ../php/executeCommandOnElement.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $_GET['function']    Function's name.
* @param $_GET['param']       Function's parameters.
* @param $_GET['redirectto']  The redirect after executing the code url. Gets processed by the function.
*/

require( "../include/init.inc" );
require( "../php/getFirstLastFinal.inc" ); $id = $firstFinal{0}; $num = substr( $firstFinal, 1);
require( "../php/getDBIbyID.inc" ); /* -> $DBI */

// functionName => callerRestriction
  $allowedFunctions = Array( "forceLockOn" => IS_USER,              // sets the lock not taking respect of an existing lock
                             "setSeeingUID" => IS_CORRECTOR,        // sets the observed UID (for correctors).
                             "reOpenAllLabInputs" => IS_CORRECTOR,
                             "closeAllLabInputs" => IS_CORRECTOR,
                             "reMapUidTeam" => IS_USER_RIGHTS_EDITOR
                            );

if ( !( isset($_GET['param']) && 
        isset($_GET['function']) &&
        isset($_GET['redirectTo']) &&
        array_key_exists( $_GET['function'], $allowedFunctions ) && 
        $usr->isOfKind( $allowedFunctions[$_GET['function']] ) // retriction fulfilled?
       ) /* valid call? */   
    ){
        trigger_error( $lng->get("notAllowed"), E_USER_ERROR );
        exit;
      }
       
if ( !$element = $DBI->getData2idx( $num ) ){
                                              trigger_error( $lng->get(strtolower( $id )."Number").$num." ".$lng->get("doesNotExist"), E_USER_ERROR );
                                              exit;
                                             }

eval( '$element->'.$_GET['function']."( ".stripslashes( $_GET['param'] )." );" );
?>