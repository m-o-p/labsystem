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
require('../vendor/autoload.php');
require( "../include/init.inc");
$id = 'l';
$userRestriction  = IS_CONTENT_EDITOR;
require( "../php/getDBIbyID.inc" ); /* -> $DBI */
require( '../include/XIlib.inc' ); // H E L P E R functions
include("../php/phpgit/GetCurrentHash.php");
include (INCLUDE_DIR."/classes/GitDBInterface.inc");
include (INCLUDE_DIR."/classes/LabGitDBInterface.inc");

global $gDBI, $lgDBI;

$pge->title       = $lng->get( strtolower($id)."ManageTitle" );
$pge->matchingMenu= $lng->get( "MnuEntryXIport" );
$pge->visibleFor  = IS_CONTENT_EDITOR;

if ( !$pge->isVisible() ){ // directly show warning and close.
  require( $cfg->get("SystemPageLayoutFile") );
  exit;
}

$pge->put( EM::manageTop( $id ) );
$pge->put('<div class="labsys_mop_h2">'.$pge->title.'</div>'."\n");

$gitIndex = mysqli_fetch_assoc($gDBI->returnFirstIndex());
#print_r($gitdata);

// Token authentication
$client = new Gitlab\Client();
$client->setUrl($gitIndex["url"]);
$client->authenticate($gitIndex["gitToken"], Gitlab\Client::AUTH_HTTP_TOKEN);


$project = $client->Repositories()->branch($gitIndex["gitId"], 'master');
$indexes = json_decode($client->repositoryFiles()->getRawFile($gitIndex["gitId"], 'Index.txt', $project['commit']['short_id']), true);

$pge->put('<h3> gitlab URL: '. $gitIndex["url"]);
$pge->put('<h4>'.'<table><thead>
                          <tr>
                            <th>lab title</th>
                            <th>download</th>
                          </tr>
                        </thead>');

for ($row = 0; $row < count($indexes); $row++) {
    $pge->put('<tbody>
                          <tr>
                            <td>'.$indexes[$row]['lab-name'].'</td>
                            <td><button id=index'. $row . 'class="editbtn">download</button></td>
                          </tr>      
                </tbody>');
}
$pge->put('</table>');


$pge->put('<h3> local');
  // show list
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

$pge->put('<h4>'.'<table>
<thead>
  <tr>
    <th>lab index</th>
    <th>title</th>
    <th>status</th>
    <th>update</th>
    <th>commit</th>
    <th>pull</th>
  </tr>
</thead>
<tbody>');


while ( $element = $DBI->getNextData() ){
    $pge->put( '<tr>');
    $pge->put( '<td>' . $element->idx .  '</td>');
    $pge->put( '<td>' . $element->title .  '</td>');
    
    $data = mysqli_fetch_assoc($lgDBI->returnGitID($element->idx));
    if($data){
        
        $onlineHash = checkCurrentHash($gitIndex["url"], $gitIndex["gitToken"], $data['gitId']);
        if ($data['gitHash'] == $onlineHash){
            $pge->put( '<td> Up-to-date </td>');
        }
        else 
        {
            $pge->put( '<td> Online Ahead </td>');
        }
        /*
        if($element->calculateLocalHash() == $data["localHash"]){
            $pge->put( '<td> Up-to-date </td>');
        }
        else {
            $pge->put( '<td> <button id=update' . $data['gitId'].  'class="editbtn">update</button></td>');
        }
        */
        $pge->put( '<td> <button id=update' . $data['gitId'].  'class="editbtn">update</button></td>');
        $pge->put( '<td> <button id=commit' . $data['gitId'].  'class="editbtn">commit</button></td>');
        $pge->put( '<td> <button id=pull' . $data['gitId'].  'class="editbtn">pull</button></td>');
    }
    else{
        $pge->put( '<td> not a git project </td>');
        $pge->put( '<td> unavailable </td>');
        $pge->put( '<td> <button id=create' . $data['gitId'].  'class="editbtn">create</button></td>');
        $pge->put( '<td> unavailable </td>');
    }


    $pge->put( '</tr>');
}
    

$pge->put('</tbody>
</table>');
// show!
  require( $cfg->get("SystemPageLayoutFile") );
?>
