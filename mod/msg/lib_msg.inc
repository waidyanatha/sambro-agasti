<?php
/**
*
*
* Library functions for implementation independant messaging
* Main messaging controller functions with modular plugin architecture
* 
* PHP version 5
*
* LICENSE: This source file is subject to LGPL license
* that is available through the world-wide-web at the following URI:
* http://www.gnu.org/copyleft/lesser.html
*
* @author     Mifan Careem <mifan@respere.com>
* @copyright  Respere - http://respere.com/
* @package	   Sahana - http://sahana.lk/
* @subpackage
* @version	  $Id: lib_msg.inc,v 1.6 2008/09/15 13:57:35 franboon Exp $
* @license	  http://www.gnu.org/copyleft/lesser.html GNU Lesser General Public License (LGPL)
*/
global $global;
require_once $global['approot']."/inc/lib_errors.inc";
require_once $global['approot']."/mod/msg/lib_sms_main.inc";
/**
 * Send Text/Short message as SMS via messaging controller
 * Dependant on underlying gateway selected and stored as conf value
 * @param $recipients mixed_array recipient number array
 * @param $message string message to be sent
 * @return int delivery status-0: failed; 1: success
 */

/**
** @comment : temporarily commented out to accommodate clicktell until modem is fixed, Nuwan 2011-01-18
**
function shn_msg_send_short_message($recipients,$message)
{
	global $global;
	global $conf;
	
	$plugin_name = $conf['mod_msg_plugin'];
	
	if(shn_msg_check_sms_gateway()){
		if(file_exists($global['approot']."/mod/msg/plugins/"."$plugin_name"."/handler_"."$plugin_name".".inc")){
			include_once $global['approot']."/mod/msg/plugins/"."$plugin_name"."/handler_"."$plugin_name".".inc";
			add_confirmation(_t('Gateway plugin started...'));
			shn_msg_plugin_send_sm($recipients,$message);
			shn_msg_store_sent_messages($recipients,$message);		
			return 1;
		}
		
		//Handle exceptions
		else{
			add_error(_t("The configured SMS gateway has not been implemented"));
			return 0;//Message Sending Failed
		}
	}
	else{
		add_error("The SMS Gateway plugin is not running");
		return 0;
	}
	
	
}
*/
/**
** @comment: temporarily addedd code to include click tell, Nuwan 2011-01-18
**
*/

function shn_msg_send_short_message($receipients,$message)
{
	global $global;
	global $conf;
//	echo "SMS";
	$sender="RTBPMSG";
	$msg=str_replace(" ","%20",$message);
	$msg = substr($msg,0,160);
	foreach($receipients as $receipient){
//*	$url="http://m.mytoday.com/delivery/smshandler?appid=1&to=$receipient&text=$msg&from=$sender";
// temp $url string to include clickatell relaplace this with the above $url string when modem is ready
	//$url="http://api.clickatell.com/http/sendmsg?user=itunbtc&password=YQcgOXcFAWGDHL&api_id=KKRH92&to=$receipient&text=$msg";
	//$url="http://api.clickatell.com/http/sendmsg?user=waidyanatha&password=kotchchi7281&api_id=3281715&to=$receipient&text=$msg";
	$url="http://api.clickatell.com/http/sendmsg?user=itunbtc&password=YQcgOXcFAWGDHL&api_id=3401257&to=$receipient&text=$msg";
	

	$sms=file_get_contents($url);
	if (isset($sms)){
          $status_sms[$receipient]=1;
		  add_confirmation(_t('SMS sent successfully to '.$receipient));
  
		  }
    else
          $status_sms[$receipient]=0;
	}
			  shn_msg_store_sent_messages($receipients,$message);

	return 1;
	
}

/**
 * This function will process the incoming message sent via GET & POST
 * Currently this function will be used by Kannel and SMSTools
 * @output The output should be text and depened on the gateway.
 */
