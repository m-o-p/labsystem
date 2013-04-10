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

/*
 * This php generates the ePub logo using ../plugins/LSE/Util/CoverImageGenerator.php.
 * This file is used on the accessibleLabs page to show a cover image nevt to the link.
 */
require('../include/init.inc');
$file=tempnam(sys_get_temp_dir(), 'ePub_CoverImage_');;
include('../plugins/LSE/Util/CoverImageGenerator.php');
$myCG = new LSE_Util_CoverImageGenerator();
// set cover and imprint up:
require( '../include/setupEpubFrontMatter.inc');
$myCG->setSrcImagePath( $epubConfig['coverImage'] );
$myCG->setDstImagePath($file);
// Cover text:
$myCG->setText( $cfg->get('SystemTitle') );
if ( $GLOBALS['url']->available('address') ){
  $address = $GLOBALS['url']->get('address'); // address provided
  require( "../php/getFirstLastFinal.inc" ); $id = $firstFinal{0}; $num = substr( $firstFinal, 1);
  if ($id == 'l'){
    require( "../php/getDBIbyID.inc" ); /* -> $DBI */
    if ( $element = $DBI->getMenuData2idx( $num ) ){
      $myCG->setText( $element->title );
    }
  }
}
$myCG->generate();
header("Content-length: ".filesize($file));
header("Content-type: ".mime_content_type($file));
readfile($file);
?>