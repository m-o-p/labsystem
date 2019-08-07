<?php
include_once('Element.php');

/**
 * A book is a special LSE_Element
 * 
 * A book contains all the elements that are being exported.
 * A book contains one or many l elements each representing one LAB
 * 
 * @author bibek
 *
 */
class LSE_Book extends LSE_Element
{
    protected $userStyleSheetPath;
    
    /**
     * @return the $title
     */
    public function getTitle()
    {
        return $this->getOption('title');
    }

    public function getAuthors()
    {
        return $this->getOption('authors');
    }

    public function getComment()
    {
        return $this->getOption('comment');
    }

    public function getLang()
    {
        return $this->getOption('lang');
    }
    
    public function getCoverImage()
    {
        return $this->getOption('coverImage');
    }
    
    public function getPreface()
    {
        return $this->getOption('preface');
    }

	public function __construct()
    {
        parent::__construct();
        $this->type = 'book';
    }
    
    public function setTitle($title)
    {
        $this->addOption('title', $title);
    }
    
    public function setAuthors($authors)
    {
        $this->addOption('authors', $authors);
    }
    
    public function setComment($comment)
    {
        $this->addOption('comment', $comment);
    }
    
    public function setLang($lang)
    {
        $this->addOption('lang' ,$lang);
    }
    
    public function setCoverImage($pathToImage)
    {
        $this->addOption('coverImage', $pathToImage);
    }
    
    public function setUserStyleSheetPath($userStyleSheetPath)
    {
        $this->userStyleSheetPath = $userStyleSheetPath;
    }
    
    public function setPreface($preface)
    {
        $this->addOption('preface', $preface);
    }
    
    public function getUserStyleSheetPath()
    {
        return $this->userStyleSheetPath;
    }
    
    public function buildGraph($parentFilter = NULL)
    {
        // first key is the id of the book itself
        $graph = array();
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
    
    /**
     * Returns the list of current child elements. Filtering can be done with
     * a. ParentType Filter
     * b. ElementType Filter
     * 
     * @param array|string $parentFilter
     * @param array|string $elementFilter
     * @return array List of children of Book
     */
    public function getElementTable($parentFilter = NULL, $elementFilter = NULL)
    {
        $elementTable = array();
        foreach ($this->elements as $element) {
            if ($parentFilter) {
                if (!LSE_Util::checkParentType($element->getId(), $parentFilter)) {
                    continue;
                }
            }
            
            if ($elementFilter) {
                if (!is_array($elementFilter)) {
                    $elementFilter = array($elementFilter);
                }
                if ( !in_array(LSE_Util::getTypeFromId($element->getId()), $elementFilter)) {
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
     * Returns an array of all l elements
     * 
     * @return array List of all l elements
     */
    public function getChapters()
    {
        return $this->getElementTable(NULL, 'l');
    }
    
    public function renderMultiChapterFrontPage() {}
    public function renderMultiChapterTOC() {}
    
    /**
     * Renders the chapter with given id say l3
     * 
     * @param unknown_type $chapterId
     */
    public function renderChapter($chapterId)
    {
        $graph = $this->buildGraph();
        $graph = $graph[$chapterId];
        
        $output = $this->renderRecursiveGraph($graph);
        $toc    = $this->renderTableOfContents($chapterId );
        
        $lElement = $this->elements[$chapterId];
        $lElement->addOption('toc', $toc);
        
        // Let lElement's decorator handle the rendering
        return $lElement->decorator->decorate($lElement->type, $output, $lElement);
    }

    /**
     * Book elements need to be rendered is specific order. The function takes care of that ordering.
     * 
     * @see LSE_Element::render()
     */
    public function render()
    {
        $chapters = $this->getChapters();
        
        // just take the first chapter of the chapter list and pass it
        // Only for compatibility purpose.
        list($chapterId, $chapterValue) = each($chapters);
        return $this->renderChapter($chapterId);
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
    
    /**
     * Chapter id is something like l3
     * 
     * @param string $chapterId
     */
    protected function renderTableOfContents($chapterId)
    {
        $graph = $this->buildGraph(array('l', 'C'));
        $graph = array( $chapterId => $graph[$chapterId] ); // we don't want graph for other l elements
        
        $elementTable = $this->getElementTable();
        return $this->buildNavigation($graph, $elementTable);
    }
    
    protected function buildNavigation($graph, $elementTable)
    {
//        var_dump($graph);
//        var_dump($elementTable);
        
        if ( ! count($graph) )
            return '';
        
        $outputTemplate = "<li><a href='#%s'>%s</a>%s</li>\n";
        $output = '';
        foreach ( $graph as $id => $element ) {
            if (!isset($elementTable[$id])) {
                print 'Continuing for $id : ' . $id . "\n";
                continue;
            }
//            print 'Building for $id : ' . $id . "\n";
            $elementId = $elementTable[$id][0];
            $elementLabel = LSE_Util::filterPTag(htmlspecialchars($elementTable[$id][1], ENT_COMPAT, 'UTF-8', false));
            $childOutput = $this->buildNavigation($element, $elementTable);
            
            // @todo this should not be here, move into templates somehow
            $output .= sprintf($outputTemplate, $elementId, $elementLabel, $childOutput);
        }
        
        if ($output) {
//            print "============================" . $output; 
            return "<ul>\n$output</ul>\n";
        }
        else
            return $output;
    }
}
