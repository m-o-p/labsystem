<?php
class LSE_Util
{
    /**
     * Decodes id into sub parts
     */
    public static function getIdParts($string)
    {
        $separator = ".";
        return explode($separator, $string);
    }
    
    public static function getTypeFromPart($string)
    {
        if (strlen($string) < 2) {
            return $string;
        }
        
        return substr($string, 0, 1);
    }
    
    /**
     * Returns the type from Id
     * @param string $id
     */
    public static function getTypeFromId($id)
    {
        $idParts = self::getIdParts($id);
        if (!count($idParts)) return false;
        return self::getTypeFromPart( $idParts[ count($idParts) - 1]);
    }
    
    /**
     * Checks whether given parentTypes are the immediate parent of element identified by fullId
     * 
     * @param unknown_type $fullId
     * @param unknown_type $parentType
     */
    public static function checkParentType($fullId, $parentType)
    {
        if ( !is_array($parentType) ) {
            $parentType = array($parentType);
        }
        $parts = self::getIdParts($fullId);
        if (($numOfParts = count($parts)) < 2) return false;
        
        $parentId = $parts[ $numOfParts - 2 ];
        $actualParentType = self::getTypeFromPart($parentId);
        return (in_array($actualParentType, $parentType));
    }
    
    public static function filterPTag($string)
    {
        // @todo LowC sometimes has & instead of escaped &amp; for example in PasteBin & Feedback
        $string = utf8_encode($string);
        // Debug when HTML errors occur...
        // echo( '<br><hr>'.htmlentities( $string ).'<hr><br>' );
        $string = preg_replace_callback('/(href[\s]*=[\s]*["\'])(\.\.\/.*)["\']/U', 
        array('LSE_Util', 'relativeToAbsoluteURI'), $string); 
        
        $domDoc = new DOMDocument();
        $domDoc->recover = true;
        $domDoc->strictErrorChecking = false;
        
        // we need to wrap it inside div because by default if we just put normal text, it is wrapped inside 
        // a p tag eg, if string is "a <p>b</p> c" then result string would be "<p>a </p><p>b</p> c" Note last c is
        // not enclosed in p. Strange
        $domDoc->loadHTML("<div>$string</div>");
        return self::get_inner_html($domDoc->documentElement->firstChild->firstChild);
//        print $result . "///////////////////////\n";
//        if (strpos($string, "Paste") === 0)
//            var_dump(debug_backtrace());
//        return $result;
    }
    
    public static function get_inner_html( $node )
    {
        $innerHTML= '';
        $children = $node->childNodes;
        foreach ($children as $child) {
            $innerHTML .= $child->ownerDocument->saveXML( $child );
        }
    
        return $innerHTML;
    }
    
    public static function getFullURI()
    {
        $pageURL = (isset($_SERVER["HTTPS"]) && $_SERVER["HTTPS"] == "on") ? "https://" : "http://";
        if ($_SERVER["SERVER_PORT"] != "80") {
            $pageURL .= $_SERVER["SERVER_NAME"].":".$_SERVER["SERVER_PORT"].$_SERVER["REQUEST_URI"];
        } 
        else {
            $pageURL .= $_SERVER["SERVER_NAME"].$_SERVER["REQUEST_URI"];
        }
        return $pageURL;
    }
    
    public static function relativeToAbsoluteURI( $matches, $baseUrl = NULL )
    {
        if ( $baseUrl == NULL ) {
            $baseUrl = self::getFullURI();
        }
        $relativeUrl = $matches[2];
        require_once('LSE/includes/url_to_absolute.php');
        $absoluteUrl =  htmlspecialchars(url_to_absolute( $baseUrl, $relativeUrl ), ENT_COMPAT, 'ISO-8859-1', FALSE);
        return $matches[1] . $absoluteUrl . '"';
    }
    
    public static function fileExists($filename, $useIncludePath = null)
    {
        if (!$useIncludePath) {
            return file_exists($filename);
        }
        else {
            $ps = explode(":", ini_get('include_path'));
            foreach ($ps as $path)
            {
                if (file_exists($path.'/'.$filename)) return true;
            }
            if (file_exists($filename)) return true;
            return false;
        }
    }
    
    public static function string_decode($string)
    {
        return $string;
//        return html_entity_decode($string, ENT_COMPAT, 'UTF-8');
    }
    
    public static function string_encode($string)
    {
        return $string;
//        return htmlentities($string, ENT_COMPAT, 'UTF-8');
    }
}
