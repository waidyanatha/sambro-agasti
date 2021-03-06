<?php

/**
* The Sahana Messaging Module
*
* PHP version 5
*
* LICENSE: This source file is subject to LGPL license
* that is available through the world-wide-web at the following URI:
* http://www.gnu.org/copyleft/lesser.html
*
* @author       	Mifan Careem <mifan@respere.com>
* @copyright  		Respere - http://respere.com/
* @package      	sahana
* @subpackage   	msg
* @tutorial
* @version      	$Id$
* @license       	http://www.gnu.org/copyleft/lesser.html GNU Lesser General
* @contribution    This code was developed and contributed to the project 
* by Respere Lanka (Pvt) Ltd.
*/

/**
 * 
 * @param $source_xml
 * @param $conversion_xsl
 * @return unknown_type
 */
function shn_msg_xsl_transform($xml_id,$xsl_name){
	$output=0;
	//print $xsl_name;
	//$xsl_name='short_to_sms';
	//print $xml_id;
	//Get source xml from parameter
	$source_file = shn_msg_alert_get_xml_from_db($xml_id);
//print $source_file;
	// creating & loading DOMDocument with XML
	$xml = new DOMDocument;

	$xml->substituteEntities = true;
	if ($xml->loadXML($source_file) == false){ // loadXML failed
    	if($xsl_name != 'view_data')
    	{
			add_error('Failed to load source XML '); // if document is not valid XML
    	}
		return false;
	}
	//print $xml->saveXML();
	//Set sent time to current time - store XML in memory
	//shn_msg_alert_set_sent_cap_time(&$xml);
	

	shn_msg_alert_populate_session_on_type($xml,$xsl_name);

	//Get XSL for relevant conversion
	//print $xsl_name;
	//$test = shn_msg_test($test);
	
	$conversion_xsl_location = shn_msg_alert_get_xsl_from_fs($xsl_name);
	//print $conversion_xsl_location;
	$converstion_xsl_file = file_get_contents($conversion_xsl_location);
	if($converstion_xsl_file==''){
		add_error('Empty XSL File');
		return false;
	}
	
	// 	Loading XSLT site 
	$stylesheet = new DOMDocument;
	$stylesheet->substituteEntities = true;
	if ($stylesheet->loadXML($converstion_xsl_file) == false){
		add_error('Failed to load XSLT file');
		return false;
	}


	// XSLT transformation
	$xsl = new XSLTProcessor();
	$xsl->registerPHPFunctions();
	$xsl->importStyleSheet($stylesheet);
	$output = $xsl->transformToXML($xml); // transforming

	//echo "<h2>"._('Original CAP XML')."</h2>";	
	// Print XML output
	//echo "<pre>"."\n";
//	print($source_file);
//	print(htmlspecialchars($source_file));

	$print_xml = xml_pretty_printer($source_file, true);
//	print $print_xml;
	//echo "</pre>\n";
	//echo "transformation";
	//print $xml->saveXML();
	if($xsl_name != 'view_data')
	{
		echo "<h2>"._('CAP MESSAGE :: ').$xml_id."</h2>";
	}
	
	if($xsl_name == "voice_to_ivr"){
	
		if(!file_exists("../mod/msg/ivrtext/"))
		{
			mkdir("../mod/msg/ivrtext/");
		}
		
		$list=shn_msg_alert_details($xml_id);
		
		
		/*$insert_file_array = array();
		$insert_file_array['ivr_alert_id'] = $xml_id;
		$insert_file_array['ivr_alert_name'] = $list['name'];
		$insert_file_array['ivr_alert_content'] = $list['name'];
		
		global $global;
		$db = $global['db'];
		//$rset = $global['db']->AutoExecute('msg_ivr_alert', $insert_file_array, 'INSERT');
		$sql = "SELECT * FROM msg_ivr_alert";
		/* if(!$res = mysql_query($sql))
		{
			die("Invalid");
		} */
		/*$result = $db->Execute($sql);
		var_dump($sql);
		if(!$result->EOF){
			$message = $result->fields['message_content'];
		}
		var_dump($message);
		$xml_save_to_file = fopen("../mod/msg/ivrtext/".$xml_id.".txt","w");
		fwrite($xml_save_to_file, $output);
		fclose($xml_save_to_file);*/
	
	}
	return $output;
	
}

