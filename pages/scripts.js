/**
<!--
    labsystem.m-o-p.de - 
                    the web based eLearning tool for practical exercises
    Copyright (C) 2010  Marc-Oliver Pahl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
-->
*/
/**
 * The javascript function used to confirm events (e.g. deleting elements).
 */

/**
 * The function is part of the functions.js that phpMyAdmin contains
 *
 * This function is called to ensure that an action (e.g. deleting elements) is really wanted.
 *
 * @param   object   the link
 * @param   object   the confirm box message
 *
 * @return  boolean  whether to run the query or not
 */
function confirmLink(theLink, theMsg)
{
    // Confirmation is not required in the configuration file
    // or browser is Opera (crappy js implementation)
    if (theMsg == '' || typeof(window.opera) != 'undefined') {
        return true;
    }

    var is_confirmed = confirm( theMsg );
    if (is_confirmed) {
        theLink.href += '&isConfirmed=1';
    }

    return is_confirmed;
} // end of the 'confirmLink()' function

/**
 * This variable is used for the dirty bit functionality.
 * The dirty bit is set by the inputs when they are changed.
 * It is cleared by the save vuttons.
 * If not cleared it shows a warning.
 */
var isDirty = false;

function dirtyWarning(){
  if (isDirty) return confirm(discardChangesWarning);
  return retVal;
}

/**
 * Zooms and unzooms a thumbnail.
 * Used in LiElement.inc.
 **/
if (typeof String.prototype.endsWith !== 'function') {
    String.prototype.endsWith = function(suffix) {
        return this.indexOf(suffix, this.length - suffix.length) !== -1;
    };
}

function zoomPreviewImage(imageTag){
	var mySuffix=".thumb.jpg";
	var regEx = /(filename=)([^\&]+)/;
	var imgFileName = imageTag.src.match(regEx)[2];
	if (imgFileName.endsWith(mySuffix)){
		// We see the thumbnail.
		imageTag.src=imageTag.src.replace( regEx, '$1' + imgFileName.substring(0,imgFileName.length-mySuffix.length) );
		imageTag.style.display="block";
	}else{
		// We see the big image.
		imageTag.src=imageTag.src.replace( regEx, '$1' + imgFileName+mySuffix );
		imageTag.style.display="inline";
	}
}

/**
 * Shows the example solution of this input for the given team.
 */
function showExampleSolutionHere(iIdx,teamNr){
	exampleSolutionElement=document.getElementById("i"+iIdx+"_exampleSolution");
	targetElement=document.getElementById("i"+iIdx+"_exampleSolution_"+teamNr);
	if (targetElement.className==exampleSolutionElement.className){
		// close
		targetElement.innerHTML = "";
		targetElement.className=exampleSolutionElement.className+"_hidden";
	}else{
		//open
		targetElement.innerHTML = exampleSolutionElement.innerHTML;
		targetElement.className=exampleSolutionElement.className;
	}
}