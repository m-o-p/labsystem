<?php

$title = $pge->replaceConstants( $pge->title );

global $twig;
global $pge;
echo $twig->render("page.html", array(
  'title' => $title,
  'page' => $pge->show(),
  ));
?>
