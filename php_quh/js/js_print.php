<?php
/*
print.php - JavaScript support

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


/*
  PHP

  js_print_init() 


  JavaScript

  js_print_xy()
    print (html) string at x- and ypos
  js_print_mouse_cb()
    callback function for use with js_mouse_callback_func()
    object from js_print_xy() will follow the mouse afterwards

TODO: deprecate js_print_mouse()
*/
function
js_print_init ()
{
?><script type="text/javascript"><!--


function
js_print_xy (id, string, xpos, ypos)
{
  if (is_ns4)
    document.write ("<layer name=\""
                   +id
                   +"\" left=\"1\" top=\"1\" visibility=\"show\" z-index=\""
                   +id
                   +">"
                   +object
                   +"</layer>");
  else
    document.write ("<div id=\""
                   +id
                   +"\" style=\"position:absolute; z-index:"
                   +id
                   +" visibility:visible; top:1px; left:1px; width:1;\">"
                   +string
                   +"</div>");

  if (is_ns4)
    {
      document.layers[id].left = xpos;
      document.layers[id].top = ypos;
    }
  else if (is_ie)
    {
      document.all[id].style.pixelLeft = xpos;
      document.all[id].style.pixelTop = ypos;
    }
  else
    {
      document.getElementById(id).style.left = xpos;
      document.getElementById(id).style.top = ypos;
    }
}


function
js_print_mouse_cb (e)
{
//  if (is_ns4)
    {
//      document.layers[0].left = window.event.x + document.body.scrollLeft;
//      document.layers[0].top = window.event.y + document.body.scrollTop;
    }
//  else
    {
      document.getElementById(0).style.left = e.pageX;
      document.getElementById(0).style.top = e.pageY;
    }
}


<?php
/*
function
js_print_mouse (string)
{
  js_print_xy (0, string, 0, 0);
  js_mouse_callback_func (js_print_mouse_cb);
}
*/
?>
--></script><?php
}


?>