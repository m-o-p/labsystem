<?php
/** This file handles the XMLHttpRequest of the insertEmojiSelection() function in scripts.js
 * Created by PhpStorm.
 * User: desi
 * Date: 10/3/17
 * Time: 2:27 PM
 */

require_once ("../include/init.inc");
global $eDBI;

if (!empty($_REQUEST['elemId'])) {
    $elemId = $_REQUEST['elemId'];
} else {
    echo "Wrong element ID!";
}

if (!empty($_REQUEST['uid'])) {
    $uid = $_REQUEST['uid'];
} else {
    echo "Wrong user ID!";
}

if (!empty($_REQUEST['emojiId'])) {
    $emojiId = $_REQUEST['emojiId'];
} else {
    echo "Wrong emoji ID!";
}

//$emojiDBI = new EmojiDBInterface();
//$insert = $emojiDBI->insertRow($_REQUEST['elemId'], $_REQUEST['uid'], $_REQUEST['emojiId']);

$eDBI->insertRow($_REQUEST['elemId'], $_REQUEST['uid'], $_REQUEST['emojiId']);


echo "OK";

