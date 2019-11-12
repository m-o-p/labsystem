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

// For utf-8 support:
header('Content-type: text/html; charset=utf-8');
//header('Content-type: text/html; charset=iso-8859-1'); // Seems to fix problems with Umlauten for the moment?!

/**
 * This is the systems page layout.
 * By editing this file you change the entire look.
 *
 * The current look is:
 *
 * |-------------------------------------------------------|
 * |   labsys_mop_topHeader                                |
 * |-------------------------------------------------------|
 * |   labsys_mop_topmostHeader | labsys_mop_topSysinfo    |
 * |-------------------------------------------------------|
 * |      |                                                |
 * |      |                                                |
 * |      |                                                |
 * |      |                                                |
 * | menu |    labsys_mop_content                          |
 * |      |                                                |
 * |      |                                                |
 * |      |                                                |
 * |      |                                                |
 * |      |                                                |
 * |-------------------------------------------------------|
 * |   labsys_mop_bottomFooter                             |
 * ---------------------------------------------------------
 *
 * @module     ../include/page_layout.inc
 * @author     Marc-Oliver Pahl
 * @copyright  Marc-Oliver Pahl 2005
 * @version    1.0
 */

require(INCLUDE_DIR . "/twig.inc");

$template = $twig->load('base.html.twig');

// Unnecessary???
//$loadPrettyPrint = $cfg->doesExist('google-code-prettify') && $cfg->get('google-code-prettify') == 'TRUE';


require(INCLUDE_DIR . "/header.inc");
require(INCLUDE_DIR . "/menu_v2.inc");
require(INCLUDE_DIR . "/footer.inc");


if ($pge->get() == "" and $pge->isVisible()) {
  $pge->renderModeLegacy = false;
} else {
  $pge->renderModeLegacy = true;
}


$template->display([
  'cfg' => $cfg,
  'lng' => $lng,
  'pge' => $pge,
  'url' => $url,
  'usr' => $usr,
  'syslang' => $runningSystemLanguage,
  'nav' => [
    'prev' => $linkToPreviousPage,
    'next' => $linkToNextPage,
    'toc' => $linkToTOCPage,
  ],
  'links' => [
    'login' => $labsystem_LogInLink,
    'logout' => $labsystem_LogOutLink,
  ],
  'parts' => [
    'header' => templatevars_header(),
    'menu' => templatevars_menu(),
    'footer' => templatevars_footer(),
  ],
  'SYSALERT' => isset($SYSALERT) ? addslashes(strip_tags($SYSALERT)) : null,
  'SYSINFO' => isset($SYSINFO) ? addslashes(strip_tags($SYSINFO)) : null,
]);
