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
* @copyright  Marc-Oliver Pahl, Meyyar Palaniappan 2010
* @version    1.0
*/
require( "../include/init.inc");
$id = 'l';
require( "../php/getDBIbyID.inc" ); /* -> $DBI */

// load the config values
$cfgXIport = new Rom( parse_ini_file( $cfg->get('SystemResourcePath').'XIport.ini' ) );

$pge->title       = $lng->get( strtolower($id)."ManageTitle" );
$pge->matchingMenu= $lng->get( strtolower($id)."ManageMnuEntry" );
$pge->visibleFor  = IS_CONTENT_EDITOR;

$pge->put( EM::manageTop( $id ) );
$pge->put('<div class="labsys_mop_h2">'.$pge->title.'</div>'."\n");

// H E L P E R functions
require( '../include/XIlib.inc' );

// do the import/ export
  if (isset($_POST) && count($_POST) > 0){
    foreach ($_POST as $key => $value){
      $pge->put( $key.' --- '.$value.'<br>' );
     
      if ( $value == 'IMPORT' ){ // doImport
/*******************************************************
  ToDo: Do a switch as in export.
  Create a new Element of the read type like
    $newInput = new LiElement(  $_POST['IDX'],
                                $_POST['QUESTION'], 
                                $_POST['EXAMPLE_SOLUTION'],
                                $_POST['POSSIBLE_CREDITS'],
                                $userRights, 
                                ($_POST["VISIBLE_ONLY_IN_COLLECTION"] == "1"),
                                ""
                               );
  make it persistent:
    * @return int The idx of the newly created element.
  $newIdx = $iDBI->newInput( $newInput ){
 *******************************************************/     
// doImport
      }else{
// doExport
        $filesToBeExported = array(); // this array collects all additional files like images to be exported
    // get the lab      
        $labToExport = $lDBI->getData2idx( $key );
    // add export information to history
        $labToExport->history = $value.' by '.$usr->foreName.' '.$usr->surName.' ('.$usr->userName.') from '.$_SERVER['SERVER_NAME'].' at '.date('r')."\r\n".$labToExport->history; 
        
    // get all elements in lab as an array
        $labElementArray = explode( ' ',
                                str_replace( array('( ', ' )'), array('', ''),  // remove "( ", " )"
                                  'C'.$labToExport->preLab->idx.' '.$labToExport->preLab->buildStructure(true, true).
                                  ' C'.$labToExport->lab->idx.' '.$labToExport->lab->buildStructure(true, true)
                                )
                              );
    // build the array that contains the renaming: [oldID] => exportedID
        createIdMapping( $labElementArray );
    // get the html preview
        // Remove user rights 
        $oldUsrRights = $usr->userRights;
        $usr->userRights -= IS_CONTENT_EDITOR;
        $htmlPreviewLab = $labToExport->show( 'l'.$key.'.all' );
        $usr->userRights = $oldUsrRights;
      // remove the example solutions and relocate the embedded objects
        removeExampleSolutions($htmlPreviewLab, $labElementArray, $filesToBeExported);
      // relocate linked objects like images, linked files, etc.
        processContent($htmlPreviewLab, $labElementArray, $filesToBeExported);
        fileWrite( 'preview.html', $htmlPreviewLab, $labToExport->uniqueID );

    // export the lab element itself first:        
      // replace the elements according to the renaming:
        $labToExport->preLab = $labElementArray[ 'C'.$labToExport->preLab->idx ];
        $labToExport->lab = $labElementArray[ 'C'.$labToExport->lab->idx ];
      // set the index of the exported lab to 1
        $labToExport->idx = 1;
        processContent( $labToExport->title, $labElementArray, $filesToBeExported );
        processContent( $labToExport->authors, $labElementArray, $filesToBeExported );
        processContent( $labToExport->comment, $labElementArray, $filesToBeExported );
      // persist the lab
        $pge->put( persistElement( $labToExport, $labToExport->uniqueID ) );

    // Iterate through the elements
        foreach ($labElementArray as $value=>$newID){
          $nextElement = $GLOBALS[ substr($value, 0, 1)."DBI" ]->getData2idx( substr($value, 1) );
          $exportContent = '';
          
          switch ( substr($value, 0, 1) ){
/*******************************************************
  ToDo: the elements are loaded now. The data is in the respective fields.
  Do all necessary parsings and store the files (txt+pictures+local_material)!
  Fill $exportContent and write it at the bottom once!
 *******************************************************/
            case 'C':
            case 'c':
              reindexCollectionElements( $nextElement->contents, $labElementArray );
              break;
            case 'p':
              processContent( $nextElement->title, $labElementArray, $filesToBeExported );
              processContent( $nextElement->contents, $labElementArray, $filesToBeExported );
              break;
            case 'm':
              processContent( $nextElement->question, $labElementArray, $filesToBeExported );
              processContent( $nextElement->answerExplanation, $labElementArray, $filesToBeExported );
              for ($i=0; $i < $nextElement->answerCount; $i++)
                processContent( $nextElement->answerArray[$i], $labElementArray, $filesToBeExported );
              break;
            case 'i':
              processContent( $nextElement->question, $labElementArray, $filesToBeExported );
              processContent( $nextElement->exampleSolution, $labElementArray, $filesToBeExported );
              break;
            default:
              $pge->put( 'ELEMENT NOT EXPORTED! '.$value );
              break;
          }
        // renumber element
          $nextElement->idx = substr( $newID, 1);
          $pge->put( persistElement( $nextElement, $labToExport->uniqueID ) );
        } // /foreach
      handleAdditionalFiles( $filesToBeExported );
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
     // show the property row
      $pge->put( $element->showExportImportRow( $element->idx, false ) );
/*******************************************************
  ToDo: Use the logic to read the l-Elements from the file in (that you built at the import above probably).
  Create them as with the import (only fill title and idx and what is really needed).
  Call for those the function below then...
 *******************************************************/
      $pge->put( $element->showExportImportRow( $element->idx, true ) );
    }
     
  // saving
    $pge->put("<input TABINDEX=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("yesIconfirm")."\" accesskey=\"s\">" );
  
  
  // close the form
    $pge->put("</FORM>");
  
  // the bottom menu
    $pge->put( EM::manageBottom( $id ) );

  }  // show list

// show!
  require( $cfg->get("SystemPageLayoutFile") );
?>
