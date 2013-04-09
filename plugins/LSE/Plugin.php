<?php
require_once('LSE/Util.php');

/**
 * We do not bundle EPub.php with our code. The following header code notifies the user to download and install
 * PHPePub themselves.
 */
if (!LSE_Util::fileExists('PHPePub/EPub.php', true)) {
    require_once('LSE/includes/SPT/View.php');
    $oView = new SPT_View();
    $oView->render(LSE_ROOT . '/templates/library-not-found.phtml');
    exit(0);
}

require('PHPePub/EPub.php');

/**
 * Wrapper on top of EPub class.
 * 
 * Adds a new parameter $extraNav to addChapter(). This helps in building hierarchial navigation in .ncx file.
 * Also changed all private variables to protected variables.
 * 
 * @author Bibek Shrestha <bibekshrestha [at] gmail.com>
 * @todo   Uses a temporary hack right now.in addChapter. Should think of a better separation.
 */
class LSE_Plugin extends EPub
{
    /**
     * Extending EPub::addChapter() to include extra navigation
     * @see EPub::addChapter()
     */
//    function addChapter($chapterName, $fileName, $chapterData, $autoSplit = FALSE, $externalReferences = EPub::EXTERNAL_REF_IGNORE, $baseDir = "", $extraNav = array())
//    {
//        if ( $this->isFinalized ) {
//            return FALSE;
//        }
//        $fileName = preg_replace('#\\\#i', "/", $fileName);
//        $fileName = preg_replace('#^[/\.]+#i', "", $fileName);
//        
//        $htmlDir = pathinfo($fileName);
//        $htmlDir = preg_replace('#^[/\.]+#i', "", $htmlDir["dirname"] . "/");
//        
//        $chapter = $chapterData;
//        if ( $autoSplit && is_string($chapterData) && mb_strlen($chapterData) > $this->splitDefaultSize ) {
//            $splitter = new EPubChapterSplitter();
//            
//            $chapterArray = $splitter->splitChapter($chapterData);
//            if ( count($chapterArray) > 1 ) {
//                $chapter = $chapterArray;
//            }
//        }
//        
//        if ( ! empty($chapter) && is_string($chapter) ) {
//            if ( $externalReferences !== EPub::EXTERNAL_REF_IGNORE ) {
//                $this->processChapterExternalReferences($chapter, $externalReferences, $baseDir, $htmlDir);
//            }
//            
//            $this->zip->addFile($chapter, $fileName);
//            $this->fileList[$fileName] = $fileName;
//            $this->chapterCount ++;
//            $this->opf_manifest .= "\t\t<item id=\"chapter" . $this->chapterCount . "\" href=\"" . $fileName . "\" media-type=\"application/xhtml+xml\" />\n";
//            $this->opf_spine .= "\t\t<itemref idref=\"chapter" . $this->chapterCount . "\" />\n";
//            
//            if ( empty($extraNav) ) {
//                $this->ncx_navmap .= "\n\t\t<navPoint id=\"chapter" . $this->chapterCount . "\" playOrder=\"" . $this->chapterCount . "\">\n\t\t\t<navLabel><text>" . $chapterName . "</text></navLabel>\n\t\t\t<content src=\"" . $fileName . "\" />\n\t\t</navPoint>\n";
//            } else {
//                // also process extraNavigation
//                // added by Bibek Shrestha
//                // temporary hack, should use other classes as necessary
//                // each array element has id, ChapterName
//                // print_r($this->buildExtraNavString($extraNav['graph'], $extraNav['elementTable']));
//                $startx = 1; // toc already takes first position
//                /*
//                $this->ncx_navmap = "<navPoint id='tableOfcontents' playOrder='1'>\n" . 
//                    "<navLabel><text>Table Of Contents</text></navLabel>\n" . 
//                    "<content src='$fileName#tableOfContents'/>\n" . 
//                    "</navPoint>\n";
//                // */
//                $this->ncx_navmap .= $this->buildExtraNavString($extraNav['graph'], $extraNav['elementTable'], $startx, $fileName);
//            }
//        
//        } else 
//            if ( is_array($chapter) ) {
//                $partCount = 0;
//                $this->chapterCount ++;
//                
//                $oneChapter = each($chapter);
//                while ($oneChapter) {
//                    list ($k, $v) = $oneChapter;
//                    $c = $v;
//                    if ( $externalReferences !== EPub::EXTERNAL_REF_IGNORE ) {
//                        $this->processChapterExternalReferences($c, $externalReferences, $baseDir);
//                    }
//                    $partCount ++;
//                    $this->zip->addFile($c, $fileName . "-" . $partCount);
//                    $this->fileList[$fileName . "-" . $partCount] = $fileName . "-" . $partCount;
//                    
//                    $this->opf_manifest .= "\t\t<item id=\"chapter" . $this->chapterCount . "-" . $partCount . "\" href=\"" . $fileName . "-" . $partCount . "\" media-type=\"application/xhtml+xml\" />\n";
//                    
//                    $this->opf_spine .= "\t\t<itemref idref=\"chapter" . $this->chapterCount . "-" . $partCount . "\" />\n";
//                    $oneChapter = each($chapter);
//                }
//                
//                $this->ncx_navmap .= "\n\t\t<navPoint id=\"chapter" . $this->chapterCount . "-1\" playOrder=\"" . $this->chapterCount . "\">\n\t\t\t<navLabel><text>" . $chapterName . "</text></navLabel>\n\t\t\t<content src=\"" . $fileName . "-1\" />\n\t\t</navPoint>\n";
//            }
//        return TRUE;
//    }