function shn_msg_receive_short_message(){
    global $global;
    global $conf;
    
    $plugin_name = $conf['mod_msg_plugin'];

    if(shn_msg_check_sms_gateway()){
        if(file_exists($global['approot']."/mod/msg/plugins/"."$plugin_name"."/handler_"."$plugin_name".".inc")){
            include_once $global['approot']."/mod/msg/plugins/"."$plugin_name"."/handler_"."$plugin_name".".inc";
            
            $received_message=shn_msg_plugin_receive_sm();
            if(isset($received_message)){
                    $rec=$global['db']->AutoExecute('msg_received_messages',$received_message,'INSERT');
                    if(!$rec){
                        echo "Sahana server Error : Database Error"; //echoes to sms daemon not to user
                        $error_message = _shn_msg_sms_error_code(2);
                        // this should also in future send off an email to admin
                        //Inform the SMS Sender of the error. code 2 as per lib_sms_errors.inc
                        if($error_message)
                            _shn_msg_sms_error_msg_send($received_message['sender'], $error_message);
                    }    
                    else{
                        //echo "SMS Received"; //echoes to sms daemon not to user
                        //Push SMS to lib_sms_main and handle it
                        shn_msg_push_to_sms_handler($received_message);
                    }    
            }
            return 1;
        }
        else
            echo "Sahana server Error :The configured SMS gateway has not been implemented";
    }
    else
        echo "Sahana server Error :The SMS Gateway plugin is not running";
}


/**
 * Check gateway availability via controller
 * @return bool 0:fail ; 1:success
 */
function shn_msg_check_sms_gateway()
{
	global $global;
	global $conf;
	
	$plugin_name = $conf['mod_msg_plugin'];
	if(file_exists($global['approot']."/mod/msg/plugins/"."$plugin_name"."/handler_"."$plugin_name".".inc")){
		include_once $global['approot']."/mod/msg/plugins/"."$plugin_name"."/handler_"."$plugin_name".".inc";
		return shn_msg_plugin_check_status();
	}
	
	//Handle exceptions
	else{
		add_error(_t("The configured SMS gateway has not been implemented"));
		return 0;//Message Sending Failed
	}
	
	
}

/**
 * Store message send request for reporting purposes
 * @param mixed_array $recipients recipient numbers
 * @param string $message message string
 */
function shn_msg_store_sent_messages($recipients,$message)
{
	global $global;
	global $conf;
	
	include_once $global['approot']."/inc/lib_errors.inc";
	include_once($global['approot'].'/inc/lib_uuid.inc');
	
	$msguid = shn_create_uuid('msg');
    //Temporary Mechanism to identify if the SMS is sent through the Web App or as a result of the SMS Login Session 
    if (isset($_SESSION['user_id'])){
        $sender = $_SESSION['user_id']; //Current user of the system
    }
    else{
        $sender = "1"; //Sahana Messaging Module sending the SMS
    }
		
	$db = $global['db'];
	
	$sql = "INSERT INTO msg_message (msg_uuid, message_content)  " .
		   " VALUES ('{$msguid}','{$message}')";
	$db->Execute($sql);
	
	foreach ($recipients as $number){
		$muid = shn_create_uuid('msg');
		$query = "INSERT INTO msg_sent_messages ( m_uuid , sender_id , recipient_number , msg_uuid , status ) " .
				" VALUES ('{$muid}','{$sender}','{$number}','{$msguid}','1')";
		$db->Execute($query);
		
	}
	
	
}

/**
 * Send email message via messaging controller
 * Dependant on undelying SMTP gateway
 * @param mixed_array $recipients email addresses
 * @param string $message message string
 * @return int delivery status- 0: failed; 1: success
 */
function shn_msg_send_email($recipients,$message,$subject=false)
{
	global $global;
	include_once $global['approot'].'/mod/msg/lib_mail.inc';
	if(shn_msg_validate_email($recipients)){
		return _shn_msg_send_group_mail($message,$recipients,$subject);
	}
	else{
		add_error('Error sending mail');
		display_errors();
		return false;
	}
	
}

function shn_msg_validate_sms($recipients)
{
	global $global;
	include_once $global['approot'].'/inc/lib_validate.inc';
	$sms_error = false;
	foreach($recipients as $recipient){
		if(!shn_valid_number(trim($recipient))){
			add_error($recipient._t(' is an invalid mobile number'));
			$sms_error = true;
		}
	}
	if($sms_error)
		return false;
	return true;

}

function shn_msg_validate_email($recipients)
{
	global $global;
	include_once $global['approot'].'/inc/lib_validate.inc';
	$email_error = false;
	foreach($recipients as $recipient){
		if(!shn_valid_email(trim($recipient))){
			add_error($recipient._t(' is an invalid email address'));
			$email_error = true;
		}
	}
	if($email_error)
		return false;
	return true;
}

