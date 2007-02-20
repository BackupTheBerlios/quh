<?php
/*
widget.php - HTML widget wrappers for PHP

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
require_once ("widget_js.php");
require_once ("widget_css.php");

/*
static char *
ng_httpd_widget_def (st_widget_t *widget_p)
{
  static char buf[MAXBUFSIZE];
  char *p = buf;

  *p = 0;
  if (widget_p->name)
    switch (widget_p->type)
      {
        case NG_HR_W:
        case NG_IMG_W:
        case NG_SUBMIT_W:
        case NG_IMAGE_W:
        case NG_TEXTAREA_W:
        case NG_TEXT_W:
        case NG_FILE_W:
          sprintf (strchr (p, 0), " name=\"%s\"", widget_p->name);
          break;
      }

  if (widget_p->value)
    switch (widget_p->type)
      {
#ifndef  USE_HTML4
        case NG_SUBMIT_W:
        case NG_IMAGE_W:
#endif
        case NG_TEXT_W:
        case NG_FILE_W:
          sprintf (strchr (p, 0), " value=\"%s\"", widget_p->value);
          break;
                                            
      }

  if (widget_p->width > 0)
    switch (widget_p->type)
      {
        case NG_TEXT_W:
          sprintf (strchr (p, 0), " size=\"%d\"", widget_p->width);
          break;
                                           
        case NG_TEXTAREA_W:
          sprintf (strchr (p, 0), " cols=\"%d\"", widget_p->width);
          break;

        case NG_HR_W:
        case NG_IMG_W:
        case NG_SUBMIT_W:
        case NG_IMAGE_W:
          sprintf (strchr (p, 0), " width=\"%d\"", widget_p->width);
          break;
      }

  if (widget_p->height > 0)
    switch (widget_p->type)
      {
        case NG_TEXT_W:
          sprintf (strchr (p, 0), " maxlength=\"%d\"", widget_p->height);
          break;
                                           
        case NG_TEXTAREA_W:
          sprintf (strchr (p, 0), " rows=\"%d\"", widget_p->height);
          break;

        case NG_HR_W:
        case NG_IMG_W:
        case NG_SUBMIT_W:
          sprintf (strchr (p, 0), " height=\"%d\"", widget_p->height);
          break;
      }
    
  if (widget_p->border > -1)
    switch (widget_p->type)
      {
        case NG_HR_W:
        case NG_TEXT_W:
        case NG_IMG_W:
        case NG_IMAGE_W:
        case NG_SUBMIT_W:
        case NG_BODY_W:
        case NG_TEXTAREA_W:
        case NG_RADIO_W:
        case NG_CHECKBOX_W:
          sprintf (strchr (p, 0), " border=\"%d\"", widget_p->border);
          break;
      }

  if (widget_p->bgcolor)
    switch (widget_p->type)
      {
//        case NG_SUBMIT_W:
//          break;
        case NG_BODY_W:
          sprintf (strchr (p, 0), " bgcolor=\"%s\"", widget_p->bgcolor);
          break;
      }

  if (widget_p->fgcolor)
    switch (widget_p->type)
      {
//        case NG_SUBMIT_W:
//          break;
        case NG_BODY_W:
          sprintf (strchr (p, 0), " fgcolor=\"%s\"", widget_p->fgcolor);
          break;
      }

  if (widget_p->tooltip)
    switch (widget_p->type)
      {
        case NG_IMG_W:
        case NG_TEXT_W:
        case NG_FILE_W:
        case NG_TEXTAREA_W:
        case NG_PASSWORD_W:
        case NG_SELECT_W:
        case NG_IMAGE_W:
        case NG_SUBMIT_W:
        case NG_A_W:
          sprintf (strchr (p, 0), " title=\"%s\"", widget_p->tooltip);
          break;
#if 0
        default:
          sprintf (strchr (p, 0), " alt=\"%s\"", widget_p->tooltip);
          break;
#endif
      }

  return p;
}


static char *
ng_widget_span_tooltip (st_widget_t *p)
{
#ifdef  USE_HTML4
  static char buf[MAXBUFSIZE];

  if (p->tooltip)
    {
      sprintf (buf, "<span>%s</span>", p->tooltip);
      return buf;
    }
#endif
  return "";
}

*/


