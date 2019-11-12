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
 * This page is the interface to restrict one's rights temporarily.
 * This makes sense to disable some edit functionality (and buttons)...
 *
 * @module     ../pages/myRights.php
 * @author     Marc-Oliver Pahl
 * @copyright  Marc-Oliver Pahl 2005
 * @version    1.0
 *
 * HTML W3 VALIDATED!
 */
require("../include/init.inc");
require(INCLUDE_DIR . "/twig.inc");


$pge->title = $lng->get("titleMyRightsPge");
$pge->matchingMenu = $lng->get("MnuEntryMyRights");
$pge->visibleFor = IS_USER;

$notRemovableRights = intval(IS_USER);
$newUsrRights = $notRemovableRights;


$rights_choices = [];
for ($i = 1; $i <= MAX_USER_ROLE; $i = $i << 1) {
  array_push($rights_choices, $i);

  // does the user have this right?
  if ($usr->isOfKind($i, $usr->userMaximumRights)) {
    if (isset($_POST['UR_' . $i]) && $_POST['UR_' . $i] == $i) {
      $newUsrRights |= $i;
    }
  }
}

if (!empty($_POST) && ($newUsrRights != $usr->userRights)) {
  if (!isset($_POST['thisTabOnly'])) {
    $usr->saveCurrentRights($newUsrRights);
    makeLogEntry('system', 'user rights changed to ' . $newUsrRights . '/' . $usr->userMaximumRights);
    $url->rem('myRights');
  } else {
    //only in this tab...
    $url->put('myRights', $newUsrRights);
    // validity checked above already!
    $usr->userRights = $newUsrRights;
  }
}

$pge->add_context('rights_choices', $rights_choices);
$pge->add_context('rights', (!empty($_POST) ? $newUsrRights : $usr->userRights));
$pge->add_context('nonRemovableRights', [$notRemovableRights,]);
$pge->add_context('thisTabOnly', isset($_POST['thisTabOnly']));

$pge->set_template('pages/myRights.html.twig');

require($cfg->get("SystemPageLayoutFile"));
