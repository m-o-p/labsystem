<?php

require_once( INCLUDE_DIR."/classes/Wdbc.inc" );

class EmojiDBInterface{
    var $myDBC;
    var $myTable;

    function __construct(){
        global $wDBI;
        $this->myDBC = $wDBI;
        $this->myTable = "emoji_selections";
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

        // checks whether the user has already given an input for this element, (elemId, uid) - primary key
        $checkKey = $this->myDBC->row_exists("*", $this->myTable, "elemId = '". $elemId ."' and uid = '". $uid ."'");
        $resultCheckKey = $checkKey->fetch_array();

        if ($resultCheckKey[0] == 0) {

            // if the entry does not exist insert the new row
            $this->myDBC->mkInsert( "elemId = '".$elemId."', uid = '". $uid ."', emojiId = '". $emojiId . "'", $this->myTable);

        } else {
            // select the emojiId for which there is an entry in the table
            $select = $this->myDBC->mkSelect("emojiId", $this->myTable, "elemId = '". $elemId ."' and uid = '". $uid ."'");
            $resultSelect = $select->fetch_array();

            // check if the user has clicked on the same emoji for the second time
            if ($resultSelect[0] == $emojiId) {
                // if she has - delete the entry
                $this->myDBC->mkDelete($this->myTable, "elemId = '". $elemId ."' and uid = '". $uid ."'");
            } else {
                // if the user has clicked on a new emoji - update the table entry
                $this->myDBC->mkUpdate("emojiId = '". $emojiId ."'", $this->myTable, "elemId = '". $elemId ."' and uid = '". $uid ."'");
            }

        }
    }
}

?>
