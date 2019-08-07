<?php
/** This file handles the XMLHttpRequest of the insertEmojiSelection() function in scripts.js*/

require_once ("../include/init.inc");

if (! ($usr->isOfKind ( IS_USER )) /* valid call? */
    ){
        trigger_error ( $lng->get ( "NotAllowedToMkCall" ), E_USER_ERROR );
        exit ();
}

global $eDBI;

if (!empty($_REQUEST['elemId'])) {
    $elemId = $_REQUEST['elemId'];
} else {
    echo "Missing element ID!"; // TODO: use language strings
}

if (!empty($_REQUEST['uid'])) {
    $uid = $_REQUEST['uid'];
} else {
    echo "Missing user ID!";
}

if (!empty($_REQUEST['emojiId'])) {
    $emojiId = $_REQUEST['emojiId'];
} else {
    echo "Missing emoji ID!";
}

//$emojiDBI = new EmojiDBInterface();
//$insert = $emojiDBI->insertRow($_REQUEST['elemId'], $_REQUEST['uid'], $_REQUEST['emojiId']);

$eDBI->insertRow($_REQUEST['elemId'], $_REQUEST['uid'], $_REQUEST['emojiId']);


echo "OK";

?>