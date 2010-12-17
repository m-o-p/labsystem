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
* Helper functions used by ../pages/XIport.php
*
* @module     ../include/XIlib.inc
* @author     Meyyar Palaniappan, Marc-Oliver Pahl 
* @copyright  Meyyar Palaniappan, Marc-Oliver Pahl 2010
* @version    1.0
*/

////////////////////////////// called externally /////////////////////////////////////////
/**
* Gets the array of the current elements and returns the mapping array key->
* for the values to be reindexed correctly.
*
* @param array $Orig          An array containing all elements to be exported as values.
* @return array               The mapping for the indexes.
*/
function createIdMapping( &$labElementArray ){
        natsort( $labElementArray ); // sort values lexicographically
        //remove stared values as they describe not existing elements:
        $labElementArray = array_filter( $labElementArray, 'notStared' );
        $labElementArray = array_flip( $labElementArray ); // exchange keys and values (+ remove duplicate elements...)
        $lastSeenElement = '';
        $elementCounter = 99;
        foreach ($labElementArray as $key => $value){
          if ( $lastSeenElement != $key[0] ){ // new element ID
            $lastSeenElement = $key[0];
            $elementCounter = 1;
          }
          $labElementArray[ $key ] = $lastSeenElement.$elementCounter++;
        }
}

/**
* Persists the element given.
*
* @param object &$element     The element.
* @return string              Status information
*/
function persistElement( &$element, $labUID ){
  $fileName = $element->elementId.substr( '0000000'.$element->idx, -7 ).'.txt';
  fileWrite( $fileName, $element->getSerialized(), $labUID );
  //$element->initFromSerialized( $element->getSerialized() ); // identity!
  return '<div class="labsys_mop_elements_menu_'.strtolower( $element->elementId ).'">'."\r\n".
         htmlentities($element->title).
         ' <img src="../syspix/button_next_13x12.gif" width="13" height="12" border="0" alt="-&gt;" title="-&gt;"> '.
         $fileName.
         ' <img src="../syspix/button_export2disk_30x12.gif" width="30" height="12" border="0" alt="next" title="export">'.
         "</div>\r\n";
}

/**
* Write the contents into file with the fileName  into the given filePath
*
* @param $fileNameWithPath    Path and Name of the file to be created.
* @param $content             Content to be written in the file.
* @return
*/
function fileWrite( $fileName, $content, $labUID ) {
  global $cfg;
  $fileNameWithPath = $cfg->get('exportImportDir').$labUID.'/'.$fileName;
  if ( !file_exists(dirname($fileNameWithPath)))
    mkdir(dirname($fileNameWithPath),0755 , true) 
      or trigger_error("Error creating folder ".dirname($fileNameWithPath), E_USER_ERROR);
  $fh = fopen($fileNameWithPath, 'w') 
    or trigger_error("Can't open file ".$fileNameWithPath, E_USER_ERROR);
  fwrite($fh, $content);
  fclose($fh);
}

/**
* Removes the example solutions from the HTML code and relocates
* Images and documents.
*
* @param string &$HTMLcode          The html code to be altered.
* @param array &$IDmappingArray     The array containing the mappingbetween the element IDs.
* @param array &$filesToBeExported  Collects all additional files like images to be exported.
* @return
*/
function removeExampleSolutions(&$HTMLcode, &$IDmappingArray, &$filesToBeExported){
//  parse example solution for inputs out.
//  = remove content between:
//  <div class="labsys_mop_i_example_solution">
//  </div>
}

/**
* Processes the $content:
*   - reindexes linked elements
*   - relocates images to $IMAGE_PATH
*   - adds images to $filesToBeExported
*   - relocated linked objects like PDF files
*   - adds linked objects to $filesToBeExported
*
* @param string &$HTMLcode          The html code to be altered.
* @param array &$IDmappingArray     The array containing the mappingbetween the element IDs.
* @param array &$filesToBeExported  Collects all additional files like images to be exported.
* @return
*/
function processContent( &$content, &$IDmappingArray, &$filesToBeExported ){
// Meyyar, this is probably your replace elements function.
// reindexes linked elements

// relocates images to $IMAGE_PATH
// adds images to $filesToBeExported

// relocated linked objects like PDF files
// adds linked objects to $filesToBeExported
}

/**
* Reindexes a collection's $content.
*
* @param string &$HTMLcode          The html code to be altered.
* @param array &$IDmappingArray     The array containing the mappingbetween the element IDs.
* @return
*/
function reindexCollectionElements( &$content, &$IDmappingArray ){
	$contents = explode(' ', $content);
  $content = '';
	foreach ( $contents as $element )
		if (isset($IDmappingArray[$element])) // as there might have been non existing elements not for all values a mapping must exist!
      $content .= ' '.$IDmappingArray[$element];

  $content = substr( $content, 1); // cut first space
}

/**
* Does the necessary processing with the files collected over the processingabove.
*
* @param array &$filesToBeExported  Collects all additional files like images to be exported.
* @return
*/
function handleAdditionalFiles( &$filesToBeExported ){
}

////////////////////////////// called only internally /////////////////////////////////////////
/**
* Called by createIdMapping as helper to remove stared values as they describe not existing elements
*
* @param sting $value   The value to be checked.
* @return boolean       Is the value not starred?
*/
function notStared( $value ){
  return $value[0] != '*';
}

?>