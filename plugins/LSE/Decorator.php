<?php

require_once('Util.php');
require_once('includes/SPT/View.php');

class LSE_Decorator
{
    /**
     * Generates string from the element
     * 
     * @param type $type
     * @param output $content
     * @param LSE_Element $element
     */
    public function decorate($type, $childContent, $element)
    {
        switch ($type) {
            case 'Ll':
                return $this->decorateLowl($childContent, $element);
                
            case 'BC':
                return $this->decorateBigC($childContent, $element);
                
            case 'Lc':
                return $this->decorateLowC($childContent, $element);
            
            case 'Lp':
                return $this->decorateLowP($childContent, $element);
                
            case 'Lm':
                return $this->decorateLowM($childContent, $element);
                
            case 'Li':
                return $this->decorateLowI($childContent, $element);
                
            default:
                throw new Exception('Decoratory type ' . $type . ' not found');
                return $this->decorateDefault($childContent, $element);
        }
    }
    
    public function decorateDefault($content, $element)
    {
        // return $content;
    }
    
    public function decorateLowl($childContent, $element)
    {
        $oView = new SPT_View();
        $vars = array(
            'title'        => $element->getOption('title'),
            'childContent' => $childContent,
            'id'           => $element->getId(),
            'author'       => $element->getOption('authors'),
            'comment'      => $element->getOption('comment'),
            'toc'          => $element->getOption('toc'),
//            'userStyleSheetPath' => $element->getUserStyleSheetPath(),
        );
        $oView->assign($vars);
        return $oView->render(LSE_ROOT . "/templates/decorators/Ll.phtml", true);
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
