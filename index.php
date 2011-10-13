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
  if ( isset( $_GET['config'] ) ) $config = $_GET['config']; // config provided
   else $config = 'demo'; // use as default config: e.g. config_course23.ini -> &config=course23

  if ( isset( $_GET['address'] ) ) $address = $_GET['address']; // address provided
   else $address = 'p3'; // use this (startpage is 3) as defaul value

  if ($address == 'accessableLabs') header ('Location: pages/accessableLabs.php?config='.$config);
  else if ($address == 'register') header ('Location: pages/register.php?config='.$config);
  else header ('Location: pages/view.php?address='.$address.'&config='.$config);

/*
 * You might create other forwaders to other pages:
 *  For example we use such forwarders to direct the members
 *  to the lab index page etc. lab.php -> pages/view.php?address=p3&config=demo
 */
?>
