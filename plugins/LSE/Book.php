<?php
include_once('Element.php');

/**
 * Current assumption is a Book is a single html file.
 * Division of Chapters will be done using TOC
 * 
 * Enter description here ...
 * @author bibek
 *
 */
class LSE_Book extends LSE_Element
{
    protected $title;
    protected $authors;
    protected $comment;
    protected $lang;
    protected $userStyleSheetPath;
    
    /**
     * @return the $title
     */
    public function getTitle()
    {
        return $this->title;
    }

	/**
     * @return the $authors
     */
    public function getAuthors()
    {
        return $this->authors;
    }

	/**
     * @return the $comment
     */
    public function getComment()
    {
        return $this->comment;
    }

	/**
     * @return the $lang
     */
    public function getLang()
    {
        return $this->lang;
    }

	public function __construct()
    {
        parent::__construct();
        $this->type = 'book';
    }
    
    public function setTitle($title)
    {
        $this->title = $title;
    }
    
    public function setAuthors($authors)
    {
        $this->authors = $authors;
    }
    
    public function setComment($comment)
    {
        $this->comment = $comment;
    }
    
    public function setLang($lang)
    {
        $this->lang = $lang;
    }
    
    public function setUserStyleSheetPath($userStyleSheetPath)
    {
        $this->userStyleSheetPath = $userStyleSheetPath;
    }
    
    public function getUserStyleSheetPath()
    {
        return $this->userStyleSheetPath;
    }
    
    public function buildGraph($parentFilter = NULL)
    {
        // first key is the id of the book itself
        $graph = array($this->getId() => array());
        foreach ($this->elements as $element) {
            
            // Filter items by parent type
            // Sometimes we filter out leaf nodes which have small c as parent and only take those with BigC
            if ($parentFilter) {
                if (!LSE_Util::checkParentType($element->getId(), $parentFilter)) {
                    continue;
                }
            }
            
            $idParts = LSE_Util::getIdParts($element->getId());
            $idPartsOfParent = array_slice($idParts, 0, count($idParts) - 1);
            
            // we assume when a child comes, its parent would already be added to the graph
            $tree = &$graph;
            foreach ($idParts as $key => $idPart) {
                $parentId = implode( ".", array_slice($idParts, 0, $key + 1) );
                if (isset($tree[ $parentId ])) {
                    $tree = &$tree[ $parentId ];
                } else {
                    $tree[ $parentId ] = array();
                    $tree = &$tree[ $parentId ];
                }
            }
        }
        
        return $graph;
    }
    
    public function getElementTable($parentFilter = NULL)
    {
        $elementTable = array($this->getId() => array($this->getId(), $this->getTitle()));
        foreach ($this->elements as $element) {
            if ($parentFilter) {
                if (!LSE_Util::checkParentType($element->getId(), $parentFilter)) {
                    continue;
                }
            }
            $elementTable[$element->getId()] = array(
                $element->getId(),
                $element->getOption('title'),
            );
        }
        return $elementTable;
    }
    
    /**
     * Book elements need to be rendered is specific order. The function takes care of that ordering.
     * 
     * @see LSE_Element::render()
     */
    public function render()
    {
        $graph        = $this->buildGraph();
        $firstElement = each($graph);
        $output       = $this->renderRecursiveGraph($firstElement['value']);
        $toc          = $this->renderTableOfContents();
        $this->addOption('toc', $toc);
        return $this->decorator->decorate($this->type, $output, $this);
    }
    
    protected function renderRecursiveGraph($graph)
    {
        if (!count($graph)) return '';
        
        $output = '';
        foreach ($graph as $elementId => $subGraph) {
            $subGraphOutput = $this->renderRecursiveGraph($subGraph);
            $element = $this->elements[$elementId];
            $elementOutput = $element->decorator->decorate($element->type, $subGraphOutput, $element);
            $output .= $elementOutput;
        }
        
        return $output;
    }
    
    protected function renderTableOfContents()
    {
        $graph = $this->buildGraph(array("l", "C"));
        $elementTable = $this->getElementTable(array("l", "C"));
        return $this->buildNavigation($graph, $elementTable);
    }
    
    public function buildNavigation($graph, $elementTable)
    {
//        var_dump($graph);
//        var_dump($elementTable);
        
        if ( ! count($graph) )
            return '';
        
        $outputTemplate = "<li><a href='#%s'>%s</a>%s</li>\n";
        $output = '';
        foreach ( $graph as $id => $element ) {
            $elementId = $elementTable[$id][0];
            $elementLabel = LSE_Util::filterPTag(htmlspecialchars($elementTable[$id][1], ENT_COMPAT, 'UTF-8', false));
            $childOutput = $this->buildNavigation($element, $elementTable);
            
            // @todo this should not be here, move into templates somehow
            $output .= sprintf($outputTemplate, $elementId, $elementLabel, $childOutput);
        }
        
        return "<ul>\n$output</ul>\n";
    }
}
