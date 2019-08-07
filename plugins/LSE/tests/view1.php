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
* View the $firstFinal element from the database.
*
* @module     ../pages/view.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $_GET['address'] Address of element to be shown.
*/
require( "../include/init.inc" );
require( "../php/getFirstLastFinal.inc" ); $id = $firstFinal{0}; $num = substr( $firstFinal, 1);
require( "../php/getDBIbyID.inc" ); /* -> $DBI */

require_once( "../plugins/LSE/Exporter.php");
$epubInstance = LSE_Exporter::getInstance();

$l5 = $DBI->getData2idx( 5 ); // this is multicast
$l6 = $DBI->getData2idx( 6 ); // this is static routing
 
$epubConfig = array(
    'title'  => 'iLab2',
    'author' => 'Multiple Authors',
);

$epubInstance->setOptions($epubConfig);

$l5->show( 'l5.epub', '' );
$l6->show( 'l6.epub', '' );

$epubInstance->render();
exit(0);

makeLogEntry( 'view', 'show', $url->get("address") );
  
//require( $cfg->get("SystemPageLayoutFile") );
?>
