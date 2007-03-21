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
<BODY onLoad="scrollit(100)">
<SCRIPT>
<!--
function scrollit(seed) {
var m1 = "Hope you've enjoyed your stay here!";
var m2 = " JavaScript Kit. Your ultimate";
var m3 = " source for JavaScripts!";
var m4 = "";
var msg=m1+m2+m3+m4;
var out = " ";
var c = 1;
if (seed > 100) {
seed--;
cmd="scrollit("+seed+")";
timerTwo=window.setTimeout(cmd,100);
}
else if (seed <= 100 && seed > 0) {
for (c=0 ; c < seed ; c++) {
out+=" ";
}
out+=msg;
seed--;
window.status=out;
cmd="scrollit("+seed+")";
timerTwo=window.setTimeout(cmd,100);
}
else if (seed <= 0) {
if (-seed < msg.length) {
out+=msg.substring(-seed,msg.length);
seed--;
window.status=out;
cmd="scrollit("+seed+")";
timerTwo=window.setTimeout(cmd,100);
}
else {
window.status=" ";
timerTwo=window.setTimeout("scrollit(100)",75);
}
}


<form action="script.cgi" onSubmit="return js_misc_validate()">


function
js_misc_validate
{
  if
  {
    // something is wrong
    alert('There is a problem with the first field');
    return false; // if onSubmit gets false as return then no submit will be done
  }
  else if (value of next field is or isn't something)
  {
    // something else is wrong
    alert('There is a problem with...');
    return false;
  }
  // If the script gets this far through all of your fields
  // without problems, it's ok and you can submit the form

  return true;
}
*/

/*Accessing Values

Having read the Objects and Properties page, you should now know how to find out the values of form elements through the DOM. We're going to be using the name notation instead of using numbered indexes to access the elements, so that you are free to move around the fields on your page without having to rewrite parts of your script every time. A sample, and simple, form may look like this:

<form name="feedback" action="script.cgi" method="post" onSubmit="return checkform()">
<input type="text" name="name">
<input type="text" name="email">
<textarea name="comments"></textarea>
</form>

Validating this form would be considerably simpler than one containing radio buttons or select boxes, but any form element can be accessed. Below are the ways to get the value from all types of form elements. In all cases, the form is called feedback and the element is called field.
Text Boxes, <textarea>s and hiddens

These are the easiest elements to access. The code is simply

document.feedback.field.value

You'll usually be checking if this value is empty, i.e.

if (document.feedback.field.value == '') {
	return false;
}

That's checking the value's equality with a null String (two single quotes with nothing between them). When you are asking a reader for their email address, you can use a simple » address validation function to make sure the address has a valid structure.
Select Boxes

Select boxes are a little trickier. Each option in a drop-down box is indexed in the array options[], starting as always with 0. You then get the value of the element at this index. It's like this:

document.feedback.field.options
[document.feedback.field.selectedIndex].value

You can also change the selected index through JavaScript. To set it to the first option, execute this:

document.feedback.field.selectedIndex = 0;

Check Boxes

Checkboxes behave differently to other elements  their value is always on. Instead, you have to check if their Boolean checked value is true or, in this case, false.

if (!document.feedback.field.checked) {
	// box is not checked
	return false;
}

Naturally, to check a box, do this

document.feedback.field.checked = true;

Radio Buttons

Annoyingly, there is no simple way to check which radio button out of a group is selected  you have to check through each element, linked with Boolean AND operators . Usually you'll just want to check if none of them have been selected, as in this example:

if (!document.feedback.field[0].checked &&
!document.feedback.field[1].checked &&
!document.feedback.field[2].checked) {
	// no radio button is selected
	return false;
}

You can check a radio button in the same way as a checkbox.
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
*/
/*
function
js_img_resize (img_name, w, h)
{
  img_name.width = w;
  img_name.height = h;
}
*/
?>
<?php
/*
misc.php - JavaScript misc support

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

  js_misc_init() 


  JavaScript

  js_misc_validate()
    check submit for empty values
*/
function
js_misc_init ()
{
?><script type="text/javascript">
//<![CDATA[


/*
<BODY onLoad="scrollit(100)">
<SCRIPT>
<!--
function scrollit(seed) {
var m1 = "Hope you've enjoyed your stay here!";
var m2 = " JavaScript Kit. Your ultimate";
var m3 = " source for JavaScripts!";
var m4 = "";
var msg=m1+m2+m3+m4;
var out = " ";
var c = 1;
if (seed > 100) {
seed--;
cmd="scrollit("+seed+")";
timerTwo=window.setTimeout(cmd,100);
}
else if (seed <= 100 && seed > 0) {
for (c=0 ; c < seed ; c++) {
out+=" ";
}
out+=msg;
seed--;
window.status=out;
cmd="scrollit("+seed+")";
timerTwo=window.setTimeout(cmd,100);
}
else if (seed <= 0) {
if (-seed < msg.length) {
out+=msg.substring(-seed,msg.length);
seed--;
window.status=out;
cmd="scrollit("+seed+")";
timerTwo=window.setTimeout(cmd,100);
}
else {
window.status=" ";
timerTwo=window.setTimeout("scrollit(100)",75);
}
}
}
//]]>
</SCRIPT>



<form action="script.cgi" onSubmit="return js_misc_validate()">


function
js_misc_validate
{
  if
  {
    // something is wrong
    alert('There is a problem with the first field');
    return false; // if onSubmit gets false as return then no submit will be done
  }
  else if (value of next field is or isn't something)
  {
    // something else is wrong
    alert('There is a problem with...');
    return false;
  }
  // If the script gets this far through all of your fields
  // without problems, it's ok and you can submit the form

  return true;
}
*/

--></script><?php
}

/*Accessing Values

Having read the Objects and Properties page, you should now know how to find out the values of form elements through the DOM. We're going to be using the name notation instead of using numbered indexes to access the elements, so that you are free to move around the fields on your page without having to rewrite parts of your script every time. A sample, and simple, form may look like this:

<form name="feedback" action="script.cgi" method="post" onSubmit="return checkform()">
<input type="text" name="name">
<input type="text" name="email">
<textarea name="comments"></textarea>
</form>

Validating this form would be considerably simpler than one containing radio buttons or select boxes, but any form element can be accessed. Below are the ways to get the value from all types of form elements. In all cases, the form is called feedback and the element is called field.
Text Boxes, <textarea>s and hiddens

These are the easiest elements to access. The code is simply

document.feedback.field.value

You'll usually be checking if this value is empty, i.e.

if (document.feedback.field.value == '') {
	return false;
}

That's checking the value's equality with a null String (two single quotes with nothing between them). When you are asking a reader for their email address, you can use a simple » address validation function to make sure the address has a valid structure.
Select Boxes

Select boxes are a little trickier. Each option in a drop-down box is indexed in the array options[], starting as always with 0. You then get the value of the element at this index. It's like this:

document.feedback.field.options
[document.feedback.field.selectedIndex].value

You can also change the selected index through JavaScript. To set it to the first option, execute this:

document.feedback.field.selectedIndex = 0;

Check Boxes

Checkboxes behave differently to other elements  their value is always on. Instead, you have to check if their Boolean checked value is true or, in this case, false.

if (!document.feedback.field.checked) {
	// box is not checked
	return false;
}

Naturally, to check a box, do this

document.feedback.field.checked = true;

Radio Buttons

Annoyingly, there is no simple way to check which radio button out of a group is selected  you have to check through each element, linked with Boolean AND operators . Usually you'll just want to check if none of them have been selected, as in this example:

if (!document.feedback.field[0].checked &&
!document.feedback.field[1].checked &&
!document.feedback.field[2].checked) {
	// no radio button is selected
	return false;
}

You can check a radio button in the same way as a checkbox.
*/


















you can capture all the keys !!

Apr 21st, 2003 03:18
Shashank Tripathi, david koenig, http://sniptools.com/jskeys


Please try the following code. It works for capturing key presses on 
IE, Netscape, Opera 7, and somewhat quirkily with Opera 6 (numbers and 
uppercase letters should work). For this to work with Mozilla etc, 
perhaps a more DOM compliant code would be better (for some working 
code check out this site: http://sniptools.com/jskeys) 


CODE SAMPLE: 


<html><head>
<script language="JavaScript">
<!--
function keyHandler(e)
{
    var pressedKey;
    if (document.all)    { e = window.event; }
    if (document.layers) { pressedKey = e.which; }
    if (document.all)    { pressedKey = e.keyCode; }
    pressedCharacter = String.fromCharCode(pressedKey);
    alert(' Character = ' + pressedCharacter +
          ' [Decimal value = ' + pressedKey + ']');
}
document.onkeypress = keyHandler;
//-->
</script></head>
<body>Press any key on your computer now.</body></html>

















<script language="JavaScript">
<!--
/*
*/

var message="Hey YOU!\nStop Stealing my pictures or\nFACE THE PUNISHMENT!"



function click(e) {
if (document.all) {
if (event.button == 2) {
alert(message);
return false;
}
}
if (document.layers) {
if (e.which == 3) {
alert(message);
return false;
}
}
}
if (document.layers) {
document.captureEvents(Event.MOUSEDOWN);
}
document.onmousedown=click;
// -->

</script>
k22
















A Javascript Code - When you press a link, there is a sound~
sakiaqua
Oct 4 2004, 07:31 AM
	I find it quite funny and special so I share the code to everybody~
But maybe you all know already~^^

1.Put this between <head></head>
<bgsound src="#" id=msound loop=1 autostart="true">

2.Put this in <body>
<a href="#" onMouseOver="document.all.msound.src='do.aif'">Do</a>
<a href="#" onMouseOver="document.all.msound.src='re.aif'">Re</a>
<a href="#" onMouseOver="document.all.msound.src='mi.aif'">Mi</a>

Remember to change the link of the music~^^

Try it~^^~













QUOTE
Below is the code required to rotate the text used in the title bar and status bar


QUOTE
CODE
<script LANGUAGE='JavaScript'>
<!--

var default1 = "defautl title"; // only shown once at page load
var text1 = "title2";
var text2 = "title 3";
var text3 = "title 4";
var changeRate = 2000; // 1000 = 1 second
var messageNumber = 0;

function changeStatus() {
  if (messageNumber == 0) {
     window.status=default1;
     document.title=default1;
    }
  else if (messageNumber == 1) {
     window.status=text1;
     document.title=text1;
    }
  else if (messageNumber == 2) {
     window.status=text2;
     document.title=text2;
     }
  else if (messageNumber == 3) {
     window.status=text3;
     document.title=text3;
     messageNumber = 0;
     }

  messageNumber++;
  setTimeout("changeStatus();",changeRate);

}

changeStatus(); // leave here to run right away

// -->
</SCRIPT>













	

Let Visitors Bookmark Ur Site To Get Repeat Visitors! - easy code to let visitors add a bookmark to ur site..
shiv
Mar 21 2006, 12:25 AM
cheap web hostinglet ur visitors easily add a bookmark to your site. fully configurable, you can specify the url and name for the link.

Notice from szupie:
Source: http://www.itechies.net/tutorials/jscript/bookmark.htm


QUOTE
Step 1:
put the following script in the head of your page.place the code below between the and tags within the HTML page that you want them to bookmark.

<script LANGUAGE="JAVASCRIPT" TYPE="TEXT/JAVASCRIPT">
<!--

var url = "http://your url";
var title = "your title";

function makeLink(){
if(document.all)
window.external.AddFavorite(url,title)
}

// -->
</script>


Step 2:

put the following in the BODY of your page. place the code below between the <BODY> and BODY> tags within the HTML page that you would like to let them bookmark.

<A HREF="java script:makeLink()">Bookmark Us</A>













Hi, this simple one line script changes the image size and source on your thumbnail picture without reloading the page or using popups, just paste this code into the BODY section of your HTML document:
CODE
<img src="yourimage.jpg" width="150" height="200" onclick="this.src='yourimage.jpg';this.height=400;this.width=300" ondblclick="this.src=yourimage.jpg';this.height=200;this.width=150">










bildschirm breite und hoehe
screen.width
screen.height 




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

