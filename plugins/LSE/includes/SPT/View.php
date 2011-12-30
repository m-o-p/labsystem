<?php
class SPT_View
{
// I don't think we need this.
//    protected $_pathRoot = NULL;
//    
//    public function setPathRoot($pathRoot)
//    {
//        $this->_pathRoot = $pathRoot;
//    }
    
    public function assign($vars)
    {
        foreach ($vars as $key => $value) {
            if ( '_' != substr($key, 0, 1)) $this->{$key} = $value;
        }
    }

    public function render($template, $return = false)
    {
        if ( $return ) {
            ob_start();
            require($template);
            $output = ob_get_clean();
            return $output;
        }
        else {
            require($template);
        }
    }
}

/*
$view = new View();
$view->assign(array('a' => 'AA', 'b' => 'BB', 'c' => 'CC'));
print $view->render('file.tpl', true);
*/