<?php
/*
widget.php - HTML widget wrappers in PHP

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
//require_once ("misc/misc.php");   // sprint_r()


// flags
define ("WIDGET_RO", 1);           // widget is read-only (textarea, ...)
define ("WIDGET_FOCUS", 2);        // document focus is on this widget (text, textarea, ...)
define ("WIDGET_SUBMIT", 32);      // widget does submit the whole form
define ("WIDGET_CHECKED", 4);      // widget is checked (checkbox, radio, ...)
define ("WIDGET_CSS", 8);          // widget uses CSS for tooltips/appearance
//define ("WIDGET_JS", 16);          // widget uses JS for tooltips/appearance
define ("WIDGET_DISABLED", 64);    // widget is inactive 


class misc_widget
{
  var $focus = NULL;
  var $name = NULL;
  var $method = NULL;
  var $img_l = NULL, $img_r = NULL, $img_bl = NULL, $img_b = NULL, $img_br = NULL;

function
widget_init ($font_family, $font_size, $color, $background_color,
             $menu_background_image, $menu_border_color, $css_flags, $js_flags)
{
  widget_css_init ($font_family, $font_size, $color, $background_color,
                   $menu_background_image, $menu_border_color, $css_flags);
  widget_js_init ($js_flags);
}


function
widget_start ($name, $target, $method)
{
  $this->name = $name;
  $this->method = $method;

  return "<form name=\""
        .$name
        ."\" method=\""
        .$method
        ."\" action=\""
        .$target
        ."\""
        .(!strcasecmp ($method, "POST") ? " enctype=\"multipart/form-data\"" : "")
        .">";
}


function
widget_end ()
{
  $p = "</form>";

  if (isset ($this->focus))
    if (!is_null ($this->focus))
      $p .= "<script type=\"text/javascript\"><!--\n\n"
           ."  document."
           .$this->name
           ."."
           .$this->focus
           .".focus();\n\n"
           ."--></script>";

  $this->name = NULL;
  $this->focus = NULL;

  return $p;
}


function
widget_text ($name, $value, $tooltip, $size, $maxlength, $flags)
{
  if ($flags & WIDGET_FOCUS)
    $this->focus = $name;

  return "<input class=\"widget_text\" type=\"text\" name=\""
        .$name
        ."\" value=\""
        .$value
        ."\" size=\""
        .$size
        ."\" maxlength=\""
        .$maxlength
        ."\" title=\""
        .$tooltip
        ."\""
        .($flags & WIDGET_DISABLED ? " disabled" : "")
        .">\n";
}


function
widget_textarea ($name, $value, $tooltip, $cols, $rows, $flags)
{
  if ($flags & WIDGET_FOCUS)
    $this->focus = $name;

  return "<textarea class=\"widget_textarea\" name=\""
        .$name
        ."\" title=\""
        .$tooltip
        ."\" cols=\""
        .$cols
        ."\" rows=\""
        .$rows
        ."\""
        .($flags & WIDGET_RO ? " readonly=\"readonly\"" : "")
        .($flags & WIDGET_DISABLED ? " disabled" : "")
        .">\n"
        .$value
        ."</textarea>";
}


function
widget_hidden ($name, $value, $flags)
{
  if ($flags & WIDGET_FOCUS)
    $this->focus = $name;

  return "<input class=\"widget_hidden\" type=\"hidden\" name=\""
        .$name
        ."\" value=\""
        .$value
        ."\">\n";
}


function
widget_submit ($name, $label, $tooltip, $flags)
{
  if ($flags & WIDGET_FOCUS)
    $this->focus = $name;

  return "<input class=\"widget_submit\" type=\"submit\" name=\""
        .$name
        ."\" value=\""
        .$label
        ."\" title=\""
        .$tooltip
        ."\">\n";
}


function
widget_reset ($name, $label, $tooltip, $flags)
{
  if ($flags & WIDGET_FOCUS)
    $this->focus = $name;

  return "<input class=\"widget_reset\" type=\"reset\" name=\""
        .$name
        ."\" value=\""
        .$label
        ."\" title=\""
        .$tooltip
        ."\""
        .($flags & WIDGET_DISABLED ? " disabled" : "")
        .">\n";
}


function
widget_image ($name, $value, $img, $w, $h, $tooltip, $flags)
{
  if ($flags & WIDGET_FOCUS)
    $this->focus = $name;

  return "<button class=\"widget_image\" type=\"submit\""
        .($flags & WIDGET_DISABLED ? " disabled" : "")
        .">"
        ."<img src=\""
        .$img
        ."\""
        .($w != -1 ? " width=\"".$w."\"" : "")
        .($h != -1 ? " height=\"".$h."\"" : "")
        ." border=\"0\""
        ." alt=\""
        .$tooltip
        ."\">"
//        ."<span>"
//        .$tooltip
//        ."</span>"
        ."</button>\n";
}


function
widget_checkbox ($name, $tooltip, $flags)
{
  if ($flags & WIDGET_FOCUS)
    $this->focus = $name;

  return "<input class=\"widget_checkbox\" type=\"checkbox\""
        ." name=\""
        .$name
        ."\""
        .($flags & WIDGET_CHECKED ? " checked" : "")
        ." title=\""
        .$tooltip
        ."\""
        .($flags & WIDGET_DISABLED ? " disabled" : "")
        .">\n";
}


function
widget_radio ($name, $value_array, $tooltip, $flags)
{
  if ($flags & WIDGET_FOCUS)
    $this->focus = $name;

  $p = "";
  $i_max = sizeof ($value_array);
  for ($i = 0; $i < $i_max; $i++)
    $p .= "<input class=\"widget_radio\" type=\"radio\""
         .(!$i ? " checked" : "")
         ." name=\""
         .$name
         ."\""
         ." title=\""
         .$tooltip
         ."\""
         ." value=\""
         .$value_array[$i]
         ."\""
         .($flags & WIDGET_DISABLED ? " disabled" : "")
         .">\n";

  return $p;
}


/*
  In PHP versions earlier than 4.1.0, $HTTP_POST_FILES should be used instead
  of $_FILES.

  $_FILES['userfile']['name']
    The original name of the file on the client machine. 
  $_FILES['userfile']['type']
    The mime type of the file, if the browser
    provided this information. An example would be "image/gif". This mime
    type is however not checked on the PHP side and therefore don't take its
    value for granted.
  $_FILES['userfile']['size']
    The size, in bytes, of the uploaded file. 
  $_FILES['userfile']['tmp_name']
    The temporary filename of the file in which the uploaded file was stored on the server. 
  $_FILES['userfile']['error']
    The error code associated with this file upload. This element was added in PHP 4.2.0 

  UPLOAD_ERR_OK          0; There is no error, the file uploaded with success. 
  UPLOAD_ERR_INI_SIZE    1; The uploaded file exceeds the upload_max_filesize directive in php.ini. 
  UPLOAD_ERR_FORM_SIZE   2; The uploaded file exceeds the MAX_FILE_SIZE directive that was specified in the HTML form. 
  UPLOAD_ERR_PARTIAL     3; The uploaded file was only partially uploaded. 
  UPLOAD_ERR_NO_FILE     4; No file was uploaded. 
  UPLOAD_ERR_NO_TMP_DIR  6; Missing a temporary folder. Introduced in PHP 4.3.10 and PHP 5.0.3. 
  UPLOAD_ERR_CANT_WRITE  7; Failed to write file to disk. Introduced in PHP 5.1.0. 
  UPLOAD_ERR_EXTENSION   8; File upload stopped by extension. Introduced in PHP 5.2.0. 

  related php.ini settings
    if (post_max_size > upload_max_filesize) in php.ini
      otherwise you will not be able to report the correct error in case of a
      too big upload ! Also check the max-execution-time (upload-time could be
      added to execution-time)

    if (post >post_max_size) in php.ini
      $_FILES and $_POST will return empty
*/
function
widget_upload ($name, $label, $tooltip, $upload_path, $max_file_size, $flags)
{
  if (strcasecmp ($this->method, "POST"))
    return "widget_upload() requires method=\"POST\" and enctype=\"multipart/form-data\" in the form tag\n";

  if ($flags & WIDGET_FOCUS)
    $this->focus = $name;

/*
  The data encoding type, enctype, MUST be specified as enctype="multipart/form-data"
  MAX_FILE_SIZE must precede the file input field
  Name of input element determines name in $_FILES array
*/
  $p = $this->widget_hidden ("MAX_FILE_SIZE", $max_file_size, 0)
      ."<input class=\"widget_upload\" type=\"file\""
      ." name=\""
      .$name
      ."\""
//      ." value=\""
//      .$value
//      ."\""
      ." title=\""
      .$tooltip
      ."\""
      .($flags & WIDGET_DISABLED ? " disabled" : "")
      .">\n"
      .$this->widget_submit ($name, $label, $tooltip, 0);

  if (!$_FILES)
    return $p;

//  $p .= sprint_r ($_FILES); // debug

  if (move_uploaded_file ($_FILES[$name]["tmp_name"],
                          $upload_path
                         ."/"
                         .basename($_FILES[$name]["name"])) == FALSE)
    {
//    FALSE
    }

  $s = Array ();
  $s[UPLOAD_ERR_OK] =           "OK";
  $s[UPLOAD_ERR_INI_SIZE] =     "The uploaded file exceeds the upload_max_filesize directive ("
                               .ini_get ("upload_max_filesize")
                               .") in php.ini";
  $s[UPLOAD_ERR_FORM_SIZE] =    "The uploaded file exceeds the MAX_FILE_SIZE directive ("
                               .$max_file_size
                               .") that was specified in the HTML form";
  $s[UPLOAD_ERR_PARTIAL] =      "The uploaded file was only partially uploaded";
  $s[UPLOAD_ERR_NO_FILE] =      "No file was uploaded";
  $s[UPLOAD_ERR_NO_TMP_DIR] =   "Missing a temporary folder";
//  if (defined (UPLOAD_ERR_CANT_WRITE))
//    $s[UPLOAD_ERR_CANT_WRITE] = "Failed to write file to disk";
//  if (defined (UPLOAD_ERR_EXTENSION))
//    $s[UPLOAD_ERR_EXTENSION] =  "File upload stopped by extension";

  $e = $s[$_FILES[$name]["error"]];
  if (!$e)
    $e .= "Unknown File Error.";

  $p .= $e;

//  $p .= "\n\n\n".sprint_r ($s); // debug

//  print_r ($_FILES); // debug

  return $p;
}
  