function shn_msg_alert_get_xml_from_db($xml_id)
{
	global $global;
	$db=$global['db'];
	
	$sql = "SELECT file FROM msg_alerts WHERE alert_uuid='{$xml_id}'";
	$results = $db->Execute($sql);
	$xml = $results->fields['file'];
	return $xml;
}

function shn_msg_alert_put_xml_file_to_db($xml_id,$xml_dom)
{
	global $global;
	$db=$global['db'];
	$xml_doc='';
	$sql = "UPDATE msg_alerts SET file='{$xml_doc}' WHERE alert_uuid='{$xml_id}'";
	$results = $db->Execute($sql);
}

function shn_msg_alert_get_xsl_from_fs($xsl_name)
{	
//Get XSDL from parameter
	global $global;
	$xsl_location = realpath(dirname(__FILE__));
	$xsl_location .= '/../../res/msg/cap/xsl/';
	$xsl_location .= shn_msg_alert_get_xsl_file_name($xsl_name);
	return $xsl_location;
}

function shn_msg_alert_get_xsl_file_name($name,$type='cap')
{
	//print $name;
	global $conf;
	$xsl_conf_name ='mod_msg_';
	$xsl_conf_name .=$type;
	$xsl_conf_name .='_'.$name;
	//print $xsl_conf_name;
	return $conf[$xsl_conf_name];
}

function shn_msg_alert_show_converted_message_send_form($text,$type=null)
{
	//shn_form_fopen('send_message',null,array('req'=>false));
	//print($type);
	$xml_id = $_SESSION['msg']['send']['xml_id'];
	if($type == 'view_data_indv')
	{
		shn_form_fopen('view_list_alert',null,null);
		print($text);
		shn_form_hidden(array('seq'=>'alert_more'));
//		$xml_id = $_SESSION['msg']['send']['xml_id'];
		shn_form_hidden(array('xml_id'=>$xml_id));
		shn_form_submit("More");
		shn_form_fclose();
/** 
 * @author Nuwan (waidyanatha@gmail.com)
 * @comment now we can remove the view_alert menu and use this form as an all pupose one
 * @todo make publish to web mandatory
**/
		$panel ="<a href='index.php?mod=msg&act=send_alert&seq=select_contact&xml_id=$xml_id'>Send</a>&nbsp;&nbsp;&nbsp;<a href='index.php?mod=msg&act=update_alert&id=$xml_id'>Update</a>&nbsp;&nbsp;&nbsp;<a href='index.php?mod=msg&act=delete_alert&id=$xml_id'>Delete</a>";
		print "<br><strong>$panel</strong>";

	}
	else if ($type == 'view_data_indv_more')
	{
/** 
 * @author Nuwan (waidyanatha@gmail.com)
 * @comment now we can remove the view_alert menu and use this form as an all pupose one
 * @todo make publish to web mandatory
**/
		$panel ="<a href='index.php?mod=msg&act=send_alert&seq=select_contact&xml_id=$xml_id'>Send</a>&nbsp;&nbsp;&nbsp;<a href='index.php?mod=msg&act=update_alert&id=$xml_id'>Update</a>&nbsp;&nbsp;&nbsp;<a href='index.php?mod=msg&act=delete_alert&id=$xml_id'>Delete</a>";

		print "<strong>$panel</strong><br><br>";
		print($text);
		//$text = $text;
		print "<br><strong>$panel</strong>";
	}
	else if($type == 'view_data')
	{
		print($text);
	}
	else
	{
	$display_text = 'Converted Message: ';
	$var_name = 'message';

	
	switch ($type){
		case 'short_to_sms':
			//clear session
			$display_text .= 'Short SMS';
			$var_name .= '_short_sms';
			break;
		case 'short_to_dlabel':
                        $display_text .= 'Dynamic Label';
                        $var_name .= '_short_dlabel';
                        break;
		case 'short_to_email':
			$display_text .= 'Short Email';
			$var_name .= '_short_email';
			break;
		case 'long_to_email':
			$display_text .= 'Long Email';
			$var_name .= '_long_email';
			break;
		case 'long_to_signage':
                        $display_text .= 'Long Signage';
                        $var_name .= '_long_signage';
                        break;
		case 'voice_to_ivr':
			$display_text .= 'Voice IVR';
			$var_name .= '_voice_ivr';
			break;
		case 'short_to_tweet':
			$display_text .= 'Short Tweet';
			$var_name .= '_short_tweet';
			break;
		default:
	}
//	var_dump($display_text.$var_name);
	$_SESSION['ivr_msg'] = $text;
	$_SESSION['ivr_msg_id'] = $_GET['id'];
	$_SESSION['ivr_var_name'] = $var_name;
	shn_form_textarea($display_text,$var_name,null,array('value'=>$text));
	}
	
	/*
	shn_form_hidden(array('to'=>$_SESSION['msg']['send']['to']));
	shn_form_submit('Send Message');
	shn_form_fclose();
	*/
	
}

