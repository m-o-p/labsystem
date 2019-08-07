<?php

/**
 * Assuming we run our script from path/view/filename.php, 
 * LSE_PATH_LABSYSTEM will point to path/view
 */
if (!defined('LSE_PATH_LABSYSTEM'))
    define('LSE_PATH_LABSYSTEM', getcwd());

if (!defined('LSE_ROOT'))
    define('LSE_ROOT', dirname(__FILE__));

/**
 * If LSE_DEBUG is true, it prints the html to the browser instead sending epub file.
 */
$debug = FALSE;
//$debug = TRUE;
define('LSE_DEBUG', $debug);

/**
 * Setting up include path to contain the plugins folder
 * 
 * This way we can import classes with include_once('PluginName/PluginClass.php')
 */
set_include_path(implode(PATH_SEPARATOR, array(
    get_include_path(),
    realpath(LSE_ROOT . "/..")
)));
