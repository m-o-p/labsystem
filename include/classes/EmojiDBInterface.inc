#<?php

if ( isset( $GLOBALS["eDBI"] ) ) return; /* require_once doesn't work well */


require_once( INCLUDE_DIR."/classes/Wdbc.inc" );

class EmojiDBInterface{
    var $myDBC;
    var $myTable;

    function __construct(){
        global $wDBI;
        $this->myDBC = $wDBI;
        $this->myTable = "emoji_selections";

	/** check if DB exists
	$isDB = $this->myDBC->table_exists( $this->myTable );
	if ( !$isDB ) // if not fix yourself.
	// TODO: Put SQL file into setup and use that one here => only one place...
	{
		$sqlq = "CREATE TABLE emoji_selections (
		id INT(32) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
		emojiId VARCHAR(32),
		elemId VARCHAR(32),
		uid VARCHAR(64),
		history VARCHAR(256)
		)";
		$this->myDBC->query( $sqlq );
	}
	*/
   }

    /** Finds the number of selections of a given emoji for a given element. Uses the full address of the element.
     *
     * @param $emojiId      emoji ID
     * @param $elemId       full address of the element
     * @return The          the return value of the query
     */
    function findNumOfSelectionsFor($emojiId, $elemId){
        $result = $this->myDBC->mkSelectCount("emojiId", $this->myTable, "emojiId = '" . $emojiId . "' and elemId = '". $elemId . "'");
        return $result;
    }

    /** Finds the number of selections of a given emoji for a given element. Uses the full address of the element.
     *
     * @param $uid	    uid of the user
     * @param $elemId       full address of the element
     * @return $result      returns the chosen emoji id.
     */
    function findUserSelectionsFor($uid, $elemId){
        $result = $this->myDBC->mkSelect("emojiId", $this->myTable, "elemId = '" . $elemId . "' and uid = '". $uid . "'");
        return $result;
    }


    /** Finds the number of selections for a given emoji type.
     *
     * @param $emojiId      emoji ID
     * @return The          the return value of the query
     */
    function allEmojisOfType($emojiId){
        $result = $this->myDBC->mkSelectCount("emojiId", $this->myTable, "emojiId = '" . $emojiId . "'");
        return $result;
    }

     /** Finds the number of selections for a given element.
     *
     * @param $emojiId      emoji ID
     * @return The          the return value of the query
     */
    function allEmojisByElemId($elemId){
        $result = $this->myDBC->mkSelectCount("elemId", $this->myTable, "elemId = '" . $elemId . "'");
        return $result;
    }


    /** Finds the number of selections of a given emoji for a given element. Uses the shortcut and index of the element.
     *
     * @param $shortcutAndIndex     shortcut and index of the element
     * @param $emojiID              emoji ID
     * @return The                  the return value of the query
     */
    function findNumOfSelForElementShortCutAndIndex ($shortcutAndIndex, $emojiID) {
        $result = $this->myDBC->mkSelectCount("emojiId", $this->myTable, "elemId like '%". $shortcutAndIndex ."' and emojiId = '". $emojiID ."'");
        return $result;
    }

    /** Handles the entries in the DB. Called when a user clicks on an emoji.
     *  Decides whether to insert, delete or update an entry.
     *
     * @param $elemId       full address of the element
     * @param $uid          user ID
     * @param $emojiId      emoji ID
     */
    function insertRow($elemId, $uid, $emojiId){
        // TODO: use mkUpdIns from DBConnection (should do exactly this)

        // checks whether the user has already given an input for this element, (elemId, uid) - primary key
        $checkKey = $this->myDBC->row_exists("uid", $this->myTable, "elemId = '". $elemId ."' and uid = '". $uid ."'");
        $resultCheckKey = $checkKey->fetch_array();
        if ($resultCheckKey[0] == 0) {

            // if the entry does not exist insert the new row
            $this->myDBC->mkInsert( "elemId = '".$elemId."', uid = '". $uid ."', emojiId = '". $emojiId . "', history = ''", $this->myTable);

        } else {
		$historyArray = $this->myDBC->mkSelect("history", $this->myTable, "elemId = '" . $elemId . "' and uid = '". $uid . "'");
		$history = $historyArray->fetch_array();
		$this->myDBC->mkUpdate("emojiId = '". $emojiId ."'", $this->myTable, "elemId = '". $elemId ."' and uid = '". $uid ."'" );
		$this->myDBC->mkUpdate("history = '" . $emojiId.";".$history[0] . "'", $this->myTable, "elemId = '". $elemId ."' and uid = '". $uid ."'" );
	}
    }
}

$GLOBALS["eDBI"] = new EmojiDBInterface();
