<?php
require_once('LSE/EPub.php');

class EPubTest extends PHPUnit_Framework_TestCase
{
    public function setUp()
    {
        
    }
    
    public function testConstructor()
    {
        $epub = new LSE_EPub();
        $book = $epub->getBook();
        $this->assertEquals('LSE_Book', get_class($book));
    }
    
    // Set options sets the value in the book object
    public function testSetOptions()
    {
        $expected = array(
            'key1' => 'value1',
            'key2' => 'value2',
            'key3' => 3,
            'key4' => NULL,
        );
        
        $epub = new LSE_EPub();
        $epub->setOptions($expected);
        
        $book = $epub->getBook();
        $this->assertEquals($expected, $book->getOptions());
    }
    
    public function testSave()
    {
        $epub = new LSE_EPub();
        
        $mockBook = $this->getMock('LSE_Book', array('addElement'));
        
        $element = new LSE_Element();
        $element->setDecorator($epub->getDecorator());
        $element->setType( 'typeTest' );
        $element->setId( 'idTest' );
        $element->setContent( 'contentTest' );
        $element->setOptions( array( 'key1test' => 'value1test', 'key2test' => 'value2test') );
        
        $mockBook->expects($this->once())->method('addElement')->with($element);
        
        $epub->setBook($mockBook);
        $epub->save('typeTest', 'idTest', 'contentTest', array( 'key1test' => 'value1test', 'key2test' => 'value2test'));
        
    }
}