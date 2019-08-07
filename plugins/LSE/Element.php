<?php

require_once('Decorator.php');

class LSE_Element
{
    protected $id;
    protected $elements = array();
    protected $type;
    protected $title;
    protected $content;
    protected $renderedContent;
    protected $options = array();
    
    protected $decorator;
    
    /**
     * @return the $content
     */
    public function getContent()
    {
        return $this->content;
    }

	/**
     * @return the $id
     */
    public function getId()
    {
        return $this->id;
    }

	/**
     * @return the $options
     */
    public function getOptions()
    {
        return $this->options;
    }

	/**
     * @param array $options
     */
    public function setOptions($options)
    {
        $this->options = $options;
    }
    
    public function addOption($key, $value)
    {
        $this->options[$key] = $value;
    }

    public function getOption($name)
    {
        if ( isset($this->options[$name]) ) {
            return $this->options[$name];
        } else {
            return NULL;
        }
    }
	/**
     * @param string $content
     */
    public function setContent($content)
    {
        $this->content = $content;
    }

	/**
     * @param string $type
     */
    public function setType($type)
    {
        $this->type = $type;
    }

	public function __construct()
    {
    }
    
    public function setId($id)
    {
        $this->id = $id;
    }
    
    public function setDecorator($decorator)
    {
        $this->decorator = $decorator;
    }
    
    /**
     * Add the element as child of current element
     *
     * @param $element LSE_Element the element to add as child
     * @return void
     */
    public function addElement(LSE_Element $element)
    {
        $this->elements[$element->getId()] = $element;
    }
    
    public function getElements()
    {
        return $this->elements;
    }
    
    /**
     * Returns a rendered element
     *
     * Recursively renders all child elements first, adds their string and passes them to 
     * decorator to do formatting
     *
     * @return string
     */
    public function render()
    {
        $output = '';
        foreach ($this->elements as $element) {
            $output .= $element->render();
        }
        
//        var_dump($this);
        return $this->decorator->decorate($this->type, $output, $this);
    }
}
