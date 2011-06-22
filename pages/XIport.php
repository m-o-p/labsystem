<?php
/**
 *  labsystem.m-o-p.de - 
 *                  the web based eLearning tool for practical exercises
 *  Copyright (C) 2010  Marc-Oliver Pahl, Meyyar Palaniappan
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
* Lists all property rows (editable) of elements type.
*
* @module     ../pages/export_import_labs.php
* @author     Marc-Oliver Pahl, Meyyar Palaniappan
* @version    1.0
*/
require( "../include/init.inc");
$id = 'l';
require( "../php/getDBIbyID.inc" ); /* -> $DBI */

require( '../include/XIlib.inc' ); // H E L P E R functions

$pge->title       = $lng->get( strtolower($id)."ManageTitle" );
$pge->matchingMenu= $lng->get( strtolower($id)."ManageMnuEntry" );
$pge->visibleFor  = IS_CONTENT_EDITOR;

$pge->put( EM::manageTop( $id ) );
$pge->put('<div class="labsys_mop_h2">'.$pge->title.'</div>'."\n");

// do the import/ export
  if (isset($_POST) && count($_POST) > 0){
    foreach ($_POST as $key => $value){
      //$pge->put( $key.' --- '.$value.'<br>' );
     
      if ( $value == 'IMPORT' ){ // doImport
        $subDir = base64_decode( $key ); // the bas64 encoding is needed as the form.input in HTML replaces . by _ and so the name of the directory gets disturbed :(
        $GLOBALS['exportUID'] = $subDir;
        
      // load information about the lab
        $labToImport = new LlElement( 0, 0, '', '', '', 1, 1, '', false, false, false, false, '' );
        $labToImport->initFromSerialized( file_get_contents($cfg->get('exportImportDir').$subDir.'/l0000002.txt') );

      // create the mapping from the directory and create the "empty" DB objects for the elements
        $labElementArray = createIdImportMappingInitDB( $labToImport->uniqueID );
        
        $newLabId = $labElementArray['l2'];
        
        $pge->put( '<h3>'.$labToImport->title.' ('.$labToImport->uniqueID.' <img src="../syspix/button_importFromDisk_30x12.gif" width="30" height="12" border="0" alt="import" title="import"> '.$newLabId.')</h3>'."\r\n" );

      // import elements
        foreach ($labElementArray as $value=>$newID){
          $nextElement = $GLOBALS[ $newID[0]."DBI" ]->getData2idx( substr($newID, 1) ); // load existing empty DB object
          $nextElement->initFromSerialized( file_get_contents($cfg->get('exportImportDir').$subDir.'/'.$value[0].str_pad( substr($value, 1), 7, "0", STR_PAD_LEFT ).'.txt') );
          
          processElement( $nextElement, $labElementArray, 1, true );
          
        // renumber element
          $nextElement->idx = substr( $newID, 1);
          
          $pge->put( persistElement( $nextElement, '', true ) );
        } // /foreach

        $externallyLinked = $cfg->get('exportImportDir').$subDir.'/data/externallyLinked.txt';
        if ( file_exists( $externallyLinked ) )
          $pge->put( '<pre>'.$externallyLinked.':'."\r\n".file_get_contents( $externallyLinked ).'</pre>' );

// doImport
      }else{
// doExport
    // get the lab      
        $labToExport = $lDBI->getData2idx( $key );
        $pge->put( '<h3>'.$labToExport->title.' ('.$labToExport->uniqueID.' <img src="../syspix/button_export2disk_30x12.gif" width="30" height="12" border="0" alt="next" title="export">)</h3>'."\r\n" );       
    // get all elements in lab as an array
        $labElementArray = explode( ' ',
                                str_replace( array('( ', ' )'), '',  // remove "( ", " )"
					     (($labToExport->preLab->idx != 1) ?'c'.$labToExport->preLab->idx.' '.$labToExport->preLab->buildStructure(true, true).' ':''). // 1 means empty
					     (($labToExport->lab->idx != 1)?'c'.$labToExport->lab->idx.' '.$labToExport->lab->buildStructure(true, true).' ':'') // 1 means empty
                                ).'l'.$key
                              );

    // build the array that contains the renaming: [oldID] => exportedID
        createIdMapping( $labElementArray );

      // Needed in some XIlib functions.
        $GLOBALS['exportUID'] = $labToExport->uniqueID;
        $GLOBALS['externallyLinkedElements'] = array();
        fileWrite( 'images/readme.txt', 'In this directory the images are stored.', $GLOBALS['exportUID'] );
        fileWrite( 'data/readme.txt', 'In this directory the data files are stored.', $GLOBALS['exportUID'] );
        
    // get the HTML PREVIEW
        // Remove user rights 
        $oldUsrRights = $usr->userRights;
        $usr->userRights -= IS_CONTENT_EDITOR;
        $htmlPreviewLab = $labToExport->show( 'l'.$key.'.all' );
        $usr->userRights = $oldUsrRights;
      // remove the example solutions and relocate the embedded objects
        removeExampleSolutions($htmlPreviewLab);
      // relocate linked objects like images, linked files, etc.
        processContent($htmlPreviewLab, $key, $labElementArray, false);
        $pge->put(  '<div class="labsys_mop_elements_menu_l">preview.html'.
                    ' <img src="../syspix/button_export2disk_30x12.gif" width="30" height="12" border="0" alt="export" title="export">'.
                    "</div>\r\n" );
        fileWrite( 'preview.html', 
                   '
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<HTML>
  <HEAD>
    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
    <meta http-equiv="Content-Language" content="en">
    <meta name="generator" content="labsystem.m-o-p.de">
    <link rel="stylesheet" type="text/css" href="../css/sys/labsys_mop_basic.css">         
    <link rel="stylesheet" type="text/css" href="../css/labsys_user_style_ilab2_ss10.css">
    <link rel="stylesheet" type="text/css" href="../css/sys/labsys_mop_print_theme.css" media="print">
    <link rel="shortcut icon" href="../syspix/favicon.ico">
    <script src="../pages/scripts.js" type="text/javascript" language="javascript"></script>
    <TITLE>labs - administration [mop@ilab2 ws10]</TITLE>
  </HEAD><BODY><div class="labsys_mop_content">
                   '.
                   $htmlPreviewLab.
                   '
                   </div></BODY></HTML>
                   ', 
                   $labToExport->uniqueID );
    // /get the HTML PREVIEW

    // Iterate through the elements
        foreach ($labElementArray as $value=>$newID){         
          $nextElement = $GLOBALS[ $value[0]."DBI" ]->getData2idx( substr($value, 1) );
         
          processElement( $nextElement, $labElementArray, $key );
          
        // renumber element
          $nextElement->idx = substr( $newID, 1);
          $pge->put( persistElement( $nextElement, $labToExport->uniqueID ) );
        } // /foreach
      fileWrite(  'data/externallyLinked.txt', 
                  'The following external ressources are linked in this lab:'."\n".
                  implode( "\n", $GLOBALS['externallyLinkedElements'] ), 
                  $GLOBALS['exportUID'] );
      } // /doExport
    }
  } // end of do the import
  else{ // show list
  // additional note
    if ( $lng->get( 'lExportImportNote' ) != "" ) $pge->put( "<div class=\"labsys_mop_note\">\n".$lng->get( 'lExportImportNote' )."</div>\n" );
    
  
  // sorting
   // get array of sorter keys from DBInterface
    $sortArray = $GLOBALS[ $id."DBI" ]->sortableByArray();
   // fill $sorter with the sorters html code and set $orderBy and $asc
    require( "../pages/sorter.inc" );
  // the sorter
    $pge->put( $sorter );
  
  // iterate over all elements ordered by $orderBy, $asc
    $DBI->getAllData( $orderBy, $asc ); 
   
    $pge->put('<FORM NAME="export_import" METHOD="POST" ACTION="#">');
    while ( $element = $DBI->getNextData() ){ 
      $pge->put( $element->showExportImportRow( $element->idx, true ) ); // show the property row
    }
    
    // Importable labs
    $importableLabs = getLabsFromDirectory( $cfg->get('exportImportDir') );
    foreach( $importableLabs as $key=>$value )
     $pge->put( $value->showExportImportRow( '', false ) ); // show the property row
     
  // saving
    $pge->put("<input TABINDEX=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("yesIconfirm")."\" accesskey=\"s\" onclick='isDirty=false'>" );
  
  
  // close the form
    $pge->put("</FORM>");
  
  // the bottom menu
    $pge->put( EM::manageBottom( $id ) );

  }  // show list

// show!
  require( $cfg->get("SystemPageLayoutFile") );
?>
