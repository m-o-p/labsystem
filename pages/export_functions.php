<?php


/************** INPUTS ******************/
$Original = array ('l12','c121' , 'c15' , 'p12' , 'm34' , 'm45' , 'C34', 'p45' , 'C367' , 'i18');
$Variable_lab = '367'; //Content of lab prelab_collection_idx
$Variable_collections = 'c15 C367 p12 m45'; //content of collections field 'contents'

//below the contents of p element
$Variable_element = '[HTML]
<p>
<center><img src="../pix/ilab2/multisip/Setup.png" width="751" height="374" /></center>
</p>

<p>You should be able to do the cabling and configure the routing on all the Linux PCs. Make sure you <b>set default routes properly</b> (to the Cisco routers), they will be used for routing to multicast address later. The interface eth0 is the interface closest to the WIFI Antenna</p>';

$Variable_element2='[HTML]
<h3><img src="../syspix/button_manage_elements_13x12.gif" width="13" height="12" border="0"> __ELEMENTTITLE__</h3>
<a href="http://www.m-o-p.de/life/src2/2007-07-27_studienarbeit_pahl_eLearning.pdf">hier</a>.
<img src="../pix/ilab2/multisip/Setup.png" width="751" height="374" />
<table align="center" cellspacing="10px" style="width:600px; table-layout:fixed; ">
<tr><td></td><td width="50px;"><td></td></tr><!-- table-layout fixed => sizes set to first (this) row -->

 <tr>
  <td align="right">
    <a href="../pages/edit.php?address=p45&saveAsNew&__LINKQUERY__"><b>new page</b>...</a>
  </td>
  <td class="labsys_mop_p_row">
    <img src="../syspix/button_new_39x36.gif" width="39" height="36" border="0">
  </td>
  <td align="left">
    <a href="../pages/manage.php?address=p12&__LINKQUERY__"><b>manage pages</b>...</a>
  </td>
 </tr>

 <tr>
  <td align="right">
    <a href="../pages/edit.php?address=m34&saveAsNew&__LINKQUERY__"><b>new multiple choice</b>...</a>
  </td>
  <td class="labsys_mop_m_row">
    <img src="../syspix/button_new_multichoice_39x36.gif" width="39" height="36" border="0">
  </td>
  <td align="left">
    <a href="../pages/manage.php?address=m34&__LINKQUERY__"><b>manage multiple choices</b>...</a>
  </td>
 </tr>

 <tr>
  <td align="right">
    <a href="../pages/edit.php?address=i18&saveAsNew&__LINKQUERY__"><b>new input</b>...</a>
  </td>
  <td class="labsys_mop_i_row">
    <img src="../syspix/button_new_input_39x36.gif" width="39" height="36" border="0">
  </td>
  <td align="left">
    <a href="../pages/manage.php?address=i18&__LINKQUERY__"><b>manage inputs</b>...</a>
  </td>
 </tr>

 <tr>
  <td align="right">
    <a href="../pages/edit.php?address=c15&saveAsNew&__LINKQUERY__"><b>new collection</b>...</a>
  </td>
  <td class="labsys_mop_c_row">
    <img src="../syspix/button_new_collection_39x36.gif" width="39" height="36" border="0">
  </td>
  <td align="left">
    <a href="../pages/manage.php?address=c15&__LINKQUERY__"><b>manage collections</b>...</a>
  </td>
 </tr>

 <tr>
  <td align="right">
    <a href="../pages/edit.php?address=l12&saveAsNew&__LINKQUERY__"><b>new lab</b>...</a>
  </td>
  <td class="labsys_mop_l_row">
    <img src="../syspix/button_new_lab_39x36.gif" width="39" height="36" border="0">
  </td>
  <td align="left">
    <a href="../pages/manage.php?address=l12&__LINKQUERY__"><b>manage labs</b>...</a><br>
    <a href="../pages/export_import_labs.php?address=l&__LINKQUERY__"><b>export/ import labs</b>...</a><br>
  </td>
 </tr>

 <tr>
  <td align="right">
    <a href="../pages/edit.php?address=s1&saveAsNew&__LINKQUERY__"><b>new schedule</b>...</a>
  </td>
  <td class="labsys_mop_s_row">
    <img src="../syspix/button_new_schedule_36x36.gif" width="36" height="36" border="0">
  </td>
  <td align="left">
    <a href="../pages/manage.php?address=s&__LINKQUERY__"><b>manage schedules</b>...</a>
  </td>
 </tr>
</table>

<div class="labsys_mop_note">
<div class="labsys_mop_h3">This page is dynamic!</div>
<p>The big advantage is that you can easily add your own templates (<a href="../pages/edit.php?address=p2&__LINKQUERY__"><img src="../syspix/button_edit_13x12.gif" width="13" height="12" border="0"></a>)!</p>
<ul>
  <li>create your template</li>
  <li>save it</li>
  <li>copy the link (<img src="../syspix/button_link2_13x12.gif" width="13" height="12" border="0">)</li>
  <li>add it to your section here</li>
  <li>replace "__LINKQUERY__" by "saveAsNew&__LINKQUERY&#95;&#95;" (the save as new checkbox will be enabled then)</li>
  <li>done</li>
</ul>
</div>';
/*************************************/

