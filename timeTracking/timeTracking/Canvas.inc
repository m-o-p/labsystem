<?php

/**
 * This class generates an HTML5 canvas script and allows to put graphic elements on it.
 *
 * @author Fabian Oehlmann
 *
 */
class Canvas {
  private $script = '';

  /**
   * Puts the script together and returns the final script. Takes as arguments the size of the
   * resulting canvas, which should be big enough to see all the contents, which are drawn.
   *
   * @param unknown $width  Width of the canvas
   * @param unknown $height Height of the canvas
   * @return string         The resulting script
   */
  function finish($width, $height) {
    $canvas = '<canvas id="ttrack" width="' . $width . '" height="' . $height .
    '" style="border:1px solid #000000; background-color:#FFFFFF">' . "\n";
    $canvas .= 'Your browser does not support the HTML5 canvas tag.' . "\n";
    $canvas .= '</canvas>' . "\n";

    $canvas .= '<a class="bu" id="ttrackimglink">download</a>';

    $canvas .= '<script>' . "\n";
    $canvas .= 'var c = document.getElementById("ttrack");' . "\n";
    $canvas .= 'var ctx = c.getContext("2d");' . "\n";
    $canvas .= $this->script;
    $canvas .= '
            var dataUrl = c.toDataURL();
            document.getElementById("ttrackimglink").href = dataUrl;
        </script>' . "\n";
    return $canvas;
  }


  /**
   * Draws a circle.
   *
   * @param unknown $x          The x-coordinate of the center
   * @param unknown $y          The y-coordinate of the center
   * @param unknown $radius     Radius of the circle
   * @param string $fillstroke  (optional) Whether to 'fill' the circle or just 'stroke' the outline
   * @param string $color       (optional) The color of the circle
   */
  function circle($x, $y, $radius, $fillstroke = 'stroke', $color = '#000000') {
    $this->script .= '
			ctx.save();
			ctx.beginPath();
			ctx.strokeStyle="' . $color . '";
			ctx.fillStyle="' . $color . '";
			ctx.arc(' . $x . ',' . $y . ',' . $radius . ',0,2*Math.PI);
			ctx.' . $fillstroke . '();
			ctx.restore();
	';
  }

  /**
   * Writes a text to a position.
   *
   * @param string $label     What to write
   * @param unknown $x        The x-coordinate
   * @param unknown $y        The y-coordinate
   * @param unknown $rotation How much the text should be rotated
   */
  function label($label, $x, $y, $rotation) {
    $this->script .= '
			ctx.save();
			ctx.translate(' . $x . ', ' . $y . ');
			ctx.rotate(' . $rotation . ');
			ctx.fillText("' . $label . '", 0, 0);
			ctx.restore();
	';
  }

