#!/usr/bin/php5
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
 *
 * This script is meant to be called by the shell to backup the databases of a certain instance.
 *
 */

if (php_sapi_name() != 'cli'){
  echo('Script must be run from CLI.'.PHP_EOL);
  exit;
}

if ($argc < 3){
  echo('Backups the database of the given instance.'.PHP_EOL.PHP_EOL);
  echo('Usage: '.$argv[0].' [configFileID] [destination directory]'.PHP_EOL);
  echo('  [configFileID]           Which configuration to take, e.g. ilab2_2012ws.'.PHP_EOL);
  echo('  [destination directory]  Where to store the backup, e.g. /tmp/backupIlab.'.PHP_EOL);
}

$instanceConfigID = $argv[1];
$destinationDirectory = $argv[2].'/'.date('Y-m');

define( "INCLUDE_DIR", "../include" );
require( "../include/configuration.inc" );

echo('Backing the instance '.$instanceConfigID.' ('.$currentConfig.') up to the directory '.$destinationDirectory.'.'.PHP_EOL);
if (!is_dir($destinationDirectory)){
  mkdir($destinationDirectory, 448, true);
}

$dataBasesToBackup = array("Working", "Data", "User");

foreach ($dataBasesToBackup as $value){
  $destination = $destinationDirectory.'/'.$cfg->get($value.'DatabaseName').'__'.date('Y-m-d_H-i').'.sql.gz';
  echo($value.'DataBase: '.$cfg->get('WorkingDatabaseName').' -> '.$destination.PHP_EOL);
  passthru( 'mysqldump -u"'.$cfg->get($value.'DatabaseUserName').
            '" -p"'.$cfg->get($value.'DatabasePassWord').
            '" -h"'.$cfg->get($value.'DatabaseHost').
            '" "'.$cfg->get($value.'DatabaseName').'" | gzip >"'.$destination.'"'.PHP_EOL);
}
?>