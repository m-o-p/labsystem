<?php
/** This file handles the XMLHttpRequest of the updateEmojiMenu() function in scripts.js */

require_once ("../include/init.inc");
global $eDBI;

if (!empty($_REQUEST['elemId'])) {
    $elemId = $_REQUEST['elemId'];
} else {
    echo "Wrong element ID!";
}

$selections = $eDBI->findNumOfSelectionsFor(1, $elemId);
$elem=$selections->fetch_array();
$data["like"] = $elem[0];

$selections = $eDBI->findNumOfSelectionsFor(2, $elemId);
$elem=$selections->fetch_array();
$data["confused"] = $elem[0];

$selections = $eDBI->findNumOfSelectionsFor(3, $elemId);
$elem=$selections->fetch_array();
$data["bored"] = $elem[0];

$selections = $eDBI->findNumOfSelectionsFor(4, $elemId);
$elem=$selections->fetch_array();
$data["dislike"] =  $elem[0];

header('Content-type:application/json');
echo json_encode( $data );
