<?php

function templatevars_header() {
  global $url, $usr, $cfg, $runningSystemLanguage, $labsystem_LogInLink, $labsystem_LogOutLink;

  return [
    'display_items' => [
      'login' => !$usr->isOfKind(IS_USER),
      'logout' => $usr->isOfKind(IS_USER),
      'dropdown' => $usr->isOfKind(IS_CORRECTOR) || $usr->isOfKind(IS_EX_SOLUTION_VIEWER) || $usr->isOfKind(IS_CONTENT_EDITOR),
      'hideAnswers' => $usr->isOfKind(IS_CORRECTOR),
      'seeMe-all' => $usr->isOfKind(IS_CORRECTOR),
      'pseudeNames' => $usr->isOfKind(IS_CORRECTOR),
      'exampleSolutions' => $usr->isOfKind(IS_EX_SOLUTION_VIEWER),
      'wysiwyg' => $usr->isOfKind(IS_CONTENT_EDITOR) && $cfg->doesExist('tinyMCE') && ($cfg->get('tinyMCE') == 'TRUE'),
    ],
    'hrefs' => [
      'login' => $url->rawLink2($labsystem_LogInLink),
      'logout' => $url->rawLink2($labsystem_LogOutLink),
      'hideAnswers-toggle' => $url->toggle_link('hideAnswers'),
      'seeMe-all' => $url->rawLink2(NULL, Array('seeMe' => 'all'), removeFromArray($_GET, 'hideAnswers')),
      'clear-seeMe' => $url->rawLink2(NULL, NULL, removeFromArray($_GET, 'seeMe')),
      'pseudoNames-toggle' => $url->toggle_link('pseudoNames'),
      'exampleSolutions-toggle' => $url->toggle_link('hideExampleSolutionInThisTab'),
      'wysiwyg-toggle' => $url->toggle_link('tinyMCE'),
      'lang' => $url->rawLink2('../pages/selectSysLng.php'),
    ],
    'states_active' => [
      'hideAnswers' => !$url->available('hideAnswers'),
      'pseudoNames' => $url->available('pseudoNames'),
      'exampleSolutions' => !$url->available('hideExampleSolutionInThisTab'),
      'wysiwyg' => $url->available('tinyMCE'),
    ],
    'additional_strings' => [
      'username' => $usr->userName,
      'displaying_data_of' => $usr->theSeeingDescr() ? $usr->theSeeingDescr() : '–',
      'lang' => $runningSystemLanguage,
    ]
  ];
}
