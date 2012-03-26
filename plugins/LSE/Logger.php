<?php
class LSE_Logger
{
    const DEBUG = 1;
    protected $_className;
    
    public function __construct($className)
    {
        $this->_className = $className;
    }
    
    public function log($mixed, $name = null, $type = LSE_Logger::DEBUG)
    {
        // if debugger is enabled globally
        if (LSE_DEBUG && $type == LSE_Logger::DEBUG) {
            print "<pre>";
            print $this->_className . " :: ";
            if ($name !== null) {
                print $name . " :: "; var_dump($mixed);
            }
            else {
                var_dump($mixed);
            }
            print "</pre>";
        }
    }
}