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
  $allowedFunctions = Array( "reOpenAllLabInputs" => IS_CORRECTOR,
                             "closeAllLabInputs" => IS_CORRECTOR,
                             "reMapUidTeam" => IS_USER_RIGHTS_EDITOR
                            );

if ( !( $GLOBALS['url']->available('function') &&
        $GLOBALS['url']->available('redirectTo') &&
        array_key_exists( $GLOBALS['url']->get('function'), $allowedFunctions ) &&
        $usr->isOfKind( $allowedFunctions[$GLOBALS['url']->get('function')] ) // retriction fulfilled?
       ) /* valid call? */
    ){
        trigger_error( $lng->get("notAllowed"), E_USER_ERROR );
        exit;
      }

if ( !$element = $DBI->getData2idx( $num ) ){
                                              trigger_error( $lng->get(strtolower( $id )."Number").$num." ".$lng->get("doesNotExist"), E_USER_ERROR );
                                              exit;
                                             }

// assemble the parameters
$param_arr = array();
$counter = 0;
while ( $GLOBALS['url']->available('param'. $counter) ) {
    array_push($param_arr, $GLOBALS['url']->get('param'. $counter));
    $counter++;
}
call_user_func_array(array($element, $GLOBALS['url']->get('function')), $param_arr);

?>
