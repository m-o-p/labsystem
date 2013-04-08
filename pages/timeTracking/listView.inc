<?php
    $pge->put('<ul>');
    foreach( $allBuckets as $key=>$value){
      $pge->put('<li><a href="'.$url->link2('/pages/view.php?address='.$key).'">'.$key.'</a>: '.getTitle($key).'<ul>');
      foreach ($value as $logEntry){
        $pge->put('<li>'.date('r', $logEntry['timestampInt']).': '.$logEntry['action']."\t -&gt; ".$logEntry['idx'].': '.$logEntry['resourceID'].': '.$logEntry['referrerID'].': '.$logEntry['teamNr']);
      }
      $pge->put('</ul>');
    }
    $pge->put('</ul>');
?>