<?php

if ( isset( $GLOBALS["gDBI"] ) ) return; /* require_once doesn't work well */


require_once( INCLUDE_DIR."/classes/Ddbc.inc" );

class GitDBInterface extends Ddbc{
    var $myDBC;
    var $myTable;

    function __construct(){
        global $dDBI;
        $this->myDBC = $dDBI;
        $this->myTable = "git_index";

	/** check if DB exists 
	$isDB = $this->myDBC->table_exists( $this->myTable );
	if ( !$isDB ) // if not fix yourself.
	// TODO: Put SQL file into setup and use that one here => only one place...
	{
		$sqlq = "CREATE TABLE git_index (
		id INT(32) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
		gitId INT(32),
		title VARCHAR(32),
        url VARCHAR(500),
		gitToken VARCHAR(64)
		)";
		$this->myDBC->query( $sqlq );
	}
	*/
   }

    function returnIndices(){
        $result = $this->myDBC->mkSelect("gitId", $this->myTable);
        return $result;
    }
    
    function returnFirstIndex(){
        $result = $this->myDBC->mkSelect("*", $this->myTable, "id = 1");
        return $result;
    }
    
    function returnToken($gitid){
        $result = $this->myDBC->mkSelect("gitToken ", $this->myTable, "gitId = '" . $gitid . "'");
        return $result;
    }
    
    function insertIndex($gitid, $title, $gittoken){
        $this->myDBC->mkInsert("gitId = '".$gitid."', title = '". $title ."', gitToken = '". $gittoken . "'", $this->myTable );
    }

    function deleteIndex($gitid){
        $this->myDBC->mkDelete($this->myTable, "gitId = '".$gitid."'");
    }
}

$GLOBALS["gDBI"] = new GitDBInterface();
