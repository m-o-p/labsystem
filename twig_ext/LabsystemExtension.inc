<?php

require_once '../vendor/autoload.php';

use \Twig\Extension\AbstractExtension;
use \Twig\TwigFilter;


class LabsystemExtension extends AbstractExtension {
  public function getFilters() {
    return [
      new TwigFilter('replaceLabsystemConstants', [$this, 'replaceLabsystemConstants']),
    ];
  }

  public function replaceLabsystemConstants($text) {
    global $pge;

    return $pge->replaceConstants($text);
  }
}