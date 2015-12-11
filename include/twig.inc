<?php

require_once "../plugins/TWIG/Twig-1.23.1/lib/Twig/Autoloader.php";
Twig_Autoloader::register();

global $twig;
$twig = new Twig_Environment(new Twig_Loader_Filesystem("../templates/"), array());

class Project_Twig_Extension extends Twig_Extension implements Twig_Extension_GlobalsInterface {
  public function getName() {
    return 'labsystem';
  }

  public function getFilters() {
    return array(
      new Twig_SimpleFilter('t', function ($string) {
        global $lng;
        return $lng->get($string);
      }, array('is_safe' => array('html'))),
      new Twig_SimpleFilter('cfg', function ($string) {
        global $cfg;
        return $cfg->get($string);
      }, array('is_safe' => array('html'))),
      new Twig_SimpleFilter('tdate', function ($data) {
        global $lng;
        return date( $lng->get("DateFormat"), $data );
      }),
      new Twig_SimpleFilter('link2', function ($data) {
        global $url;
        return $url->link2($data);
      }, array('is_safe' => array('html'))),
    );
  }

  public function getFunctions() {
    return array(
      new Twig_SimpleFunction('link2', function ($data) {
        global $url;
        return $url->link2($data);
      }, array('is_safe' => array('html'))),
    );
  }

    public function getTests()
    {
      return array(
       new Twig_SimpleTest('allowed', function ($kind) {
         global $usr;
         return $usr->isOfKind( $kind );
       }),
      );
    }

  public function getGlobals()
  {
    return array(
      'IS_GUEST' => IS_GUEST,
      'IS_USER' => IS_USER,
      'IS_USER_RIGHTS_EDITOR' => IS_USER_RIGHTS_EDITOR,
      'IS_CONTENT_EDITOR' => IS_CONTENT_EDITOR,
      'IS_MAIL_SUPPORTER' => IS_MAIL_SUPPORTER,
      'IS_ALL_MAILER' => IS_ALL_MAILER,
      'IS_MAIL_RECEIVER' => IS_MAIL_RECEIVER,
      'IS_SCHEDULER' => IS_SCHEDULER,
      'IS_EX_SOLUTION_VIEWER' => IS_EX_SOLUTION_VIEWER,
      'IS_DB_USER_ADMIN' => IS_DB_USER_ADMIN,
      'IS_CONFIG_EDITOR' => IS_CONFIG_EDITOR,
    );
  }
}

$twig->addExtension(new Project_Twig_Extension());
?>
