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
  var $queryString;

  function Url(){
    $this->queryString = "";
  }

  /**
  * Adds the $string to the new queryString. link2() uses this. THE way to add things to the url.
  * @param $string  The content to be added.
  */
  function put( $string ){
    if ( $string != "" ) $this->queryString .= "&".$string;
  }
  
  /**
  * Empties the current querystring.
  */
  function clearQueryString(){
    $this->queryString = "";
  }
  
  /**
  * Returns a value from the current queryString, the current QS (oldQueryString) or the new one (newQueryString).
  * @param $key   The identifier of the element whos value should be returned.
  */
  function get( $key ){
    if ( $key == 'newQueryString' ){
      // transform & -> &amp; for valid HTML
      $newQS = str_replace( '&amp;', '&', substr( $this->queryString, 1 ) ); // if there are any transformed don't recode them!
      $newQS = str_replace( '&', '&amp;', $newQS );
      return $newQS;
    }
    if ( $key == 'newQueryStringRaw' ) return substr( $this->queryString, 1 );
    if ( $key == 'oldQueryString' )return $_SERVER[ 'QUERY_STRING' ];
    if ( isset( $_GET[$key] ) ) return $_GET[$key];
    else return '';
  }
  
  /**
  * Is $key present in current queryString?
  * @param $key   The identifier of the element whos value should be returned.
  */
  function available( $key ){
    return isset( $_GET[$key] );
  }
  
  /**
  * Removes the given string (p.e. foo=bar) from the new QueryString ($querySting).
  * @param $string  The text to be removed.
  */
  function rem( $string ){ 
    $this->queryString = str_replace( "&".$string, "", $this->queryString );
  }
  
  /**
  * Copys the given $key with its value to the new queryString. If not present the $key is added without value ( = void ).
  * @param $key     The key to be preserved.
  */
  function preserve( $key ){
    if ( isset( $_GET[$key] ) ) 
      if ( $_GET[$key] == "" ) $this->put( $key ); else $this->put( $key."=".$_GET[$key] );
  }
  
  /**
  * Rewrites the url by adding the necessary surroundings ( p.e. inside, config= ) set by preserve etc. .
  * This is used for header redirects.
  * "Normal" links on the page need encoded ampersands! -> link2
  * @param $destUrl   The url to link to.
  * @return The full link.
  */
  function rawLink2( $destUrl ){
    if ( $this->get("newQueryString") == "" ) return $destUrl; /* no query string */
    $parsedDest = parse_url( $destUrl ); /* parse given url */
    $newD = ""; /* rebuild it */
    if ( isset($parsedDest['scheme']) ) $newD .= $parsedDest['scheme']."://";
    if ( isset($parsedDest['host']) ) $newD .= $parsedDest['host'];
    if ( isset($parsedDest['user']) ) $newD .= $parsedDest['user'];
    if ( isset($parsedDest['pass']) ) $newD .= ":".$parsedDest['pass'];
    if ( isset($parsedDest['user']) || isset($parsedDest['pass']) ) $newD .= "@";
    if ( isset($parsedDest['path']) ) $newD .= $parsedDest['path']."?";
    if ( isset($parsedDest['query']) ) $newD .= $parsedDest['query']."&";
    $newD .= $this->get("newQueryStringRaw");
    if ( isset($parsedDest['fragment']) ) $newD .= "#".$parsedDest['fragment'];

    return $newD;
  }
  
  /**
   * Also replaces ampersands for correct HTML-Representation.
   *
   * @param $destUrl   The url to link to.
   * @return The full link with replaced ampersands.
  */
  function link2( $destUrl ){
    $newD = $this->rawLink2( $destUrl );
    // transform all (even given) & -> &amp;
    $newD = str_replace( '&amp;', '&', $newD ); // if there are any transformed don't recode them!
    $newD = str_replace( '&', '&amp;', $newD );
    return $newD;
  }
}
$url = new Url();
?>