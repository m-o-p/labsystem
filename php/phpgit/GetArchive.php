<?php

require('../../vendor/autoload.php');

if (!empty($_REQUEST['id'])) {
    $id = $_REQUEST['id'];
} else {
    exit();
}

if (!empty($_REQUEST['token'])) {
    $token = $_REQUEST['token'];
} else {
    exit();
}

if (!empty($_REQUEST['url'])) {
    $url = $_REQUEST['url'];
} else {
    exit();
}

// Token authentication
$client = new Gitlab\Client();
$client->setUrl($_REQUEST['url']);
$client->authenticate($_REQUEST['token'], Gitlab\Client::AUTH_HTTP_TOKEN);


$params = array(
    'branch' => 'master',
);

$labfile = fopen("labfile.zip", "w") or die("Unable to open file!");
fwrite($labfile , $client->repositories()->archive($_REQUEST['id'],$params,'zip'));
fclose($labfile);

try {
    $phar = new PharData('labfile.zip');
    $phar->extractTo('lab/', null, true); // extract all files, and overwrite
} catch (Exception $e) {
    print_r($e);
}

?>
