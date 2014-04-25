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
*/
$ERRORMAIL_RECEIVER = ''; // If you enter a valid email address here and you have mailing configured you can receive an error report

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
          htmlentities($html, ENT_QUOTES | ENT_IGNORE).
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

  $errno = htmlentities( $errno, ENT_QUOTES | ENT_IGNORE  );
  $errstr = htmlentities( $errstr, ENT_QUOTES | ENT_IGNORE  );
  $errfile = htmlentities( $errfile, ENT_QUOTES | ENT_IGNORE );
  $errline = htmlentities( $errline, ENT_QUOTES | ENT_IGNORE );
  switch ($errno) {
  case E_USER_ERROR: // most severe case gets whole page...
    echo( '<html>'.
          '<head><title>labsys.m-o-p.de-error: ['.$errno.'] '.$errstr.'</title></head>'.
          '<body style="background-color: #444444; padding: 2em;">' );

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
    echo( "<p>[Fatal error] <b>$errstr</b><br /><br />\n".
          "<span style=\"font-size: 0.9em;\">  On line $errline of file $errfile".
          ", PHP " . PHP_VERSION . " (" . PHP_OS . ")</span><br />\n".
          "aborting...</p>" );

// the back link
    echo( '<div style="margin-top: 1em; padding: 0.2em; text-align: center; border: solid 1px gray;"><a href="javascript: history.back();">&lt; b a c k</a></div>' );

// support ticket link
    echo( '<div style="margin-top: 1em; padding: 0.2em; text-align: center; border: solid 1px gray;"><a href="http://sourceforge.net/p/labsystem/bugtracker/new/">open a support ticket &gt;</a></div>' );

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

//This error handler sends an email with all relevant information
function mail_error_handler($number, $message, $file, $line, $vars){
  global $ERRORMAIL_RECEIVER;

  if ( isset($ERRORMAIL_RECEIVER) && $ERRORMAIL_RECEIVER != ''
       && isset($_SESSION["mailAddress"]) ){ // mail only errors user experience that are logged in
    $email = 'URL causing the error:<br>'.
             'http'.( isset($_SERVER['HTTPS']) ? 's' : '').'://'.$_SERVER['SERVER_NAME'].$_SERVER['REQUEST_URI']."<br>".
             ( isset($_SERVER['HTTP_REFERER']) ? 'Last page before:<br>'.$_SERVER['HTTP_REFERER'].'<br>' : '').
             ( isset($_SESSION["uid"]) ? 'User experiencing the error:  '.$_SESSION["foreName"].' '.$_SESSION["surName"].' ('.$_SESSION["mailAddress"].')<br>'.
                                         'Rights while error occured:   '.$_SESSION["userRights"]."<br>"
                                         : '')."
                  <p>An error ($number) occurred on line
                  <strong>$line</strong> and in the <strong>file: $file.</strong>
                  <p> $message </p>";

    $email .= "<pre>" . print_r($vars, 1) . "</pre>";

    $headers = 'Content-type: text/html; charset=iso-8859-1' . "\r\n";

    // Email the error to someone...
    if ( ($number !== E_NOTICE) && ($number < 2048) ){
      if (isset($_SERVER['HTTP_FROM']) && $_SERVER['HTTP_FROM'] == 'googlebot(at)googlebot.com'){
        // The Googlebot seems to play around with parameters causing many errors for missing (unlinked) pages.
        // Not to get bothered by them it is excluded here.
        // file_put_contents( '/tmp/googlebot_urls.txt', 'http'.( isset($_SERVER['HTTPS']) ? 's' : '').'://'.$_SERVER['SERVER_NAME'].$_SERVER['REQUEST_URI']."\r\n", FILE_APPEND );
        // error_log($email, 3, '/tmp/googlebot.txt');
      } else {
        error_log($email, 1, $ERRORMAIL_RECEIVER, $headers);
      }
    }

    // Make sure that you decide how to respond to errors (on the user's side)
    // Either echo an error message, or kill the entire project. Up to you...
    // The code below ensures that we only "die" if the error was more than
    // just a NOTICE.
    //if ( ($number !== E_NOTICE) && ($number !== E_WARNING) && ($number < 2048) ) {
    //      die("There was an error. Sorry.");
    //}
  }

  // call the "normal" graphical error handler for the user:
  myErrorHandler($number, $message, $file, $line, $vars);
}


// Install the new error handler.
$old_error_handler = set_error_handler('mail_error_handler');
?>