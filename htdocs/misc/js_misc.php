<?php
/*
widget_js.php - JavaScript widget functions

Copyright (c) 2006 - 2007 NoisyB


This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
require_once ("js_window.php");
require_once ("js_mouse.php");
require_once ("js_print.php");
require_once ("js_panel.php");


/*
  set focus on a form tag
    document.<formname>.<widgetname>.focus();

  close active window
    window.close();

  y/n question
    if (confirm (question))
      ...;

  open url
    location.href = url;
    window.location = url;

  open url in frame
    top[<framename>].location.href = url;

  window.open (url, windowname, arg, ...)
    can be used in onclick="new_window ()" or onload="new_window ()" or as url "javascript:new_window ()"

  args
    screenX=pixels      position of the window in pixels from the left of the screen in Netscape 4+
    screenY=pixels      position of the window in pixels from the top of the screen in Netscape 4+
    left=pixels         position of the window in pixels from the left of the screen in IE 4+
    top=pixels          position of the window in pixels from the top of the screen in IE 4+
    width=pixels        defines the width of the new window.
    height=pixels       defines the height of the new window.

    resizable=yes/no    whether or not you want the user to be able to resize the window.
    scrollbars=yes/no   whether or not to have scrollbars on the window
    toolbar=yes/no      whether or not the new window should have the browser navigation bar at the top
    location=yes/no     whether or not you wish to show the location box with the current url
    directories=yes/no  whether or not the window should show the extra buttons
    status=yes/no       whether or not to show the window status bar at the bottom of the window
    menubar=yes/no      whether or not to show the menus at the top of the window
    copyhistory=yes/no  whether or not to copy the old browser window's history list to the new window

  Width of the document
    document.width

  Height of the document
    document.height

  Width of window
    self.innerWidth;  // ns4
    window.innerWidth - 5;  // ns6
    document.body.clientWidth; // ie

  Height of window
    self.innerHeight;  // ns4
    window.innerHeight - 5;  // ns6
    document.body.clientHeight; // ie

  Popup text at fixed pos
    <div id="text" name="text" style="position:absolute; left:166px; top:527px; width:665px; height:94px; z-index:1"></div>
    function output (s)
      {
        obj = eval("text");
        obj.innerHTML = s;
      }
    <... onMouseOver="output('hello')">
*/
define ("WIDGET_JS_MOUSE", 1);
define ("WIDGET_JS_PRINT", 2);
define ("WIDGET_JS_WINDOW", 4);
define ("WIDGET_JS_PANEL", 8);

define ("WIDGET_JS_ALL", WIDGET_JS_MOUSE|WIDGET_JS_PRINT|WIDGET_JS_WINDOW|WIDGET_JS_PANEL);


function
widget_js_init ($flags)
{
?><script type="text/javascript"><!--

<?php
/*
is_ns4 = 0;
is_ie = 0;
is_other = 1;
is_op5 = 0;
*/
  $agent = $_SERVER["HTTP_USER_AGENT"];

  echo "is_ns4 = "
      .(stristr ($agent, "Netscape") ? "1" : "0")
      .";\n";
  echo "is_ie = "
      .(stristr ($agent, "Microsoft") ? "1" : "0")
      .";\n";
  echo "is_other = (!is_ns4 && !is_ie);\n";
  echo "is_op5 = 0;\n";
    
?>


function
js_img_resize (img_name, w, h)
{
  img_name.width = w;
  img_name.height = h;
}


--></script><?php

  if ($flags & WIDGET_JS_MOUSE)
    js_mouse_init ();

  if ($flags & WIDGET_JS_WINDOW)
    js_window_init ();

  if ($flags & WIDGET_JS_PRINT)
    js_print_init (); 

// called by widget_panel()
//  if ($flags & WIDGET_JS_PANEL)
//    js_panel_init ($url_array, $img_array, $w, $h, $tooltip);
}





























