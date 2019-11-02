<?php

require_once '../twig_ext/LabsystemExtension.inc';


//Twig
require_once '../vendor/autoload.php';
$twig_loader = new \Twig\Loader\FilesystemLoader('../templates');
$twig = new \Twig\Environment($twig_loader, [
//  'cache' => '../templates.cache',
  'cache' => false,
]);

$twig->addExtension(new LabsystemExtension());
