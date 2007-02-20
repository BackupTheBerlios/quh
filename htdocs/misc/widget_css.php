<?php
/*
widget_css.php - CSS code for HTML widgets

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
//require_once ("widget_js.php");
//require_once ("widget_css.php");


function
widget_css_init ($font_family, $font_size, $color, $background_color,
             $tt_font_family, $tt_font_size, $tt_font_color, $tt_background_image, $tt_border_color,
             $menu_background_image, $menu_border_color)
{
?><style type="text/css" media="all">
<!--


#menu
{
  text-decoration:none;
  position:relative;
  z-index:0;
}


#menu + span
{
  display:none;
  z-index:0;
}


#menu:hover + span
{
  font:<?php echo $font_size; ?>px <?php echo $font_family; ?>;
  display:block;
  position:absolute;
  top:1em;
  left:0px;
  background:url("<?php echo $menu_background_image; ?>");
  padding:5px;
  z-index:1;
  white-space:nowrap;
}


#menu + span a
{
  position:relative;
  text-decoration:none;
  z-index:1;
}


#menu:hover + span a
{
  text-decoration:none;
  display:block;
  position:relative;
  text-align:left;
  background-color:#fff;
  color:#00f;
  z-index:1;
}


#menu + span a:hover 
{
  position:relative;
  text-decoration:none;
  background-color:#00f; 
  color:#fff;
  z-index:1;
}


#href
{
  text-decoration:none;
}


#href:hover
{
  color:<?php echo $color; ?>;
  background-color:<?php echo $background_color; ?>;
  text-decoration:none;
}


#href:visited:hover
{
  color:<?php echo $color; ?>;
  background-color:<?php echo $background_color; ?>;
  text-decoration:none;
}


#href:visited
{
  color:<?php echo $background_color; ?>;
  text-decoration:none;
}


#img
{
  font:<?php echo $font_size; ?>px <?php echo $font_family; ?>;
  position: relative;
  z-index:0;
  text-decoration:none;
}


#img:hover
{
  font:<?php echo $font_size; ?>px <?php echo $font_family; ?>;
  text-decoration:none;
  z-index:1;
}


#img span
{
  font:<?php echo $font_size; ?>px <?php echo $font_family; ?>;
  display:none;
}


#img:visited:hover
{
  text-decoration:none;
}


#img:visited
{
  color:<?php echo $background_color; ?>;
  text-decoration:none;
}


#tt
{
  position: relative;
  z-index:0;
  text-decoration:none;
}


#tt:hover
{
  color:<?php echo $color; ?>;
  background-color:<?php echo $background_color; ?>;
  text-align:left;
  text-decoration:none;
  z-index:1;
}


#tt span
{
  font:<?php echo $font_size; ?>px <?php echo $font_family; ?>;
  display:none;
}


#img:hover span,#tt:hover span
{
  font:<?php echo $font_size; ?>px <?php echo $font_family; ?>;
  position:absolute;
  top:30px;
  left:10px;
  padding:5px;
  display:block;
//  white-space:nowrap;
  background:url("<?php echo $tt_background_image; ?>");
  color:<?php echo $tt_font_color; ?>;
  text-align:left
  text-decoration:none
}


#tt:visited:hover
{
  color:<?php echo $color; ?>;
  background-color:<?php echo $background_color; ?>;
  text-decoration:none;
}


#tt:visited
{
  color:<?php echo $background_color; ?>;
  text-decoration:none;
}


-->
</style><?php
}

?>
