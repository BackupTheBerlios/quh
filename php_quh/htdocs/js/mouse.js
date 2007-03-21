/*
mouse.js - JavaScript mouse support

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
TODO: merge into event.js

  js_mouse_callback_func()
    func will receive mouse events
  js_mouse_get_xpos()
  js_mouse_get_ypos()
  js_mouse_a()
    link with mouse sensitive pictures
    name == needed identifier; must be unique
*/


function
js_mouse_callback_func (func)
{
  if (is_ns4)
    {
      document.captureEvents (Event.MOUSEMOVE);
      document.onmousemove = func;
    }
  else if (is_ie)
    document.onmousemove = func;
  else
    document.onmousemove = func;
}


function
js_mouse_get_xpos (e)
{
  if (!is_ie)
    var xmax = window.innerWidth + window.pageXOffset;

  if (is_ns4)
//    return Math.min (e.pageX, Xmax);
    return e.pageX;
  else if (is_ie)
    return window.event.x + document.body.scrollLeft;

//  return Math.min (e.pageX, Xmax);
  return e.pageX;
}


function
js_mouse_get_ypos (e)
{
  if (!is_ie)
    var ymax = window.innerHeight + window.pageYOffset;

  if (is_ns4)
//    return Math.min (e.pageY, ymax);
    return e.pageY;
  else if (is_ie)
    return window.event.y + document.body.scrollTop;

//  return Math.min (e.pageY, ymax);
  return e.pageY;
}

