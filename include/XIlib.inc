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
 * @author     Marc-Oliver Pahl, Meyyar Palaniappan
 * @version    2.0
 */

////////////////////////////// called externally /////////////////////////////////////////
/**
 * Creates the mapping array out of the array containing the existing element indices as vale.
 *
 * @param array $labElementArray  An array containing all elements to be exported as values.
 * @return                        Acts directly on the given array.
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
 * @param string $labUID       The unique identifier of this lab (UID for export).
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
         ' <img src="../syspix/button_export2disk_30x12.gif" width="30" height="12" border="0" alt="export" title="export">'.
         "</div>\r\n";
}

/**
 * Writes the $content into $fileName into $cfg->get('exportImportDir').
 *
 * @param $fileNameWithPath    Path and Name of the file to be created.
 * @param $content             Content to be written in the file.
 * @param string $labUID       The unique identifier of this lab (UID for export).
 * @return
 */
function fileWrite( $fileName, $content, $labUID ) {
	global $cfg;
	$fileNameWithPath = $cfg->get('exportImportDir').$labUID.'/'.$fileName;
	if ( !file_exists(dirname($fileNameWithPath)))
	mkdir(dirname($fileNameWithPath),0755 , true)
	or trigger_error("Error creating folder ".dirname($fileNameWithPath), E_USER_ERROR);
	$fh = fopen($fileNameWithPath, 'w')
	or trigger_error("Can't create/ open file ".$fileNameWithPath, E_USER_ERROR);
	fwrite($fh, $content);
	fclose($fh);
}

/**
 * Removes the example solutions from $HTMLcode.
 *
 * @param string  &$HTMLcode                The html code to be altered.
 * @return                                  Alters the code directly.
 */
function removeExampleSolutions(&$HTMLcode){ 
  // inputs
	$HTMLcode= preg_replace_callback( '#(?<=<!-- example solution -->)'. // Assertion "must be preceded by"
                                    '(.*)'.
                                    '(?=<!-- /example solution -->)'.
                                    '#msU',
                                      create_function(
                                          '$matches',
                                          'return "<br><br><br><br>";'
                                      ), 
                                   $HTMLcode
                                   );
  // MCs
  // remove checked boxes
	$HTMLcode= str_replace(' checked="checked"', '', $HTMLcode);
  //remove answer explanations
	$HTMLcode= preg_replace_callback( '#(<div class="labsys_mop_m_answer_explanation">'. // Assertion "must be preceded by"
                                    '.*'.
                                    '</div>)'.
                                    '#msU',
                                      create_function(
                                          '$matches',
                                          'return "";'
                                      ), 
                                   $HTMLcode
                                   );
  // remove disabled (just for fun)
	$HTMLcode= str_replace(' disabled="disabled" ', '', $HTMLcode);
  // remove graying showing right solutions
	$HTMLcode= preg_replace_callback( '#(<span class="labsys_mop_grayed">)'. // Assertion "must be preceded by"
                                    '(.*)'.
                                    '(</span>)'.
                                    '#msU',
                                      create_function(
                                          '$matches',
                                          'return $matches[2];'
                                      ), 
                                   $HTMLcode
                                   );
  // remove graying showing right solutions
	$HTMLcode= preg_replace_callback( '#(<a href="/pages/XIport.php\?)(\S+)#',
                                      create_function(
                                          '$matches',
                                          'return "...";'
                                      ), 
                                   $HTMLcode
                                   );               

                                   

}

/**
 * Processes the $content:
 *   - reindexes linked elements
 *   - relocates images to $IMAGE_PATH
 *   - adds images to $filesToBeExported
 *   - relocated linked objects like PDF files
 *   - adds linked objects to $filesToBeExported
 *
 * @param string  &$HTMLcode                The html code to be altered.
 * @param int     $labNumber                The number of the lab to be exported.
 * @param array   &$IDmappingArray          The array containing the mappingbetween the element IDs.
 * @param array   &$ImagefilesToBeExported  Collects all image files  to be exported.
 * @param array   &$OtherfilesToBeExported  Collects all additional files  to be exported.
 * @return
 */
function processContent( &$HTMLcode, $labNumber, &$IDmappingArray, $doCopy=true){
	reMapIDs($HTMLcode, $labNumber, $IDmappingArray); // Replace the linked internal elements in $HTMLcode identified by address= or #
  processLocalPix($HTMLcode, $doCopy);
  processLocalFiles($HTMLcode, $doCopy);
}

////////////////////////////// called only internally /////////////////////////////////////////
/**
 * Remaps the relative internal links in $HTMLcode according to the $IDmappingArray.
 * The elements are identified by address= or # (anchor in page).
 *
 * @param string $HTMLcode        The HTML code. It will be altered during this processing.
 * @param int $labNumber          The number of the lab. The match is against IDs starting with l$labNumber.
 * @param array &$IDmappingArray  The array containing the mapping between the element IDs.
 *                                NOTE: The l? element must be included! (necessary at import).
 * @return
 */
  /** helper functions */
  /**
   * Maps the element to its mapping element or returns the emlement if no maping exists.
   * @param string $oldID  Element ID to be mapped.
   * @return string        The nes ID of the given element.
   */
  function mapIt( $oldID ){
    return array_key_exists( $oldID, $GLOBALS['IDmappingArray'] ) ? $GLOBALS['IDmappingArray'][$oldID] : $oldID;
  }
  /** end of functions */