    /**
     * Sets the table of contents.
     * 
     * @param array $graph
     * @param array $elementTable
     * @param int $startId starting id to generate playOrder
     * @param string $fileName
     */
    function buildExtraNavString($graph, &$elementTable, &$startId = 1, $fileName = '')
    {
        if ( ! count($graph) )
            return '';
        
        $output = '';
        $outputTemplate = "<navPoint id='%s' playOrder='%d'>\n" . "<navLabel><text>%s</text></navLabel>\n" . "<content src='%s'/>\n" . "%s\n" . "</navPoint>\n";
        
        if ( $startId == 1 && isset($this->fileList["CoverPage.html"])) {
            $output = sprintf($outputTemplate, "", 0, 'CoverPage', "CoverPage.html", '', '');
        }
        
        $origFilename = $fileName;
        foreach ( $graph as $id => $element ) {
            $elementStartId = $startId;
            $startId ++;
            $elementId = $elementTable[$id][0];
            
            // On recursion, the first time, fileName is empty. As such l3, l4, l5 is set only for first entry
            if (empty($origFilename)) $fileName = $elementId . '.html';
            $elementLabel = $elementTable[$id][1];
            $childOutput = $this->buildExtraNavString($element, $elementTable, $startId, $fileName);
            
            // @todo same problem here, string "PasteBin & Feedback" was received at some point
            $output .= sprintf($outputTemplate, $elementId, $elementStartId, 
                LSE_Util::filterPTag(htmlspecialchars($elementLabel, ENT_COMPAT, 'UTF-8', false))
                , $fileName . '#' . $elementId, $childOutput);
        }
        
        return $output;
    }
    
    public function getDocRoot()
    {
        return $this->docRoot;
    }
    
    public function setDocRoot($docRoot)
    {
        $docRoot .= (substr($docRoot, -1) == "/")?"":"/";
        $this->docRoot = $docRoot;
    }
    
    // Builds the Ncx Navigation with the given Graph and ElementMap
    public function setNcxFromGraph($graph, $elementTable)
    {
        $this->ncx_navmap = $this->buildExtraNavString($graph, $elementTable);
    }
}
