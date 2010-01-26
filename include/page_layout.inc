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

// For utf-8 support:
//  header('Content-type: text/html; charset=utf-8');
/**
* This is the systems page layout.
* By editing this file you change the entire look.
*
* The current look is:
*
  ------------------------------------
  |   labsys_mop_topmostHeader       |
  |----------------------------------|
  |   labsys_mop_topHeader           |
  |----------------------------------|
  |   labsys_mop_topSysinfo          |
  |----------------------------------|
  |      |                           |
  |      |                           |
  |      |                           |
  |      |                           |
  | menu |    labsys_mop_content     |
  |      |                           |
  |      |                           |
  |      |                           |
  |      |                           |
  |      |                           |
  |----------------------------------|
  |   labsys_mop_bottomFooter        |
  ------------------------------------
*
* @module     ../include/page_layout.inc
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*/

// alert/ info data may come via url or via set $SYS{INFO, ALERT}
if ( $url->available('sysinfo') ) if ( isset($SYSINFO) ) $SYSINFO .= htmlentities( html_entity_decode( $url->get('sysinfo') ) ); 
                                                 else $SYSINFO  = htmlentities( html_entity_decode( $url->get('sysinfo') ) );
if ( $url->available('sysalert') ) if ( isset($SYSALERT) ) $SYSALERT .= htmlentities( html_entity_decode( $url->get('sysalert') ) ); 
                                                   else $SYSALERT  = htmlentities( html_entity_decode( $url->get('sysalert') ) );
                                                   
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<HTML>
  <HEAD>
    <meta http-equiv="Content-Language" content="<?php $lng->say("Content-Language"); ?>">
    <meta name="generator" content="labsystem.m-o-p.de">
    <link rel="stylesheet" type="text/css" href="<?php $cfg->say("SystemStyleSheet"); ?>">
    <?PHP if ( $cfg->doesExist('SysOverridingSheet') ) 
            /* with this sheet you can easily make adoptions without rewriting the whole 
               stylesheet and without putting it into the user's stylesheet... */
            echo('<link rel="stylesheet" type="text/css" href="'.$cfg->get('SysOverridingSheet').'">'."\n"); 
            /* For browsers with the ability to switch between stylesheets you could use:
               <LINK type="text/css" title="green" rel="alternate stylesheet" href="../css/sys/labsys_mop_green_theme.css">
             */
     ?>
     
    <link rel="stylesheet" type="text/css" href="<?php $cfg->say("UserStyleSheet"); ?>">

    <link rel="stylesheet" type="text/css" href="<?php $cfg->say("PrintStyleSheet"); ?>" media="print">

    <link rel="shortcut icon" href="<?php $cfg->say("Favicon"); ?>">
    
    <script src="../pages/scripts.js" type="text/javascript" language="javascript"></script>

