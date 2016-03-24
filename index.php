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
 * This script forwards to the startpage.
 * Put the default configuration (the one that appears when no "config="
 * is present in the URL) in the second line where it says else $config = 'demo';
 */
  require( "include/classes/Url.inc" );      // Include url handling and rewriting stuff. => Object $url.
                                             // needed to get parameters from the url ($url->get, ->available)
  require_once( "include/config.inc" );      // contains DEFAULT_INSTANCE required below

  if ( $GLOBALS['url']->available('config') ) $config = $GLOBALS['url']->get('config'); // config provided
   else $config = $DEFAULT_INSTANCE;

  if ( $GLOBALS['url']->available('address') ) $address = $GLOBALS['url']->get('address'); // address provided
   else $address = 'p3'; // use this (startpage is 3) as defaul value

  if ($address == 'accessibleLabs' || $address == 'accessableLabs') header ('Location: pages/accessibleLabs.php?config='.$config.( $GLOBALS['url']->available('inside') ? '&inside=true' : '' ).( $GLOBALS['url']->available('nomenu') ? '&nomenu=true' : '' ) );
  else if ($address == 'register') {
    if (strpos($_SERVER['SERVER_NAME'],'ilab2') === 0){
      header ('Location: pages/view.php?config=useradmin&address=p251'.( $GLOBALS['url']->available('nomenu') ? '&nomenu=true' : '' ) );
    }else{
      header ('Location: pages/view.php?config=useradmin&address=p252'.( $GLOBALS['url']->available('nomenu') ? '&nomenu=true' : '' ) );
    }
  }
  else header ('Location: pages/view.php?address='.$address.'&config='.$config.( $GLOBALS['url']->available('inside') ? '&inside=true' : '' ).'&nomenu=true' );

/*
 * You might create other forwaders to other pages:
 *  For example we use such forwarders to direct the members
 *  to the lab index page etc. lab.php -> pages/view.php?address=p3&config=demo
 */
?>
