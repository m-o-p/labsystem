<?php
require('../../vendor/autoload.php');



// Token authentication
$client = new Gitlab\Client();
$client->setUrl('https://gitlab.lrz.de/');
$client->authenticate('iy1xG3PXm-zHLid4ZvUG', Gitlab\Client::AUTH_HTTP_TOKEN);

$project = $client->Repositories()->branch('74921', 'master');


$indexes = json_decode($client->repositoryFiles()->getRawFile('74921', 'Index.txt', $project['commit']['short_id']), true);

print_r($indexes);

if (is_null($indexes)){
    $indexes = array(
        array(
            'lab-name' => 'index',
            'lab-id' => '74921',
        ),
        array(
            'lab-name' => 'The basics',
            'lab-id' => '74760',
        )
    );
}
$indexes = json_encode($indexes);
print_r($indexes);

$parameters = array(
    'branch' => 'master',
    'commit_message' => 'Create empty index file',
    'file_path' => 'Index.txt',
    'content' => $indexes
);

$index_commit = $client->repositoryFiles()->updateFile('74921',$parameters);
print_r($index_commit)

?>