function
widget_password ($name, $tooltip, $flags)
{
  if ($flags & WIDGET_FOCUS)
    $this->focus = $name;

  return "<input class=\"widget_password\" type=\"password\""
        ." name=\""
        .$name
        ."\""
        ." title=\""
        .$tooltip
        ."\""
        .($flags & WIDGET_DISABLED ? " disabled" : "")
        .">\n";
}


function
widget_select ($img, $name, $img_array, $name_array, $value_array, $tooltip, $flags)
{
  if ($flags & WIDGET_FOCUS)
    $this->focus = $name;

  $p = "";

  if ($img)
    $p .= "<img class=\"widget_select\" src=\""
         .$img
         ."\" border=\"0\""
         .($tooltip ? " title=\"".$tooltip."\"" : "")
         .($tooltip ? " alt=\"".$tooltip."\"" : "")
         .">";

  if ($flags & WIDGET_CSS)
    {
      $p .= "<span>";
    
      $i_max = max (sizeof ($img_array), sizeof ($name_array), sizeof ($value_array));
      for ($i = 0; $i < $i_max; $i++)
        $p .= "<a href=\""
             .$value_array[$i]
             ."\">"
             .$name_array[$i]
             ."</a>\n";
    
      $p .= "</span>";
    }
  else
    {
      $p .= "<select style=\"background-image:url('".$img."');\""
           .($flags & WIDGET_SUBMIT ? " onchange=\"this.form.submit();\"" : "")
           ." name=\""
           .$name
           ."\""
           .($tooltip ? " title=\"".$tooltip."\"" : "")
           ."\""
           .($flags & WIDGET_DISABLED ? " disabled" : "")
           .">\n";
    
      $p .= "<option selected"
//           ." style=\"background-image:url('".$img."');\""
           .">";
      $i_max = max (sizeof ($img_array), sizeof ($name_array), sizeof ($value_array));
      for ($i = 0; $i < $i_max; $i++)
        $p .= "<option"
//             .(!$i ? " selected" : "")
//             .(!$i ? " style=\"background-image:url('".$img."');\"" : "")
             ." value=\""
             .$value_array[$i]
             ."\">"
             .($img_array[$i] ? "<img src=\"".$img_array[$i]."\" border=\"0\">" : "")
             .$name_array[$i]
             ."</option>\n";
    
      $p .= "</select>";
    }

  return $p;
}