  /**
   * Draws a rectangle.
   *
   * @param unknown $x1 The x-coordinate of the first corner
   * @param unknown $y1 The y-coordinate of the first corner
   * @param unknown $x2 The x-coordinate of the opposing corner
   * @param unknown $y2 The y-coordinate of the opposing corner
   */
  function rectangle($x1, $y1, $x2, $y2) {
    $this->script .= '
		ctx.save();
		ctx.fillStyle="#FFFFFF";
		ctx.fillRect(' . $x1 . ',' . $y1 . ',' . $x2 . ',' .
		$y2 . ');
		ctx.strokeStyle="#FFFFFF";
		ctx.strokeRect(' . $x1 . ',' . $y1 . ',' . $x2 .
		',' . $y2 . ');
		ctx.restore();
	';
  }

  /**
   * Colors the surface between a line and a straight line of the same height.
   *
   * @param unknown $x1   The x-coordinate of the beginning of the line
   * @param unknown $y1   The y-coordinate of the beginning of the line
   * @param unknown $x2   The x-coordinate of the ending of the line
   * @param unknown $y2   The y-coordinate of the ending of the line
   * @param unknown $x3   The x coordinate of the straight line, i.e. until where the surface should be colored
   * @param string $color What color to use
   */
  function shadow($x1, $y1, $x2, $y2, $x3, $color = '#d5d5d5') {
    $this->script .= '
        ctx.save();
        ctx.beginPath();
        ctx.moveTo(' . $x1 . ',' . $y1 . ');
        ctx.lineTo(' . $x2 . ',' . $y2 . ');
        ctx.lineTo(' . $x3 . ',' . $y2 . ');
        ctx.lineTo(' . $x3 . ',' . $y1 . ');
        ctx.closePath();
        ctx.fillStyle="' . $color . '";
        ctx.fill();
        ctx.restore();
    ';
  }

  /**
   * Writes a textbox for the legend of the graph. The size of the textbox is adjusted automatically
   * to the number and size of the strings it will contain. Changing the font in the graph might
   * break this, since HTML5 canvases only provide a function for measuring the width of a text, but
   * not for measuring the height.
   *
   * @param unknown $strings  An array containing all the strings, which should be in the textbox
   * @param unknown $x2       The x-coordinate of the top-right corner
   * @param unknown $y1       The y-coordinate of the top-right corner
   */
  function legend($strings, $x2, $y1) {
    if (count($strings) == 0) {
      return;
    }
    $this->script .= '
		ctx.save();
		var strings = ["' . implode('", "', $strings) . '"];
		var widths = [];
		for (var i=0;i<strings.length;i++) {
			widths.push(ctx.measureText(strings[i]).width);
		}
		var maxTxtLength=Math.max.apply(null, widths);
		ctx.strokeStyle="#000000";
		ctx.fillStyle="#FFFFFF";
		var x2=' . $x2 . ';
		var y1=' . $y1 . ';
		var boxwidth = maxTxtLength+6;
		var boxheight = (i*13)+4;
		ctx.fillRect(x2-boxwidth,y1,boxwidth,boxheight);
		ctx.strokeRect(x2-boxwidth,y1,boxwidth,boxheight);
		ctx.fillStyle="#000000";
		for (i=0;i<strings.length;i++) {
			ctx.fillText(strings[i], x2-boxwidth+3, y1+((i+1)*13)-1);
		}
		ctx.restore();
	';
  }

  /**
   * Puts a title of the graph on top of the image.
   *
   * @param unknown $title  The title
   */
  function title($title) {
    $this->script .= '
		ctx.save();
		ctx.font="24px Arial";
		var txt="' . $title . '";
		var txtwidth=ctx.measureText(txt).width;
		ctx.translate((c.width/2)-(txtwidth/2), 27);
		ctx.fillText(txt, 0, 0);
		ctx.restore();
	';
  }

  /**
   * Draws a dashed line.
   *
   * @param unknown $x1 The x-coordinate of the beginning of the line
   * @param unknown $y1 The y-coordinate of the beginning of the line
   * @param unknown $x2 The x-coordinate of the ending of the line
   * @param unknown $y2 The y-coordinate of the ending of the line
   */
  function lineDashed($x1, $y1, $x2, $y2) {
    $this->script .= '
		ctx.beginPath();
		ctx.lineWidth=0.5;
		ctx.setLineDash([5,5]);
		ctx.moveTo(' . $x1 . ',' . $y1 . ');
		ctx.lineTo(' . $x2 . ',' . $y2 . ');
		ctx.stroke();
		ctx.setLineDash([1,0]);
		ctx.closePath();
	';
  }

  /**
   * Draws a line
   *
   * @param unknown $x1   The x-coordinate of the beginning of the line
   * @param unknown $y1   The y-coordinate of the beginning of the line
   * @param unknown $x2   The x-coordinate of the ending of the line
   * @param unknown $y2   The y-coordinate of the ending of the line
   * @param string $width The width of the line
   * @param string $color The color of the line
   */
  function line($x1, $y1, $x2, $y2, $width = '1', $color = '#000000') {
    $this->script .= '
    	ctx.save();
    	ctx.beginPath();
    	ctx.lineWidth=' . $width . ';
    	ctx.strokeStyle="' . $color . '";
    	ctx.moveTo(' . $x1 . ',' . $y1 . ');
    	ctx.lineTo(' . $x2 . ',' . $y2 . ');
    	ctx.stroke();
    	ctx.closePath();
    	ctx.restore();
	';
  }
}

?>
