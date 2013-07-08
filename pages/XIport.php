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
$userRestriction  = IS_CONTENT_EDITOR;
require( "../php/getDBIbyID.inc" ); /* -> $DBI */

require( '../include/XIlib.inc' ); // H E L P E R functions

$pge->title       = $lng->get( strtolower($id)."ManageTitle" );
$pge->matchingMenu= $lng->get( "MnuEntryXIport" );
$pge->visibleFor  = IS_CONTENT_EDITOR;

if ( !$pge->isVisible() ){ // directly show warning and close.
  require( $cfg->get("SystemPageLayoutFile") );
  exit;
}

$pge->put( EM::manageTop( $id ) );
$pge->put('<div class="labsys_mop_h2">'.$pge->title.'</div>'."\n");

// do the import/ export
  if (isset($_POST) && count($_POST) > 0){
    // UserStyleSheet preprocessing (needed for export and import)
      $tagNames = array(); // holds only the tag names
      $styleDefinitions = array(); // holds the full definition
      // both arrays are joined via their numeric index!
      parseStyleSheet( file_get_contents( $cfg->get('UserStyleSheet') ),
                       $tagNames,
                       $styleDefinitions );

    foreach ($_POST as $key => $value){
      //$pge->put( $key.' --- '.$value.'<br>' );

      if ( $value == 'IMPORT' ){ // doImport
        $subDir = base64_decode( $key ); // the bas64 encoding is needed as the form.input in HTML replaces . by _ and so the name of the directory gets disturbed :(
        $GLOBALS['exportUID'] = $subDir;

      // load information about the lab
        $labToImport = new LlElement( 0, 0, '', '', '', 1, 1, '', false, false, false, false, '' );
        $labToImport->initFromSerialized( file_get_contents($cfg->get('exportImportDir').$subDir.'/data/l0000002.txt') );

      // create the mapping from the directory and create the "empty" DB objects for the elements
        $labElementArray = createIdImportMappingInitDB( $labToImport->uniqueID );

        $newLabId = $labElementArray['l2'];

        $pge->put( '<h3>'.$labToImport->title.' ('.$labToImport->uniqueID.' <img src="../syspix/button_importFromDisk_30x12.gif" width="30" height="12" border="0" alt="import" title="import"> '.$newLabId.' <a href="'.$url->link2('../pages/edit.php?address='.$newLabId).'">edit...</a>)</h3>'."\r\n" );

      // import elements
        $importCounter = 0; // used for setting the schedules accordingly when importing
        foreach ($labElementArray as $value=>$newID){
          $nextElement = $GLOBALS[ $newID[0]."DBI" ]->getData2idx( substr($newID, 1) ); // load existing empty DB object

          $importBaseDir = $cfg->get('exportImportDir').$subDir.'/';
          $importDataDirectory = 'data';
          $importFileName = '/'.$value[0].str_pad( substr($value, 1), 7, "0", STR_PAD_LEFT ).'.txt';
          switch( $value[0] ){
              // i and m can be stored with and without solutions.
            case 'i':
            case 'm':
              $importDataDirectory = EXPORT_DATA_DIR_PUBLIC; // default: take them from the public directory without solutions
              if ( file_exists($importBaseDir.EXPORT_DATA_DIR_PRIVATE.$importFileName) ){ $importDataDirectory = EXPORT_DATA_DIR_PRIVATE; }// if the version with solutions exists: take it!
            default:
              $nextElement->initFromSerialized( file_get_contents( $importBaseDir.$importDataDirectory.$importFileName ) ); // init the next element
          }

          processElement( $nextElement, $labElementArray, 1, true );

        // renumber element
          $nextElement->idx = substr( $newID, 1);

          $pge->put( persistElement( $nextElement, '', true ) );
        } // /foreach

      // Integrate css/user_styles.css into the current user stylesheet.
        if (file_exists( $cfg->get('exportImportDir').$subDir.'/css/user_styles.css' ) ){
          // load from the user_styles.css import
          $importTagNames = array(); // holds only the tag names
          $importStyleDefinitions = array(); // holds the full definition
          // both arrays are joined via their numeric index!
          parseStyleSheet( file_get_contents($cfg->get('exportImportDir').$subDir.'/css/user_styles.css' ),
                           $importTagNames,
                           $importStyleDefinitions );

          $existingStylesHashed = array();
          $importStylesHashed = array();
          $importStyleMapping = array();
          // hash the existing styles
          getTagHashes( $tagNames, $existingStylesHashed, $importStyleMapping /*dummy*/ ); // hash existing
          // hash the possibly new styles to be imported
          getTagHashes( $importTagNames, $importStylesHashed, $importStyleMapping ); // hash the ones to be imported

          // identify all tags that are not already in the user stylesheet
          $notAlreadyThereTagsHashes = array_diff( $importStylesHashed, $existingStylesHashed );

          $newStyles = array();
          foreach( $notAlreadyThereTagsHashes as $values )
            $newStyles[] = $importStyleDefinitions[ $importStyleMapping[$values] ];
          // add the data to the file
          if ( count($newStyles) > 0 ){
            array_unshift( $newStyles, "\n\n".'/* Following styles imported by '.$usr->foreName.' '.$usr->surName.' ('.$usr->userName.') from '.$_SERVER['SERVER_NAME'].' at '.date('r').' */' );
            file_put_contents( $cfg->get( "UserStyleSheet" ), join($newStyles), FILE_APPEND | LOCK_EX);
            $pge->put(  '<pre>'.htmlentities(implode( "<br>\n", $newStyles ))."</pre>\n".
                        '<div class="labsys_mop_elements_menu_l">'.
                        'user_styles.css <img src="../syspix/button_importFromDisk_30x12.gif" width="30" height="12" border="0" alt="import" title="import">'.
                        $cfg->get( "UserStyleSheet" ).
                        "</div>\r\n" );
          }
        }

        $externallyLinked = $cfg->get('exportImportDir').$subDir.'/files/externallyLinked.txt';
        if ( file_exists( $externallyLinked ) )
          $pge->put( '<pre>'.$externallyLinked.':'."\r\n".file_get_contents( $externallyLinked ).'</pre>' );

        // create new schedule
        $newIdx = createNew( 's' );
        // load it
        $newSchedule = $GLOBALS[ 'sDBI' ]->getData2idx( $newIdx );
        $newSchedule->id = $newLabId[0];
        $newSchedule->num = substr( $newLabId, 1);
        // Make a 2 weeks schedule starting in $newSchedule->num weeks:
        $startTime = strtotime( '+'.$newSchedule->num.' weeks 0:0:0' );
        $endTime = strtotime( '+'.(2+$newSchedule->num).' weeks 23:59:59');
        $newSchedule->start = $startTime;
        $newSchedule->stop = $endTime;
        $pge->put( persistElement( $newSchedule, '', true ) );
        $pge->put( '<a href="'.$url->link2('../pages/edit.php?address=s'.$newIdx).'">Please edit the schedule...</a>' );
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

        deleteExportDirectory( $GLOBALS['exportUID'] ); // empty export directory
        $usedClasses = array(); // empty used styles!

        fileWrite( 'images/readme.txt', 'In this directory the images are stored.', $GLOBALS['exportUID'] );
        fileWrite( 'files/readme.txt', 'In this directory the data files are stored.', $GLOBALS['exportUID'] );
        fileWrite( 'data/readme.txt', 'In this directory the serialized database data files are stored.', $GLOBALS['exportUID'] );
        fileWrite( EXPORT_DATA_DIR_PRIVATE.'/readme_im.txt', 'In this directory the serialized i, m elements are stored WITH solutions. If you do not want to distribute them remove this directory.', $GLOBALS['exportUID'] );
        fileWrite( EXPORT_DATA_DIR_PUBLIC.'/readme_im.txt', 'In this directory the serialized i, m elements are stored WITHOUT solutions. This version is used if the directory '.EXPORT_DATA_DIR_PRIVATE.' is not available.', $GLOBALS['exportUID'] );
        fileWrite( 'css/readme.txt', 'In this directory the style sheets for the preview and the user_styles.css for import are stored.', $GLOBALS['exportUID'] );

    // get the HTML PREVIEW
        // Remove user rights
        $oldUsrRights = $usr->userRights;
        $usr->userRights -= IS_CONTENT_EDITOR;
        $htmlPreviewLab = '[HTML]'.$labToExport->show( 'l'.$key.'.all' );
        $usr->userRights = $oldUsrRights;

      // remove the example solutions and relocate the embedded objects
        removeExampleSolutions($htmlPreviewLab);

      // relocate linked objects like images, linked files, etc.
        processContent($htmlPreviewLab, $key, $labElementArray, false);

        $htmlPreviewLab = substr( $htmlPreviewLab, 6 ); // remove the [HTML] again as it is only needed for right processing.
      // relocate syspix to global server
        $htmlPreviewLab = str_replace( '../syspix/', 'http://labsystem.m-o-p.de/syspix/', $htmlPreviewLab );

        $pge->put(  '<div class="labsys_mop_elements_menu_l">preview.html'.
                    ' <img src="../syspix/button_export2disk_30x12.gif" width="30" height="12" border="0" alt="export" title="export">'.
                    "</div>\r\n" );
      // copy the system stylesheets for preview
        $file2importWithPath = $cfg->get('SystemStyleSheet');
        copy( $file2importWithPath,
              $cfg->get('exportImportDir').$labToExport->uniqueID.'/css/system.css' ) or trigger_error("Can't copy file ".$file2copyWithPath, E_USER_WARNING);
        if ( $cfg->get('SysOverridingSheet') != '' ){
          $file2importWithPath = $cfg->get('SysOverridingSheet');
          copy( $file2importWithPath,
                $cfg->get('exportImportDir').$labToExport->uniqueID.'/css/system_override.css' ) or trigger_error("Can't copy file ".$file2copyWithPath, E_USER_WARNING);
        }
        $file2importWithPath = $cfg->get('PrintStyleSheet');
        copy( $file2importWithPath,
              $cfg->get('exportImportDir').$labToExport->uniqueID.'/css/print.css' ) or trigger_error("Can't copy file ".$file2copyWithPath, E_USER_WARNING);
      // user css gets processed below with the export as only used styles are exported
        fileWrite( 'preview.html',
                   '
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<HTML>
  <HEAD>
    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
    <meta http-equiv="Content-Language" content="en">
    <meta name="generator" content="labsystem.m-o-p.de">
    <link rel="stylesheet" type="text/css" href="css/system.css">
    '.( $cfg->get('SysOverridingSheet') != '' ? '<link rel="stylesheet" type="text/css" href="css/system_override.css">' : '' ).
    '<link rel="stylesheet" type="text/css" href="css/user_styles.css">
    <link rel="stylesheet" type="text/css" href="css/print.css" media="print">
    <link rel="shortcut icon" href="http://labsystem.m-o-p.de/syspix/favicon.ico">
    <script src="http://labsystem.m-o-p.de/syspix/pages/scripts.js" type="text/javascript" language="javascript"></script>
    <TITLE>'.$labToExport->title.'</TITLE>
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
      $pge->put(  '<div class="labsys_mop_elements_menu_l">files/externallyLinked.txt'.
                  ' <img src="../syspix/button_export2disk_30x12.gif" width="30" height="12" border="0" alt="export" title="export">'.
                  "</div>\r\n" );
   // StyleSheet processing
      //$styleDefinitions = array(); // holds the full definition

      $tagStylePointer = getTagPointer( $tagNames ); // Tag => array( indices from $styleDefinitions );

      $stylesToExport = array(); // gets filled now
      foreach ( $GLOBALS['usedClasses'] as $value )
        if ( isset( $tagStylePointer[$value] ) )
          foreach ( $tagStylePointer[$value] as $index )
            $stylesToExport[] = $styleDefinitions[ $index ];

      fileWrite(  'css/user_styles.css',
                  join( $stylesToExport ),
                  $GLOBALS['exportUID'] );
      $pge->put(  '<pre>'.htmlentities(implode( "\n", $stylesToExport ))."</pre>\n".
                  '<div class="labsys_mop_elements_menu_l">user_styles.css'.
                  ' <img src="../syspix/button_export2disk_30x12.gif" width="30" height="12" border="0" alt="export" title="export">'.
                  "</div>\r\n" );


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
    $pge->put( '<h4>'.$lng->get('exportFollowingLabs').' ('.$cfg->get('exportImportDir').')</h4>' );
    while ( $element = $DBI->getNextData() )
      $pge->put( $element->showExportImportRow( $element->idx.': ', true ) ); // show the property row

    // Importable labs
    $pge->put( '<h4>'.$lng->get('importFollowingLabs').' ('.$cfg->get('exportImportDir').')</h4>' );
    $importableLabs = getLabsFromDirectory( $cfg->get('exportImportDir') );
    foreach( $importableLabs as $key=>$value )
      $pge->put( $value->showExportImportRow( '', false ) ); // show the property row

  // saving
    $pge->put("<input TABINDEX=\"".$pge->nextTab++."\" type=\"submit\" class=\"labsys_mop_button\" value=\"".$lng->get("yesIconfirm")."\" accesskey=\"s\" onclick='isDirty=false; document.getElementById(\"progressBar\").style.display = \"inline\";'> ".
              '<div id="progressBar" style="display: none;"><img src="../syspix/labsystem_wait_17x117.gif" width="117" height="17"></div>' );


  // close the form
    $pge->put("</FORM>");

  // the bottom menu
    $pge->put( EM::manageBottom( $id ) );

  }  // show list

// show!
  require( $cfg->get("SystemPageLayoutFile") );
?>