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

// An example API call
$project = $client->Repositories()->branch('69064', "master");

$ch = curl_init($url.'/api/v4/projects/'.$id.'/access_requests');                                                                      
curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "POST");                                                                                                                                    
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);                                                                      
curl_setopt($ch, CURLOPT_HTTPHEADER, array(                                                                          
    'PRIVATE-TOKEN: '. $token )                                                                     
);                                                                                                       
                                                                                                                     
$result = curl_exec($ch);

echo $result

?>


