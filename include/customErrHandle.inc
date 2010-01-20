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
* This module contains a custom error handler that makes the generated
* errors look more gently...
*
* @module     ../include/customErrorHandler.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/

/**
* Displays the error.
*
* @param $cssColor    The errors cssColor (like red, yellow, green) (Color of question mark).
* @param $html        The html-code to be displayed.
* @return
*/
function displayError( $cssColor, $html ){
    echo( '<div>'.
          '<span style="color: '.$cssColor.'; font-size: 1.2em; font-weight: bold;">!</span> '.
          $html.
          '</div>' );
}
/**
* Handles the error.
*
* @param $errno       Kind of the error.
* @param $errstr      An Errorstring.
* @param $errfile     Sourcefille the error occured in.
* @param $errline     Codeline the error occured.
* @return
*/

function myErrorHandler($errno, $errstr, $errfile, $errline) 
{
  if ( defined ( 'E_STRICT' ) && ($errno == constant ( 'E_STRICT' ) ) ) return; // ignore php5 warnings of missing public etc.

  switch ($errno) {
  case E_USER_ERROR: // most severe case gets whole page...
    echo( '<html>'.
          '<head><title>labsys.m-o-p.de-error: ['.$errno.'] '.$errstr.'</title></head>'.
          '<body style="background-color: #444444; padding: 2em;">'
         );
         
    echo( '<div style="color: #666666; font-family: Verdana, Sans-serif; font-size: 5em; margin-bottom:-0.4em;">'.
          'ATTENTION'.
          '</div>'
         );

    echo( '<div style="border: solid gray 1px; background-color: white; color: black; font-family: Verdana, Sans-serif; padding: 0.5em;">' );

// the question mark  
    echo( '<div style="color: red; font-size: 5em; font-weight: bold; margin-top: -0.2em; width: 50px; float: left;">'.
          '!'.
          '</div>'
         );

// the error msg
    echo( "[Fatal error] <b>$errstr</b><br /><br />\n".
          "<span style=\"font-size: 0.9em;\">  On line $errline of file $errfile".
          ", PHP " . PHP_VERSION . " (" . PHP_OS . ")</span><br />\n".
          "aborting...<br />\n" );

// the back link       
    echo( '<div style="margin-top: 1em; padding: 0.2em; text-align: center; border: solid 1px gray;"><a href="javascript: history.back();">&lt; b a c k</a></div>' );
    
    echo( '</div>' );
    echo( '<div style="color: #666666; font-family: Verdana, Sans-serif; font-size: 2em; text-align: right;">'.
          'labsystem.m-o-p.de'.
          '</div>'
         );
    echo( '</body></html>' );
    exit(1);
    break;
  case E_USER_WARNING:
    displayError( '#ffff77', 
                  "<b>$errstr</b>: On line $errline of file $errfile\n"
                 );
    break;
  case E_USER_NOTICE:
    displayError( '#33ff33', 
                  "<b>$errstr</b>: On line $errline of file $errfile\n"
                 );
    break;
  default:
    displayError( '#6666ff', 
                  "<b>$errstr</b>: On line $errline of file $errfile\n"
                 );
    break;
  }
}

// Install the new error handler.
$old_error_handler = set_error_handler("myErrorHandler");
?>