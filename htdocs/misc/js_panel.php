<?php
/*
  PHP

  js_panel_init()
*/
function
js_panel_init ($img, $w, $h)
{
?>
<script language="JavaScript">
<!--

// TODO: set these with PHP
var img_w = 170;
var img_h = 125;
var img_n = 5;



function
js_panel_get_img_array ()
{
  var pic = new Array (picture1, picture2, picture3, picture4, picture5);
  return pic;
}


function
js_panel_func (n, xpos)
{
  var min_zoom = 0.5;
  var max_zoom = 2;
  var f = 0;

  f = (n * (img_w / 2) + (img_w * 2) - xpos) / (img_w * 2);
  f = Math.pow (f, 2) * -1;
//  f *= -1;
  f++;

  f = Math.min (Math.max (f, min_zoom), max_zoom);

  return f;
}


function
js_panel_event (e)
{
  var xpos = js_mouse_get_xpos (e);
  var img = js_panel_get_img_array ();

  for (i = 0; i < img_n; i++)
    {
      var f = js_panel_func (i, xpos);
      js_img_resize (img[i], img_w * f, img_h * f);
    }
}


function
js_panel_event_ignore (e)
{
  var w = img_w * 0.5;
  var h = img_h * 0.5;
  var img = js_panel_get_img_array ();

  for (i = 0; i < img_n; i++)
    js_img_resize (img[i], w, h);
}


//-->
</script><?


}


?>