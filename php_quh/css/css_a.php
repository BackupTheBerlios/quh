<?php
/*
css_a.php - CSS code for A widget

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
  css_a_init()
*/
function
css_a_init ()
{
?><style type="text/css" media="all">
<!--

a
{
  text-decoration:none;
}


a:hover
{
  color:#fff;
  background-color:#00f;
//  color:WindowText;
//  background-color:Window;
  text-decoration:none;
}


.widget_a
{
  position: relative;
  z-index:0;
  text-decoration:none;
}


.widget_a:hover
{
  z-index:1;
}


.widget_a span
{
  display:none;
}


.widget_a:hover span
{
  font:12px sans-serif,arial;
  position:absolute;
  top:30px;
  left:10px;
  padding:2px;
  display:block;
//  white-space:nowrap;
  color:InfoText;
  background-color:InfoBackground;
  border:1px solid #000;
  text-align:left;
  text-decoration:none;
}

-->
</style><?php
}


?>