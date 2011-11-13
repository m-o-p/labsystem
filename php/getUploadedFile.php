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
* Gets called from an input to download uploaded files. Make sure to protect the $cfg->UploadDirectory via htaccess rsp the server's config.
*/

/**
* @module     ../php/getUploadedFile.php
* @author     Sebastian Hoecht, Marc-Oliver Pahl
* @version    1.0
*/

require_once( "../include/init.inc" );
require_once( "../include/classes/elements/LiDBInterfaceAnswers.inc" );

if (($usr->currentTeam != $GLOBALS['url']->get('team')) && !$usr->isOfKind( IS_CORRECTOR )) {
	        trigger_error('Permission denied', E_USER_ERROR);
          exit;
}
if (!ctype_digit($GLOBALS['url']->get('team')) || !ctype_digit($GLOBALS['url']->get('iIdx'))) {
	        trigger_error('Wrong parameters', E_USER_ERROR);
          exit;
}

$fileWhiteList = LiDBInterfaceAnswers::getFiles($GLOBALS['url']->get('team'), $GLOBALS['url']->get('iIdx'));
if (!array_key_exists(urldecode($GLOBALS['url']->get('filename')), $fileWhiteList)) {
          trigger_error('Invalid file', E_USER_ERROR);
          exit;
}

$filePath = LiDBInterfaceAnswers::getUploadDirectory($GLOBALS['url']->get('iIdx'), $GLOBALS['url']->get('team')).'/'.$GLOBALS['url']->get('filename');

if (!file_exists($filePath)) {
	        trigger_error('File doesn\'t exist: '.$filePath, E_USER_ERROR);
          exit;
}

header("Content-Type: ".mime_content_type($filePath));

$fp = fopen($filePath, 'rb');
fpassthru($fp);
fclose($fp);
?>