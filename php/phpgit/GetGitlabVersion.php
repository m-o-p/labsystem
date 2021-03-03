<?php
require('../../vendor/autoload.php');

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

// An example API call
$version = $client->Version()->show();
print_r($version);
?>