function
widget_a ($url, $img, $w, $h, $label, $tooltip, $flags)
{
  if ($flags & WIDGET_FOCUS)
    $this->focus = $name;

  $p = "<a class=\"widget_a\""
      ." href=\""
      .$url
      ."\""
      .($flags & WIDGET_DISABLED ? " disabled" : "")
      .">"
      ."<nobr>";

  if ($img)
    $p .= "<img src=\""
         .$img
         ."\""
         .($w != -1 ? " width=\"".$w."\"" : "")
         .($h != -1 ? " height=\"".$h."\"" : "")
         ." border=\"0\""
         ." alt=\""
         .$tooltip
         ."\">";

  $p .= $label
       ."</nobr>";

  if ($tooltip)
    $p .= "<span>"
         .$tooltip
         ."</span>";
 
  $p .= "</a>";

  return $p;
}


function
widget_img ($name, $img, $w, $h, $border, $alt, $tooltip, $flags)
{
  if ($flags & WIDGET_FOCUS)
    $this->focus = $name;

  $p = "<img class=\"widget_img\" src=\""
      .$img
      ."\""
      ."name=\""
      .$name
      ."\""
      .($w != -1 ? " width=\"".$w."\"" : "")
      .($h != -1 ? " height=\"".$h."\"" : "")
      ." border=\""
      .$border
      ."\" alt=\""
      .$alt
      ."\""
      ." title=\""
      .$tooltip
      ."\">"
      .($flags & WIDGET_CSS ? "<span>".$tooltip."</span>" : "")
      ."\n";

  return $p;
}


