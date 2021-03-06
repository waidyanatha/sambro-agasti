<?php
/**
 * This library will help developers to use xajax 
 * 
 * @package    framework
 * @author J P Fonseka <jo@respere.com>
 * @copyright  Respere http://www.respere.com
 * @license    http://www.gnu.org/copyleft/lesser.html GNU Lesser General Public License (LGPL)
 */

//include the xajax library
require_once($global['approot']."/3rd/xajax/xajax.inc.php");

//set the xajx object which developer should use in there code
global $global;
$global['xajax'] = new xajax('index.php?stream=ajax&mod=xst&act=xajax','');

/**
 * these functions are just to make it convienent for the developers
 */
function shn_xajax_registerFunction($name){
    global $global;
    $global['xajax']->registerFunction($name);
	shn_xajax_printJavascript();
}

/**
 * print javascript function this will be called in html stream library if xajax 
 * object exist in global variable
 */
function shn_xajax_printJavascript(){
    global $global;
    //set the default uri
    $sRequestURI='index.php?stream=ajax&mod=xst&act=xajax';
    $global['xajax']->setRequestURI($sRequestURI);
    $global['xajax']->printJavascript("res/js");
//java script to xajax loading message
?>
<script type="text/javascript">
<!--
            xajax.loadingFunction = 
                function(){xajax.$('loadingMessage').style.display='block';alert('start');};
            function hideLoadingMessage()
            {
                xajax.$('loadingMessage').style.display = 'none';alert('stop');
            }
            xajax.doneLoadingFunction = hideLoadingMessage;
// --></script>
<div id="loadingMessage" style="display: none;">
    Loading...
</div>
<?php
}

?>