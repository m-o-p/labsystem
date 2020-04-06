<?php
/** This file handles the XMLHttpRequest of the updateEmojiMenu() function in scripts.js */

require_once ("../include/init.inc");
global $eDBI, $usr;
/**
if (!$usr->isOfKind( IS_USER )){
        trigger_error ( $lng->get ( "NotAllowedToMkCall" ), E_USER_ERROR );
        exit ();
}
*/

if (!empty($_REQUEST['elemId'])) {
    $elemId = $_REQUEST['elemId'];
} else {
    trigger_error ( $lng->get ( "elementMissing" ), E_USER_ERROR );
    exit();
}

$data = [];
$selections = $eDBI->findNumOfSelectionsFor(1, $elemId);
$elem=$selections->fetch_array();
$data['like'] = $elem[0];

$selections = $eDBI->findNumOfSelectionsFor(2, $elemId);
$elem=$selections->fetch_array();
$data["confused"] = $elem[0];

$selections = $eDBI->findNumOfSelectionsFor(3, $elemId);
$elem=$selections->fetch_array();
$data["sleep"] = $elem[0];

$selections = $eDBI->findNumOfSelectionsFor(4, $elemId);
$elem=$selections->fetch_array();
$data["dislike"] =  $elem[0];

header('Content-type:application/json');
echo json_encode( $data );

?>
