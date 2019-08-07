<?php

interface LSE_Engine
{
    /**
     * 
     * Save individual LS elements
     * @param string $type LS element types, can contain book, BC, lc, lp, li, lm
     * @param string $id 
     * @param unknown_type $content
     * @param unknown_type $options
     * 
     * @return NULL
     */
    public function save($type, $id, $content, array $options = array());
    
    /**
     * Performs the rendering of the EPub file
     */
    public function render();
}
