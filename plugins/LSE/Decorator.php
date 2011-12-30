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
            'author'  => utf8_encode($element->getAuthors()),
            'comment' => $element->getComment(),
        );
        $oView->assign($vars);
        return $oView->render(LSE_ROOT . "/templates/decorators/book.phtml", true);
    }
    
    public function decorateBigC($content, $element)
    {
        
        $template = "<h3 class='bigC section' id='%s'>%s</h3>\n";
        return sprintf($template, $element->getId(), htmlentities($element->getOption('title')));
        // Do nothing since we will have this element in Lc as well
    }
    
    public function decorateLowC($content, $element)
    {
        $class = 'lowC section';
        if ( LSE_Util::checkParentType($element->getId(), "C")) {
            $class .= " parentBigC";
        }
        elseif ( LSE_Util::checkParentType($element->getId(), "l")) {
            $class .= " parentLowL"; 
        }
        $template = "<h3 class='$class' id='%s'>%s</h3>\n";
        return sprintf($template, $element->getId(), htmlentities($element->getOption('title')));
    }
    
    public function decorateLowP($content, $element)
    {
        $class = 'lowC collection_content';
        if ( LSE_Util::checkParentType($element->getId(), "C")) {
            $class .= ' parentBigC';
        }
        $template = "<div class='$class' id='%s'>%s</div>\n";
        return sprintf($template, $element->getId(), LSE_Util::filterPTag($element->getContent()));
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
