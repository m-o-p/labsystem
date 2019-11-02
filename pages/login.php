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

require($cfg->get("SystemPageLayoutFile"));

