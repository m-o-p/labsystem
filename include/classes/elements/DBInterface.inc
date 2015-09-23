<?php
/**
 * The mother class with the common functionality of all DBIs
 * @author pahl
 *
 */
class DBInterface{
  var $myDBC;
  var $myTable;
  var $alreadyFetchedElementsArray; // caches already queried database query results

  /**
   * Constructor of this class.
   */
  function DBInterface(){
    $this->alreadyFetchedElementsArray = Array();
  }

  /**
   * If the element identified by $idx was already fetched before it is directly served from memory and not queried from the DB.
   * @param unknown $idx
   * @return boolean
   */
  function getFromCache( $idx ){
    if (array_key_exists($idx, $this->alreadyFetchedElementsArray)){
      return $this->alreadyFetchedElementsArray[$idx];
    } else {
      return false;
    }
  }

  /**
   * Add the element to the cache.
   * @param unknown $element
   */
  function cache(&$newElement){
      if (isset($GLOBALS['IamOnlyGeneratingTheMenu']) && $GLOBALS['IamOnlyGeneratingTheMenu'] && $newElement->elementId == 'l'){
      // for the menu no data has to be loaded at all
      return;
    }
    $this->alreadyFetchedElementsArray[$newElement->idx] = $newElement;
  }

  /**
   * Deletes the element with $idx from the database.
   *
   * @param $idx   The element's index (also idx in the database).
   * @return BOOL  The return value of the query.
   */
  function deleteData( $idx ){
    return $this->myDBC->mkDelete( $this->myTable, "idx='".$idx."'" );
  }

  var $queryResult;
  /**
   * Queries for all elements. P.e. used at the manage page.
   *
   * @param $orderBy    Can be any of the table's keys. The result will be ordered by this key.
   * @param $asc        Should the datasets be in ascending order?
   */
  function getAllData( $orderBy="", $asc=true ){
    if ( $orderBy != "" ) if ( $asc ) $orderBy .= " ASC"; else $orderBy .= " DESC";
    $this->queryResult = $this->myDBC->mkSelect( "*", $this->myTable, "", $orderBy );
  }

  /**
   * Loads the data of all keys in the given array and stores the elements as values.
   * @param unknown $indicesArray The array to load the elements identified by the keys of.
   * @param string $isInColl
   * @param string $isInLab
   */
  function loadIdcs2Array( $indicesArray, $isInColl=false, $isInLab=false ){
    $indexSelect="idx='".implode("' OR idx='", $indicesArray)."'";
    $this->queryResult = $this->myDBC->mkSelect( "*", $this->myTable, $indexSelect );
    $resultArray = Array();
    while($nextElement=$this->getNextData($isInColl,$isInLab)){
      $resultArray[$nextElement->idx]=$nextElement;
    }
    return $resultArray;
  }

  /**
   * Returns the amount of returned datasets from the query;
   *
   * @return  amount of returned datasets from the query
   */
  function allSize(){
    return $this->myDBC->datasetsIn( $this->queryResult );
  }

  /**
   * Returns the next element from the getAllData() query or false (no more results).
   *
   * @return LcElement
   * @return FALSE     If no more datasets are available.
   */
  function getNextData($isInColl=false, $isInLab=false){
    if ($data = mysql_fetch_array($this->queryResult)){
      return $this->createElementFromDbResult( $data, $isInColl, $isInLab );
    } else {
      return false;
    }
  }
}