/************Function Calls**************/
$NewId=CreateNewIdPairs($Original);
print_r($NewId);
die();
$New_variable_lab=ReplaceLab($Variable_lab, $NewId);
$New_variable_collection=ReplaceCollections($Variable_collections, $NewId);
$files_to_be_copied = ReplaceElements(&$Variable_element2, $NewId);
/*************************************/

/************** DEBUG ******************/
echo "The New Id pairs :";
print_r($NewId);
echo "<br>";
echo "The new variable content of the replaced lab prefix_lab_collection :".$Variable_lab ." is ";
echo $New_variable_lab ." <br>";
echo "The new contents of the collections element is :" . $Variable_collections ." is ";
echo $New_variable_collection . " <br>";
echo "The new html contents of the element is :";
echo $Variable_element2 . "<br>";
echo "The image and reference files to be copied are :";
print_r ($files_to_be_copied);
/**************************************/


/************** INFO ******************/
//To create the New export Ids for the Lab to be exported
/*************************************/
function CreateNewIdPairs ($Orig){
	natsort($Orig);
  print_r($Orig);
	$NewId = array_flip($Orig);

	$letter ='';
	$index =1;
print_r($Orig);
	foreach( $Orig as $element ){
  echo $element;
		if ( $letter != ($elem_name = substr($element,0,1))) {
			$letter = $elem_name;
			$index = 1;
		}

		$NewId[$element]=$letter.$index++;
	}
	return $NewId;
}

/************** INFO ******************/
//To replace the prelab_collection_idx and lab_collection_idx of the lab element
/*************************************/
function ReplaceLab ($Variable_content , $NewId){

	return substr( $NewId['C'.$Variable_content] ,1 );

}

/************** INFO ******************/
//To replace the elements in the contents field of the Collections element
/*************************************/
function ReplaceCollections ( $Variable_content , $NewId){

	$New_content = array ();
	$contents = explode(' ',$Variable_content);
	foreach ( $contents as $element ){
		array_push ( $New_content , $NewId[$element]);
	}
	return join(" ", $New_content);

}

/************** INFO ******************/
//To replace the elements in the contents of p , m ,i
/*************************************/
function ReplaceElements ( &$Variable_content , $NewId) {

	$files = array();
	$files=array_merge($files , ParseFiles (&$Variable_content));
	$files=array_merge($files , ParseImage (&$Variable_content));
	ReplaceAddress(&$Variable_content , $NewId);
	return remove_empty_values($files);

}

/************** INFO ******************/
//To parse and replace the images in the elements
/*************************************/
function ParseImage ( &$Variable_content  ){

	$regex  = '/(<img\s*';
	$regex .= '(.*?)\s*)';
	$regex .= 'src=[\'"]+?\s*(?P<img>\S+)\s*[\'"]+?';
	$regex .= '(\s*(.*?)\s*>)/i';

	return Parse ( &$Variable_content , $regex , 'img' , '/images/' , 'no_syspix');
}

/************** INFO ******************/
//To parse and replace the files other than images in the elements
/*************************************/
function ParseFiles ( &$Variable_content  ) {

	$regex  = '/(<a\s*';
	$regex .= '(.*?)\s*)';
	$regex .= 'href=[\'"]+?\s*(?P<files>\S+)\s*[\'"]+?';
	$regex .= '(\s*(.*?)\s*>)/i';

	return Parse ( &$Variable_content , $regex , 'files' , '/references/' , 'no_address');
}


/************** INFO ******************/
//To replace the element in the address field of the href tag 
/*************************************/
function ReplaceAddress ( &$Variable_content ,$NewId){

	$regex  = '/(\?address=)';
	$regex .= '(?P<data>\S+&)/i';
	
	preg_match_all($regex,$Variable_content,$match);
	foreach ( $match["data"] as $element ){
		$value = explode("&",$element);
		if ( $NewId[$value[0]] != null) {
		$Variable_content=str_replace($value[0], $NewId[$value[0]], $Variable_content);
		}
		else {
			echo "<h4> ERROR : The element \"".$value[0] ."\" is not exported ; Reported from RepalceAddress function </h4><br>";
		}
	}
}

/************** INFO ******************/
//Generic function to parse and replace
//return - array of files found in the parsed content
/*************************************/
function Parse ( &$Variable_content , $regex ,$match_term , $new_path , $condition  ) {

	$files = array ();
	$new_files = array ();

	preg_match_all($regex,$Variable_content,$match);
	foreach ( $match[$match_term] as $element ){
		$match_element='';
		switch($condition){
			case "no_syspix": if ( !(strpos($element , "syspix"))) {$match_element = $element;break;}
			case "no_address": if (!(strpos($element , "address")) && !(strpos($element , "syspix")))  {$match_element = $element;break;}
			//default : "Error";
		}
		array_push ( $files , $match_element);
		array_push ( $new_files,$new_path.(end (explode('/',$match_element))));
	}
	$Variable_content = str_replace($files, $new_files ,$Variable_content);
	return $files;
}


/************** INFO ******************/
//To remove empty and null values from the array
/*************************************/
function remove_empty_values($array, $remove_null = true)
{
	$new_array = array();
	$null_exceptions = array();
	foreach ($array as $key => $value){
		$value = trim($value);
		if($remove_null){
			$null_exceptions[] = '0';
		}
		if(!in_array($value, $null_exceptions) && $value != ""){
			$new_array[] = $value;
		}
	}
	return $new_array;
}

