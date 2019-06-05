<?php
/** This file handles the XMLHttpRequest of the insertEmojiSelection() function in scripts.js
 * Created by PhpStorm.
 * User: desi
 * Date: 10/3/17
 * Time: 2:27 PM
 */

require_once ("../include/init.inc");

if (!empty($_REQUEST['elemId'])) {
    $elemId = $_REQUEST['elemId'];
} else {
    echo "Wrong element ID!";
}

$emojiDBI = new EmojiDBInterface();

$selections = $emojiDBI->findNumOfSelectionsFor(1, $elemId);
$elem=$selections->fetch_array();
$data["like"] = $elem[0];

$selections = $emojiDBI->findNumOfSelectionsFor(2, $elemId);
$elem=$selections->fetch_array();
$data["confused"] = $elem[0];

$selections = $emojiDBI->findNumOfSelectionsFor(3, $elemId);
$elem=$selections->fetch_array();
$data["bored"] = $elem[0];

$selections = $emojiDBI->findNumOfSelectionsFor(4, $elemId);
$elem=$selections->fetch_array();
$data["dislike"] =  $elem[0];

header('Content-type:application/json');
echo json_encode( $data );
