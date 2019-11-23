<?php
/**
 *  labsystem.m-o-p.de -
 *                  the web based eLearning tool for practical exercises
 *  Copyright (C) 2010  Marc-Oliver Pahl
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * Login page.
 *
 * @module     ../pages/login.php
 * @author     Marc-Oliver Pahl
 * @copyright  Marc-Oliver Pahl 2005
 * @version    1.0
 */
require("../include/init.inc");
require(INCLUDE_DIR . "/twig.inc");

$url->rem("inside"); // This page is for checking in so you are supposed not to be in!

$pge->title = $lng->get("TitleLogInPage");
$pge->matchingMenu = $lng->get("MnuEntryLogIn");

$pge->set_template('pages/login.html.twig');

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
  authenticate();
}

require($cfg->get("SystemPageLayoutFile"));

function authenticate() {
  global $cfg, $url, $pge, $lng, $uDBI, $NEWSESSION;

  if (!isset($_POST['USERNAME']) || !isset($_POST['PASSWORD'])) {
    trigger_error($lng->get('NotAllNecValPosted'), E_USER_ERROR);
    return;
  }

  require_once(INCLUDE_DIR . "/classes/DBInterfaceUser.inc");
  require_once(INCLUDE_DIR . "/classes/DBInterfaceUserRights.inc");

  // preserve the current url since we will probably link back (p.e. to give an error) or add something to the url.
  $url->setToGetParameters();

  if (!($authUserData = $uDBI->authenticate($_POST['USERNAME'], $_POST['PASSWORD']))) {
    // not authenticated
    $pge->add_context('errmsg', $lng->get("AlertWrongUsrPw"));
    return;
  }

  // authenticated
  if (isset($_POST['stayLoggedIn'])) {
    // Set cookie lifetime to 1 year
    session_set_cookie_params(365 * 24 * 60 * 60);
  }
  $NEWSESSION = $authUserData["uid"];
  require(INCLUDE_DIR . "/session.inc");

  // The following field is for security reasons:
  // If the configuration would not be checked you could change the field in the url and would be logged on with your
  // current rights for the different configuration...
  $_SESSION["config"] = $url->get('config');

  makeLogEntry('system', 'login');
  makeLogEntry('system', 'loginLog');
  $GLOBALS['Logger']->logToDatabase('system', logActions::login);
  // Link to the after login page from the config file or to
  if (isset($_POST['REDIRECTTO'])) {
    header("Location: " . $url->rewriteExistingUrl($_POST['REDIRECTTO']));
  } else {
    header("Location: " . $url->rewriteExistingUrl($cfg->get("AfterLogInPage")));
  }
}

