<?php
require_once('bootstrap.php');
include_once('LSE/EPub.php');

/**
 * An exporter class to export LabSystem (LS) lessons to other formats
 *
 * Implements Facade pattern.
 * Exposes save() and render() functions to the main LS and delegates the task to
 * one of the available Engines.
 * 
 * Note: This class can be used in two ways.
 *   1. Create instance of the class using the normal way
 *      $exporter = new LSE_Exporter();
 *      $exporter->save();
 *      $exporter->render();
 *      
 *   2. Another way to use the class would be to let itself create an instance and store it
 *      $exporter = LSE_Exporter::getInstance();
 *      $exporter->save();
 *      $exporter->render();
 *      
 *      The second type maintains only one instance. This allows main LS to not worry about maintining a global
 *      scope of the object.
 * 
 * @author Bibek Shrestha <bibekshrestha@gmail.com>
 */
class LSE_Exporter
{
    /**
     * 
     * Store an instance of self, LSE_Exporter
     * @var LSE_Exporter
     */
    static protected $instance;
    
    /**
     * The engine doing the actual conversion
     * @var LSE_Engine
     */
    protected $exportEngine;
    
    /**
     * behaves as a Registry for one single instance of the object
     * @return LSE_Exporter
     */
    public static function getInstance()
    {
        if ( self::$instance == null ) {
            self::$instance = new LSE_Exporter();
        }
        return self::$instance;
    }
    
    /**
     * Deletes the instance of of LSE_Exporter
     */
    public static function removeInstance()
    {
        self::$instance = null;
    }
    
    public function __construct()
    {
        // assumption is, we could have other type of exporters in future
        $this->exportEngine = new LSE_EPub();
    }
    
    /**
     * Delegates the save task to ExportEngine
     * 
     * @param string $type
     * @param string $id
     * @param string $content
     * @param array $options
     */
    function save($type, $id, $content, array $options = array())
    {
        return $this->exportEngine->save($type, $id, $content, $options);
    }
    
    /**
     * Allow passing configuration options from Exporter to Engine class
     * 
     * @param array $option
     */
    function setOptions(array $options)
    {
        return $this->exportEngine->setOptions($options);
    }
    
    public function render()
    {
        return $this->exportEngine->render();
    }
}
