<?php
/**
 *  labsystem.m-o-p.de - 
 *                  the web based eLearning tool for practical exercises
 *  Copyright (C) 2010  Marc-Oliver Pahl
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
* This page gets called to confirm an action (p.e. delete).
* Normally that is done via script, but if you have disabled javascript this page is taken.
*
* @module     ../pages/confirm.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $_GET['text']        The text to display.
* @param $_GET['redirectTo']  The link to follow if acknowledged.
*/
require( "../include/init.inc" );

if ( !$url->available('text') || !$url->available('redirectTo') ){
                                                              trigger_error( "Not all necessary values posted!", E_USER_ERROR );
                                                              exit;
                                                            }

$pge->title   = $url->get('text');
$pge->put( stripslashes( $url->get('text') )."<br /><br />" );
$url->rem( 'inside' ); // otherwhise double (but no problem)
$pge->put(" <a href=\"".htmlentities($url->redirectToLocationUrl( $url->get( 'redirectTo' ), Array('isConfirmed' => 1) ) )."\">".$lng->get("yesIconfirm")."</a>" );

require( $cfg->get("SystemPageLayoutFile") );
?>
