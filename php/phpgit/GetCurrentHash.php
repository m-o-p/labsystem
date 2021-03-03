<?php
require('../vendor/autoload.php');

function checkCurrentHash($url, $token, $gitid){
    // Token authentication
    $client = new Gitlab\Client();
    $client->setUrl($url);
    $client->authenticate($token, Gitlab\Client::AUTH_HTTP_TOKEN);
    
    // An example API call
    $project = $client->Repositories()->branch($gitid, "master");
    print_r($project['commit']['short_id']);
}
?>