function shn_msg_alert_populate_session_on_type($xml_dom,$type)
{
	//bootstrap
	//clear session
	$_SESSION['msg']['delivery']=array();
	$xml_xpath = new DOMXPath($xml_dom);
	$xml_xpath->registerNamespace('n','urn:oasis:names:tc:emergency:cap:1.1');
	
	//print $type;
	//switch
	switch ($type){
		case 'short_to_sms':
			//clear session
			$_SESSION['msg']['sms']=array();
			break;
		case 'short_to_dlabel':
			//clear session
			$_SESSION['msg']['dlabel']=array();
			break;
		case 'long_to_email':
			$_SESSION['msg']['email']=array();
			//generate email subject and store in session
			$query = '//n:alert/n:info/n:headline';
			$value = $xml_xpath->query($query);
			$subject = $value->item(0)->textContent;
			$subject = $subject.' for ';
			$query = '//n:alert/n:info/n:area/n:areaDesc';
			$value = $xml_xpath->query($query);
			$subject = $subject.$value->item(0)->textContent;
			$_SESSION['msg']['email']['subject']= $subject;
			
			$query = '//n:alert/n:sender';
			$value = $xml_xpath->query($query);
			$sender = $value->item(0)->textContent;
			$_SESSION['msg']['email']['sender']= $sender;

			global $global;
			$path = $global['approot'].'www/tmp/';
			$file = 'cap_alert.xml';
			$_SESSION['msg']['email']['attachment']= shn_msg_alert_save_tmp_xml($path,$file,$xml_dom);
			//echo $_SESSION['msg']['email']['attachment'];
		break;
		case 'long_to_signage':
                        $_SESSION['msg']['email']=array();
                        //generate email subject and store in session
                        $query = '//n:alert/n:info/n:headline';
                        $value = $xml_xpath->query($query);
                        $subject = $value->item(0)->textContent;
                        $subject = $subject.' for ';
                        $query = '//n:alert/n:info/n:area/n:areaDesc';
                        $value = $xml_xpath->query($query);
                        $subject = $subject.$value->item(0)->textContent;
                        $_SESSION['msg']['email']['subject']= $subject;

                        $query = '//n:alert/n:sender';
                        $value = $xml_xpath->query($query);
                        $sender = $value->item(0)->textContent;
                        $_SESSION['msg']['email']['sender']= $sender;

                        global $global;
                        $path = $global['approot'].'www/tmp/';
                        $file = 'cap_alert.html';
                        $_SESSION['msg']['email']['attachment']= shn_msg_alert_save_tmp_xml($path,$file,$xml_dom);
                        //echo $_SESSION['msg']['email']['attachment'];
                break;	
		case 'short_to_email':
			$_SESSION['msg']['email']=array();
			//generate email subject and store in session
			$query = '//n:alert/n:info/n:headline';
			$value = $xml_xpath->query($query);
			$subject = $value->item(0)->textContent;
			$subject = $subject.' for ';
			$query = '//n:alert/n:info/n:area/n:areaDesc';
			$value = $xml_xpath->query($query);
			$subject = $subject.$value->item(0)->textContent;
			$_SESSION['msg']['email']['subject']= $subject;
			
			$query = '//n:alert/n:sender';
			$value = $xml_xpath->query($query);
			$sender = $value->item(0)->textContent;
			$_SESSION['msg']['email']['sender']= $sender;
			
			break;
			
		case 'view_data':
			//$_SESSION['msg']['email']=array();
			//generate email subject and store in session
			/*$query = '//n:alert/n:info/n:headline';
			$value = $xml_xpath->query($query);
			$subject = $value->item(0)->textContent;
			$subject = $subject.' for ';
			$query = '//n:alert/n:info/n:area/n:areaDesc';
			$value = $xml_xpath->query($query);
			$subject = $subject.$value->item(0)->textContent;
			$_SESSION['msg']['email']['subject']= $subject;
			
			$query = '//n:alert/n:sender';
			$value = $xml_xpath->query($query);
			$sender = $value->item(0)->textContent;
			$_SESSION['msg']['email']['sender']= $sender;*/

			global $global;
			$path = $global['approot'].'www/tmp/';
			$file = 'cap_alert.xml';
			$_SESSION['msg']['email']['attachment']= shn_msg_alert_save_tmp_xml($path,$file,$xml_dom);
			//echo $_SESSION['msg']['email']['attachment'];
			break;
		case 'view_data_indv':
			
			/*$_SESSION['msg']['email']=array();
			//generate email subject and store in session
			$query = '//n:alert/n:info/n:headline';
			$value = $xml_xpath->query($query);
			$subject = $value->item(0)->textContent;
			$subject = $subject.' for ';
			$query = '//n:alert/n:info/n:area/n:areaDesc';
			$value = $xml_xpath->query($query);
			$subject = $subject.$value->item(0)->textContent;
			$_SESSION['msg']['email']['subject']= $subject;
			
			$query = '//n:alert/n:sender';
			$value = $xml_xpath->query($query);
			$sender = $value->item(0)->textContent;
			$_SESSION['msg']['email']['sender']= $sender;*/

			global $global;
			$path = $global['approot'].'www/tmp/';
			$file = 'cap_alert.xml';
			$_SESSION['msg']['email']['attachment']= shn_msg_alert_save_tmp_xml($path,$file,$xml_dom);
			//echo $_SESSION['msg']['email']['attachment'];
			break;
			
		case 'view_data_indv_more':
			/*$_SESSION['msg']['email']=array();
			//generate email subject and store in session
			$query = '//n:alert/n:info/n:headline';
			$value = $xml_xpath->query($query);
			$subject = $value->item(0)->textContent;
			$subject = $subject.' for ';
			$query = '//n:alert/n:info/n:area/n:areaDesc';
			$value = $xml_xpath->query($query);
			$subject = $subject.$value->item(0)->textContent;
			$_SESSION['msg']['email']['subject']= $subject;
			
			$query = '//n:alert/n:sender';
			$value = $xml_xpath->query($query);
			$sender = $value->item(0)->textContent;
			$_SESSION['msg']['email']['sender']= $sender;*/

			global $global;
			$path = $global['approot'].'www/tmp/';
			$file = 'cap_alert.xml';
			$_SESSION['msg']['email']['attachment']= shn_msg_alert_save_tmp_xml($path,$file,$xml_dom);
			//echo $_SESSION['msg']['email']['attachment'];
			break;
		default:	
	}
	
}



