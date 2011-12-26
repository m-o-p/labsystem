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
* This class is the interface between my application and the database.
*/

/**
* If you decide not to use mysql but something else it might be sufficient to make
* changes to the methods implementations here.
* The class also implements some stuff that will be available with mysqli in future php 
* versions so if you have a newer version you might adopt something here.
*
* @module     ../include/classes/DBConnection.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
if ( class_exists( "DBConnection" ) ) return;

class DBConnection{
  var $link;
  var $dataBase;

  /**
  * Open a connection to the database.
  * @param $host      The server to connect to.
  * @param $user      The username to connect with.
  * @param $password  The password to connect with.
  * @param $dataBase  The database to connect to.
  * @return           A mysql connection identifier or false
  */
  function DBConnection( $host, $user, $password, $dataBase ){
    $this->link = mysql_pconnect( $host, $user, $password, 2/*CLIENT_FOUND_ROWS*/ );
    /*
"Note: When using UPDATE, MySQL will not update columns where the 
new value is the same as the old value. This creates the 
possiblity that mysql_affected_rows() may not actually equal the 
number of rows matched, only the number of rows that were literally 
affected by the query." 


As of PHP 4.3.0 (I assume, I only tried with 4.3.2), you can make 
mysql_affected_rows() return the number of rows matched, even if 
none are updated. 


You do this by setting the CLIENT_FOUND_ROWS flag in 
mysql_connect(). For some reason, not all the flags are defined in 
PHP, but you can use the decimal equivalent, which for 
CLIENT_FOUND_ROWS is 2. 

So, for example: 

$db= mysql_connect("localhost", "user", "pass", false, 2); 
mysql_select_db("mydb", $db); 

Comment by Marc-oliver Pahl: In my php version this doesn't take effect :(
    */
    if ( !$this->link ) { trigger_error( $host.' unreachable with given credentials!' , E_USER_ERROR ); exit; };
    $this->dataBase = $dataBase;
    return $this->link;
  }
  
  /**
  * Performs a query on the database. Should only be called from inside here!
  * @param  $theQuery   The query to perform.
  * @return             The return value of the query.
  */
  function query( $theQuery ){
    mysql_select_db( $this->dataBase, $this->link );
    $ret = mysql_query( $theQuery, $this->link );
    $this->reportErrors( $theQuery ); // If there was an error, it gets reported immediately. Debug.
    //echo( "<p style=\"background-color: #ff0000; color: #ffffff; padding: 5px; margin-bottom: 10px\">".$theQuery."<br>affected rows: ".mysql_affected_rows()."</p>\n");
    return $ret; // "$ret = mysql... ; do something return $ret" is no good becaus mqsql_affected_rows() and so on isn't set then!
  }
  
  /* Returns the amount of datasets returned by the query.
   * @param $queryRes The resource ID of the query.
   * @return          The amount of datasets returned by the query.
   */
  function datasetsIn( $queryRes ){
    return mysql_num_rows( $queryRes );
  }
  
  /**
  * Performs a query on the database. Should only be called from inside here!
  * @param  $what   Fields to select.
  * @param  $from   Name of the database to select from.
  * @param  $where  Clause defining where to search (p.e. name="marc").
  * @param  $order  Clause defining how to order the results (p.e. idx ASC).
  * @param  $order  Defining the grouping.
  * @return         The return value of the query.
  */
  /*private*/ function mkSelect( $what, $from, $where="", $orderBy="", $groupBy='' ){
    // SELECT $what FROM $from {WHERE $where} {GROUP BY $groupBy} {ORDER BY $orderBy}
                          $theQuery  = "SELECT ".$what." FROM ".$from;
    if ( $where != "" )   $theQuery .= " WHERE ".$where;
    if ( $groupBy != "" ) $theQuery .= " GROUP BY ".$groupBy;
    if ( $orderBy != "" ) $theQuery .= " ORDER BY ".$orderBy;
    return $this->query(  $theQuery );
  }

  /**
  * Performs an insert on the database.
  * @param  $what   Fields and values to insert.
  * @param  $into   Name of the database to insert in.
  * @return BOOL    The return value of the query.
  */
  function mkInsert( $what, $into ){
                          $theQuery  = "INSERT INTO ".$into." SET ".$what;
    return $this->query(  $theQuery );
  }
  
  /**
  * Performs an insert on the database and returns the value of the autoincremented field used.
  * @param  $what   Fields and values to insert.
  * @param  $into   Name of the database to insert in.
  * @return BOOL    The return value of the query.
  */
  function mkInsertReturnIDX( $what, $into ){
                   $theQuery  = "INSERT INTO ".$into." SET ".$what;
    $this->query("LOCK TABLES ".$into." WRITE");
    $this->query(  $theQuery );
    $idx = $this->query("SELECT LAST_INSERT_ID()");
    $this->query("UNLOCK TABLES");
    return mysql_result($idx, 0);
  }
  
  /**
  * Performs a delete on the database.
  * @param  $from   Name of the database to select from.
  * @param  $where  Clause defining where to delete (p.e. idx=42).
  * @return BOOL    The return value of the query.
  */
  function mkDelete( $from, $where ){
    $theQuery  = "DELETE FROM ".$from." WHERE ".$where;
    return $this->query(  $theQuery );
  }
  
  /**
  * Tries to Performs an update query on the database.
  * @param  $what   Fields and values to insert.
  * @param  $into   Name of the database to insert in.
  * @param  $where  Clause defining where to update (p.e. name="marc").
  * @return BOOL    The return value of the query.
  */
  function mkUpdate( $what, $into, $where ){
                          $theQuery  = "UPDATE ".$into." SET ".$what." WHERE ".$where;
    return $this->query(  $theQuery );
  }
  
  /**
  * Tries to Performs an update query on the database. If that doesn't work it makes an insert.
  * @param  $what   Fields and values to insert.
  * @param  $into   Name of the database to insert in.
  * @param  $where  Clause defining where to update (p.e. name="marc").
  * @return BOOL    The return value of the query.
  */
  function mkUpdIns( $what, $into, $where ){
    $res = $this->mkUpdate( $what, $into, $where );

    /* This is a workaround if the Selectors don't work.
       The following check is necessary because an update with nothing to change returns 0 affected rows.
       If the set flag at the db_connect above would work properly this would get unnecessary!
    */
    $theQuery  = "SELECT 'idx' FROM ".$into." WHERE ".$where;
    if (mysql_num_rows( $this->query(  $theQuery ) ) == 0){
      $toReplace = Array( "&&", "||" ); // to be replaced by , before insert!
      return $this->mkInsert( $what.( $where != '' ?  ', '.str_replace( $toReplace, ',', $where ) : '' ),
                              $into );
    } else return $res;
    
    /* 2008/ 01/ 11: mysql_affected_rows() reported 1 on Gentoo even though it should be 0?!? so this is used now
    ereg("Rows matched: ([0-9]*)", mysql_info(), $rows_matched);
    
    if ( (mysql_affected_rows() > 0) && ($rows_matched > 0) )
    // succeeded
      return $res;
    else{ // if that doesn't succeed: INSERT INTO $into SET $what.$where
      $toReplace = Array( "&&", "||" ); // to be replaced by , before insert!
      return $this->mkInsert( $what.( $where != "" ?  ", ".str_replace( $toReplace, ",", $where )), $into  : '' );
    }
       */
  }
  
  /**
  * This function reports mysql_errors if present.
  * @param  $theQuery  The resource handler of the query (If "" takes last performed).
  * @return            True if there were errors false otherwise.
  */
  function reportErrors( $theQuery="" ){
    if ( mysql_error() != '' ){
      trigger_error( /*$theQuery.*/"DB Connector --&gt; "./*$this->dataBase.': '.*/mysql_error(), E_USER_ERROR );
      return true;
    }else
      return false;
  }
  
  /**
  * Checks if a database exists on the server connected to. (only used in the setup -> not very important to implement).
  * @param  $databaseName  Name of the database to check existance of.
  * @return                True if database exists false otherwise.
  */
  function db_exists( $databaseName ){
    $result = $this->query("SHOW DATABASES");
    while( $data = mysql_fetch_array( $result ) ) if ( $databaseName == $data[0] ) return true;
    return false;
  }
  
  /**
  * Checks if a table exists in the database.
  * @param  $tableName  Name of the table to check existance of.
  * @return             True if table exists false otherwise.
  */
  function table_exists( $tableName ){
    $result = $this->query("SHOW TABLES");
    while( $data = mysql_fetch_array( $result ) ) if ( $tableName == $data[0] ) return true;
    return false;
  }
  
    
  /**
  * Checks if a column in the specified table exists in the database.
  *
  * This gets only used by the setup routine.
  *
  * @param  $tableName    Name of the table.
  * @param  $column Name  Name of the column.
  * @return               True if column exists false otherwise.
  */
  function column_exists( $tableName, $columnName ){
    $result = $this->query("SHOW COLUMNS FROM ".$tableName."");
    while( $data = mysql_fetch_array( $result ) ) if ( $columnName == $data[0] ) return true;
    return false;
  }
  
  /** It is necessary to escape all input values in order to prevent from SQL-Injection!!!
   *  So all user/ url-values MUST be escapeStringed!!!
   *
   *  Also for other database connections (PostGres etc.) keep in mnd this function
   *  is called on all values before passed to the DBQuery.
   *
   *  There might be necessity to encode special chars in a query String.
   *  p.e.: I've got to say "hello" -> I\'ve got to say \"hello\"
   */
  function escapeString( $val ){
    return mysql_real_escape_string( $val );
    /* If magic_quotes is on you must remove the quotes first!
       Otherwise there will be backslashes in your html-code...
    */
  }

  /** Analogon the other way if the DB might return escaped chars...
   */
  function unEscapeString( $val ){
    return $val;
  } 
}
?>