/**
 * Split contact list passed into categories
 * @param mixed_array $contact_array array of mixed contacts
 * @return array of email, sms, group in order list
 */
function shn_msg_get_categories_from_contact($contact_array)
{
	global $global;
	
	$email_array = array();
	$sms_array = array();
	//$group_contact_array = array();
	
	$email_pattern = '/@/';
	
	foreach($contact_array as $contacts){
		$contacts = trim($contacts);
		
		if(preg_match($email_pattern,$contacts)){
			array_push($email_array,$contacts);
		}
		//Improve this later on - for now, get something working
		//Identify phone numbers
		//Handle + sign as well at the beginning of a number
		//@todo Handle other chars as well...
		else if(substr_count($contacts,'+')>0){
			// + Available - now check whether it is at the beginning
			if(strpos($contacts,'+')==0){
				//Yes! now to see whether the rest is a number
				$number = substr($contacts,1,strlen($contacts));
				if(is_numeric($number)){
					array_push($sms_array,$contacts);
				}
			}
		}
		else if(is_numeric($contacts)){
			array_push($sms_array,$contacts);
		}
		else{
			//array_push($group_contact_array,$contacts);
			//@todo: filter group
			list($emails,$smss) = _shn_msg_get_categories_from_group($contacts);
			foreach ($emails as $email){
				array_push($email_array,$email);
			}
			foreach ($smss as $sms){
				array_push($sms_array,$sms);
			}
			
		}
		
	}
	return array($email_array,$sms_array);
}


function _shn_msg_get_categories_from_group($group)
{
	global $global;
	$db = $global['db'];
	
	//$uuid = $group;
	$uuid = _shn_msg_get_guid_from_group($group);
	
	if(!$uuid)
		return false;
	
	$group_sms_array = array();
	$group_email_array = array();
	
	$sql = "SELECT primary_method,primary_mobile,secondary_mobile,primary_email,secondary_email,grp_name " .
		    " FROM msg_people_contact,msg_people_group where msg_people_contact.m_uuid=msg_people_group.m_uuid " .
		    " AND msg_people_group.m_uuid='{$uuid}'";
	$sql = "SELECT primary_method,primary_mobile,secondary_mobile,primary_email,secondary_email " .
			" FROM msg_people_contact as mp,msg_group_membership as mg where mp.m_uuid=mg.person_id " .
			" AND mg.group_id='{$uuid}'";
	$result = $db->Execute($sql);
	while(!$result->EOF){
		switch($result->fields['primary_method']){
			case 'mobile':
						 array_push($group_sms_array,$result->fields['primary_mobile']);
						 //Handle secondary number
						 if($result->fields['secondary_mobile']!=0){
						 	array_push($group_sms_array,$result->fields['secondary_mobile']);
						 }
						 break;
			case 'email':
						array_push($group_email_array,$result->fields['primary_email']);
						 //Handle secondary email
						 if($result->fields['secondary_email']!=''){
						 	array_push($group_sms_array,$result->fields['secondary_email']);
						 }
						 break;
			default:
						 //Handle all methods
						 array_push($group_sms_array,$result->fields['primary_mobile']);
						 //Handle secondary number
						 if($result->fields['primary_mobile']!=0){
						 	array_push($group_sms_array,$result->fields['secondary_mobile']);
						 }
						 array_push($group_email_array,$result->fields['primary_email']);
						 //Handle secondary email
						 if($result->fields['primary_email']!=''){
						 	array_push($group_sms_array,$result->fields['secondary_email']);
						 }
						 break;
		}
		$result->MoveNext();
	}
	return array ($group_email_array,$group_sms_array);
}

function _shn_msg_get_guid_from_group($group_entry)
{
	/*
	$pattern = '^\<.+\:';
	$pattern = '.+';
	if(ereg($pattern,$group_entry,$reg))
		return $reg[0];
	else
		return false;
	*/
	$pattern = '/\{/';
	$guid = split(":",$group_entry);
	//print_r ($guid);
	//@Commented as it was probably for debugging.
	$val = preg_replace($pattern,'',$guid[0]);
	//echo "GID:".$val;
	return $val;
	
}

?>
