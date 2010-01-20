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
* @module     ../php/executeCommandOnElement.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $_GET['address']       Id (only type) of the element the function should be called on. (not passed by post for getFirstLastFinal.inc to work).
* @param $_POST['IDX']          num of the element the function should be called on.
* @param $_POST['FUNCTIONNAME'] Name of the function to be called (can only be out of predefinedset blow).
* @param $_POST['SESSION_ID']   To verify that the user is the user that set the call and is logged in.
*/
require( "../include/init.inc" );
require( "../php/getFirstLastFinal.inc" ); $id = $firstFinal{0};
require( "../php/getDBIbyID.inc" ); /* -> $DBI */

// Only predefined functions are allowed
// Otherwise this would be a security hole since any LOGGED IN USER (sessionId) could post any function...
$allowedFunctions = Array( "saveUserAnswer()",
                           "closeLabInputs()",
                           "setUserAnswerLock()",
                           "checkPreLab()",
                           "saveCorrectorStuff()",
                           "updateStatus()",
                           "save()"
                          );

if ( !( isset($_POST['SESSION_ID']) && 
          ($_POST['SESSION_ID'] != "") && 
          ($_POST['SESSION_ID'] == session_id()) &&
          isset($_POST['FUNCTIONNAME']) &&
          in_array ($_POST['FUNCTIONNAME'], $allowedFunctions) ) /* valid call? */   
       ) trigger_error( $lng->get("notAllowed"), E_USER_ERROR );
       
$num = $_POST['IDX'];

// element not present?
  if ( !$element = $DBI->getData2idx( $num ) )
        trigger_error( $lng->get(strtolower( $id )."Number").$num." ".$lng->get("doesNotExist"), E_USER_ERROR );

// call the function
  eval("\$element->".$_POST['FUNCTIONNAME'].";");
?>