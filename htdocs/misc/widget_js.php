<?php
/*
js.php - JavaScript wrappers

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


function
js_init ()
{
?><script type="text/javascript">
<!--


function
js_widget_focus ()
{
  document.input.answer.focus();
}


function
//js_window_open (url, title, w, h, resizable, scrollbars, toolbar, location, directories, status, menubar, copyhistory)
js_window_open ()
{
//  window.open (url,title,'width='
//                        +w
//                        +',height='
//                        +h
//                        +',resizable='
//                        +resizable
 
  window.open('guicon64.php','mywindow','width=400,height=450,resizable=no,scrollbars=yes,toolbar=no,location=no,directories=no,status=no,menubar=no,copyhistory=no');
}


function
js_window_close ()
{
  window.close ();
}


function
js_window_resize (w, h)
{
  var width = w != -1 ? w : document.width;
  var height = h != -1 ? h : document.height;

  var contentWidth = width + 30;
  var contentHeight = height + 10;

  var displayAreaHeight = window.innerHeight;
  var displayAreaWidth = window.innerWidth;

  var deltaHeight = contentHeight - displayAreaHeight;
  var deltaWidth = contentWidth - displayAreaWidth;

  window.resizeBy (deltaWidth, deltaHeight + 20); // +20 because of the tooltips
}


-->
</script><?php
}


/*
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

  can be used in onclick="new_window ()" or onload="new_window ()" or as url "javascript:new_window ()"
*/
/*
function
js_window ($url, $title, $w, $h, $resizable, $scrollbars, $toolbar, $location, $directories, $status, $menubar, $copyhistory)
{
}
*/






/*
function
yesno (question, url)
{
  if (confirm (question))
    window.location = url;
}


function
openurl (frame,url)
{
  if (frame != \"\")
    top[frame].location.href = cmd;
  else
    location.href = cmd;
}


const char *
ng_httpd_js_openurl_onclick (const char *frame, const char *url)
{
  static char buf[MAXBUFSIZE];

  sprintf (buf, "openurl('%s', '%s');", frame, url);

  return (const char *) &buf;
}


  function pos (id, object, xpos, ypos)
    {
      if (object)
        {
          switch (version)
            {
              case 4:
                document.write (\"<layer name=\"\" + id
                                +
                            \"\" left=\"1\" top=\"1\" visibility=\"show\" z-index=\"\"
                            + id + \"\">\" + object + \"</layer>\");
            break;
          default:\"
            document.write (\"<div id=\"\" + id
                            + \"\" style=\"POSITION: absolute; Z-INDEX: \" + id
                            +
                            \"VISIBILITY: visible; TOP: 1px; LEFT: 1px; width:1;\">\"
                            + object + \"</div>\");
            break;
          }
      }
    switch (version)
      {
      case 4:
        document.layers[id].left = xpos;
        document.layers[id].top = ypos;
        break;
      default:
        document.getElementById(id).style.left = xpos;
        document.getElementById(id).style.top = ypos;
        break;
      }
  }",



function
pos_ie (id, object, xpos, ypos)
{
  if (object)
    {
          document.write (\"<div id=\"\" + id
                          + \"\" style=\"POSITION: absolute; Z-INDEX: \" + id
                          +
                          \"VISIBILITY: visible; TOP: 1px; LEFT: 1px; width:1;\">\"
                          + object + \"</div>\");
    }
  document.all[id].style.pixelLeft = xpos;
  document.all[id].style.pixelTop = ypos;
}


  function
  move (id, xpos, ypos)
  {
    pos (id, 0, xpos, ypos);
  }
  var fm_id = 0;
  var fm_x = 0;
  var fm_y = 0;
  function
  processEvent (e)
  {
    move (fm_id, e.pageX + fm_x, e.pageY + fm_y);
  }",





  function
  move_ie (id, xpos, ypos)
  {
    pos (id, 0, xpos, ypos);
  }
  



  var followmouse_id = -1;

  function
  processEvent (e)
  {
    var id = followmouse_id;
    switch (version)
      {
      case 4:
        move (id, window.event.x + document.body.scrollLeft,
              window.event.y + document.body.scrollTop);
        break;
      default:
        move (id, e.pageX, e.pageY);
        break;
      }
  }


  function
  followmouse (id, x, y)
  {
    fm_id = id;
    fm_x = x;
    fm_y = y;
    document.captureEvents (Event.MOUSEMOVE);
    document.onmousemove = processEvent;
  }


  function
  followmouse_ie (id)
  {
    followmouse_id = id;
    document.captureEvents (Event.MOUSEMOVE);
    document.onmousemove = processEvent;
  }
*/


/*
const char *ng_httpd_js_resize_body_onload[] = {
  "resize();",
};

const char *
ng_httpd_js_new_window_body_onload (const char *url, const char *title)
{
  static char buf[MAXBUFSIZE];

  sprintf (buf, "new_window(%s, %s);", url, title);

  return (const char *) &buf;
}


const char *
ng_httpd_js_yesno_body_onload (const char *question, const char *url)
{
  static char buf[MAXBUFSIZE];

  sprintf (buf, "yesno('%s', '%s');", question, url);

  return (const char *) &buf;
}


ng_httpd_js_openurl_onclick (const char *frame, const char *url)
{
  static char buf[MAXBUFSIZE];

  sprintf (buf, "openurl('%s', '%s');", frame, url);

  return (const char *) &buf;
}
*/
