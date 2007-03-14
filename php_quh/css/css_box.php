<?php
/*
css_box.php - CSS code for BOX widget

Copyright (c) 2007 NoisyB


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
  css_box_init()
*/
function
css_box_init ()
{
?><style type="text/css" media="all">
<!--

<?php
/*
.clip
{
  clip:rect(10px 50px 50px 70px);
  position:absolute;
}
*/
?>

.widget_box_t
{
  background-image:url('images/box_t.png');
  background-repeat:repeat-x;
}

.widget_box_tr
{
  background-image:url('images/box_tr.png');
  background-repeat:no-repeat;
}

.widget_box_l
{
  background-image:url('images/box_l.png');
  background-repeat:repeat-y;
}

.widget_box_r
{
  background-image:url('images/box_r.png');
  background-repeat:repeat-y;
}

.widget_box_bl
{
  background-image:url('images/box_bl.png');
  background-repeat:no-repeat;
}

.widget_box_b
{
  background-image:url('images/box_b.png');
  background-repeat:repeat-x;
}

-->
</style><?php
}


?>