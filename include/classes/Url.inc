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
* This class is used to handle all url stuff (p.e. add SID, inside=true, config=, ... if necessary).
*/

/**
* An instance of this class gets created at the end: $url.
*
* @module     ../include/classes/Url.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
class Url
{
  var $getParams;

  function __construct(){
    $this->getParams = Array();
  }

  /**
  * Adds the given key value pair to the internal list of Url parameters;
  * these parameters are added by default when using link2 (unless overwritten
  * with a different set of $base_parameters)
  * @param $key    The key to be added.
  * @param $value  The corresponding value
  */
  function put( $key, $value='' ) {
    $this->getParams[$key] = $value;
  }

  /**
  * Use the given GET parameters as the internal list of parameters (discarding
  * any previous changes)
  */
  function setToGetParameters( ){
      $this->getParams = $_GET;
  }

  /**
  * Returns a value from the given GET parameters or the special sysinfo / sysalert parameters from the cookie
  * @param $key   The identifier of the element whos value should be returned.
  */
  function get( $key ){
    if ( isset( $_GET[$key]) ) return $_GET[$key];
    else if (  isset($_SESSION["uid"]) && // Session running?
               ( ($key == 'sysinfo') || ($key == 'sysalert') ) &&
               isset($_SESSION[$key])
             ){
                $val = $_SESSION[$key];
                unset($_SESSION[$key]); // remove value as it is only used by page layout and should be empty now
                return urldecode($val); // If session is running use the cookie for transport
              }
    else return '';
  }


  /**
  * Return the maintained Url parameters as urlencoded list, properly encoded for use in HTML
  */
  function htmlUrlParams( ) {
    return htmlspecialchars(http_build_query($this->getParams));
  }

  /**
  * Is $key present in current queryString?
  * @param $key   The identifier of the element whos existance should be checked
  */
  function available( $key ){
    return isset( $_GET[$key] ) || ( isset($_SESSION["uid"]) && // Session running?
                                     ( ($key == 'sysinfo') || ($key == 'sysalert') ) &&
                                     isset($_SESSION[$key]) && 
                                     (!empty($_SESSION[$key]))
                                    ); // If session is running the cookie can be used for transport;
  }

  /**
  * Removes the given key from the internal list of Url parameters.
  * @param $key  The key to be removed.
  */
  function rem( $key ){
    unset($this->getParams[$key]); // non-existing keys are silently ignored by unset
  }

  /**
  * Copys the given $key with its value from the GET parameters to the internal list of Url
  * paramters. If key is present as GET parameter the request is silently ignored.
  * @param $key     The key to be preserved.
  */
  function preserve( $key ){
    if ( isset( $_GET[$key] ) )
      $this->getParams[$key] = $_GET[$key];
  }

  /**
  * Get the parameters to generate a link; by default this merges the internal list of Url
  * parameters with given $additional_parameters (which can overwrite keys)
  * @param $additionalParams  Add params to the internal list
  * @param $baseParams        Use this array for the base parameters instead of the default Url parameters
  */
  function getMergedParams( $additionalParams, $baseParams=NULL ) {
    if (!isset($baseParams))
      $baseParams = $this->getParams;
    if (isset($additionalParams))
      return $additionalParams + $baseParams; // values in the left-hand array override base params; order relevant!
    else
      return $baseParams;
  }

  /**
  * Adds parameters to a given destination url and returns the complete URL properly encoded;
  * if the link is to be embedded in HTML, use link2 instead, which properly encodes characters
  * with a special meaning in HTML.
  * @param $destUrl           The destionation to link to (defaults to PHP_SELF)
  * @param $additionalParams  Parameters to use in addition to the base parameters (none by default)
  * @param $baseParams        Parameters to use as base (defaults to the maintained list of URL params)
  * @return                   The full link.
  */
  function rawLink2( $destUrl=NULL, $additionalParams=NULL, $baseParams=NULL ){
    if (!isset($destUrl)) {
      $destUrl = $_SERVER['PHP_SELF'];
      //$destUrl = parse_url($_SERVER['REQUEST_URI'], PHP_URL_PATH);
    }
    $getParams = $this->getMergedParams($additionalParams, $baseParams);
    if ( empty($getParams) )
      return $destUrl; /* no query string */
    else
      return $destUrl . '?' . http_build_query($getParams);
  }

  /**
  * Same as rawLink2, but suitable to be used in HTML (properly encoded HTML special characters).
  */
  function link2( $destUrl=NULL, $additionalParams=NULL, $baseParams=NULL ){
    return htmlspecialchars($this->rawLink2( $destUrl, $additionalParams, $baseParams));
  }

  /**
  * Take a complete URL (only url encoded, not HTML encoded) possibly including GET parameters already
  * and add the maintaned Url parameters to it (and possibly additional paramters); the result is again
  * only URLencoded, htmlencoding still needs to be done if required
  * @param $url               an URL possibly containing get parameters (separated by a ?)
  * @param $additionalParams  Parameters to use in addition to the base parameters (none by default)
  * @param $baseParams        Parameters to use as base (defaults to the maintained list of URL params)
  * @return                   a complete new URL, properly URLencoded (but not HTML encoded)
  */
  function rewriteExistingUrl( $url, $additionalParams=NULL, $baseParams=NULL ) {
    $fragment = '';
    $fragStart = strpos($url, '#');
    if ($fragStart !== FALSE) {
      $fragment = substr($url, $fragStart); // includes the # character
      $url = substr($url, 0, $fragStart);
    }
    $params = Array();
    $getStart = strpos($url, '?');
    if ($getStart !== FALSE) {
      parse_str(substr($url, $getStart + 1), $params);
      $url = substr($url, 0, $getStart);
    }
    return $this->rawLink2($url, $this->getMergedParams($additionalParams, $params), $baseParams) . $fragment;
  }
}
$url = new Url();
?>
