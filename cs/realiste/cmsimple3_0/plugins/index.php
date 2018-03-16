<?php
/*
CMSimple Plugin Loader
CMSimple version 3.0 - October 23. 2007

© Peter Andreas Harteg. Based on original script by © Michael Svarrer.
Distributed under the CMSimple license with permission by copyright holder.
For licence, see notice in /cmsimple/cms.php and http://www.cmsimple.dk/?Licence

Usage:

This plugin loader will load compatible plugins avaliable under the
plugins folder into CMSimple:

Step 0 - In /cmsimple/config.php set $cf['plugins']['folder']="plugins"; (or set plugins_folder to plugins under CMSimple configuration settings)
Step 1 - Installation is done and you should be ready to install plugins

For more information, see http://www.cmsimple.dk/?Addons_and_plugins
*/

if (!isset($sn)){die('access denied'); }

// Plugin loader language backward compatibility
// This is to be deleted when $tx['filetype'] & $tx['plugins']-variables are set in major language files
if(!isset($tx['plugins']['plugin']))$tx['plugins']['plugin'] = 'plugin';
if(!isset($tx['plugins']['select_plugin']))$tx['plugins']['select_plugin'] = 'Select plugin ...';
if(!isset($tx['filetype']['plugin_main']))$tx['filetype']['plugin_main'] =
'Main settings';
if(!isset($tx['filetype']['plugin_stylesheet']))$tx['filetype']['plugin_stylesheet'] =
$tx['filetype']['stylesheet'];
if(!isset($tx['filetype']['plugin_config']))$tx['filetype']['plugin_config'] = $tx['filetype']['config'];
if(!isset($tx['filetype']['plugin_language']))$tx['filetype']['plugin_language'] = $tx['filetype']['language'];
if(!isset($tx['plugins']['menu_main']))$tx['plugins']['menu_main'] = ucfirst(strtolower($tx['filetype']['plugin_main']));
if(!isset($tx['plugins']['menu_css']))$tx['plugins']['menu_css'] = ucfirst(strtolower($tx['filetype']['plugin_stylesheet']));
if(!isset($tx['plugins']['menu_config']))$tx['plugins']['menu_config'] = ucfirst(strtolower($tx['filetype']['plugin_config']));
if(!isset($tx['plugins']['menu_language']))$tx['plugins']['menu_language'] = ucfirst(strtolower($tx['filetype']['plugin_language']));
if(!isset($tx['plugins']['menu_help']))$tx['plugins']['menu_help'] = ucfirst(strtolower($tx['editmenu']['help']));

// Load plugins
$pia = array();
if ($fh = opendir($pth['folder']['plugins'])) {
while (false !== ($fd = readdir($fh))) {
if (is_dir($pth['folder']['plugins'].$fd)&&$fd!="."&& $fd!="..")$pia[] = $fd;
}  closedir($fh); }

if(count($pia)>0){

foreach($pia as $pi){
PluginFiles($pi);
if (!@include($pth['file']['plugin_index']))die($tx['heading']['error'].' '.$tx['error']['cntopen'].' '.$pth['file']['plugin_index']);
@include($pth['file']['plugin_language']);
@include($pth['file']['plugin_config']);
if (file_exists($pth['file']['plugin_stylesheet']))$hjs .= tag('link rel="stylesheet" href="'.$pth['file']['plugin_stylesheet'].'" type="text/css"');
}

if ($adm){
foreach($pia as $pi){
PluginFiles($pi);
@include($pth['file']['plugin_admin']);
}
$o=plugin_select().$o;
PluginFiles($plugin);
}}

// Plugin functions

function plugin_lang($i){
global $pth;
foreach($i as $j)if(file_exists($pth['folder']['plugin'].$j))return $pth['folder']['plugin'].$j;
}

function PluginFiles($plugin) {
global $cf, $pth, $sl, $adm;
$pth['folder']['plugin'] = $pth['folder']['plugins'].$plugin.'/';
$pth['file']['plugin_index'] = $pth['folder']['plugin'].'index.php';
$pth['file']['plugin_admin'] = $pth['folder']['plugin'].'admin.php';
$pth['file']['plugin_config'] = $pth['folder']['plugin'].'config/config.php';
$pth['file']['plugin_stylesheet'] = $pth['folder']['plugin'].'css/stylesheet.css';
$pth['file']['plugin_language']=plugin_lang(array('languages/'.$sl.'.php','languages/'.$cf['language']['default'].'.php',$pth['folder']['plugin'].'languages/en.php'));
if ($adm) $pth['file']['plugin_help']=plugin_lang(array('help/help_'.$sl.'.htm','help/help_'.$cf['language']['default'].'.htm','help/help_en.htm','help/help.htm'));
}

// Menu - called by plugin loader
function plugin_select() {
global $sn,$plugin,$pth,$tx,$pia;
$t ='<table class="edit" width="100%" cellpadding="1" cellspacing="0" border="1"><tr><td><form>'.ucfirst(strtolower($tx['plugins']['plugin'])).': <select name="Plugins" onchange="location.href=this.options[this.selectedIndex].value" ><option value="?'.amp().'normal">'.$tx['plugins']['select_plugin'].'</option>';
foreach($pia as $pi){
if (file_exists($pth['file']['plugin_admin']))$t .= '<option value="'.$sn.'?&amp;'.$pi.amp().'normal"'.(($plugin == $pi)?' selected="selected"':'').'>'.ucfirst($pi).'</option>';	}
return $t.'</select></form></td></tr></table>';
}

// Submenu - called by plugin
function print_plugin_admin($m) {
global $sn, $plugin, $pth, $tx, $pia;
return '<table class="edit" width="100%" cellpadding="1" cellspacing="0" border="1">'.($m=='on'?'<tr><td><a href="'.$sn.'?'.amp().$plugin.amp().'admin=plugin_main'.amp().'action=edit">'.$tx['plugins']['menu_main'].'</a></td>':'').
(file_exists($pth['file']['plugin_stylesheet'])?'<td><a href="'.$sn.'?'.amp().$plugin.amp().'admin=plugin_stylesheet'.amp().'action=edit">'.$tx['plugins']['menu_css'].'</a></td>':'').
(file_exists($pth['file']['plugin_config'])?'<td><a href="'.$sn.'?'.amp().$plugin.amp().'admin=plugin_config'.amp().'action=array">'.$tx['plugins']['menu_config'].'</a></td>':'').
(file_exists($pth['file']['plugin_language'])?'<td><a href="'.$sn.'?'.amp().$plugin.amp().'admin=plugin_language'.amp().'action=array">'.$tx['plugins']['menu_language'].'</a></td>':'').
(file_exists($pth['file']['plugin_help'])?'<td><a href="'.$pth['file']['plugin_help'].'" target="_blank">'.$tx['plugins']['menu_help'].'</a></td>':'').'</tr></table>';
}

function plugin_admin_common($action, $admin, $plugin) {
global $file,$action;
if($action=='plugin_edit')$action='array';
if($action=='plugin_text')$action='edit';
if($action=='plugin_save'||$action=='plugin_textsave')$action='save';
$file=$admin;
}
?>