<?php
require_once('LSE/Renderer/Interface.php');
require_once('LSE/Logger.php');
require_once('LSE/Util/CoverImageGenerator.php');
require_once('LSE/includes/SPT/View.php');
class LSE_Renderer_MultiChapter implements LSE_Renderer_Interface
{
    protected $_log;
    protected $_engine;
    protected $_plugin;

    public function __construct(LSE_Engine $engine)
    {
        $this->_log = new LSE_Logger('LSE_Renderer_MultiChapter');
        $this->_engine = $engine;
        $this->_plugin = $engine->getEpub();
    }

    public function render()
    {
        $this->_setupCoverPageAndCoverImage();
        $this->_setupFrontMatter();
        $this->_setupMultiChapterTOC();
        $this->_setupPreface();
        $this->_setupChapters();
        $this->_finalize();
    }

    protected function _setupCoverPageAndCoverImage()
    {
        $vars = array('book' => $this->_engine->getBook());
        $coverImage = $this->_engine->getBook()->getCoverImage();
        if ( $coverImage ) {
            $ig = new LSE_Util_CoverImageGenerator();
            $srcPath = $coverImage;
            $dstPath = tempnam(sys_get_temp_dir(), 'LSE_CoverImage_');
            $text    = $this->_engine->getBook()->getTitle();
            $ig->setSrcImagePath($srcPath);
            $ig->setDstImagePath($dstPath);
            $ig->setText($text);
            $ig->generate();
            $this->_plugin->setCoverImage('coverImage', file_get_contents($dstPath), 'image/png');
            
            // Readers like Kindle could use a separate startpage
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

    protected function _setupMultiChapterTOC()
    {
        $book = $this->_engine->getBook();
        $graph = $book->buildGraph(array("l"));
        $elementTable = $book->getElementTable();

        // Add a TOC in each chapter in graph
//        foreach ($graph as $key => $lElement) {
//            $graph[$key] = array("toc" => array()) + $lElement;
//        }
        // $elementTable += array('toc' => array('toc', 'Table of Contents'));

        $graphPrefix = array();
        $elementTablePrefix = array();

        // $graphPrefix['multiChapterTocPage'] = array();
        // We don't show TOC in TOC Page itself
        // $elementTablePrefix['multiChapterTocPage'] = array('multiChapterTocPage', 'Table of Contents');

        if ($book->getPreface()) {
            $graphPrefix['preface'] = array();
            $elementTablePrefix['preface'] = array('preface', 'Preface');
        }
        $graph = $graphPrefix + $graph;
        $elementTable = $elementTablePrefix + $elementTable;

//        $this->_log->log($graph);
//        $this->_log->log($elementTable);

        $this->_plugin->setNcxFromGraph($graph, $elementTable);

        $view = new SPT_View();
        $view->assign(array('graph' => $graph, 'elementTable' => $elementTable));
        $tocChapter = $view->render(LSE_ROOT . '/templates/multichapter/toc.phtml', true);

        $this->_plugin->addChapter( 'multiChapterTocPage', 'multiChapterTocPage.html', $tocChapter, FALSE,
            EPub::EXTERNAL_REF_ADD);
    }

    protected function _setupPreface()
    {
        $preface = $this->_engine->getBook()->getPreface();
        if ($preface) {
            $view = new SPT_View();
            $view->assign(array('preface' => $preface));
            $prefaceContent = $view->render(LSE_ROOT . '/templates/multichapter/preface.phtml', true);
            $this->_plugin->addChapter( 'preface', 'preface.html', $prefaceContent, FALSE, EPub::EXTERNAL_REF_ADD);
        }
    }

    protected function _setupChapters()
    {
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