class misc_widget
{
  var $focus = NULL;
  var $name = NULL;


function
widget_init ($font_family, $font_size, $color, $background_color,
             $tt_font_family, $tt_font_size, $tt_font_color, $tt_background_image, $tt_border_color,
             $menu_background_image, $menu_border_color)
{
  return widget_css_init ($font_family, $font_size, $color, $background_color,
             $tt_font_family, $tt_font_size, $tt_font_color, $tt_background_image, $tt_border_color,
             $menu_background_image, $menu_border_color);
}


function
widget_start ($name, $target, $method, $focus)
{
  $this->name = $name;
  $this->focus = $focus;

  return "<form name=\""
        .$name
        ."\" method=\""
        .$method
        ."\" action=\""
        .$target
        ."\">\n";
}


function
widget_end ()
{
  $p = "</form>\n";

  if (isset ($this->focus))
    if (!is_null ($this->focus))
      $p .= "<script type=\"text/javascript\"><!--\n\n"
           ."  document."
           .$this->name
           ."."
           .$this->focus
           .".focus();\n\n"
           ."--></script>\n";

  $this->name = NULL;
  $this->focus = NULL;

  return $p;
}


function
widget_text ($name, $value, $tooltip, $size, $maxsize)
{
  return "<input type=\"text\" name=\""
         .$name
         ."\" value=\""
         .$value
         ."\">\n";
}


function
widget_textarea ($name, $value, $tooltip, $cols, $rows, $ro)
{
  return "<textarea name=\""
         .$name
         ."\" cols=\""
         .$cols
         ."\" rows=\""
         .$rows
         ."\""
         .($ro ? " readonly=\"readonly\"" : "")
         .">\n"
         .$value
         ."</textarea>";
}


function
widget_hidden ($name, $value)
{
  return "<input type=\"hidden\" name=\""
         .$name
         ."\" value=\""
         .$value
         ."\">\n";
}


function
widget_submit ($name, $label, $tooltip)
{
  return "<input type=\"submit\" name=\""
         .$name
         ."\" value=\""
         .$label
         ."\" title=\""
         .$tooltip
         ."\">\n";
}


function
widget_reset ($name, $label, $tooltip)
{
  return "<input type=\"reset\" name=\""
         .$name
         ."\" value=\""
         .$label
         ."\" title=\""
         .$tooltip
         ."\">\n";
}


function
widget_image ($name, $value, $img, $label, $w, $h, $tooltip)
{
  return "<button class=\"tt\" type=\"submit\""
//ng_httpd_widget_def (p)
        ."><img src=\""
        .$img
        ."\" border=\"0\">"
//        ."<span>"
//        .$tooltip
//        ."</span>"
        ."</button>\n";
}


function
widget_checkbox ($name, $checked, $tooltip)
{
  return "<input type=\"checkbox\""
        ." name=\""
        .$name
        ."\""
        .($checked ? " checked" : "")
        ." title=\""
        .$tooltip
        ."\">";
}


function
widget_radio ($name, $value_array, $tooltip)
{
  $p = "";
  $i_max = sizeof ($value_array);
  for ($i = 0; $i < $i_max; $i++)
    $p .= "<input type=\"radio\""
         .!$i ? " checked" : ""
         ." name=\""
         .$name
         ."\""
         ." title=\""
         .$tooltip
         ."\""
         ." value=\""
         .$value_array[$i]
         ."\"> "
         .$value_array[$i]
         ."<br>\n";

  return $p;
}


function
widget_file ($name, $value, $tooltip)
{
  return "<input type=\"file\""
        ." name=\""
        .$name
        ."\""
        ." value=\""
        .$value
        ."\""
        ." title=\""
        .$tooltip
        ."\">\n";
}


function
widget_password ($name, $tooltip)
{
  return "<input type=\"password\""
        ." name=\""
        .$name
        ."\""
        ." title=\""
        .$tooltip
        ."\">\n";
}


function
widget_select ($img, $name, $img_array, $name_array, $value_array, $tooltip)
{
  $p = "";

  if ($img)
    $p .= "<img id=\"menu\" src=\""
         .$img
         ."\" border=\"0\""
//         .($tooltip ? " alt=\"".$tooltip."\"" : "")
         .">";

//  if ($name)
//    $p .= $name;

  $p .= "\n  <span>\n";

  $i_max = max (sizeof ($img_array), sizeof ($name_array), sizeof ($value_array));
  for ($i = 0; $i < $i_max; $i++)
    $p .= "    <a href=\""
         .$value_array[$i]
         ."\">"
         .$name_array[$i]
         ."</a>\n";

  $p .= "  </span>\n";

  return $p;
}
/*
function
widget_select ($img, $name, $img_array, $name_array, $value_array, $tooltip)
{
  $p = "";

  if ($img)
    $p .= "<img id=\"menu\" src=\""
         .$img
         ."\" border=\"0\""
//         .($tooltip ? " alt=\"".$tooltip."\"" : "")
         .">";

  $p .= "<select onChange=\"this.form.submit();\" name=\""
       .$name
       ."\""
//       .ng_httpd_widget_def (p)
       .">\n";

  $i_max = max (sizeof ($img_array), sizeof ($name_array), sizeof ($value_array));
  for ($i = 0; $i < $i_max; $i++)
    $p .= "<option"
         .(!$i ? " selected" : "")
         ." value=\""
         .$value_array[$i]
         ."\">"
         .($img_array[$i] ? "<img src=\"".$img_array[$i]."\" border=\"0\">" : "")
         .$name_array[$i]
         ."</option>\n";

  $p .= "</select>\n";


  return $p;
}
*/


function
widget_a ($url, $img, /* $w, $h, */ $label, $tooltip)
{
  $p = "";

  if ($url)
    {
      $p .= "<a";

      if ($img)
        $p .= " id=\"img\"";
      else if ($tooltip)
        $p .= " id=\"tt\"";
      else
        $p .= " id=\"href\"";

      $p .= " href=\""
           .$url
           ."\">";
    }

  if ($img)
    {
      $p .= "<nobr>";
      $p .= "<img src=\""
           .$img
           ."\" border=\"0\"";

      if ($tooltip)
        $p .= " alt=\""
           .$tooltip
           ."\"";

      $p .= ">";
      $p .= "</nobr>";
    }
  else if (!$label)
    $p .= $tooltip;

  if ($tooltip)
    $p .= "<span>"
         .$tooltip
         ."</span>";

  if ($label)
    {
      $p .= "<nobr>";
      $p .= $label;
      $p .= "</nobr>";
    }

  if ($url)
    $p .= "</a>";

  return $p;
}


function
widget_img ($img, $w, $h, $border, $alt, $tooltip)
{
  return "<img src=\""
        .$img
        .($w != -1 ? "\" width=\"".$w : "")
        .($h != -1 ? "\" height=\"".$h : "")
        ."\" border=\""
        .$border
        ."\" alt=\""
        .$alt
        ."\" title=\""
        .$tooltip
        ."\">";
}


function
widget_trans ($w, $h)
{
  return "<img src=\"images/trans.png\" border=\"0\" width=\""
        .$w
        ."\" height=\""
        .$h
        ."\" alt=\"images/trans.png not found\">";
}


function
widget_gauge ($percent)
{
  return "<table border=\"0\" width=\"640\" cellspacing=\"0\" cellpadding=\"0\">\n"
        ."  <tr>\n"
        ."    <td width=\""
        .$percent
        ."%\" bgcolor=\"#00ff00\">&nbsp;</td>\n"
        ."    <td bgcolor=\"#ff0000\">&nbsp;</td>\n"
        ."  </tr>\n"
        ."</table>\n";
}


/*
function
widget_config ()
{
// configuration
// export the htdocs to apache
// run on which port?
// allow external connections?
}
*/

}


