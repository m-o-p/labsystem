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

$project = $client->Repositories()->branch($_REQUEST['id'], 'master');


$indexes = json_decode($client->repositoryFiles()->getRawFile($_REQUEST['id'], 'Index.txt', $project['commit']['short_id']), true);
print_r($indexes);

for ($row = 0; $row < count($indexes); $row++) {
        print_r ($indexes[$row]['lab-name']);    
}
?>


