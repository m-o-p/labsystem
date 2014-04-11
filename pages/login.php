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
* Login page.
*
* @module     ../pages/login.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/
require( "../include/init.inc" );

$url->rem( "inside=true" ); // This page is for checking in so you are supposed not to be in!

$pge->title       = $lng->get("TitleLogInPage");
$pge->matchingMenu= $lng->get( "MnuEntryLogIn" );
$pge->put("<div class=\"labsys_mop_h2\">".$pge->title."</div>\n".
          $lng->get("TxtLogInPage")."\n".
          "<FORM NAME=\"login\" METHOD=\"POST\" ACTION=\"".$url->link2("../php/authenticate.php")."\">");

if ( $GLOBALS['url']->available('redirectTo') )
  // Cause authenticate.php to redirect to that page instead of the default one from the config.
  // Happens if you have a link to a special page but are not logged in. So after log on you get directed there...
  $pge->put('<input type="hidden" name="REDIRECTTO" value="'.trim(preg_replace('/\s+/', '', $url->get('redirectTo'))).'">');

$pge->put('
  <table class="labsys_mop_loginTable" align="center">

  <tr>
  <td class="labsys_mop_keyCell" width="50%">'.$lng->get("userName").'</td>
  <td class="labsys_mop_inputFieldCell" width="50%"><input TABINDEX="1" type="text" name="USERNAME" id="USERNAME" class="labsys_mop_input_fullwidth" value=""></td>
  </tr>

  <tr>
  <td class="labsys_mop_keyCell">'.$lng->get("passWord").'</td>
  <td class="labsys_mop_inputFieldCell"><input TABINDEX="2" type="password" name="PASSWORD" maxlength="255" class="labsys_mop_input_fullwidth" value=""></td>
  </tr>

  <tr>
  <td class="labsys_mop_buttonCell" colspan="2">
  <input TABINDEX="3" type="submit" name="login" class="labsys_mop_button" value="'.$lng->get("MnuEntryLogIn").'">
  </td>
  </tr>

  </table>

  </FORM>
  <p class="labsys_mop_note">'.$lng->get("NoteLogInPage").'</p>');

$pge->put( '<div class="labsys_mop_note">'."\r\n".
           '<div style="text-align: right;">'."\r\n".
           '<a href="http://labsystem.m-o-p.de" target="_blank"><img src="../syspix/labsystem_76x7.gif" width="76" height="7" style="border: 0;" alt="labsystem" /></a>'."\r\n".
           '</div>'."\r\n".
           '</div>'."\r\n" );

$pge->put('
<script language="JavaScript" type="text/javascript">
<!--
if (document.login) document.login.USERNAME.focus();
//-->
</script>
');

require( $cfg->get("SystemPageLayoutFile") );
?>
