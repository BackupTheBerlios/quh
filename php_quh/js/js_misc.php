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
?>