function
widget_trans ($w, $h, $flags)
{
  if ($flags & WIDGET_FOCUS)
    $this->focus = $name;

  return "<img class=\"widget_trans\""
        ." src=\"images/trans.png\""
        .($w != -1 ? " width=\"".$w."\"" : "")
        .($h != -1 ? " height=\"".$h."\"" : "")
        ." border=\"0\""
        ." alt=\"images/trans.png not found\">";
}


function
widget_gauge ($percent, $flags)
{
  if ($flags & WIDGET_FOCUS)
    $this->focus = $name;

  return "<table class=\"widget_gauge\" border=\"0\" width=\"640\" cellspacing=\"0\" cellpadding=\"0\">\n"
        ."  <tr>\n"
        ."    <td width=\""
        .$percent
        ."%\" bgcolor=\"#00ff00\">&nbsp;</td>\n"
        ."    <td bgcolor=\"#ff0000\">&nbsp;</td>\n"
        ."  </tr>\n"
        ."</table>\n";
}


function
widget_panel ($url_array, $img_array, $tooltip)
{
  js_panel_init ($url_array, $img_array, $tooltip);

  return "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n"
        ."<tr>\n"
        ."    <td height=\"10\" colspan=\"4\" onMouseOver=\"js_mouse_callback_func (js_panel_event_ignore);\">\n"
        ."    </td> \n"
        ."  </tr>\n"
        ."  <tr>\n"
        ."    <td width=\"10\" height=\"140\" valign=\"bottom\" onMouseOver=\"js_mouse_callback_func (js_panel_event_ignore);\">\n"
        ."    </td>\n"
        ."    <td width=\"14%\" valign=\"bottom\" onMouseOver=\"js_mouse_callback_func (js_panel_event);\">\n"
        ."    </td>\n"
        ."    <td width=\"86%\" valign=\"bottom\" onMouseOver=\"js_mouse_callback_func (js_panel_event);\">\n"
        ."<nobr>\n"
        ."<a href=\"test3.php\" target=\"_blank\"><img name=\"picture1\" src=\"images/panel1.png\" width=\"68\" height=\"50\" border=\"0\"></a>\n"
        ."<a href=\"test3.php\" target=\"_blank\"><img name=\"picture2\" src=\"images/panel2.png\" width=\"68\" height=\"50\" border=\"0\"></a>\n"
        ."<a href=\"test2.php\" target=\"_blank\"><img name=\"picture3\" src=\"images/panel3.png\" width=\"68\" height=\"50\" border=\"0\"></a>\n"
        ."<a href=\"test3.php\" target=\"_blank\"><img name=\"picture4\" src=\"images/panel4.png\" width=\"68\" height=\"50\" border=\"0\"></a>\n"
        ."<a href=\"test3.php\" target=\"_blank\"><img name=\"picture5\" src=\"images/panel5.png\" width=\"68\" height=\"50\" border=\"0\"></a>\n"
        ."</nobr>\n"
        ."    </td>\n"
        ."    <td width=\"10\" valign=\"bottom\" onMouseOver=\"js_mouse_callback_func (js_panel_event_ignore);\">\n"
        ."    </td>\n"
        ."  </tr>\n"
        ."  <tr>\n"
        ."    <td height=\"10\" colspan=\"4\" onMouseOver=\"js_mouse_callback_func (js_panel_event_ignore);\">\n"
        ."    </td> \n"
        ."  </tr>\n"
        ."</table>\n";
}


