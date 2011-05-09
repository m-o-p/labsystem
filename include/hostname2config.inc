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
 
/* An interesting feature is to use name based hosts 
 * to select the proper configuration files.
 *
 * In the following script the $_GET['config'] gets
 * rewritten according to the hostname.
 */
// ILAB2 -> ilab2_ in config
if (strtoupper(substr($_SERVER['SERVER_NAME'], 0, 5)) == 'ILAB2') $configPrefix='ilab2_';
if (strtoupper(substr($_SERVER['SERVER_NAME'], 0, 4)) == 'ILAB') $configPrefix='ilab_';
if (strtoupper(substr($_SERVER['SERVER_NAME'], 0, 4)) == 'YLAB') $configPrefix='ilab2_';

// to always add the infix of the hostname the following code may help:
// $configPrefix=strtolower(substr($_SERVER['SERVER_NAME'], 0, strpos($_SERVER['SERVER_NAME'], '.'))).'_';

?>
