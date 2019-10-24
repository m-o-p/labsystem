<?php

function templatevars_footer() {
  global $pagecreation_starttime;

  $mtime = explode(" ", microtime());
  $mtime = $mtime[1] + $mtime[0];
  $endtime = $mtime;


  return [
    'additional_strings' => [
      'totaltime' => round(($endtime - $pagecreation_starttime), 4),
      'ticketbody' => urlencode('http' . (isset($_SERVER['HTTPS']) ? 's' : '') . '://' . $_SERVER['HTTP_HOST'] . '/' . $_SERVER['REQUEST_URI']),
      'servername' => $_SERVER['SERVER_NAME'],
    ]
  ];
}
