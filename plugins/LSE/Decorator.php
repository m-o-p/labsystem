<?php

require_once('LSE/Util.php');
require_once('LSE/includes/SPT/View.php');

class LSE_Decorator
{
    /**
     * Generates string from the element
     * 
     * @param type $type
     * @param output $content
     * @param LSE_Element $element
     */
    public function decorate($type, $content, $element)
    {
        switch ($type) {
            case 'book':
                return $this->decorateBook($content, $element);
                
            case 'BC':
                return $this->decorateBigC($content, $element);
                
            case 'Lc':
                return $this->decorateLowC($content, $element);
            
            case 'Lp':
                return $this->decorateLowP($content, $element);
                
            case 'Lm':
                return $this->decorateLowM($content, $element);
                
            case 'Li':
                return $this->decorateLowI($content, $element);
                
            default:
                return $this->decorateDefault($content, $element);
        }
    }
    
    public function decorateDefault($content, $element)
    {
        return $content;
    }
    
    public function decorateBook($content, $element)
    {
        $oView = new SPT_View();
        $vars = array(
            'title'   => $element->getTitle(),
            'content' => $content,
            'id'      => $element->getId(),
            'author'  => $element->getAuthors(),
            'comment' => $element->getComment(),
            'toc'     => $element->getOption('toc'),
            'userStyleSheetPath' => $element->getUserStyleSheetPath(),
        );
        $oView->assign($vars);
        return $oView->render(LSE_ROOT . "/templates/decorators/book.phtml", true);
    }
    
    public function decorateBigC($childContent, $element)
    {
        $oView = new SPT_View();
        $vars = array(
            'title'        => $element->getOption('title'),
            'childContent' => $childContent,
            'content'      => $element->getContent(),
            'id'           => $element->getId(),
        );
        $oView->assign($vars);
        return $oView->render(LSE_ROOT . "/templates/decorators/BigC.phtml", true);
    }
    
    /**
     * @todo strange we also receive BigC in LowC
     */
    public function decorateLowC($content, $element)
    {
        $includePageBreak = false;
        $isParentBigC = LSE_Util::checkParentType($element->getId(), "C");
        $selfType = LSE_Util::getTypeFromId($element->getId());
        if ($isParentBigC && $selfType == 'c') {
            $includePageBreak = true;
        }
        
        $oView = new SPT_View();
        $vars = array(
            'title'            => $element->getOption('title'),
            'content'          => $content,
            'id'               => $element->getId(),
            'includePageBreak' => $includePageBreak,
        );
        $oView->assign($vars);
        return $oView->render(LSE_ROOT . "/templates/decorators/LowC.phtml", true);
    }
    
    public function decorateLowP($content, $element)
    {
        $includePageBreak = false;
        if ( LSE_Util::checkParentType($element->getId(), "C") ) {
            $includePageBreak = true;
        }
        $oView = new SPT_View();
        $vars = array(
            'content'          => $element->getContent(),
            'id'               => $element->getId(),
            'includePageBreak' => $includePageBreak,
        );
        $oView->assign($vars);
        return $oView->render(LSE_ROOT . "/templates/decorators/LowP.phtml", true);
    }
    
    public function decorateLowI($content, $element)
    {
        $oView = new SPT_View();
        $vars = array(
            'id'       => $element->getId(),
            'question' => $element->getContent(),
        );
        $oView->assign($vars);
        $output = $oView->render(LSE_ROOT . '/templates/decorators/lowI.phtml', true);
        return $output;
    }
    
    public function decorateLowM($content, $element)
    {
        $oView = new SPT_View();
        $vars = array(
            'id'       => $element->getId(),
            'question' => $element->getOption('question'),
            'answers'  => $element->getOption('answerArray'),
        );
        $oView->assign($vars);
        $output = $oView->render(LSE_ROOT . '/templates/decorators/lowM.phtml', true);
        return $output;
    }
}
