<?php

require_once INCLUDE_DIR . '/../twig_ext/LabsystemExtension.inc';


//Twig
require_once INCLUDE_DIR . '/../vendor/autoload.php';
$twig_loader = new \Twig\Loader\FilesystemLoader(INCLUDE_DIR . '/../templates');
$twig = new \Twig\Environment($twig_loader, [
//  'cache' => '../templates.cache',
  'cache' => false,
]);

$twig->addExtension(new LabsystemExtension());
