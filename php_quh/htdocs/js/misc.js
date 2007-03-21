/*
misc.js - JavaScript misc support

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
  set focus on a form tag
    document.<formname>.<widgetname>.focus();

  close active window
    window.close();

  y/n question
    if (confirm (question))
      ...;

  status line
    window.status = status;

  window title
    document.title = title;

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
    fullscreen=yes/no   whether or not the window should have fullscreen size

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

  disables right click menu
    oncontextmenu="return false;"

  resize images
    <img src=... name="image_xyz">
    image_xyz.width = w;
    image_xyz.height = h;

  play sound (onMouseOver)
    <a href="#" onMouseOver="document.all.msound.src='sound.aif'">...</a>

  delay
    function doSomething() {}
    [window.]setTimeout("doSomething();", delay);
    doSomething(); // leave here to run right away

  bookmark
    function bookmark ()
      {
        if (document.all)
          window.external.AddFavorite (url, title);
      }
    <a href="javascript:bookmark()">...</a>

  screen width and height
    screen.width
    screen.height

  changes the image size and source on your thumbnail picture
    <img src="yourimage.jpg" width="150" height="200"
     onclick="this.src='yourimage.jpg';this.height=400;this.width=300"
     ondblclick="this.src=yourimage.jpg';this.height=200;this.width=150">
*/


/*
Accessing Values

Having read the Objects and Properties page, you should now know how to find
out the values of form elements through the DOM. We're going to be using the
name notation instead of using numbered indexes to access the elements, so
that you are free to move around the fields on your page without having to
rewrite parts of your script every time. A sample, and simple, form may look
like this:

<form name="feedback" action="script.cgi" method="post" onSubmit="return checkform()">
<input type="text" name="name">
<input type="text" name="email">
<textarea name="comments"></textarea>
</form>

Validating this form would be considerably simpler than one containing radio
buttons or select boxes, but any form element can be accessed. Below are the
ways to get the value from all types of form elements. In all cases, the
form is called feedback and the element is called field. Text Boxes,
<textarea>s and hiddens

These are the easiest elements to access. The code is simply

document.feedback.field.value

You'll usually be checking if this value is empty, i.e.

if (document.feedback.field.value == '') {
	return false;
}

That's checking the value's equality with a null String (two single quotes
with nothing between them). When you are asking a reader for their email
address, you can use a simple » address validation function to make sure the
address has a valid structure. Select Boxes

Select boxes are a little trickier. Each option in a drop-down box is
indexed in the array options[], starting as always with 0. You then get the
value of the element at this index. It's like this:

document.feedback.field.options
[document.feedback.field.selectedIndex].value

You can also change the selected index through JavaScript. To set it to the first option, execute this:

document.feedback.field.selectedIndex = 0;

Check Boxes

Checkboxes behave differently to other elements  their value is always on.
Instead, you have to check if their Boolean checked value is true or, in
this case, false.

if (!document.feedback.field.checked) {
	// box is not checked
	return false;
}

Naturally, to check a box, do this

document.feedback.field.checked = true;

Radio Buttons

Annoyingly, there is no simple way to check which radio button out of a
group is selected you have to check through each element, linked with
Boolean AND operators . Usually you'll just want to check if none of them
have been selected, as in this example:

if (!document.feedback.field[0].checked &&
!document.feedback.field[1].checked &&
!document.feedback.field[2].checked) {
	// no radio button is selected
	return false;
}

You can check a radio button in the same way as a checkbox.
*/


function
js_mouse_a (url, name, img1, img2)
{
  document.write ("<a href=\""
                 +url
                 +"\" onmouseover=\"document."
                 +name
                 +".src='"
                 +img1
                 +"'\""
                 +" onmouseout=\"document."
                 +name
                 +".src='"
                 +img2
                 +"'\">"
                 +"<img src=\""
                 +img1
                 +"\" border=\"0\" name=\""
                 +name
                 +"\"></a>");
}


/*
  js_print_xy()
    print (html) string at x- and ypos
  js_print_mouse_cb()
    callback function for use with js_mouse_callback_func()
    object from js_print_xy() will follow the mouse afterwards

TODO: deprecate js_print_mouse()
*/
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


/*
function
js_print_mouse (string)
{
  js_print_xy (0, string, 0, 0);
  js_mouse_callback_func (js_print_mouse_cb);
}
*/