/*
// CSS Browser Selector   v0.2.5
// Documentation:         http://rafael.adm.br/css_browser_selector
// License:               http://creativecommons.org/licenses/by/2.5/
// Author:                Rafael Lima (http://rafael.adm.br)
// Contributors:          http://rafael.adm.br/css_browser_selector#contributors
var css_browser_selector = function() {
	var 
		ua=navigator.userAgent.toLowerCase(),
		is=function(t){ return ua.indexOf(t) != -1; },
		h=document.getElementsByTagName('html')[0],
		b=(!(/opera|webtv/i.test(ua))&&/msie (\d)/.test(ua))?('ie ie'+RegExp.$1):is('gecko/')? 'gecko':is('opera/9')?'opera opera9':/opera (\d)/.test(ua)?'opera opera'+RegExp.$1:is('konqueror')?'konqueror':is('applewebkit/')?'webkit safari':is('mozilla/')?'gecko':'',
		os=(is('x11')||is('linux'))?' linux':is('mac')?' mac':is('win')?' win':'';
	var c=b+os+' js';
	h.className += h.className?' '+c:c;
}();
<script src="css_browser_selector.js" type="text/javascript"></script>

function
is_debug ()
{
  document.write ("<br>navigator.appVersion: "
                 +navigator.appVersion
                 +"<br>navigator.appName: "
                 +navigator.appName
                 +"<br>document.layers: "
                 +document.layers
                 +"<br>document.all: "
                 +document.all
                 +"<br>document.getElementById: "
                 +document.getElementById);
}


function
is_mac ()
{
  return navigator.appVersion.indexOf ("Mac") != -1;
}


function
is_ns4 ()
{
  if (document.layers)
    return 1;
  if (parseInt (navigator.appVersion) >= 4 &&
      navigator.appName.indexOf ("Netscape") != -1)
    return 1;
  return 0;
}


function
is_ie ()
{
  if (document.all)
    return 1;
  if (parseInt (navigator.appVersion) >= 4 &&
      navigator.appName.indexOf ("Microsoft") != -1)
    return 1;
  return 0;
}


function
is_ns6 ()
{
  if (document.getElementById && !document.all)  // ns6
    return 1;
  if (parseInt (navigator.appVersion) >= 6 &&
      navigator.appName.indexOf ("Netscape") != -1)
    return 1;
  return 0;
}


function
is_moz ()
{
  if (document.getElementById && !document.all)  // ns6
    return 1;
  if (parseInt (navigator.appVersion) >= 6 &&
      navigator.appName.indexOf ("Mozilla") != -1)
    return 1;
  return 0;
}

function
is_op5 ()
{
  return 0;
}


function
is_other ()
{
  if (is_ns4 () || is_ie () || is_ns6 () || is_moz () || is_op5 ())
    return 0;
  return 1;
}


function
js_element_get_w (Elem)
{
  if (is_ns4)
    {
      var elem = getObjNN4(document, Elem);
      return elem.clip.width;
    }

  if (is_other)
    var elem = document.getElementById(Elem);
  else if (is_ie)
    var elem = document.all[Elem];

  if (is_op5)
    return elem.style.pixelWidth;

  return elem.offsetWidth;
}


function
js_element_get_h (Elem)
{
  if (is_ns4)
    {
      var elem = getObjNN4(document, Elem);
      return elem.clip.height;
    }

  if (is_other)
    var elem = document.getElementById(Elem);
  else if (is_ie)
    var elem = document.all[Elem];

  if (is_op5)
    return elem.style.pixelHeight;

  return elem.offsetHeight;
}


function
js_img_get_w (img_name)
{
  if (is_ns4)
    {
      var img = getImage (img_name);
      return img.width;
    }

  return js_element_get_w (img_name);
}


function
js_img_get_h (img_name)
{
  if (is_ns4)
    {
      var img = getImage (img_name);
      return img.height;
    }

  return js_element_get_h (img_name);
}
*/






















/*
var obj_xpos = new Array ();
var obj_ypos = new Array ();
var obj_rad = new Array ();
var obj_angle = new Array ();

obj_xpos[0] = (get_width() - 418) / 2;
obj_ypos[0] = 40;
obj_rad[0] = 600;
obj_angle[0] = 270;

function
anim ()
{
  var id = 0;

  if (obj_rad[id] > 20)
    obj_rad[id] -= (2 + Math.sqrt (obj_rad[id] / 100));

  obj_angle[id] -= 6 + (obj_rad[id] / 100);
  obj_angle[id] %= 360;

  move (id,
        obj_xpos[id] + obj_rad[id] * Math.cos (obj_angle[id] * 0.017453293),
        obj_ypos[id] + obj_rad[id] * Math.sin (obj_angle[id] * 0.017453293));

  setTimeout ("anim()", 20);
}

function
index_anim()
{
document.write ("<br><br><br><br><br><br><br><br>");
pos (0, "<img src=\"images/logo_large.png\" width=\"418\" height=\"121\" border=\"0\">", (get_width() - 418) / 2, 40);
anim ();
//followmouse (1, 0,-30);
}
*/







?>