function
widget_box_start ($img_tl, $img_t, $img_tr, $img_l, $img_r, $img_bl, $img_b, $img_br)
{
  $this->img_l  = $img_l;
  $this->img_r  = $img_r;
  $this->img_bl = $img_bl;
  $this->img_b  = $img_b;
  $this->img_br = $img_br;

  return "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n"
        ."  <tr>\n"
        ."    <td>\n"
        ."<img src=\"images/box_tl.png\">\n"
        ."    </td>\n"
        ."    <td class=\"widget_box_t\">\n"
        ."    </td>\n"
        ."    <td class=\"widget_box_tr\">\n"
        ."    </td>\n"
        ."  </tr>\n"
        ."  <tr>\n"
        ."    <td class=\"widget_box_l\">\n"
        ."    </td>\n"
        ."    <td valign=\"top\" style=\"background-color:#fff\">\n"
        ."test\n"
        ."    </td>\n"
        ."    <td class=\"widget_box_r\">\n"
        ."    </td>\n"
        ."  </tr>\n"
        ."  <tr>\n"
        ."    <td class=\"widget_box_bl\">\n"
        ."    </td>\n"
        ."    <td class=\"widget_box_b\">\n"
        ."    </td>\n"
        ."    <td> \n"
        ."<img src=\"images/box_br.png\" border=\"0\">\n"
        ."    </td>\n"
        ."  </tr>\n"
        ."</table>\n";
}


function
widget_box_end ()
{
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
  $img_array = Array ("images/logo.png", "images/logo.png", "images/logo.png");
  $name_array = Array ("name1", "name2", "name3");
  $value_array = Array ("value1", "value2", "value3");
  $url_array = Array ("", "", "", "", "");
  $img_array2 = Array ("images/picture1.gif", 
                       "images/picture2.gif",
                       "images/picture3.gif",
                       "images/picture4.gif",
                       "images/picture5.gif");

  $p = ""
      .$w->widget_start ("name", $_SERVER['PHP_SELF'], "POST")
      ."<br>widget_img(): "
      .$w->widget_img ("logo", "images/logo.png", -1, -1, 0, "alt", "tooltip", WIDGET_CSS) // w, h, border
      ."<hr>widget_submit(): "
      .$w->widget_submit ("name", "label", "tooltip", 0)
      ."<hr>widget_reset(): "
      .$w->widget_reset ("name", "label", "tooltip", 0)
      ."<hr>widget_image(): "
      .$w->widget_image ("name", "value", "images/logo.png", "label", -1, -1, "tooltip", 0) // w, h
      ."<hr>widget_checkbox(): "
      .$w->widget_checkbox ("name", "tooltip", WIDGET_CHECKED) // checked
      ."<hr>widget_radio(): "
      .$w->widget_radio ("name", $value_array, "tooltip", 0)
      ."<hr>widget_text(): "
      .$w->widget_text ("name", "value", "tooltip", 50, 50, 0) // size, maxsize
      ."<hr>widget_upload(): "
      .$w->widget_upload ("upload_file", "label", "tooltip", "/mnt/incoming", 4096, 0)
      ."<hr>widget_password(): "
      .$w->widget_password ("name", "tooltip", 0)
      ."<hr>widget_hidden(): "
      .$w->widget_hidden ("name", "value", 0)
      ."<hr>widget_textarea(): "
      .$w->widget_textarea ("name", "value", "tooltip", 35, 10, 0) // cols, rows, ro
      ."<hr>widget_textarea(disabled): "
      .$w->widget_textarea ("name", "value", "tooltip", 35, 10, WIDGET_DISABLED) // cols, rows, ro
      ."<hr>widget_select(): "
      .$w->widget_select (NULL, "name", $img_array, $name_array, $value_array, "tooltip", 0)
      ."<hr>widget_a(): "
      .$w->widget_a ("url", "images/logo.png", -1, -1, "label", "tooltip", 0)
      ."<hr>widget_trans(): "
      .$w->widget_trans (100, 100, 0)
      ."<hr>widget_gauge(): "
      .$w->widget_gauge (20, 0)
      ."<hr>widget_panel (): "
      .$w->widget_panel ($url_array, $img_array2, "tooltip")
      ."<hr>widget_box_start() + test + widget_box_end(): "
      .$w->widget_box_start ("images/cloud_tl.png",
                             "images/cloud_t.png",
                             "images/cloud_tr.png",
                             "images/cloud_l.png",
                             "images/cloud_r.png",
                             "images/cloud_bl.png",
                             "images/cloud_b.png",
                             "images/cloud_br.png")
      .$w->widget_box_end ()
//      ."<hr>widget_tabs(): "
//      .$w->widget_tabs ("name", $value_array, "tooltip", 0)
      .$w->widget_end ();

  echo $p;
}


?>
