<?php
/*
 * This php generates theePub logo using ../plugins/LSE/Util/CoverImageGenerator.php.
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
$myCG->setText( $cfg->get('SystemTitle') );
$myCG->generate();
header("Content-length: ".filesize($file));
header("Content-type: ".mime_content_type($file));
readfile($file);
?>