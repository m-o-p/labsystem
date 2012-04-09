<?php
/*
 * This php directly shows the generated logo.
 * This is handy for playing around with the logo settings.
 * To use it place this file directly into the plugins directory.
 */
$file='/tmp/test.png';
include('LSE/Util/CoverImageGenerator.php');
$myCG = new LSE_Util_CoverImageGenerator();
$myCG->setSrcImagePath('../pix/ilab2_logo.png');
$myCG->setDstImagePath($file);
$myCG->setText('ilab2 winter 11');
$myCG->generate();
header("Content-length: ".filesize($file));
header("Content-type: ".mime_content_type($file));
readfile($file);
?>