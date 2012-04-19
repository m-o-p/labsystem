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
  * If no config is given this config is loaded:
  */
  define('DEFAULT_CONFIG', 'demo');

/**
 * This script forwards to the startpage.
 * Put the default configuration (the one that appears when no "config="
 * is present in the URL) in the second line where it says else $config = 'demo';
 */
  require( "include/classes/Url.inc" );      // Include url handling and rewriting stuff. => Object $url.
                                             // needed to get parameters from the url ($url->get, ->available)

  if ( $GLOBALS['url']->available('config') ) $config = $GLOBALS['url']->get('config'); // config provided
   else $config = DEFAULT_CONFIG; // use as default config: e.g. course32 here or &config=course23 in the URL -> config_course23.ini

  if ( $GLOBALS['url']->available('address') ) $address = $GLOBALS['url']->get('address'); // address provided
   else $address = 'p3'; // use this (startpage is 3) as defaul value

  if ($address == 'accessibleLabs') header ('Location: pages/accessibleLabs.php?config='.$config.( $GLOBALS['url']->available('inside') ? '&inside=true' : '' ));
  else if ($address == 'register') header ('Location: pages/register.php?config='.$config.( $GLOBALS['url']->available('inside') ? '&inside=true' : '' ));
  // The link above leads to the scenario that each instance has its own registration list.
  // In Munich we want to have only one registration list for the whole course.
  // Thus we link to one registration instance for all courses, namely the useradmin.
  // The configurations become vhost_useradmin then for distinction between different
  // course types (e.g. ilab_userabmin and ilab2_useradmin).
  // else if ($address == 'register') header ('Location: pages/register.php?config=useradmin');
  else header ('Location: pages/view.php?address='.$address.'&config='.$config.( $GLOBALS['url']->available('inside') ? '&inside=true' : '' ) );

/*
 * You might create other forwaders to other pages:
 *  For example we use such forwarders to direct the members
 *  to the lab index page etc. lab.php -> pages/view.php?address=p3&config=demo
 */
?>