function
widget_test ($w)
{
  $img_array = Array ();
  $name_array = Array ();
  $value_array = Array ();

  $p = ""
      .$w->widget_start ("name", "target", "method", NULL)
      ."<br>widget_img(): "
      .$w->widget_img ("images/logo.png", -1, -1, 0, "alt", "tooltip") // w, h, border
      ."<hr>widget_submit(): "
      .$w->widget_submit ("name", "label", "tooltip")
      ."<hr>widget_reset(): "
      .$w->widget_reset ("name", "label", "tooltip")
      ."<hr>widget_image(): "
      .$w->widget_image ("name", "value", "images/logo.png", "label", -1, -1, "tooltip") // w, h
      ."<hr>widget_checkbox(): "
      .$w->widget_checkbox ("name", 1, "tooltip") // checked
      ."<hr>widget_radio(): "
      .$w->widget_radio ("name", $value_array, "tooltip")
      ."<hr>widget_text(): "
      .$w->widget_text ("name", "value", "tooltip", 50, 50) // size, maxsize
      ."<hr>widget_file(): "
      .$w->widget_file ("name", "value", "tooltip")
      ."<hr>widget_password(): "
      .$w->widget_password ("name", "tooltip")
      ."<hr>widget_hidden(): "
      .$w->widget_hidden ("name", "value")
      ."<hr>widget_textarea(): "
      .$w->widget_textarea ("name", "value", "tooltip", 35, 10, 0) // cols, rows, ro
      ."<hr>widget_select(): "
      .$w->widget_select (NULL, "name", $img_array, $name_array, $value_array, "tooltip")
      ."<hr>widget_a(): "
      .$w->widget_a ("url", "images/logo.png", "label", "tooltip")
      ."<hr>widget_trans(): "
      .$w->widget_trans (100, 100)
      ."<hr>widget_gauge(): "
      .$w->widget_gauge (20)
      .$w->widget_end ();

  echo $p;
}


?>
