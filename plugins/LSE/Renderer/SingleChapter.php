<?php
require_once('LSE/Renderer/Interface.php');
require_once('LSE/Logger.php');
require_once('LSE/Util/CoverImageGenerator.php');
class LSE_Renderer_SingleChapter implements LSE_Renderer_Interface
{
    protected $_log;
    protected $_engine;
    protected $_plugin;
    
    public function __construct(LSE_Engine $engine)
    {
        $this->_log = new LSE_Logger('LSE_Renderer_SingleChapter');
        $this->_engine = $engine;
        $this->_plugin = $engine->getEpub();
    }
    
    public function render()
    {
        $this->_setupCoverPageAndCoverImage();
        $this->_setupFrontMatter();
        $this->_setupTOC();
        $this->_setupChapters();
        $this->_finalize();
    }
    
    protected function _setupCoverPageAndCoverImage()
    {
        $vars['book'] = $this->_engine->getBook();
        $coverImage = $this->_engine->getBook()->getCoverImage();
        if ( $coverImage ) {
            $ig = new LSE_Util_CoverImageGenerator();
            $srcPath = $coverImage;
            $dstPath = tempnam('/tmp', 'LSE_CoverImage_');
            $text    = $this->_engine->getBook()->getTitle();
            $ig->setSrcImagePath($srcPath);
            $ig->setDstImagePath($dstPath);
            $ig->setText($text);
            $ig->generate();
            $this->_plugin->setCoverImage('coverImage', file_get_contents($dstPath), 'image/png');
            
            // Readers like Kindle could use a separate startpage
            // images/coverImage will be created by Plugin, we just reference to it from coverpage
            $vars['imagePath'] = "images/coverImage";
            $view = new SPT_View();
            $view->assign($vars);
            $coverPage = $view->render(LSE_ROOT . '/templates/coverpage.phtml', true);
            $this->_plugin->addChapter( 'coverpage', 'CoverPageInner.html', $coverPage, FALSE);
            
            unlink($dstPath);
        }
    }
    
    protected function _setupFrontMatter()
    {
        $vars = array('book' => $this->_engine->getBook());
        $view = new SPT_View();
        $view->assign($vars);
        $frontMatter = $view->render(LSE_ROOT . '/templates/frontmatter.phtml', true);
        
        // Since everything is done relative to $this->_plugin->docRoot, we have to reset it
        $this->_plugin->addChapter( 'frontmatter', 'frontmatter.html', $frontMatter, FALSE, EPUB::EXTERNAL_REF_ADD);
    }
    
    protected function _setupTOC()
    {
        $book = $this->_engine->getBook();
        $graph = $book->buildGraph(array("l", "C"));
        $elementTable = $book->getElementTable();
        
        $this->_plugin->setNcxFromGraph($graph, $elementTable);
    }
    
    protected function _setupChapters()
    {
        // we have only one chapter but the same function should work on both single and multi chapter
        $book = $this->_engine->getBook();
        $chapters = $book->getChapters();
        $output = '';
        foreach ($chapters as $chapterId => $chapter) {
            $output = $book->renderChapter($chapterId);
            
            if (LSE_DEBUG) {
                print $output;
            }
            else {
                $this->_plugin->addChapter( $chapterId, $chapterId . '.html', $output, FALSE, EPub::EXTERNAL_REF_ADD);
            }
        }
    }
    
    protected function _finalize()
    {
        $isFinalized = $this->_plugin->finalize();
        $this->_log->log($isFinalized, 'isFinalized');

        $bookTitle = str_replace(' ', '_', strtolower($this->_engine->getBook()->getTitle()));
            
            // bookTitle is usually htmlencoded, so decode this first
        $bookTitle = LSE_Util::filterPTag($bookTitle);
        if (!LSE_DEBUG) {
            $this->_plugin->sendBook($this->_engine->getBook()->getTitle());
        }

        return NULL;
    }
}