<?PHP /* TinyMCE (WYSIWYG-Editor) integration */
    if ($url->available('tinyMCE')) 
      echo('
    <script language="javascript" type="text/javascript" src="../tiny_mce/tiny_mce.js"></script>
    <script language="javascript" type="text/javascript">

      function myCustomInitInstance(inst) {
              alert("HTML-code will be bad if you save!");
      }

      // Called when content is loaded from textarea
      // Remove beginning [HTML]-tag of labsystem fields
      function myCustomSetupContent(editor_id, body, doc) {
             if ( body.innerHTML.substring(0, 6) == "[HTML]" )
                      body.innerHTML = body.innerHTML.substring(6);
      }

     // Custom save callback, gets called when the contents is to be submitted
     // Add beginning [HTML]-tag to labsystem fields
     function myCustomSave(id, content)
     {
         return "[HTML]\n"+content;
     }


      tinyMCE.init({
        // only fields called "CONTENTS" should get TinyMCEed
        mode : "exact",
        elements : "CONTENTS, ANSWER",
        // Should the user be asked if TinyMCE should get started? (onClick)
        ask : false,
        init_instance_callback : "myCustomInitInstance",
        setupcontent_callback : "myCustomSetupContent",
        save_callback : "myCustomSave",
        // Labsystems stylesheet to format the content correctly
        content_css : "'.$cfg->get("UserStyleSheet").'"
      });
    </script>
    ');
?>
    
    <TITLE><?php echo( $pge->title ); ?> [<?php echo( $usr->userName."@" ); $cfg->say("SystemTitle"); ?>]</TITLE>
  </HEAD>
  
  <BODY>
    <table class="labsys_mop_maintable" align="center" cellpadding="0" cellspacing="0">
      <?php /* table layout fixed: first row sets the width */
        echo("<tr><td class=\"labsys_mop_ableMenu\"></td>". // show/ hide menu
             "<td class=\"labsys_mop_menu".retIfTrue( isset( $_GET['nomenu'] ), "_hidden" )."\"></td>". // the menu
             "<td></td></tr>\n"); // the content area
      ?>
<!-- ************************* start of topmost header ************************* -->
      <?php if ( $cfg->get('TopmostHeader') != '' )
        echo('<tr><td colspan="3" class="labsys_mop_topmostHeader">'.$cfg->get('TopmostHeader').'</td></tr>'); ?>
<!-- ************************* stop of topmost header ************************* -->

<!-- ************************* start of top header ************************* -->
      <?php if ( $cfg->get('TopHeader') != '' )
        echo('<tr><td colspan="3" class="labsys_mop_topHeader">'.$cfg->get('TopHeader').'</td></tr>' ); ?>
<!-- ************************* stop of topnmost header ************************* -->

<!-- ************************* start of top sysinfo ************************* -->
      <tr><td colspan="3" class="labsys_mop_topSysinfo">
        <?php 
          if ( isset($SYSINFO) )  echo("<span class=\"labsys_mop_systeminfo\">".$SYSINFO."</span> ");
          if ( isset($SYSALERT) ) echo("<span class=\"labsys_mop_systemalert\">".$SYSALERT."</span> ");
          if ( $usr->isSeeingSomeonesData() ) 
            echo( '<span class="labsys_mop_seeingSomeone">'.
                  '<a href="'.$url->link2( '../php/executeOnLab.php?address=l1&function=setSeeingUID&param=%22%22&redirectTo='.urlencode( $_SERVER['REQUEST_URI'] ) ).'">'.
                  '['.$lng->get("dataOf").' '.$usr->theSeeingDescr().' '.
                  '<img src="../syspix/button_stop_9x9.gif" width="9" height="9" style="border: none;" alt="'.$lng->get( 'stopSeeingData' ).'">'.
                  '</a>'.
                  '<a href="'.$url->rawLink2( $_SERVER['REQUEST_URI'].'&hideAnswers=true' ).'">'.
                  '<img src="../syspix/button_paused_9x9.gif" width="9" height="9" style="border: none;" alt="'.$lng->get( 'stopSeeingData' ).'">'.
                  '</a>'.
                  ' ]'.
                  '</span> ' );
          if ( $url->available('hideAnswers') ) 
            echo( '<span class="labsys_mop_seeingSomeone">'.
                  '['.$lng->get("dataOf").' '.
                  '<img src="../syspix/button_paused_9x9.gif" width="9" height="9" style="border: none;" alt="'.$lng->get( 'stopSeeingData' ).'">'.
                  ' ]'.
                  '</span> ' );
          echo(date( $lng->get("DateFormat") )); 
        ?>
      </td></tr>
<!-- ************************* stop of top sysinfo ************************* -->

<!-- ************************* start of menu ************************* -->
      <tr><?php /* hide/ show menu table row */
          $menuVisible = !isset( $_GET['nomenu'] );
          $url2 = new Url(); $url2->put( $url->get("oldQueryString") );
          if ( $menuVisible ) $url2->put( "nomenu=true" ); else $url2->rem( "nomenu=true" );
         
          $link = $url2->link2( $_SERVER['PHP_SELF'] ); /* the fragment (p.e. "#top7") of the uri will be lost since it is not available to php */
          echo("<td class=\"labsys_mop_ableMenu\" ".
                    "onmouseover='javascript:this.className=\"labsys_mop_ableMenuhover\";' ".
                    "onmouseout='javascript:this.className=\"labsys_mop_ableMenu\";' ".
                    "onClick='javascript:location.href=\"".$link."\";' >".
               "<a href=\"".$link."\">".
               retIfTrue( $menuVisible, "&lt;<br>&lt;", "&gt;<br>&gt;" ).
               "</a></td>");
        ?>
        <td class="labsys_mop_menu<?php if (!$menuVisible) echo("_hidden"); ?>">
        <?php if ($menuVisible) require( INCLUDE_DIR."/menu.inc"); ?>
        </td>
<!-- ************************* stop of menu ************************* -->
        
<!-- ************************* start of content ************************* --><td class="labsys_mop_content">


<?php $pge->show(); ?>


<!-- ************************* stop of content ************************* --></td>
      </tr>
      <tr><td colspan="3" class="labsys_mop_bottomFooter">
        <?php 
          $cfg->say("BottomFooter"); 
          /**
            *   Please keep the link to the project page here to enable other people to find and use the system...
            *   Thank you!
            */
          echo(' &copy; <a href="http://labsystem.m-o-p.de" target="_blank"><img src="../syspix/labsystem_76x7.gif" width="76" height="7" style="border: 0;" alt="labsystem" /></a> <a href="http://www.pahl.de?site=__contact&subject=labsystem@'.
               $_SERVER['SERVER_NAME'].'" target="_blank">mop</a> 2005-2010'); // Please do not remove this acknowledgement to me thanks!
        ?>
      </td></tr>
    </table>
  </BODY>
  <?php
    if ( isset($SYSALERT) ) {
      echo("<script language=\"JavaScript\">\n<!-- \nalert( '".addslashes( $SYSALERT )."' ); \n//-->\n</script>");
    }
  ?>
</HTML>