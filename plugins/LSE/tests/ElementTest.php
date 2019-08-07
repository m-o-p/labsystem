<?php
require_once('LSE/Element.php');

class ElementTest extends PHPUnit_Framework_TestCase
{
    public function testAddElement()
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
        
        $mainElement = new LSE_Element();
        $mainElement->addElement($element);
        $mainElement->addElement($element1);
        $mainElement->addElement($element2);
        $mainElement->addElement($element3);
        $mainElement->addElement($element4);
        
        $expected = array(
            'a1'       => $element,
            'a1.b1'    => $element1,
            'a1.b1.c1' => $element2,
            'a2'       => $element3,
            'a3.b3.c3' => $element4,
        );
        $this->assertEquals($expected, $mainElement->getElements());
    }
}