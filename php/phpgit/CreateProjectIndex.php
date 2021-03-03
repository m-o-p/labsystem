<?php
require('../../vendor/autoload.php');

if (!empty($_REQUEST['title'])) {
    $id = $_REQUEST['title'];
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

$project = $client->projects()->create($_REQUEST['title'], [
    'description' => $_REQUEST['title'],
    'visibility' => 'private',
]);

$parameters = array(
    'branch' => 'master',
    'commit_message' => 'Create empty index file',
    'file_path' => 'Index.txt',
    'content' => 'none'
);

$index_commit = $client->repositoryFiles()->createFile($project['id'],$parameters);

header('Content-type:application/json');
echo json_encode( $index_commit );

?>


