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
* View the $firstFinal element from the database.
*
* @module     ../pages/view.php
* @author     Marc-Oliver Pahl
* @copyright  Marc-Oliver Pahl 2005
* @version    1.0
*
* @param $_GET['address'] Address of element to be shown.
*/
$debug=FALSE;
#
# Configuration: Enter the url and key. That is it.
#  url => URL to api/task/cron e.g http://yourdomain.com/support/api/tasks/cron
#  key => API's Key (see admin panel on how to generate a key)
#

$config = array(
	'url'=>'http://ilabxp.net.in.tum.de/api/http.php/tickets.json',
	'key'=>'B54467A9901CD2AECA9BB39D32712B39' //<api key>
);

$ticketCategories = array(
		"Help! with a lab",
		"Feedback/ Suggestion",
		"Inquiry about a correction"
);

require( "../include/init.inc" );
require( "../php/getFirstLastFinal.inc" ); 

// fetch first final for context
$id = $firstFinal{0}; $num = substr( $firstFinal, 1);
require( "../php/getDBIbyID.inc" ); /* -> $DBI */
if ( !$element = $DBI->getData2idx( $num ) ){
	header("HTTP/1.0 404 Not Found");
	trigger_error( $lng->get(strtolower( $id )."Number").$num." ".$lng->get("doesNotExist"), E_USER_ERROR );
	die();
}

// fetch last final
$id = $lastFinal{0}; $num = substr( $lastFinal, 1);
require( "../php/getDBIbyID.inc" ); /* -> $DBI */
if ( !$lastElement = $DBI->getData2idx( $num, ($firstFinal != $lastFinal), (strtolower($firstFinal{0})=='l') ) ){
	header("HTTP/1.0 404 Not Found");
	trigger_error( $lng->get(strtolower( $id )."Number").$num." ".$lng->get("doesNotExist"), E_USER_ERROR );
	die();
}

$pge->title        = 'Feedback form for "'.$lastFinal.': '.$lastElement->title.'"';
$pge->matchingMenu = $lastElement->getMatchingMenu();
$pge->visibleFor   = IS_USER;

$ticket_id = 0;

if (isset($_POST['topicId']) && $_POST['topicId'] == 'notSelected'){
	$GLOBALS['SYSALERT'] .= $lng->get('didNotSelectCategory');
} else if (isset($_POST['message'])){
// 	foreach($_POST as $key=>$value){
// 		$pge->put('<div>'.$key.':='.$value.'</div>'.PHP_EOL);
// 	}

	# Fill in the data for the new ticket, this will likely come from $_POST.
	
	$data = array(
// 	'name'      =>      'greezybacon',
// 	'email'     =>      'mailbox@host.com',
// 	'subject'   =>      'Test API message',
// 	'message'   =>      'This is a test of the osTicket API',
	'ip'        =>      $_SERVER['REMOTE_ADDR']
// 	'attachments' => array(),
	);
	$data = array_merge($_POST, $data);
	$data['link2Element'] = '<a href="'.$data['link2Element'].'">Open the element</a>';
	
	#pre-checks
	if (!$debug&&!($cfg->doesExist('ticketReceiverUID')&&!empty($cfg->get('ticketReceiverUID')))){
 		function_exists('curl_version') or die('CURL support required');
	}
	function_exists('json_encode') or die('JSON support required');
	
// 	$data['attachments'][] =
// 	array('filename.pdf' =>
// 			'data:application/pdf;base64,' .
// 			base64_encode(file_get_contents('/path/to/filename.pdf')));
	
	if (!$debug&&!($cfg->doesExist('ticketReceiverUID')&&!empty($cfg->get('ticketReceiverUID')))){
		// normal ticket system handler
		$data['topicId'] = $data['topicId']+1; // osTicket categories start with 1...
		#set timeout
		set_time_limit(30);
		
		#curl post
		$ch = curl_init();
		curl_setopt($ch, CURLOPT_URL, $config['url']);
		curl_setopt($ch, CURLOPT_POST, 1);
		curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($data));
		curl_setopt($ch, CURLOPT_USERAGENT, 'osTicket API Client v1.7');
		curl_setopt($ch, CURLOPT_HEADER, FALSE);
		curl_setopt($ch, CURLOPT_HTTPHEADER, array( 'Expect:', 'X-API-Key: '.$config['key']));
		curl_setopt($ch, CURLOPT_FOLLOWLOCATION, FALSE);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
		$result=curl_exec($ch);
		$code = curl_getinfo($ch, CURLINFO_HTTP_CODE);
		curl_close($ch);
	
		if ($code != 201){
			trigger_error( 'Unable to create ticket: '.$result, E_USER_ERROR );
			return false;
			exit;
		}
		
		$ticket_id = (int) $result;
	}else{
		$text = $data['message'].PHP_EOL.
		PHP_EOL.'-- '.PHP_EOL;
		foreach( $data as $itemName=>$itemValue){
			if ($itemName == 'message'){
				continue; // handled before
			}
			$text.=$itemName.': '.($itemName=='topicId'?$ticketCategories[$itemValue]:$itemValue).PHP_EOL;
		}
		
		if (!$debug&&($cfg->doesExist('ticketReceiverUID')&&!empty($cfg->get('ticketReceiverUID')))){
			// Mail handler
			$mailFunc->sendMail($usr, $cfg->get('ticketReceiverUID'), '['.$ticketCategories[$data['topicId']].'] '.$data['subject'], $text);
			$ticket_id = 42; // why 42? well.
		}else{
			// Debug handler
			$pge->put('<div class="labsys_mop_ticketElementView">'.htmlentities(json_encode($data)).'</div>');
			
			$pge->put(nl2br($text));
			$ticket_id = 23;
		}
	}
	
	# Continue onward here if necessary. $ticket_id has the ID number of the
	# newly-created ticket
}
if ($ticket_id==0){
	$pge->put( '<div class="labsys_mop_ticketElementView">'.$lastElement->show( $url->get('address'), '' ).'</div>' );
}

