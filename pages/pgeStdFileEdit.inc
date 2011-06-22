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
* Generates the standard file edit page (used to edit the user stylesheet and the menu).
*
* Attention: The savin script ../php/saveFile.php saves only predefined files! (security)
*            So don't forget to add your files there if you plan to use this...
*
* @module     ../pages/pgeStdFileEdit.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $file2edit The relative path of the file to be edited.
*/

$pge->title       = $file2edit;
$pge->matchingMenu= $matchingMenu;
$pge->visibleFor  = $userRestriction;

$pge->put('<div class="labsys_mop_h2">'.$pge->title.'</div>'."\n".
          ' <FORM class="labsys_mop_std_form" NAME="fileEdit" METHOD="POST" ACTION="'.$url->link2("../php/saveFile.php").'">'."\n".
          '   <input type="hidden" name="FILENAME" value="'.$file2edit.'">'."\n".
          '   <input type="hidden" name="REDIRECTTO" value="'.urlencode($url->link2( $_SERVER['PHP_SELF'] ) ).'">'."\n".
          '   <input type="hidden" name="SESSION_ID" value="'.session_id().'">'."\n".
          '   <textarea TABINDEX="1" id="fc" name="FILECONTENT" class="labsys_mop_textarea" rows="'.$cfg->get("DefaultTextareaRows").'" onchange="isDirty=true">'."\n");
           
$pge->put( htmlentities( file_get_contents($file2edit) ) );

$pge->put('</textarea>'."\n".
          '   <input TABINDEX="2" type="submit" class="labsys_mop_input" value="'.$lng->get("save").'" accesskey="s" onclick="isDirty=false">'."\n".
 
          ' </FORM>'."\n".
            
          '<script language="JavaScript" type="text/javascript">'."\n".
          '<!--'."\n".
          '  if (document.fileEdit) document.fileEdit.fc.focus();'."\n".
          '//-->'."\n".
          '</script>');

makeLogEntry( 'system', 'edit open', $file2edit );
?>