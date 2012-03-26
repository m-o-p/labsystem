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
        $imgWidth  = 590;
        $imgHeight = 750;
        $fontSize  = 68;
        $font      = '../plugins/LSE/Util/arial.ttf';
        
        // Create the image
        $im = imagecreatetruecolor($imgWidth, $imgHeight);
        
        // Create some colors
        $white = imagecolorallocate($im, 255, 255, 255);
        $grey = imagecolorallocate($im, 128, 128, 128);
        $black = imagecolorallocate($im, 0, 0, 0);
        imagefilledrectangle($im, 0, 0, $imgWidth - 1, $imgHeight - 1, $white);
        
        // Load existing image
        $srcFilename = $this->_srcImagePath;
        $srcImgSize = getimagesize($srcFilename);
        //echo( $srcFilename );
        $srcImg = imagecreatefromstring(file_get_contents($srcFilename));
        imagecopyresized($im, $srcImg, 0, 0, 0, 0, $imgWidth, $imgHeight, $srcImgSize[0], $srcImgSize[1]);
        
        // The text to draw
        $text = $this->_text;
        $text = str_replace(" ", "\n", $text);
        // Replace path by your own font path
        
        
        // Add some shadow to the text
        imagettftext($im, $fontSize, 0, 14, 84, $white, $font, $text);
        
        // Add the text
        imagettftext($im, $fontSize, 0, 10, 80, $black, $font, $text);
        
        // Using imagepng() results in clearer text compared with imagejpeg()
        imagepng($im, $this->_dstImagePath);
        imagedestroy($im);
    }
}