$pge->put( '<h1>'.$pge->title.'</h1>'.PHP_EOL);
$pge->put( '<form class="labsys_mop_ticketForm" name="ticket" method="post" action="#">');
$pge->put( '<div class="labsys_mop_ticketRow"><label class="labsys_mop_ticketLabel" for="subject">subject</label><input class="labsys_mop_ticketInput" name="subject" id="subject" value="'.(isset($_POST['subject']) ? $_POST['subject'] : $lastElement->title.' <- '.$element->title).'"'.($ticket_id!=0?' disabled="disabled"':'').' /></div>' );
// Defined in $ticketCategories array.
$pge->put( '<div class="labsys_mop_ticketRow"><label class="labsys_mop_ticketLabel" for="topicId">category:</label> ');
$pge->put( '<select name="topicId"'.(isset($_POST['topicId']) && $_POST['topicId'] != 'notSelected' ? ' disabled="disabled"':'').'>');
$pge->put( '<option value="notSelected"'.(!isset($_POST['topicId']) || $_POST['topicId']=='notSelected' || !is_numeric($_POST['topicId']) ? ' selected="selected"':'').'>'.$lng->get('pleaseSelect').'</option>'.PHP_EOL );
		$counter = 0;
		foreach ($ticketCategories as $ticketCategory){
			$pge->put( '<option value="'.$counter.'"'.(isset($_POST['topicId']) && is_numeric($_POST['topicId']) && $_POST['topicId']==$counter ? ' selected="selected"':'').'>'.$ticketCategory.'</option>'.PHP_EOL );
			$counter++;
		}
$pge->put( '</select>' );
$pge->put( '<div class="labsys_mop_ticketRow"><label class="labsys_mop_ticketLabel" for="message">message</label><textarea class="labsys_mop_ticketTextArea" name="message" id="message" onFocus="if (this.value==\'Your Message...\'){this.select();}"'.($ticket_id!=0?' disabled="disabled"':'').'>'.(isset($_POST['message']) ? $_POST['message'] : 'Your Message...').'</textarea>' );
$pge->put('<input type="submit" id="submit-button" class="labsys_mop_ticketButton" value="'.($ticket_id==0?$lng->get("save"):'Created ticket '.$ticket_id.'" disabled="disabled').'" />');

