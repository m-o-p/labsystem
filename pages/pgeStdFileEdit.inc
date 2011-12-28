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

if ( !$pge->isVisible() ){ // directly show warning and close.
  require( $cfg->get("SystemPageLayoutFile") );
  exit;
}

$editAllAvailableFiles = ( substr( $url->get('config'), -9 ) == 'useradmin' ) && $usr->isOfKind( IS_CONFIG_EDITOR ) && isset( $filePrefix );
// if true a selector is shown and 
//   - all files are loadable&editable
//   - saving with new filename becomes possible

// File selector //////////////////////////////////////////////////////////////////////
if ($editAllAvailableFiles) {
  $selectOptions = '';
  // $filePrefix = 'config_'; // only files with this prefix are shown
  $posLastSlash = strrpos( $file2edit, '/' );
  $dir = dir( substr( $file2edit, 0, $posLastSlash ) );
  // post? override..
  $fileName = substr( $file2edit, $posLastSlash+1 );
  $fileExtension = substr( $fileName, strrpos( $fileName, '.' ) );
  
  // file selected? -> load it
  $reqFile2Edit = false;
  if ( isset( $_POST['file2edit'] ) ) $reqFile2Edit = $_POST['file2edit'];
  if ( $url->available('file2edit') ) $reqFile2Edit = $url->get('file2edit'); // post comes from the save...
  
  if ( $reqFile2Edit && ( strpos( $reqFile2Edit, '.' ) === false ) /*protect against directory walks */ ){
    $file2edit = substr( $file2edit, 0, $posLastSlash ).'/'.$filePrefix.$reqFile2Edit.$fileExtension;
    $fileName = substr( $file2edit, $posLastSlash+1 );
  }
  
  $results = array();
  $selectedEntry = '';
  while (false !== ($entry = $dir->read())) { 
        if ( !is_dir( $entry ) && 
             ( substr( $entry, 0, strlen($filePrefix) ) == $filePrefix ) && // check for prefix
             ( substr( $entry, -1*strlen($fileExtension) ) == $fileExtension ) // check for extension
            ){ 
                $temp = substr( $entry, strlen($filePrefix), strlen( $entry ) - strlen($fileExtension) - strlen($filePrefix) );
                if ($entry==$fileName) $selectedEntry = $temp;
                $results[] = $temp;
              }
  }
  
  natsort( $results );
  foreach ( $results as $value ){
    $selectOptions .= ( '<option'.( $value==$selectedEntry ? ' selected="selected"' : '' ).'>'.$value."</option>\r\n" );
  }
}

$pge->title = $file2edit; // override as it may have changed...
// /File selector //////////////////////////////////////////////////////////////////////

$pge->put('<div class="labsys_mop_h2">'.$pge->title.'</div>'."\n".
          ( $editAllAvailableFiles && ($selectOptions != '') ? 
            "<div class=\"labsys_mop_note\">\n".
            '<FORM class="labsys_mop_std_form" NAME="sort_options" METHOD="POST" ACTION="'.$GLOBALS['url']->rawLink2( $_SERVER['PHP_SELF'] ).'">'."\n".
            "   <select tabindex=".$pge->nextTab++." name='file2edit' size='1' onChange='document.sort_options.submit();'>\r\n".$selectOptions."</select>\n".
            '   <input TABINDEX="'.$pge->nextTab++.'" type="submit" class="labsys_mop_input" value="'.$lng->get("load").'" accesskey="l" onclick="isDirty=false">'."\n".
            "</FORM>\n".
             $lng->get( 'fileSelectNote' ).
             "</div>\n"
            :
            ''
          ).
          ' <FORM class="labsys_mop_std_form" NAME="fileEdit" METHOD="POST" ACTION="'.$url->link2("../php/saveFile.php").'">'."\n".
          '   <input type="hidden" name="FILENAME" value="'.$file2edit.'">'."\n".
          '   <input type="hidden" name="REDIRECTTO" value="'.urlencode($url->link2( $_SERVER['PHP_SELF'] ) ).'">'."\n".
          '   <input type="hidden" name="SESSION_ID" value="'.session_id().'">'."\n".
          '   <textarea TABINDEX="'.$pge->nextTab++.'" id="fc" name="FILECONTENT" class="labsys_mop_textarea" rows="'.$cfg->get("DefaultTextareaRows").'" onchange="isDirty=true">'."\n");
          
$pge->put( htmlentities( file_get_contents($file2edit) ) );

if ( $editAllAvailableFiles ){
  $fileNameProposal = substr( $fileName, strlen($filePrefix), strlen( $fileName ) - strlen($fileExtension) - strlen($filePrefix) );
  
  $confirmScript = 'if (document.getElementById(\'saveAs\').value != \''.$fileNameProposal.'\'){ return confirmLink(this, \''.$lng->get("confirmSaveAs").' '.substr( $file2edit, 0, $posLastSlash ).'/'.$filePrefix.'\'+document.getElementById(\'saveAs\').value+\''.$fileExtension.'?\'); }';
} else {
  $confirmScript = '';
}

$pge->put('</textarea>'."\n".
          ( $editAllAvailableFiles ? 
            "   <input tabindex=\"".$pge->nextTab++."\" type=\"text\" id=\"saveAs\" name=\"SAVEAS\" class=\"labsys_mop_input\" maxlength=\"255\" value=\"".$fileNameProposal."\" onchange='isDirty=true'>\n".
            '   <input type="hidden" name="SAVEAS_PREFIX" value="'.substr( $file2edit, 0, $posLastSlash ).'/'.$filePrefix.'">'."\n".
            '   <input type="hidden" name="SAVEAS_POSTFIX" value="'.$fileExtension.'">'."\n"            
            : '' ).
          '   <input TABINDEX="'.$pge->nextTab++.'" type="submit" class="labsys_mop_input" value="'.$lng->get("save").'" accesskey="s" onclick="isDirty=false; '.$confirmScript.'" >'."\n".
          ' </FORM>'."\n".
            
          '<script language="JavaScript" type="text/javascript">'."\n".
          '<!--'."\n".
          '  if (document.fileEdit) document.fileEdit.fc.focus();'."\n".
          '//-->'."\n".
          '</script>');

makeLogEntry( 'system', 'edit open', $file2edit );
?>