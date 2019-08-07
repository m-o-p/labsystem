<!--
    labsystem.m-o-p.de -
                    the web based eLearning tool for practical exercises
    Copyright (C) 2009  Marc-Oliver Pahl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
-->
<?php
/**
* Makes all necessary initializations for the labsystem.
* @in $_GET['config'] the config file!
*
* @module     ../setup/setup.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
define( "INCLUDE_DIR", "../include" );

/** CHECK FOR OBVIOUS USER ERRORS */
// check version
require( INCLUDE_DIR."/customErrHandle.inc" );  // The custom Error handler.
if ( version_compare( PHP_VERSION, '5.5.0' ) < 0) {
                                                        trigger_error( "You need at least PHP 5.5 to run the labsystem!", E_USER_ERROR );
                                                        exit;
                                                      }

/** IF NO OBVIOUS USER ERRORS -> Show Info Page */
if ( !isset($_POST['continue']) ){
if ( !file_exists( 'information.txt' ) ){
                                            trigger_error( "/setup/information.txt missing!", E_USER_ERROR );
                                            exit;
                                         }
  echo('
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
  <head>
    <title>Installing labsystem.m-o-p.de...</title>
  </head>
  <body style="margin: 1em; background-color: #89897c; font-family: Verdana, Sans-Serif;">
  <div style="width: 40em; background-color: #ffffff; color: #000000; padding: 1em;">

  <h3>labsystem.m-o-p.de setup started...</h3>

  <p>
    You are about to install the configuration <b>'.$_GET['config'].'</b>.
  </p>

  <p style="color: #ff0000; font-size: 0.8em;">
    Click the following button first to ensure your config file is not readable by everyone.<br>
    <br>
    <a href="../ini/config_'.$_GET['config'].'.ini" target="_blank"><button style="width: 100%">Is my ini file protected?</button></a><br><br>
    A new page will open. If you do not get an access denied warning but see the config file with your passwords, everyone can access it. <br>
    You have to make the ini directory read protected for the web server then! (see install instructions and your web server\'s manual for details.)
  </p>
  <pre>
');
readfile( 'information.txt' );
echo('
  </pre>
  <form method="POST" action="'.$_SERVER['REQUEST_URI'].'">
  <input type="hidden" name="continue" value="yes" checked="checked">
  <input type="submit" value="continue..." style="width: 100%; height: 3em;">
  </form>
  </div>
  </body>
</html>
');
  return;
}
require( INCLUDE_DIR."/classes/Url.inc" );      // Include url handling and rewriting stuff. => Object $url.
                                                // needed to get parameters from the url ($url->get, ->available)
require_once( INCLUDE_DIR."/configuration.inc" );
require_once( INCLUDE_DIR."/classes/DBConnection.inc" );

function say_done(){
                      echo('<span style="color: #55ff55;">o.k.</span><br>'."\r\n");
                   }
function say_skipped(){
                      echo('<span style="color: #ffff55;">skipped (already existing)</span><br>'."\r\n");
                      }
function say_failed($currentDb){
                      echo('<span style="color: #ff5555;">failed, mysql reports: ' . $currentDb->link->error . '</span><br>'."\r\n");
                     }
function say_title( $title ){
                      echo('<h2>'.$title.'</h2>'."\n" );
                            }
function say_toptitle( $title ){
                      echo('<h1>'.$title.'</h1>'."\n" );
                               }

/**
 * Reads the DB dump file that is meant to be executed on the given database.
 * $filename  dump file
 * &$database the database (e.g. $dataDB)
 */
function runMySqlFromFile( $filename, &$database ){
  $myFile = $filename;
  $fh = fopen($myFile, 'r');
  echo( "<br>\r\n" );
  $theData = '';
  if ($fh) while (!feof($fh)) // Loop til end of file.
  {
    $buffer = fgets($fh, 4096); // Read a line.
    $theData .= trim($buffer);
    if ( substr( $theData, -1 ) == ';' ){
      // only one command per execution!!!
      echo( '<span style="color: #aaaaaa;">'.substr( $theData, 0, strpos( $theData, ' ' ) ).':</span> ' );
      $ret = $database->query( $theData );
      if ( $ret === false ) say_failed($database); else say_done();
      $theData = '';
    }
  }
  fclose($fh);

  echo( "<br>\r\n" );
}
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
  <head>
    <title>Setupscript for labsystem.m-o-p.de</title>
    <link rel="shortcut icon" href="../syspix/favicon.ico">
  </head>

  <style type="text/css">
  <!--
    body{
          color: #ffffff;
          font-family: courier, monospace;
          font-size: 0.9em;
          margin: 2em;
          background-color: #000000;
        }
    h2{ /* for subheaders */
        margin-left: -1em;
        font-size: 1.2em;
        color: #aaaaaa;
        border-bottom: solid 1px #aaaaaa;
        margin-top: 2em;
        margin-bottom: 0.5em;
      }
    h1{
        margin-left: -1em;
        font-size: 1.5em;
        color: #cbcbcb;
        border-bottom: solid 1px #cbcbcb;
        margin-top: 4em;
      }
      a {
        color: #aaaaff;
        }
      a:hover {
        color: #ff4444;
              }
      li {
        margin-top: 0.5em;
        margin-bottom: 0.5em;
         }
  -->
  </style>

<body>
<div style="text-align: right">
  <img src="../pix/labsyslogo_443x40.gif" border="0" /><br>
</div>

<?PHP
echo( 'configuration "'.$_GET['config'].'"... '."\n" );
echo( $cfg->get("SystemTitle").'... ' );
echo('running setup script...<br />'."\n");


/************************ user database part: ***************************/
say_title( 'user database part' );
$usrDB = new DBConnection($cfg->get("UserDatabaseHost"),
                          $cfg->get("UserDatabaseUserName"),
                          $cfg->get("UserDatabasePassWord"));

echo( $cfg->get("UserDatabaseName").': ' );
if(!$usrDB->db_exists( $cfg->get("UserDatabaseName") ))
{
       $query = 'CREATE DATABASE IF NOT EXISTS '.$cfg->get("UserDatabaseName").' CHARACTER SET utf8 COLLATE utf8_general_ci';
       $ret = $usrDB->query($query);
       if ( $ret === false ) say_failed($usrDB); else say_done();
} else say_skipped();
$usrDB->selectDb($cfg->get("UserDatabaseName"));

echo( $cfg->get("UserDatabaseName").'.'.$cfg->get("UserDatabaseTable").': ' );
if(!$usrDB->table_exists( $cfg->get("UserDatabaseTable") ))
{
       $query = 'CREATE TABLE '.$cfg->get("UserDatabaseTable").'
                 (
                 '.$cfg->get("UserDBField_username").' char(255) NOT NULL UNIQUE,
                 '.$cfg->get("UserDBField_password").' char(255) NOT NULL,
                 '.$cfg->get("UserDBField_name").' char(255) NOT NULL,
                 '.$cfg->get("UserDBField_forename").' char(255) NOT NULL,
                 '.$cfg->get("UserDBField_email").' char(255) NOT NULL,
                 `desiredTeamPartner` varchar(255) NOT NULL default \'\',
                 `reasonToParticipate` text,
                 '.$cfg->get("UserDBField_uid").' char(32) NOT NULL UNIQUE,
				 `pwReminderToken` char(255) COMMENT "Stores the password reminder token.",
				 `pwReminderValidUntil` int(11) COMMENT "Stores until the token is valid in unixtime.",
                 `registerFor` varchar(255) COMMENT "Used by register.php to indicate where one registered for.",
                 `last_registered` datetime COMMENT "Stores when the last registration happened for this address (for giving the places in the right order)",
                 '.$cfg->get("User_courseID").' tinyint(1) NOT NULL default \'1\',
                 `labsys_mop_last_change` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
                 `history` text,
                 `_unassigned` tinyint(1),
                 PRIMARY KEY  ( '.$cfg->get("UserDBField_username").' ),
                 UNIQUE KEY (`'.$cfg->get("UserDBField_uid").'`),
                 INDEX( '.$cfg->get("UserDBField_username").' )
                 )';
       $ret = $usrDB->query($query);
       if ( $ret === false ) say_failed($usrDB); else say_done();
       /* create admin user */
       $result = $usrDB->mkSelect( $cfg->get("UserDBField_username"), $cfg->get("UserDatabaseTable"), $cfg->get("UserDBField_username").'="'.$cfg->get("RightsAdminUsername").'"' );
       /* does exist? */
       if ( $usrDB->datasetsIn( $result ) > 0 )
        echo("<b>admin user \"".$cfg->get("RightsAdminUsername")."\" exists!</b><br>\n");
       else{
         $query = 'INSERT INTO '.$cfg->get("UserDatabaseTable").'
                   ('.$cfg->get("UserDBField_username").',
                   '.$cfg->get("UserDBField_password").',
                   '.$cfg->get("UserDBField_name").',
                   '.$cfg->get("UserDBField_forename").',
                   '.$cfg->get("UserDBField_email").',
                   '.$cfg->get("UserDBField_uid").',
                   '.$cfg->get("User_courseID").'
                   ) VALUES (
                   \''.$cfg->get("RightsAdminUsername").'\',
                   \''.$usrDB->escapeString( password_hash( 'admin', PASSWORD_DEFAULT ) ).'\',
                   \'admin\',
                   \'Ms/ Mr\',
                   \'root@localhost\',
                   \''.md5( $cfg->get("RightsAdminUsername") ).'\',
                   \'1\'
                   )';
         $ret = $usrDB->query($query);
         if ( $ret !== false )
             echo("<h4><b>admin user \"".$cfg->get("RightsAdminUsername")."\" with password \"admin\" created!</b> (you need this to log on the system. Change the password!!!!!!)</h4><br>\n");
         else
             say_failed($usrDB);
       }
} else say_skipped();

// Add column for new course:
echo( $cfg->get("UserDatabaseName").'.'.$cfg->get("UserDatabaseTable").'-&gt;'.$cfg->get("User_courseID").': ' );
if( $usrDB->db_exists( $cfg->get("UserDatabaseName") ) &&
    $usrDB->table_exists( $cfg->get("UserDatabaseTable") ) &&
    !$usrDB->column_exists( $cfg->get("UserDatabaseTable"), $cfg->get("User_courseID") )
   )
{
       $query = 'ALTER TABLE '.$cfg->get("UserDatabaseTable").' ADD COLUMN
                 '.$cfg->get("User_courseID").' tinyint(1) NOT NULL default \'0\'';
       $ret = $usrDB->query($query);
       if ( $ret === false ) say_failed($usrDB); else say_done();
}

//Create/ Update Admin User
// admin user existing?
$result = $usrDB->mkSelect( $cfg->get("UserDBField_username"), $cfg->get("UserDatabaseTable"), $cfg->get("UserDBField_username").'="'.$cfg->get("RightsAdminUsername").'" & '.$cfg->get("User_courseID").' = 1' );
/* does exist? */
$adminExists = ( $usrDB->datasetsIn( $result ) > 0 );
if (!$adminExists ){
/* create admin user */
$query = 'INSERT IGNORE INTO '.$cfg->get("UserDatabaseTable").'
         ('.$cfg->get("UserDBField_username").',
         '.$cfg->get("UserDBField_password").',
         '.$cfg->get("UserDBField_name").',
         '.$cfg->get("UserDBField_forename").',
         '.$cfg->get("UserDBField_email").',
         '.$cfg->get("UserDBField_uid").',
         '.$cfg->get("User_courseID").'
         ) VALUES (
         \''.$cfg->get("RightsAdminUsername").'\',
         \''.crypt( 'admin', md5( $cfg->get("RightsAdminUsername") ) ).'\',
         \'admin '.$cfg->get("User_courseID").'\',
         \'Ms/ Mr\',
         \'root@localhost\',
         \''.md5( $cfg->get("RightsAdminUsername") ).'\',
         \'1\'
         )';
$ret = $usrDB->query($query);
if ( $ret === false )
    say_failed($usrDB);
else
    echo("<h4><b>admin user \"".$cfg->get("RightsAdminUsername")."\" with password \"admin\" created!</b> (you need this to log on the system. Change the password!!!!!!)</h4><br>\n");
} else say_skipped();
if ( $adminExists ){ //yes
echo("Mapping admin user \"".$cfg->get("RightsAdminUsername")."\" to course: ");
$ret = $usrDB->mkUpdate( $cfg->get("User_courseID").' = 1', $cfg->get("UserDatabaseTable"), $cfg->get("UserDBField_username").'="'.$cfg->get("RightsAdminUsername").'"' );
if ( $ret === false ) say_failed($usrDB); else say_done();
};

// add Demo user
echo("Adding demo user to course ".$cfg->get("User_courseID").": ");
if ( $_GET['config'] == 'demo' ){
  $result = $usrDB->mkSelect( $cfg->get("UserDBField_username"), $cfg->get("UserDatabaseTable"), $cfg->get("UserDBField_uid").'="participant" & '.$cfg->get("User_courseID").' = 1' );
   
	/* does exist? */
	$demoUserExists = ( $usrDB->datasetsIn( $result ) > 0 );
	if (!$demoUserExists ){
	/* create admin user */
	$query = 'INSERT IGNORE INTO '.$cfg->get("UserDatabaseTable").'
	         ('.$cfg->get("UserDBField_username").',
	         '.$cfg->get("UserDBField_password").',
	         '.$cfg->get("UserDBField_name").',
	         '.$cfg->get("UserDBField_forename").',
	         '.$cfg->get("UserDBField_email").',
	         '.$cfg->get("UserDBField_uid").',
	         '.$cfg->get("User_courseID").',
             labsys_mop_last_change
	         ) VALUES (
			"patrice", 
  			"29833a5b1a7f877f998488e057ef0a4e364e4cd4", 
  			"Participant", 
  			"Patrice", 
  			"participant@labsystem.m-o-p.de", 
  			"participant", 
  			1, 
  			"2009-04-16 13:24:24"
	         )';
	$ret = $usrDB->query($query);
	if ( $ret === false )
	    say_failed($usrDB);
	else
	    echo("<h4><b>demo user \"patrice\" created!</b></h4><br>\n");
	} else say_skipped();
}

/* A THE DATABASES */
say_toptitle( 'creating databases' ); /*******************************/

/************************ data database: ***************************/
say_title( 'data database part' );

$dataDB = new DBConnection($cfg->get("DataDatabaseHost"),
                           $cfg->get("DataDatabaseUserName"),
                           $cfg->get("DataDatabasePassWord"));

echo( $cfg->get("DataDatabaseName").': ' );
if(!$dataDB->db_exists( $cfg->get("DataDatabaseName") ))
{
       $query = 'CREATE DATABASE IF NOT EXISTS '.$cfg->get("DataDatabaseName").' CHARACTER SET utf8 COLLATE utf8_general_ci';
       $ret = $dataDB->query($query);
       if ( $ret === false ) say_failed($dataDB); else say_done();
} else say_skipped();
$dataDB->selectDb($cfg->get("DataDatabaseName"));

/************************ working database: ***************************/
say_title( 'working database part' );

$wrkDB = new DBConnection($cfg->get("WorkingDatabaseHost"),
                          $cfg->get("WorkingDatabaseUserName"),
                          $cfg->get("WorkingDatabasePassWord"));

echo( $cfg->get("WorkingDatabaseName").': ' );
if(!$wrkDB->db_exists( $cfg->get("WorkingDatabaseName") ))
{
       $query = 'CREATE DATABASE IF NOT EXISTS '.$cfg->get("WorkingDatabaseName").' CHARACTER SET utf8 COLLATE utf8_general_ci';
       $ret = $wrkDB->query($query);
       if ( $ret === false ) say_failed($wrkDB); else say_done();
} else say_skipped();
$wrkDB->selectDb($cfg->get("WorkingDatabaseName"));



/* B: SYSTEM INITIALIZATIONS */
say_toptitle( 'creating system tables' ); /***********************************/

/************************ user rights part: ***************************/
say_title( 'user rights' );

echo( 'user_rights: ' );
if(!$wrkDB->table_exists("user_rights"))
  runMySqlFromFile( 'sql_new_wdb_ur_tbl.sql', $wrkDB );
else
  say_skipped();


/************************ page element part: ***************************/
say_title( 'storage for <b>P</b>age elements' );

echo( 'pages: ' );
if(!$dataDB->table_exists("pages")){
  runMySqlFromFile( 'sql_new_ddb_pages_tbl.sql', $dataDB );
  echo("demo prelab content created<br>\n");
  echo("after login page created<br>\n");
  echo("authoring tutorial created<br>\n");
  echo("demo lab content created<br>\n");
  echo("button pictogramm explanation created<br>\n");
  echo("suggestion/ complaints page created<br>\n");

   if (substr( $_GET['config'], -9 ) == 'useradmin'){
     $query = "INSERT INTO `pages` VALUES (3, 'UserAdministration interface', '[HTML]\r\n<!-- click on the icon with the eye (second one) on the upper right to see how this page looks like! -->\r\n\r\n<h3>__ELEMENTTITLE__</h3>\r\n\r\n<p style=\"text-align: left;\">\r\n This is the user administration interface.\r\n</p>\r\n\r\n<div style=\"text-align: center;\">\r\n <img src=\"../pix/useradmin_400_60.jpg\"  width=\"400\" height=\"60\" style=\"border: 0;\" alt=\"Das Fest, Karlsruhe Germany; &copy; Marc-Oliver Pahl Summer 2005\" />\r\n <div style=\"padding-left: 0.5em; padding-right: 0.5em; text-align: center; font-size: 0.7em;\">Das Fest, Karlsruhe Germany; &copy; Marc-Oliver Pahl Summer 2005</div>\r\n</div>\r\n\r\n<p style=\"text-align: right;\">\r\n As user of the <a href=\"http://labsystem.m-o-p.de\" target=\"_blank\">labsystem</a> you can <a href=\"../pages/login.php?__LINKQUERY__\">log in here</a> and <a href=\"../pages/uaMyData.php?__LINKQUERY__\">change your personal data</a> (e.g. <a href=\"../pages/uaMyPW.php?__LINKQUERY__\">your password</a>)...<br />\r\n As user administrator you can log in here and administrate the user accounts...<br />\r\n As one having <b><a href=\"../pages/uaUnPwReminder.php?__LINKQUERY__\">forgotten password and/ or username</a></b> click <a href=\"../pages/uaUnPwReminder.php?__LINKQUERY__\">here</a>...<br />\r\n</p>', '', 0, 0, CONCAT( now(), ': Marc-Oliver Pahl' ) );";
     echo('useradmin ');
   }else{
     $query = "INSERT INTO `pages` VALUES (3, 'Demo Startpage', '[HTML]\r\n<!-- click on the icon with the eye (second one) on the upper right to see how this page looks like! -->\r\n\r\n<h3>__ELEMENTTITLE__</h3>\r\n\r\n<p style=\"text-align: left;\">\r\n <b>Well done</b>, the system is running!\r\n</p>\r\n\r\n<div style=\"text-align: center;\">\r\n <img src=\"../pix/startpage.jpg\" width=\"400\" height=\"144\" style=\"border: 0;\" alt=\"A street in Bordeaux, France; &copy; Marc-Oliver Pahl, Summer 2005\" />\r\n <div style=\"padding-left: 0.5em; padding-right: 0.5em; text-align: center; font-size: 0.7em;\">A street in Bordeaux, France; &copy; Marc-Oliver Pahl, Summer 2005</div>\r\n</div>\r\n\r\n<p style=\"text-align: right;\">\r\n Now you can <b><a href=\"../pages/login.php?__LINKQUERY__\">log in</a></b>...\r\n</p>', '', 0, 0, CONCAT( now(), ': Marc-Oliver Pahl' ));";
     echo('demo ');
   }
   $dataDB->query($query);
   echo(" startpage inserted<br>\n");

} else say_skipped();


/************************ collection element part: ***************************/
say_title( 'storage for <b>C</b>ollection elements' );

echo( 'collections: ' );
if(!$dataDB->table_exists("collections"))
  runMySqlFromFile( 'sql_new_ddb_collections_tbl.sql', $dataDB );
else
  say_skipped();

/************************ multiple_choices element part: ***************************/
say_title( 'storage for <b>M</b>ultiple choice elements' );

echo( 'multiple_choices: ' );
if(!$dataDB->table_exists("multiple_choices")){
  runMySqlFromFile( 'sql_new_ddb_multiple_choices_tbl.sql', $dataDB );
  echo("demonstration lab multiple choice created<br>\n");
} else say_skipped();

/**************************** working database ****************************/
say_title( 'storage for <b>M</b>ultiple choice answers' );

echo( 'multiple_choice_answers: ' );
if(!$wrkDB->table_exists("multiple_choice_answers"))
  runMySqlFromFile( 'sql_new_wdb_multiple_choices_tbl.sql', $wrkDB );
else
  say_skipped();

/************************ inputs element part: ***************************/
say_title( 'storage for <b>I</b>nput elements' );

echo( 'inputs: ' );
if(!$dataDB->table_exists("inputs"))
  runMySqlFromFile( 'sql_new_ddb_input_tbl.sql', $dataDB );
else
  say_skipped();

/**************************** working database ****************************/
say_title( 'storage for <b>I</b>nput answers' );

echo( 'input_answers: ' );
if(!$wrkDB->table_exists("input_answers"))
  runMySqlFromFile( 'sql_new_wdb_input_answers_tbl.sql', $wrkDB );
else
  say_skipped();

echo( 'input_answers_uid_team: ' );

if(!$wrkDB->table_exists("input_answers_uid_team"))
  runMySqlFromFile( 'sql_new_wdb_input_answers_uid_team_tbl.sql', $wrkDB );
else
  say_skipped();

echo( 'input_answers_locks: ' );
if(!$wrkDB->table_exists("input_answers_locks"))
  runMySqlFromFile( 'sql_new_wdb_input_answers_locks_tbl.sql', $wrkDB );
else
  say_skipped();

/************************ event log part: ***************************/
say_title( 'storage for event log' );

echo( 'event_log: ' );
if(!$dataDB->table_exists("event_log"))
  runMySqlFromFile( 'sql_new_wdb_event_log_tbl.sql', $wrkDB );
else
  say_skipped();

/**************************** labs database ****************************/
say_title( 'storage for <b>L</b>ab elements' );

echo( 'labs: ' );
if(!$dataDB->table_exists("labs"))
  runMySqlFromFile( 'sql_new_ddb_lab_tbl.sql', $dataDB );
else
  say_skipped();

/**************************** working database ****************************/
say_title( 'storage for <b>L</b>ab user data' );

echo( 'lab_uid_status: ' );
if(!$wrkDB->table_exists("lab_uid_status"))
  runMySqlFromFile( 'sql_new_wdb_lab_uid_status_tbl.sql', $wrkDB );
else
  say_skipped();

/***** schedule *****/
say_title( 'storage for <b>S</b>chedule data' );

echo( 'schedules: ' );
if(!$wrkDB->table_exists("schedules"))
  runMySqlFromFile( 'sql_new_wdb_schedules_tbl.sql', $wrkDB );
else
  say_skipped();

/* If necessary create menu-file */
if ( !file_exists( $cfg->get("SystemResourcePath").$cfg->get("SystemMenuFile") ) )
  if (!copy(  $cfg->get("SystemResourcePath").'menu_prototype.ini',
              $cfg->get("SystemResourcePath").$cfg->get("SystemMenuFile")
            )
      ) echo("failed to copy menu...<br>\n");
  else  echo("menu copied...<br>\n");

/* If necessary create userRoles-file */
if ( !file_exists( $cfg->get("SystemResourcePath").$cfg->get("SystemUserRoles") ) )
  if (!copy(  $cfg->get("SystemResourcePath").'default_user_roles.inc',
              $cfg->get("SystemResourcePath").$cfg->get("SystemUserRoles")
            )
      ) echo("failed to copy user roles...<br>\n");
  else  echo("user roles copied...<br>\n");

/**************************** labs database ****************************/
say_title( 'style sheets' );
/* If necessary create the userStyleSheet-file */
if ( !file_exists( $cfg->get("UserStyleSheet") ) )
  if (!copy(  '../css/sys/labsys_user_style_proto.css',
              $cfg->get("UserStyleSheet")
            )
      ) echo("failed to copy user stylesheet...<br>\n");
  else  echo("user stylesheet copied...<br>\n");

  /* If necessary create the printStyleSheet-file */
if ( !file_exists( $cfg->get("PrintStyleSheet") ) )
  if (!copy(  '../css/sys/labsys_print_style_proto.css',
              $cfg->get("PrintStyleSheet")
            )
      ) echo("failed to copy print stylesheet...<br>\n");
  else  echo("print stylesheet copied...<br>\n");


/**/
say_toptitle( 'Checking directories...' ); /***********************************/
/*
* Checks if a directory is writable and returns o.k. or failed.
* To do so it creates the subfolder "/test23r2" and deletes it if successful.
*/
$toDoChown = ""; // Collects the Linux commands that could be issued as root to set the permissions.
function checkDirectoryWritable( $configFieldName ){
  global $cfg, $toDoChown;
  $directory = ( substr( $cfg->get($configFieldName), -1, 1 ) == DIRECTORY_SEPARATOR ?
                    $cfg->get($configFieldName) : // is already a directory...
                    dirname($cfg->get($configFieldName)) ); // is not...
  echo( '<br><br><span style="color: #ffff99">['.$configFieldName.': '.$directory.']</span> ' );
  if (!file_exists($directory)){
    echo('not existing. Creating it... ');
    if (mkdir($directory, 0755, true)){
      echo('<span style="color: #77ff77;">o.k.</span>');
    }else{
      echo('<span style="color: #ff7777;">failed!</span>');
      $toDoChown .= 'mkdir '.$directory."\r\n";
    }
    echo('<br>');
  }else{
    echo('exists. Testing subfolder... Creating... ');
    if (mkdir($directory.'/test23r2', 0755, true)){
      echo('<span style="color: #77ff77;">o.k.</span> Deleting... '.(rmdir($directory.'/test23r2/') ?
                                                                       '<span style="color: #77ff77;">o.k.</span>' :
                                                                       '<span style="color: #ff7777;">failed!</span>' ));
    }else{
      echo('<span style="color: #ff7777;">failed!</span>');
      $toDoChown .= 'chown -R '.getmygid().' '.$directory."\r\n";
    }
    echo('<br>');
  }
}

say_title('User Uploads');
checkDirectoryWritable('UploadDirectory');
/* If necessary create the userStyleSheet-file */
if ( !file_exists( $cfg->get('UploadDirectory').'.htaccess' ) )
  if (!copy(  $cfg->get('SystemResourcePath').'.htaccess',
              $cfg->get('UploadDirectory').'.htaccess'
            )
      ) echo("failed to copy .htaccess to ".$cfg->get('UploadDirectory')."...<br>\n");
  else  echo(".htaccess copied to ".$cfg->get('UploadDirectory')."...<br>\n");


say_title('Export/ Import Uploads');
checkDirectoryWritable('exportImportDir');
checkDirectoryWritable('importPictureDir');
checkDirectoryWritable('importFilesDir');

checkDirectoryWritable('UserStyleSheet');

say_title('Summary expected Protection of Directories and Files');
echo('<span style="color: #ff5555">Please make sure the following access policies are met.</span><br>');
/*
* Echos $configFieldName \t its value \t $msg
*/
function fileNote( $configFieldName, $msg ){
  global $cfg;
  echo( str_pad($configFieldName,            30, ' ', STR_PAD_RIGHT).
        str_pad($cfg->get($configFieldName), 50, ' ', STR_PAD_RIGHT).
        str_pad($msg,                        30, ' ', STR_PAD_RIGHT).
        "\r\n" );
}
echo( '<pre><span style="color: #888888">' );
fileNote( 'fieldName', 'access policy</span>' );
fileNote( 'UploadDirectory', 'php rw, www <span style="color: #ff5555">deny</span>' );
fileNote( 'exportImportDir', 'php rw, www <span style="color: #ff5555">deny</span>' );
fileNote( 'SystemResourcePath', 'php rw , www <span style="color: #ff5555">deny</span>' );
fileNote( 'SystemMenuFile',  'php rw, www <span style="color: #ff5555">deny</span>' );
fileNote( 'UserStyleSheet',  'php rw, www r' );
fileNote( 'importPictureDir','php rw, www r' );
fileNote( 'importFilesDir',  'php rw, www r' );
echo( '</pre>' );

if ($toDoChown!=''){
  echo( 'You may issue the following commands under Linux (as admin):'."\r\n".
        '<pre>'.$toDoChown.'</pre>' );
}

// Upload limit
$max_upload = (int)(ini_get('upload_max_filesize'));
$max_post = (int)(ini_get('post_max_size'));
$memory_limit = (int)(ini_get('memory_limit'));
$upload_mb = min($max_upload, $max_post, $memory_limit);
echo( '
  <span style="color: #ff5555">The <b>upload limit</b> is currently set to <b>'.$upload_mb.'M</b></span><br>
  To change it change the values of the following variables in your webserver config or .htaccess.<br>
  <span style="color: #888888">(The minimum of the three restricts the upload size).</span><br>
  php_value upload_max_filesize '.$max_upload.'M<br>
  php_value post_max_size '.$max_post.'M<br>
  php_value memory_limit '.$upload_mb.'M<br>
  <br>
  You can also change the values in your php.ini which affects all hosts on the machine then.
');


$instanceID = $configPrefix.$GLOBALS['url']->get("config");
$fileName = "../backup.txt";
say_toptitle('Adding new config '.$instanceID.' to the backup.txt for backup'); /***********************************/
$oldBackupTXT = file_get_contents($fileName);
echo($instanceID.' --> '.$fileName.'... ' );
if( strpos($oldBackupTXT,$instanceID) === false) {
  $oldBackupTXT .= $instanceID.PHP_EOL;
  file_put_contents($fileName, $oldBackupTXT);
  say_done();
}else{
  say_skipped();
}

/* C: AFTER INSTALLATION NOTES */
say_toptitle( 'Installation done!' ); /***********************************/
/***** additional infos *****/
$url='http://'.$_SERVER['SERVER_NAME'].$_SERVER['REQUEST_URI'];
$url1=substr( $url, 0, strripos( $url, '/setup' ) );
$url=$url1.'/?config='.$_GET['config'];
echo('
  If you found errors above, try to solve them and run the setup script again.<br><br>
  You can access your new instance by the following URL:<br>
  <a href="'.$url.'">'.$url.'</a><br><br>
  Do not forget to change your admin user\'s password if you got the notification above!!!!
');

if (substr( $_GET['config'], -9 ) == 'useradmin') echo('<br><br>If you want to proceed with your initial setup you can <a href="'.$url1.'/setup?config=demo"><button>continue with the setup for \'demo\'...</button></a>');

echo('<br /><br />*** HAVE FUN WITH THE SYSTEM! ***<br /><br />_');


?>
</body>
</html>
