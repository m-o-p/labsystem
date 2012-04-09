<?php
class LSE_Util_CoverImageGenerator
{
    protected $_srcImagePath;
    protected $_dstImagePath;
    protected $_text;
    
    /**
     * @param string $_srcImagePath
     */
    public function setSrcImagePath($_srcImagePath)
    {
        $this->_srcImagePath = $_srcImagePath;
    }

	/**
     * @param string $_dstImagePath
     */
    public function setDstImagePath($_dstImagePath)
    {
        $this->_dstImagePath = $_dstImagePath;
    }

	/**
     * @param string $_text
     */
    public function setText($_text)
    {
        $this->_text = $_text;
    }

    /**
     * Creates a new temporary image from given source image and text.
     * 
     * The default size of the image is 600(w)x800(h) which can be changed.
     * The font is arial.ttf embedded inside this folder.
     */
    public function generate()
    {
        $imgWidth     = 590;
        $imgHeight    = 750;
        
        $fontSizeLeft = 68;
        $spaceLeft    = $fontSizeLeft+50;
        $fontSizeRight= 26;
        $spaceRight   = $fontSizeRight+20;
        
        
        $font         = '../plugins/LSE/Util/VIPER_NORA.ttf';        
        
        // Create the image
        $im = imagecreatetruecolor($imgWidth, $imgHeight);
        
        // Create some colors
        $white = imagecolorallocate($im, 255, 255, 255);
        $grey = imagecolorallocate($im, 128, 128, 128);
        $lightGrey = imagecolorallocate($im, 200, 200, 200);
        $black = imagecolorallocate($im, 0, 0, 0);
        imagefilledrectangle($im, $spaceLeft, 0, $imgWidth - 1, $imgHeight - 1, $lightGrey);
        
        imagefilledrectangle($im, 0, 0, $spaceLeft-1, $imgHeight - 1, $black);
        
        // Load existing image
        $srcFilename = $this->_srcImagePath;
        $srcImgSize = getimagesize($srcFilename);
        //echo( $srcFilename );
        $srcImg = imagecreatefromstring(file_get_contents($srcFilename));
        
        // Put the image on the cover
        // It should be 90% width:
        $desWidth = $imgWidth-$spaceLeft-$spaceRight;
        $desHeight = $imgHeight; //$desWidth/$srcImgSize[0]*$srcImgSize[1];
        $desSpaceTop = 0;
        $desSpaceLeft = $spaceLeft;
        //imagecopyresized($im, $srcImg, 0, 0, 0, 0, $imgWidth, $imgHeight, $srcImgSize[0], $srcImgSize[1]);
        imagecopyresized($im, $srcImg, $desSpaceLeft, $desSpaceTop, 0, 0, $desWidth, $desHeight, $srcImgSize[0], $srcImgSize[1]);
        
        // The text to draw
        $text = $this->_text;
        $text = str_replace(" ", "\n", $text);
        // Replace path by your own font path
        
        
        // Add some shadow to the text
        //imagettftext($im, $fontSize, 90, 84, 734, $grey, $font, $text);
        
        // Add the text
        imagettftext($im, $fontSizeLeft, 90, 85, 730, $white, $font, $text);
        
        imagettftext($im, $fontSizeRight, -90, $imgWidth-30, $desSpaceTop, $black, $font, date( 'r' ));
        
        // Using imagepng() results in clearer text compared with imagejpeg()
        imagepng($im, $this->_dstImagePath);
        imagedestroy($im);
    }
}