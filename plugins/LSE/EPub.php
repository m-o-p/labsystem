<?php
// error_reporting(E_ALL & ~E_NOTICE);
require_once('LSE/Decorator.php');
require_once('LSE/Book.php');
require_once('LSE/Element.php');
require_once('LSE/Engine.php');
require_once('LSE/Logger.php');

/**
 * EPub Engine for exporting
 * 
 * Architecturally, the class behaves as a Controller in an MVC patter. It uses LSE_Element objects as Models
 * to store the data passed through the save function. 
 * 
 * It uses an instance of Decorator to generate output.
 * 
 * Responsibilities
 * - Models : Save the rendered html string
 * - Decorator (View) : Put extra elements around it make it suitable for output
 * - PHPePUB : Class which stores the output
 * 
 * @author Bibek Shrestha <bibekshrestha@gmail.com>
 * @todo PHPePub implementation can be improved.
 */
class LSE_EPub implements LSE_Engine
{
    protected $_log;
    protected $decorator;
    protected $book;
    
    public function __construct()
    {
        $this->decorator = new LSE_Decorator();
        $this->book = new LSE_Book();
        $this->book->setDecorator($this->decorator);
        
        $this->_log = new LSE_Logger('LSE_Epub');
    }
    
    public function getBook()
    {
        return $this->book;
    }
    
    public function setBook($book)
    {
        $this->book = $book;
    }
    
    public function getDecorator()
    {
        return $this->decorator;
    }
    
    /**
     * Options are saved from calling classes.
     * The options usually have EPub specific fields like
     * - Title
     * - Author
     * - Lang
     * - Comment
     * - StyleSheetPath
     * 
     * <b>Note:</b> This completely overwrites the existing configuration of the Book Object
     * @param array $options
     */
    public function setOptions(array $options)
    {
        return $this->book->setOptions($options);
    }
    
    /**
     * @todo this would throw an exception if l element has not been initialized.
     */
    public function save($type, $id, $content, array $options = array())
    {
        $element = new LSE_Element();
        $element->setDecorator($this->decorator);
        
        $element->setType( $type );
        $element->setId( $id );
        $element->setContent( $content );
        $element->setOptions( $options );
        
        $this->book->addElement( $element );
    }
    
    public function render()
    {
        $epubPlugin = $this->getEpub();
        if ($this->book->getOption('isMultiChapterEnabled')) {
            require_once('LSE/Renderer/MultiChapter.php');
            $renderer = new LSE_Renderer_MultiChapter($this);
            
        }
        else {
            require_once('LSE/Renderer/SingleChapter.php');
            $renderer = new LSE_Renderer_SingleChapter($this);
        }
        return $renderer->render();
    }
    
    public function getEpub()
    {
        require_once('LSE/Plugin.php');
        $book = new LSE_Plugin();
        
        $title = LSE_Util::filterPTag($this->book->getTitle());
        $title = LSE_Util::string_decode($this->book->getTitle());
        $author = LSE_Util::filterPTag($this->book->getAuthors());
        $author = $this->book->getAuthors();

        $identifier   = utf8_encode( $this->book->getOption('identifier') );
        $language     = utf8_encode( $this->book->getOption('lang') );
        $description  = utf8_encode( $this->book->getOption('description') );
        $publisher    = utf8_encode( $this->book->getOption('publisher') );
        $publisherUrl = utf8_encode( $this->book->getOption('publisherUrl') );
        if (empty($publisherUrl)) $publisherUrl = 'http://labsystem.sf.net';
        $date         = utf8_encode( $this->book->getOption('date') );
        $rights       = utf8_encode( $this->book->getOption('rights') );
        $sourceUrl    = utf8_encode( $this->book->getOption('sourceUrl') );
        if (empty($sourceUrl)) $sourceUrl = 'http://labsystem.sf.net';
        
        $book->setTitle($title);
        $book->setIdentifier($identifier, EPub::IDENTIFIER_URI); // Could also be the ISBN number, prefered for published books, or a UUID.
        $book->setLanguage($language); // Not needed, but included for the example, Language is mandatory, but EPub defaults to "en". Use RFC3066 Language codes, such as "en", "da", "fr" etc.
        $book->setDescription($description);
        $book->setAuthor($author, $author); 
        $book->setPublisher($publisher, $publisherUrl); // I hope this is a non existant address :) 
        $book->setDate($date); // Strictly not needed as the book date defaults to time().
        $book->setRights($rights); // As this is generated, this _could_ contain the name or licence information of the user who purchased the book, if needed. If this is used that way, the identifier must also be made unique for the book.
        $book->setSourceURL($sourceUrl);
        
        $book->setDocRoot(LSE_PATH_LABSYSTEM);
        return $book;
    }
}