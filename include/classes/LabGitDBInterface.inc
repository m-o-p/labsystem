<?php

if ( isset( $GLOBALS["lgDBI"] ) ) return; /* require_once doesn't work well */


require_once( INCLUDE_DIR."/classes/Ddbc.inc" );

class LabGitDBInterface extends Ddbc{
    var $myDBC;
    var $myTable;

    function __construct(){
        global $dDBI;
        $this->myDBC = $dDBI;
        $this->myTable = "git_labs";

	/** check if DB exists 
	$isDB = $this->myDBC->table_exists( $this->myTable );
	if ( !$isDB ) // if not fix yourself.
	// TODO: Put SQL file into setup and use that one here => only one place...
	{
		$sqlq = "CREATE TABLE git_labs(
		id INT(32) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
        idx INT(32),
		gitId INT(32),
        gitSolutionId INT(32),
        gitIndex INT(32),
		title VARCHAR(32),
		gitHash VARCHAR(256),
        hashSolution VARCHAR(256),
        localHash VARCHAR(256)
		)";
		$this->myDBC->query( $sqlq );
	}*/
   }

   function returnGitID($idx){
       $result = $this->myDBC->mkSelect("gitId", $this->myTable, "idx = '" . $idx . "'");
       return $result;
   }
   
   function returnGitIndex($gitid){
       $result = $this->myDBC->mkSelect("gitIndex", $this->myTable, "gitId = '" . $gitid . "'");
       return $result;
   }
   
   function returnGitHash($gitid){
       $result = $this->myDBC->mkSelect("gitHash", $this->myTable, "gitId = '" . $gitid . "'");
       return $result;
   }
   
   function returnLocalHash($gitid){
       $result = $this->myDBC->mkSelect("LocalHash", $this->myTable, "gitId = '" . $gitid . "'");
       return $result;
   }
   
   function insertLab($idx, $gitid, $title, $gitindex, $githash, $localhash){
       $this->myDBC->mkUpdIns("gitId = '" . $gitid . "', title = '" . $title ."', idx= '" . $idx . "', gitHash = '" . $githash . "', localHash = '" . $localhash . "', gitIndex = '". $gitindex . "'", $this->myTable, "gitId = '" . $gitid . "'");
   }
   
   function updateGitHash($gitid, $githash){
       $this->myDBC->mkUpdate("gitHash = '" . $githash . "'", $this->myTable, "gitId = '" . $gitid . "'");
   }
   
   function updateLocalHash($gitid, $localhash){
       $this->myDBC->mkUpdate("localHash = '" . $localhash . "'", $this->myTable, "gitId = '" . $gitid . "'");
   }
   
   function deleteIndex($gitid){
       $this->myDBC->mkDelete($this->myTable, "gitId = '".$gitid."'");
   }

}

$GLOBALS["lgDBI"] = new LabGitDBInterface();
