<?php
/*
window.php - JavaScript window support

Copyright (c) 2006 NoisyB


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


/*
  PHP

  js_window_init()


  JavaScript

  js_window_open()
  js_window_close()
  js_window_resize()
*/
function
js_window_init ()
{
?><script type="text/javascript"><!--


function
js_window_open (url, title, args)
{
  window.open (url, title, args);
}


function
js_window_close ()
{
  window.close ();
}


function
js_window_resize (w, h)
{
  window.resizeBy (w + 30 - window.innerWidth, h + 10 - window.innerHeight + 20); // +20 because of the tooltips
}


--></script><?php
}


/*
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

  Width of window
    self.innerWidth;  // ns4
    window.innerWidth - 5;  // ns6
    document.body.clientWidth; // ie

  Height of window
    self.innerHeight;  // ns4
    window.innerHeight - 5;  // ns6
    document.body.clientHeight; // ie
*/



?>