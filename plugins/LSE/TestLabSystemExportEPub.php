<?php

include_once('LabSystemExportEPub.php');

$epub = LabSystemExportEPub::getInstance();
$epub->save('p', '<p>Something is here</p>', array('par1' => 'val1'));
$epub->save('c', 'This is a place holder for c', array('par2' => 'val2'));