if ($debug) {
	$pge->put( '<div class="labsys_mop_ticketRow"><label class="labsys_mop_ticketLabel" for="parentID">ParentID</label>' );
}
$pge->put( '<input '.($debug?'':'type="hidden" ').'class="labsys_mop_ticketInput" name="parentID" id="parentID" readonly="readonly" value="'.$element->elementId.$element->idx.'" /></div>' );

if ($debug) {
	$pge->put( '<div class="labsys_mop_ticketRow"><label class="labsys_mop_ticketLabel" for="parentTitle">ParentTitle</label>' );
}
$pge->put( '<input '.($debug?'':'type="hidden" ').'class="labsys_mop_ticketInput" name="parentTitle"id="parentTitle" readonly="readonly" value="'.$element->title.'" /></div>' );

if ($debug) {
	$pge->put( '<div class="labsys_mop_ticketRow"><label class="labsys_mop_ticketLabel" for="affectedElementID">affectedElementID</label>' );
}
$pge->put( '<input '.($debug?'':'type="hidden" ').'class="labsys_mop_ticketInput" name="affectedElementID" id="affectedElementID" readonly="readonly" value="'.$lastElement->elementId.$lastElement->idx.'" /></div>' );

if ($debug) {
	$pge->put( '<div class="labsys_mop_ticketRow"><label class="labsys_mop_ticketLabel" for="affectedElement">AffectedElementTitle</label>' );
}
$pge->put( '<input '.($debug?'':'type="hidden" ').'class="labsys_mop_ticketInput" name="affectedElementTitle" id="affectedElementTitle" readonly="readonly" value="'.$lastElement->title.'" /></div>' );

if ($debug) {
	$pge->put( '<div class="labsys_mop_ticketRow"><label class="labsys_mop_ticketLabel" for="name">name</label>' );
}
$pge->put( '<input '.($debug?'':'type="hidden" ').'class="labsys_mop_ticketInput" name="name" id="name" readonly="readonly" value="'.$usr->foreName.' '.$usr->surName.'" /></div>' );

if ($debug) {
	$pge->put( '<div class="labsys_mop_ticketRow"><label class="labsys_mop_ticketLabel" for="email">email</label>' );
}
$pge->put( '<input '.($debug?'':'type="hidden" ').'class="labsys_mop_ticketInput" name="email" id="email" readonly="readonly" value="'.$usr->mailAddress.'" /></div>' );

if ($debug) {
	$pge->put( '<div class="labsys_mop_ticketRow"><label class="labsys_mop_ticketLabel" for="currentTeam">current team</label>' );
}
$pge->put( '<input '.($debug?'':'type="hidden" ').'class="labsys_mop_ticketInput" name="currentTeam" id="currentTeam" readonly="readonly" value="'.$usr->currentTeam.'" /></div>' );

if ($debug) {
	$pge->put( '<div class="labsys_mop_ticketRow"><label class="labsys_mop_ticketLabel" for="link2Element">link2Element</label>' );
}
$pge->put( '<input '.($debug?'':'type="hidden" ').'class="labsys_mop_ticketInput" name="link2Element" id="link2Element" readonly="readonly" value="'.$url->link2('http'.(!empty($_SERVER['HTTPS']) ? 's':'').'://'.$_SERVER['SERVER_NAME'].'/pages/view.php', Array('address' => $url->get('address'), 'seeMe' => ($url->available('seeMe')?$url->get('seeMe'):$usr->uid))).'#'.$url->get('address').'" /></div>' );

if ($debug) {
	$pge->put( '<div class="labsys_mop_ticketRow"><label class="labsys_mop_ticketLabel" for="config">config</label>' );
}
$pge->put( '<input '.($debug?'':'type="hidden" ').'class="labsys_mop_ticketInput" name="config" id="config" readonly="readonly" value="'.$configPrefix.$GLOBALS['url']->get("config").'" /></div>' );

$pge->put( '</form>');

$pge->put('
<script language="JavaScript" type="text/javascript">
<!--
if (document.ticket){
		 document.ticket.subject.select();
		 document.ticket.subject.focus();
}
//-->
</script>
');

$GLOBALS['Logger']->logReferrerEvent();

require( $cfg->get("SystemPageLayoutFile") );
?>
