<?php
/* $Id: lib_stream_html.inc,v 1.16 2008/07/20 16:27:47 priyanga Exp $ */

/**
 *
 * <Description goes here>
 *
 * PHP version 4 and 5
 * LICENSE: This source file is subject to LGPL license
 * that is available through the world-wide-web at the following URI:
 * http://www.gnu.org/copyleft/lesser.html
 *
 * @package    framework
 * @subpackage stream_html
 * @author     Chamindra de Silva <chamindra@opensource.lk>
 * @author     Janaka Wickramasinghe <janaka@opensource.lk>
 * @copyright  Lanka Software Foundation - http://www.opensource.lk
 * @license    http://www.gnu.org/copyleft/lesser.html GNU Lesser General Public License (LGPL)
 *
 */
function shn_stream_init()
{
	global $global;
	global $conf;
	require_once ($global['approot'].'inc/lib_security/lib_acl.inc');

	$module = $global['module'];

	// include the html head tags
	shn_include_page_section('html_head',$module);

	// Start the body and the CSS container element
	?>
	<?php if(isset($_SESSION['admin']['locale']['enable']) && $_SESSION['admin']['locale']['enable']=="set"){ ?>
<body onclick="whichElement(event)" oncontextmenu="return false;">
	<?php }else{?>
<body>
	<?php }?>
<div id="container"  class="clearfix">
<div>
<img class="form_exit"   id="popup_exit" width="100%" src="theme/default/img/banner.jpg" alt="" />
</div>

<div id="wrapper"  class="clearfix">
<div id="wrapper_menu">
<p id="skip">Jump to: <a href="#content"><?=_t('Content')?></a> | <a
	href="#modulemenu"><?=_t('Module Menu')?></a></p>
<?php

// hack for the admin section such that the mene and login come from
// the admin section
if (isset($global['effective_module']) && $global['effective_module'] != null) {
	$module = $global['effective_module'];
}

// include the mainmenu and login provided there is not a module override
shn_include_page_section('mainmenu',$module);
if(shn_acl_is_enabled()==true){
	shn_include_page_section('login',$module);
}


// display the breadcrumbs: It is important that this happens after the menu definition
// the nice name is constructed from the query string
$nicename = 'shn_q_'.$_SERVER['QUERY_STRING'].'_nicename';

if ( array_key_exists($nicename, $global ))
shn_breadcrumb_push($global[$nicename]);

shn_breadcrumb_display();
// now include the main content of the page
?></div>

<!-- Left hand side menus & login form -->
<div id="content"  >
<?php

ob_start(); // start output buffer for the module output
}

function shn_stream_close()
{
	global $global;

	// flush the output buffer
	$output = ob_get_contents();

	ob_end_clean();


	// check for exceptions
	$exception_list = $global['exception_list'];
	$result = null;
	$exception = null;
	$is_abort = false;
	//echo "<h1> Exception List </h1>";
	//var_dump($exception_list);

	if(isset($exception_list)){
		foreach($exception_list as $ex){
			$res = $ex->getResult();
			$exception = $ex;
			$result = $res;
			if(isset($res) && $res==REPORT_AND_ABORT){
				break;
			}
		}

		if($result == REPORT_AND_ABORT){
			add_error($exception->getMessage());
			$is_abort = true;
		}else if($result == REPORT_AND_CONTINUE){
			add_warning($exception->getMessage());
		}
	}
	global $conf,$global;
	shn_user_feedback();
	if(count($global['submit_errors'])>0 || count($global['submit_warnings'])>0){
		// show the user feedback dialog after a delay of 2 seconds.
		if($conf['shn_user_feedback_enabled']==true){
		?>
    <script type="text/javascript">
        window.setTimeout("_showFBInternal()",2000);
    </script>
    <?php
        }
	}
	// display the errors first before the rest of the module output
	display_errors();
	display_submit_message('confirmation','Submission Successful');
	display_submit_message('warning','Warning');
	if($is_abort == false){
		echo $output;
	}

	?></div>
<!-- /content --> 


<!-- /wrapper --></div></div>
<div id='temp_footer'></div>
<!-- /container -->
<script>
//setInterval("settime()", 1000);

function settime () {
  var curtime = new Date();
  var curhour = curtime.getHours();
  var curmin = curtime.getMinutes();
  var cursec = curtime.getSeconds();
  var time = "";

<!-- time-->
  if(curhour == 0) curhour = 12;
  time = (curhour) + ":" +
         (curmin < 10 ? "0" : "") + curmin + ":" +
         (cursec < 10 ? "0" : "") + cursec;
  document.getElementById('clock').innerHTML = time;
}

</script>
</body>
</html>
<?php
}
?>
