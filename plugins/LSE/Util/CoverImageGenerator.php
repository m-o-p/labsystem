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
        $font         = '../plugins/LSE/Util/VIPER_NORA.ttf';

        // Load Logo for the area on the left
        $logoLeftFilename = '../syspix/labsyslogo_443x40.gif';
        $logoLeftImgSize  = getimagesize($logoLeftFilename);
        $logoLeftImg      = imagecreatefromstring(file_get_contents($logoLeftFilename));
        $logoLeftImg      = imagerotate( $logoLeftImg, 90, 0 );
        $logoLeftWidth    = $logoLeftImgSize[1];
        $logoLeftHeight   = $logoLeftImgSize[0];

        $fontSizeTitle = 68;
        $spaceLeft     = $logoLeftWidth;
        $fontSizeDate  = 26;
        $textSpacePadding = 10;

        $dateText      = date('r');
        $dateBB        = imageftbbox($fontSizeDate, 0, $font, $dateText);
        $spaceRight    = $dateBB[3]+$fontSizeDate+$textSpacePadding;
        $dateBBwidth   = $dateBB[4]-$dateBB[6];

        // Create the image
        $im = imagecreatetruecolor($imgWidth, $imgHeight);

        // Create some colors
        $lightGrey = imagecolorallocate($im, 245, 245, 245);
        $black = imagecolorallocate($im, 0, 0, 0);
        $labsysBlue = imagecolorallocate($im, 70, 130, 180);
        $labsysOrange = imagecolorallocate($im, 233, 150, 122);

        // Fill boxes
        imagefilledrectangle($im, $spaceLeft, 0, $imgWidth - 1, $imgHeight - 1, $lightGrey);
        imagefilledrectangle($im, 0, 0, $spaceLeft-1, $imgHeight - 1, $labsysBlue);
        imagefilledrectangle($im, $imgWidth-$spaceRight+1, 0, $imgWidth - 1, $imgHeight - 1, $labsysOrange);

        // The text to draw
        $text = $this->_text;
        $text = wordwrap($text, 9, "\n");
        $textbbox = imageftbbox($fontSizeTitle, 0, $font, $text);

        // Calculate maximum space for image
        $imageBBtop    = 2*$textSpacePadding+$textbbox[3]+$fontSizeTitle;
        $imageBBleft   = $spaceLeft+$textSpacePadding;
        $imageBBbottom = $imgHeight-$textSpacePadding;
        $imageBBright  = $imgWidth-$spaceRight;
        $imageBBwidth  = $imageBBright-$imageBBleft-$textSpacePadding;
        $imageBBheight = $imageBBbottom-$imageBBtop;

        // Load existing image
        $srcFilename = $this->_srcImagePath;
        $srcImgSize = getimagesize($srcFilename);
        $srcImg = imagecreatefromstring(file_get_contents($srcFilename));

        // Calculate the destination dimensions

        $desWidth  = $srcImgSize[0];
        $desHeight = $srcImgSize[1];
        if ( $desWidth > $imageBBwidth ){
          // image too wide -> resize height accordingly
          $desHeight = $imageBBwidth/$desWidth*$desHeight;
          // set width to maximum
          $desWidth  = $imageBBwidth;
        }

        if ( $desHeight > $imageBBheight ){
          // image too high -> resize width accordingly
          $desWidth  = $imageBBheight/$desHeight*$desWidth;
          // set height to maximum
          $desHeight = $imageBBheight;
        }

        // Place the title page image centered inside the BB under the text
        imagecopyresized($im, $srcImg, $imageBBleft+($imageBBwidth-$desWidth)/2, $imageBBtop+($imageBBheight-$desHeight)/2, 0, 0, $desWidth, $desHeight, $srcImgSize[0], $srcImgSize[1]);
        imagedestroy( $srcImg );

        // place the labsys logo on top left
        imagecopyresized($im, $logoLeftImg, 0.5*($spaceLeft-$logoLeftWidth), 0, 0, 0, $logoLeftWidth, $logoLeftHeight, $logoLeftWidth, $logoLeftHeight);
        imagedestroy( $logoLeftImg );

        // Add the title text
        imagettftext($im, $fontSizeTitle, 0, $spaceLeft+$textSpacePadding, $fontSizeTitle+$textSpacePadding, $black, $font, $text);
        // Add the date on the right
        imagettftext($im, $fontSizeDate, -90, $imgWidth-30, ($imgHeight-$dateBBwidth)/2, $lightGrey, $font, $dateText);

        // Using imagepng() results in clearer text compared with imagejpeg()
        imagepng($im, $this->_dstImagePath);
        imagedestroy($im);
    }
}