function reMapIDs( &$HTMLcode, $labNumber, &$IDmappingArray){
  $GLOBALS['IDmappingArray'] = $IDmappingArray; // we need it in the call back (not clean implementation just work around).
	$HTMLcode= preg_replace_callback(  '#(?<=address=|\#)'. // Assertion "must be preceded by" (link or anchor usage...
                                     '(l'.$labNumber.')'.              // Just replace things for the current lab
                                     '((?:\.[m, i, p, c, C]\pN)*)'.
                                     '#', // '{address=l, #l}(?P<data>(\.\p{LN})+)'
                                     create_function(
                                      '$matches',
                                      /**
                                       * The callback for preg_replace_callback.
                                       * It splits the elements and does the mapping for each element.
                                       *
                                       * @param array $matches  The matching array by evaluating the preg.
                                       * @return string         The replacement for the text.
                                       */
                                      '
                                        $parts = explode( ".", $matches[0] );
                                        foreach ( $parts as $key=>$value )
                                          $parts[ $key ] = mapIt( $value );
                                        return implode ( ".", $parts );
                                      '
                                     ),
                                     $HTMLcode
                                     );
}

/**
 * Handlers for images and a href linked LOCAL files.
 */
  /** helper functions */
  /**
   * Returns true if the url of the element is local.
   * @param string $fileWithPath  url.
   * @return bool
   */
  function isLocal( $fileWithPath ){
    return (substr( $fileWithPath, 0, 3 ) == '../');
  }
  
  /**
   * If the file passed by $localFileWithPath is recognized as local and 
   * $doCopy is set (not the case for the HTML preview), then the file
   * is copied to $destDir.
   *
   * The new valid link to the file is returned.
   * (So if nothing is done the original link is returned.)
   *
   * @param string $localFileWithPath The url of the lnked file.
   * @param string $destDir           The directory where the file should be relocated if possible.
   * @param bool   $doCopy            Only if set copy action is done. Otherwhise just relocation (false for preview.html).
   * @return string                   New url of file.
   */
  function handleLocalFile( $localFileWithPath, $destDir, $doCopy ){
    global $cfg, $pge, $externallyLinkedElements;
   
    // maybe the file is fully linked but on the local server.
    // The following assumes we run just on the hosts root.
    $localFileWithPath = preg_replace_callback(  '#(http(s?)://)?'.$_SERVER['SERVER_NAME'].'#', 
                                                 create_function(
                                                                '$matches',
                                                                'return "..";'
                                                               ),
                                                 $localFileWithPath );
    // not local: do nothing
    if ( !isLocal( $localFileWithPath ) ){
      if ($doCopy){
        $pge->put( 'non-local: |'.$localFileWithPath.'|<br>' );
        array_push( $externallyLinkedElements, $localFileWithPath );
      }
      return $localFileWithPath;
    }
    //break else:
    
    $fileName = end(explode('/', $localFileWithPath));
    if ($doCopy){
      // Here the move to server should happen...
      copy( $localFileWithPath, $cfg->get('exportImportDir').$GLOBALS['exportUID'].'/'.$destDir.$fileName )
      or trigger_error("Can't copy file ".$localFileWithPath, E_USER_ERROR);
      $pge->put(  '<div class="labsys_mop_elements_menu_s">'.
                  $localFileWithPath.
                  ' <img src="../syspix/button_export2disk_30x12.gif" width="30" height="12" border="0" alt="export" title="export">'.
                 "</div>\r\n" );
    }
    return $destDir.$fileName;
  }
  /** end of functions */
/**
 * Searches $HTMLcode for local images and if $doCopy is set copies them to the server/ directory.
 *
 * @param string &$HTMLcode          The html code to be altered.
 * @param bool   $doCopy             Only if set copy action is done. Otherwhise just relocation (false for preview.html).
 * @return                           Directly alters $HTMLcode.
 */
function processLocalPix( &$HTMLcode, $doCopy=true ){
	$HTMLcode = preg_replace_callback(   '#(?<=<img )(.* src=[\',"])(?!../syspix/|/syspix/)(\S+)([\',"])#',
                                       create_function(
                                        '$matches',
                                        'return $matches[1].handleLocalFile( $matches[2], \'images/\', '.intval($doCopy).' ).$matches[3];'
                                       ),
                                       $HTMLcode
                                       );
}

/**
 * Searches $HTMLcode for locally referenced files and if $doCopy is set copies them to the server/ directory.
 *
 * @param string &$HTMLcode          The html code to be altered.
 * @param bool   $doCopy             Only if set copy action is done. Otherwhise just relocation (false for preview.html).
 * @return                           Directly alters $HTMLcode.
 */
function processLocalFiles( &$HTMLcode, $doCopy=true ){
	$HTMLcode = preg_replace_callback(   '#(?<=<a href=)([\',"])(?!../pages/|/pages/|\#)(\S+)([\',"])#',
                                       create_function(
                                        '$matches',
                                        'return $matches[1].handleLocalFile( $matches[2], \'data/\', '.intval($doCopy).' ).$matches[3];'
                                       ),
                                       $HTMLcode
                                       );
}

/**
 * Reindexes a collection's $content.
 *
 * @param string &$HTMLcode          The html code to be altered.
 * @param array &$IDmappingArray     The array containing the mapping between the element IDs.
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
 * Called by createIdMapping as helper to remove stared values as they describe not existing elements
 *
 * @param sting $value   The value to be checked.
 * @return boolean       Is the value not starred?
 */
function notStared( $value ){
	return $value[0] != '*';
}

?>