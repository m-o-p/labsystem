<?php
require_once('LSE/Book.php');

class BookTest extends PHPUnit_Framework_TestCase
{
    public function testBuildGraph()
    {
        $element = new LSE_Element();
        $element->setId( 'a1' );
        
        $element1 = new LSE_Element();
        $element1->setId( 'a1.b1' );
        
        $element2 = new LSE_Element();
        $element2->setId( 'a1.b1.c1' );

        $element3 = new LSE_Element();
        $element3->setId( 'a2' );
        
        $element4 = new LSE_Element();
        $element4->setId( 'a3.b3.c3');
        
        $book = new LSE_Book();
        $book->addElement($element);
        $book->addElement($element1);
        $book->addElement($element2);
        $book->addElement($element3);
        $book->addElement($element4);
        
        $expectedGraph = array(
            'a1' => array(
                'a1.b1' => array(
                    'a1.b1.c1' => array(),
                ),
            ),
            'a2' => array(),
            'a3' => array(
                'a3.b3' => array(
                    'a3.b3.c3' => array(),
                ),
            ),
        );
        
        $this->assertEquals($expectedGraph, $book->buildGraph());
        
        // @todo this fails see comments 
//        $expectedFilteredGraph = array(
//            'a1' => array(
//                'a1.b1' => array(), // passes
//            ),
//            'a2' => array(), // because a2 does not have 'a' as parent
//            'a3' => array(
//                'a3.b3' => array(), // because a2 has a3.b3.c3 which does not have a as parent
//            ),
//        );
//        $this->assertEquals($expectedFilteredGraph, $book->buildGraph('a'));
    }
    
    public function testGetElementTable()
    {
        $element = new LSE_Element();
        $element->setId( 'a1' );
        
        $element1 = new LSE_Element();
        $element1->setId( 'a1.b1' );
        
        $element2 = new LSE_Element();
        $element2->setId( 'a1.b1.c1' );

        $element3 = new LSE_Element();
        $element3->setId( 'a2' );
        
        $element4 = new LSE_Element();
        $element4->setId( 'a3.b3.c3');
        $element4->setOptions(array('title' => 'Title Test'));
        
        $book = new LSE_Book();
        $book->addElement($element);
        $book->addElement($element1);
        $book->addElement($element2);
        $book->addElement($element3);
        $book->addElement($element4);
        
        $expected = array(
            'a1'       => array( 'a1', '' ),
            'a1.b1'    => array( 'a1.b1', '' ),
            'a1.b1.c1' => array( 'a1.b1.c1', '' ),
            'a2'       => array( 'a2', '' ),
            'a3.b3.c3' => array( 'a3.b3.c3', 'Title Test' ),
        );
        
        $this->assertEquals($expected, $book->getElementTable());
        
        // Check Parent Filter
        // @todo
        
        // Check Type Filter
        $expected2 = array(
            'a1'       => array( 'a1', '' ),
            'a1.b1'    => array( 'a1.b1', '' ),
            'a2'       => array( 'a2', '' ),
        );
        $this->assertEquals($expected2, $book->getElementTable(NULL, array('a', 'b')));
    }
    
    public function testSave()
    {
        
    }
}