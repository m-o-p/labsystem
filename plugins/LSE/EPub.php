<?php
// error_reporting(E_ALL & ~E_NOTICE);
require_once('LSE/Decorator.php');
require_once('LSE/Book.php');
require_once('LSE/Element.php');
require_once('LSE/Engine.php');

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
class LSE_Epub implements LSE_Engine
{
    protected $decorator;
    protected $book;
    
    public function __construct()
    {
        $this->decorator = new LSE_Decorator();
        $this->book = new LSE_Book();
    }
    
    /**
     * @todo this would throw an exception if l element has not been initialized.
     */
    public function save($type, $id, $content, array $options = array())
    {
        if ('l' == $type) {
            $element = new LSE_Book();
            $element->setDecorator($this->decorator);
            
            $element->setTitle( $options['title'] );
            $element->setAuthors( $options['authors'] );
            $element->setLang( $options['lang'] );
            $element->setComment( $options['comment'] );
            $element->setId( $id );
            
            $this->book = $element;
        }
        else {
            $element = new LSE_Element();
            $element->setDecorator($this->decorator);
            
            $element->setType( $type );
            $element->setId( $id );
            $element->setContent( $content );
            $element->setOptions( $options );
            
            $this->book->addElement( $element );
        }
    }
    
    public function render()
    {
        $output = $this->book->render();
        $graph = $this->book->buildGraph(array("l", "C"));
        $elementTable = $this->book->getElementTable(array("l", "C"));
        
        $epub = $this->getEpub();
        $epub->addChapter($this->book->getTitle(), 'Chapter1', $output, false, EPub::EXTERNAL_REF_ADD, 
            '',
            array('graph' => $graph, 'elementTable' => $elementTable)
        );
        $epub->finalize();
        $bookTitle = str_replace(' ', '_', strtolower($this->book->getTitle()));
        $epub->sendBook($bookTitle);
        return NULL;
    }
    
    public function getEpub()
    {
        require_once('LSE/Plugin.php');
        $book = new LSE_Plugin();
        
        $book->setTitle($this->book->getTitle());
        $book->setIdentifier("http://ilab.net.in.tum.de/", EPub::IDENTIFIER_URI); // Could also be the ISBN number, prefered for published books, or a UUID.
        $book->setLanguage("en"); // Not needed, but included for the example, Language is mandatory, but EPub defaults to "en". Use RFC3066 Language codes, such as "en", "da", "fr" etc.
        $book->setDescription("This is a brief description\nA test ePub book as an example of building a book in PHP");
        $book->setAuthor($this->book->getAuthors(), $this->book->getAuthors()); 
        $book->setPublisher("Technische Universität München", "http://ilab.net.in.tum.de/"); // I hope this is a non existant address :) 
        $book->setDate(time()); // Strictly not needed as the book date defaults to time().
        $book->setRights("Copyright and licence information specific for the book."); // As this is generated, this _could_ contain the name or licence information of the user who purchased the book, if needed. If this is used that way, the identifier must also be made unique for the book.
        $book->setSourceURL("http://ilab.net.in.tum.de-");
        
        $book->setDocRoot(LSE_PATH_LABSYSTEM);
        return $book;
    }
}