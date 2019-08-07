<?php
/** This file handles the XMLHttpRequest of the updateEmojiMenu() function in scripts.js */

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

$selections = $eDBI->findNumOfSelectionsFor(1, $elemId);
$elem=$selections->fetch_array();
$data['like'] = $elem[0];

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

?>