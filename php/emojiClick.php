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
    trigger_error ( $lng->get ( "elementMissing" ), E_USER_ERROR );
    exit();
}

if (!empty($_REQUEST['uid'])) {
    $uid = $_REQUEST['uid'];
} else {
    trigger_error ( $lng->get ( "uidMissing" ), E_USER_ERROR );
    exit();
}

if (!empty($_REQUEST['emojiId'])) {
    $emojiId = $_REQUEST['emojiId'];
} else {
    trigger_error ( $lng->get ( "emojiMissing" ), E_USER_ERROR );
    exit();
}

$eDBI->insertRow($_REQUEST['elemId'], $_REQUEST['uid'], $_REQUEST['emojiId']);

?>
