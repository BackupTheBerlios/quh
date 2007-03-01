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
require_once ("css_a.php");
require_once ("css_img.php");
require_once ("css_select.php");


/*
fixed background picture

body 
{
  background-image:url(test.png);
  background-repeat:no-repeat;
  background-attachment:fixed;
//  padding:0px;
}
*/
define ("WIDGET_CSS_A", 1);
define ("WIDGET_CSS_SELECT", 2);
define ("WIDGET_CSS_IMG", 4);
define ("WIDGET_CSS_ALL", WIDGET_CSS_A|WIDGET_CSS_SELECT|WIDGET_CSS_IMG);


function
widget_css_init ($font_family, $font_size, $color, $background_color,
             $menu_background_image, $menu_border_color, $flags)
{
  if ($flags & WIDGET_CSS_A)
    css_a_init ($color, $background_color);

  if ($flags & WIDGET_CSS_IMG)
    css_img_init ($color, $background_color);

  if ($flags & WIDGET_CSS_SELECT)
    css_select_init ($font_family, $font_size, $menu_background_image);
}


?>
