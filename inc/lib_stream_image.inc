<?php
/* $Id: lib_stream_image.inc,v 1.4 2008/02/18 11:17:07 ravids Exp $ */

/**
 *
 * <Description goes here>
 *
 * PHP version 4 and 5
 * * LICENSE: This source file is subject to LGPL license
 * that is available through the world-wide-web at the following URI:
 * http://www.gnu.org/copyleft/lesser.html
 *
 * @package    framework
 * @subpackage stream_image 
 * @author     Janaka Wickramasinghe <janaka@opensource.lk>
 * @copyright  Lanka Software Foundation - http://www.opensource.lk
 * @license    http://www.gnu.org/copyleft/lesser.html GNU Lesser General Public License (LGPL)
 * 
 */
 
function shn_stream_init($filename=null)
{
    if($_GET['imagetype']){
    	header("Content-type: image/{$_GET['imagetype']}");
    } else{       
        header("Content-type: image/jpeg");
    }
    if(isset($_GET['download']))
        header("Content-Disposition: attachment; filename=$file");
}

function shn_stream_close($function,$output)
{
    echo $output;
}

function shn_image_check_perms(){
	return ALLOWED;//skip authentication
}

function _shn_image_display_errors(){
	// do nothing.
	return;
}
?>
