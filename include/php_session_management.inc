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
* This module does php session management file based.
*
* The big advantage is that the user cannot manipulate the cookie (change his rights etc.).
* The original code comes from http://de2.php.net/session_set_save_handler
*
* The session.io module must be included in php otherwhise it says "session_set_save_handler()" not found!
*/

define('SESSION_DATA_SAVE_PATH', '/tmp'); // In this path the session info is stored.
// You may set it to something like "[your web dir]/sessiondata"; // without "/" at end

/**
* This cannot be put inside the config file as that gets loaded later.
*
* The path should lay outside the apache served directory so upwards
* your site root.
*
* It must be php writable (the folder!)
*/
$sess_fp = 0;

function open($save_path, $session_name)
{
  //global $sess_save_path;

  //$sess_save_path = $save_path; // Beibehalten
  return(true);
}

function close()
{
  return(true);
}

function read($id)
{
  global $sess_save_path, $sess_fp;

  $sess_file = "$sess_save_path/sess_$id";

  if ( ($sess_fp = @fopen($sess_file, "a+")) && (filesize($sess_file)>0)){
      flock($sess_fp, LOCK_EX);
      fseek($sess_fp, 0, SEEK_SET);
      $sess_data = fread($sess_fp, filesize($sess_file));
      return($sess_data);
  } else {
      return(""); // Must return "" here.
  }
}

function write($id, $sess_data)
{
  global $sess_save_path, $sess_fp;

  $sess_file = "$sess_save_path/sess_$id";
  if ($sess_fp) {
      ftruncate($sess_fp, 0);
      fseek($sess_fp, 0, SEEK_SET);
      $x = fwrite($sess_fp, $sess_data);
      flock($sess_fp, LOCK_UN);
      fclose($sess_fp);
      return $x;
  } else {
      return(false);
  }
}

function destroy($id)
{
  global $sess_save_path;
  if (isset($_COOKIE[session_name()])) setcookie(session_name(), '', time()-42000, '/');
  $sess_file = "$sess_save_path/sess_$id";
  return(@unlink($sess_file));
}

function gc($maxlifetime)
{
  global $sess_save_path;

  foreach (glob("$sess_save_path/sess_*") as $filename) {
    if (filemtime($filename) + $maxlifetime < time()) {
      @unlink($filename);
    }
  }
  return true;
}

session_set_save_handler("open", "close", "read", "write", "destroy", "gc");
?>
