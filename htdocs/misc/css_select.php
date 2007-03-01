<?php
/*
css_select.php - CSS code for SELECT widget

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

  css_select_init()
*/
function
css_select_init ($font_family, $font_size, $menu_background_image)
{
?><style type="text/css" media="all">
<!--


.widget_select
{
  text-decoration:none;
  position:relative;
  z-index:0;
}


.widget_select + span
{
  display:none;
  z-index:0;
}


.widget_select:hover + span
{
  font:<?php echo $font_size; ?>px <?php echo $font_family; ?>;
  display:inline;
  position:relative;
  background:url("<?php echo $menu_background_image; ?>");
  padding:5px;
  z-index:0;
  white-space:nowrap;
}


.widget_select + span a
{
//  position:relative;
  text-decoration:none;
  z-index:0;
}


.widget_select:hover + span a
{
  text-decoration:none;
  display:block;
//  position:relative;
  text-align:left;
  background-color:#fff;
  color:#00f;
  z-index:1;
}


.widget_select + span a:hover 
{
//  position:relative;
  text-decoration:none;
  background-color:#00f; 
  color:#fff;
  z-index:1;
}


-->
</style><?php
}


?>