function shn_msg_alert_save_tmp_xml($path,$file,$xml_dom)
{
	//echo $path;
	if(is_writable($path)){
		file_put_contents($path.$file,$xml_dom->saveXML());
		//$cap_alert = $xml_dom->saveXML();
	}
	else
		add_error('path not writeable - cannot send mail attachment');
	return $path.$file;
}

function xml_pretty_printer($xml, $html_output=FALSE)
{
	$xml_obj = new SimpleXMLElement($xml);
	$xml_lines = explode("
", $xml_obj->asXML());
	$indent_level = 0;
	
	$new_xml_lines = array();
	foreach ($xml_lines as $xml_line) {
		if (preg_match('#^(<[a-z0-9_:-]+((s+[a-z0-9_:-]+="[^"]+")*)?>.*<s*/s*[^>]+>)|(<[a-z0-9_:-]+((s+[a-z0-9_:-]+="[^"]+")*)?s*/s*>)#i', ltrim($xml_line))) {
			$new_line = str_pad('', $indent_level*4) . ltrim($xml_line);
			$new_xml_lines[] = $new_line;
		} elseif (preg_match('#^<[a-z0-9_:-]+((s+[a-z0-9_:-]+="[^"]+")*)?>#i', ltrim($xml_line))) {
			$new_line = str_pad('', $indent_level*4) . ltrim($xml_line);
			$indent_level++;
			$new_xml_lines[] = $new_line;
		} elseif (preg_match('#<s*/s*[^>/]+>#i', $xml_line)) {
			$indent_level--;
			if (trim($new_xml_lines[sizeof($new_xml_lines)-1]) == trim(str_replace("/", "", $xml_line))) {
				$new_xml_lines[sizeof($new_xml_lines)-1] .= $xml_line;
			} else {
				$new_line = str_pad('', $indent_level*4) . $xml_line;
				$new_xml_lines[] = $new_line;
			}
		} else {
			$new_line = str_pad('', $indent_level*4) . $xml_line;
			$new_xml_lines[] = $new_line;
		}
	}
	
	$xml = join("
", $new_xml_lines);


	return ($html_output) ? '<pre>' . htmlentities($xml) . '</pre>' : $xml;
}

/**
 * Set current transformation time as CAP sent time instead of creation time
 * @param $xmlid
 * @return unknown_type
 */
function shn_msg_alert_set_sent_cap_time($xml_dom)
{
	//$xml_dom = new DOMDocument();
	$today = date('Y-m-d h:m:s');
	$xml_xpath = new DOMXPath($xml_dom);
	$xml_xpath->registerNamespace('n','urn:oasis:names:tc:emergency:cap:1.1');
	$query = '//n:alert/n:sent';
	$value = $xml_xpath->query($query);
	$oldnode = $value->item(0);
	$parent = $oldnode->parentNode;
	$todayvalue = $xml_dom->createTextNode($today);
	$parent->replaceChild($todayvalue,$oldnode);
	
	//$oldnode->parentNode->replaceChild($todayvalue,$oldnode);
	
	//$value->nodeValue = $today;
	$xml_dom->saveXML();
	return $xml_dom;
	
}

function shn_msg_convert_tiny_url($weburl, $identifier)
{
	//var_dump($weburl);
	//var_dump($identifier);
	global $conf;
	$url = $weburl[0]->nodeValue;
	$xml_id = $identifier[0]->nodeValue;
	//print $xml_id;
	//print $url;
	//print $conf['tinyurl'];
	if($conf['tinyurl'])
	{
		$tiny = file_get_contents("http://tinyurl.com/api-create.php?url=$url");
	//print $tiny;
		if(!isset($tiny) || $tiny=='')
		{
			$tiny = $url;
		}
	}
	else
	{
		$tiny = $url;
	}
	
	return $tiny;
}

function shn_msg_default_title()
{
	global $conf;
	$default_title = $conf['default_title'];
	return $default_title;
}

