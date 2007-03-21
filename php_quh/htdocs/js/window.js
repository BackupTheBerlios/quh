/*
window.js - JavaScript window support

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
  js_window_open()
  js_window_close()
  js_window_resize()

  <body oncontextmenu="return false;">
    disables right click
*/
function
js_window_open (url, title, args)
{
  var win = window.open (url, title, args);
/*
  if (js_version > 1.0)
    {
      if (is_ie)
        setTimeout ('win.focus();', 250);
      else
        win.focus ();
    }

  return win;
*/
}


function
js_window_close (win)
{
  if (win)
    win.close ();
  else 
    window.close ();
}


function
js_window_resize (win, w, h)
{
  if (win)
    win.resizeBy (w + 30 - window.innerWidth, h + 10 - window.innerHeight + 20);
  else
    window.resizeBy (w + 30 - window.innerWidth, h + 10 - window.innerHeight + 20); // +20 because of the tooltips
}
