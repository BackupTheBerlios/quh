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
//require_once ("misc/misc.php");   // sprint_r()
require_once ("configure.php");


// widget_init() css flags
define ("WIDGET_CSS_A",      1);
define ("WIDGET_CSS_SELECT", 1<<1);
define ("WIDGET_CSS_IMG",    1<<2);
define ("WIDGET_CSS_BOX",    1<<3);
define ("WIDGET_CSS_SLIDER", 1<<4);
define ("WIDGET_CSS_RELATE", 1<<5);
define ("WIDGET_CSS_ALL",    WIDGET_CSS_A|WIDGET_CSS_SELECT|WIDGET_CSS_IMG|WIDGET_CSS_BOX|WIDGET_CSS_SLIDER|WIDGET_CSS_RELATE);
// widget_init() js flags
define ("WIDGET_JS_EVENT",  1);
define ("WIDGET_JS_MISC",   1<<1);
define ("WIDGET_JS_WINDOW", 1<<2);
define ("WIDGET_JS_PANEL",  1<<3);
define ("WIDGET_JS_SLIDER", 1<<5);
define ("WIDGET_JS_RELATE", 1<<6);
define ("WIDGET_JS_ALL",    WIDGET_JS_EVENT|WIDGET_JS_MISC|WIDGET_JS_WINDOW|WIDGET_JS_PANEL|WIDGET_JS_SLIDER|WIDGET_JS_RELATE);
// widget_*() flags
define ("WIDGET_RO",         1);    // widget is read-only (widget_textarea, ...)
define ("WIDGET_FOCUS",      1<<1); // document focus is on this widget (widget_text, widget_textarea, ...)
define ("WIDGET_SUBMIT",     1<<2); // widget does submit the whole form
define ("WIDGET_CHECKED",    1<<3); // widget is checked (widget_checkbox, widget_radio, ...)
define ("WIDGET_DISABLED",   1<<4); // widget is inactive 
//define ("WIDGET_VALIDATE",   1<<5); // validate value entered in widget
// widget_relate() flags
define ("WIDGET_RELATE_DIGG",      1<<6);
define ("WIDGET_RELATE_DELICIOUS", 1<<7);
define ("WIDGET_RELATE_BOOKMARK",  1<<8);
define ("WIDGET_RELATE_SEARCH",    1<<9);
define ("WIDGET_RELATE_LINK",      1<<10);
define ("WIDGET_RELATE_FRESHMEAT", 1<<11);
define ("WIDGET_RELATE_LINKTOUS",  1<<12);
define ("WIDGET_RELATE_ALL",       WIDGET_RELATE_DIGG|WIDGET_RELATE_DELICIOUS|WIDGET_RELATE_BOOKMARK|WIDGET_RELATE_SEARCH|WIDGET_RELATE_LINK|WIDGET_RELATE_FRESHMEAT|WIDGET_RELATE_LINKTOUS);

class misc_widget
{
  var $focus = NULL;
  var $name = NULL;
  var $method = NULL;
  var $img_r = NULL,
      $img_bl = NULL,
      $img_b = NULL,
      $img_br = NULL;
  var $css_flags = 0;
  var $js_flags = 0;


function
widget_init ($css_flags, $js_flags)
{
  $config = new configure ();
  $p = "";

  if ($config->have_css)
    {
      if ($css_flags & WIDGET_CSS_A)
        $p .= "<link rel=\"stylesheet\" type=\"text/css\" media=\"all\" href=\"css/a.css\">\n";

      if ($css_flags & WIDGET_CSS_IMG)
        $p .= "<link rel=\"stylesheet\" type=\"text/css\" media=\"all\" href=\"css/img.css\">\n";

      if ($css_flags & WIDGET_CSS_SELECT)
        $p .= "<link rel=\"stylesheet\" type=\"text/css\" media=\"all\" href=\"css/select.css\">\n";

      if ($css_flags & WIDGET_CSS_BOX)
        $p .= "<link rel=\"stylesheet\" type=\"text/css\" media=\"all\" href=\"css/box.css\">\n";

      if ($css_flags & WIDGET_CSS_SLIDER)
        $p .= "<link rel=\"stylesheet\" type=\"text/css\" media=\"all\" href=\"css/slider.css\">\n";

      if ($css_flags & WIDGET_CSS_RELATE)
        $p .= "<link rel=\"stylesheet\" type=\"text/css\" media=\"all\" href=\"css/relate.css\">\n";

      $this->css_flags = $css_flags;
    }

  if ($config->have_js)
    {
      $p .= "<script type=\"text/javascript\"><!--\n"
           ."var js_ver = 1.0;\n"
           ."//--></script>\n"
           ."<script language=\"JavaScript1.1\"><!--\n"
           ."js_ver = 1.1;\n"
           ."//--></script>\n"
           ."<script language=\"JavaScript1.2\"><!--\n"
           ."js_ver = 1.2;\n"
           ."//--></script>\n"
           ."<script language=\"JavaScript1.3\"><!--\n"
           ."js_ver = 1.3;\n"
           ."//--></script>\n"
           ."<script language=\"JavaScript1.4\"><!--\n"
           ."js_ver = 1.4;\n"
           ."//--></script>\n"
           ."<script language=\"JavaScript1.5\"><!--\n"
           ."js_ver = 1.5;\n"
           ."//--></script>\n"
           ."<script language=\"JavaScript1.6\"><!--\n"
           ."js_ver = 1.6;\n"
           ."//--></script>\n"
           ."<script type=\"text/javascript\"><!--\n"
           .$config->get_js()
           ."//--></script>\n";

      if ($js_flags & WIDGET_JS_EVENT)
        $p .= "<script type=\"text/javascript\" src=\"js/event.js\"></script>\n";

      if ($js_flags & WIDGET_JS_MISC)
        $p .= "<script type=\"text/javascript\" src=\"js/misc.js\"></script>\n";

      if ($js_flags & WIDGET_JS_WINDOW)
        $p .= "<script type=\"text/javascript\" src=\"js/window.js\"></script>\n";

      if ($js_flags & WIDGET_JS_PANEL)
        $p .= "<script type=\"text/javascript\" src=\"js/panel.js\"></script>\n";

      if ($js_flags & WIDGET_JS_SLIDER)
        $p .= "<script type=\"text/javascript\" src=\"js/slider.js\"></script>\n";

      if ($js_flags & WIDGET_JS_RELATE)
        $p .= "<script type=\"text/javascript\" src=\"js/relate.js\"></script>\n";

      $this->js_flags = $js_flags;
    }

  echo $p;
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
        .($target ? $target : $_SERVER['PHP_SELF'])
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

  if ($this->js_flags & WIDGET_JS_SLIDER)
    $p .= "<script type=\"text/javascript\">\n"
         ."<!--\n"
         ."var sliderEl = document.getElementById ? document.getElementById(\"slider-1\") : null;\n"
         ."var inputEl = document."
         .$this->name
         ."[\"slider-input-1\"];\n"
         ."var s = new Slider(sliderEl, inputEl);\n"
         ."s.onchange = function ()\n"
         ."  {\n"
         ."window.status = \"Value: \" + s.getValue();\n"
         ."};\n"
         ."s.setValue(50);\n"
         ."//-->\n"
         ."</script>";

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
widget_radio ($name, $value_array, $label_array, $tooltip, $vertical, $flags)
{
  if ($flags & WIDGET_FOCUS)
    $this->focus = $name;

  $p = "";
  $i_max = sizeof ($value_array);
  for ($i = 0; $i < $i_max; $i++)
    $p .= "<input class=\"widget_radio\" type=\"radio\""
         .($flags & WIDGET_SUBMIT ? " onchange=\"this.form.submit();\"" : "")
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
         ."> "
         .$label_array[$i]
         .($vertical ? "<br>" : "")
         ."\n";

  return $p;
}


/*
  In PHP versions earlier than 4.1.0, $HTTP_POST_FILES should be used instead
  of $_FILES. Use phpversion() for version information.

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

  The data encoding type, enctype, MUST be specified as enctype="multipart/form-data"
  MAX_FILE_SIZE must precede the file input field
  Name of input element determines name in $_FILES array
*/
function
widget_upload ($name, $label, $tooltip, $upload_path, $max_file_size, $mime_type, $flags)
{
  if (strcasecmp ($this->method, "POST"))
    return "widget_upload() requires method=\"POST\" and enctype=\"multipart/form-data\" in the form tag\n";

  if ($flags & WIDGET_FOCUS)
    $this->focus = $name;

  $p = $this->widget_hidden ("MAX_FILE_SIZE", $max_file_size, 0)
      ."<input class=\"widget_upload\" type=\"file\""
      ." name=\""
      .$name
      ."\""
      ." title=\""
      .$tooltip
      ."\""
      .($max_file_size ? " maxlength=\"".$max_file_size."\"" : "")
      .($mime_type ? " accept=\"".$mime_type."\"" : "")
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
/*
  if (defined (UPLOAD_ERR_CANT_WRITE))
    $s[UPLOAD_ERR_CANT_WRITE] = "Failed to write file to disk";
  if (defined (UPLOAD_ERR_EXTENSION))
    $s[UPLOAD_ERR_EXTENSION] =  "File upload stopped by extension";
*/

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

  if ($this->css_flags & WIDGET_CSS_SELECT)
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
      $p .= "<select"
           .($img ? " style=\"background-image:url('".$img."');\"" : "")
           .($flags & WIDGET_SUBMIT ? " onchange=\"this.form.submit();\"" : "")
           ." name=\""
           .$name
           ."\""
           .($tooltip ? " title=\"".$tooltip."\"" : "")
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
widget_select_int ($img, $name, $img_array, $name_array, $tooltip, $flags)
{
  $value_array = Array ();
  $i_max = max (sizeof ($name_array), sizeof ($value_array));

  for ($i = 0; $i < $i_max; $i++)
    $value_array[$i] = $i;

  return $this->widget_select ($img, $name, $img_array, $name_array, $value_array, $tooltip, $flags);
}


function
widget_a ($url, $target, $img, $w, $h, $label, $tooltip, $flags)
{
  if ($flags & WIDGET_FOCUS)
    $this->focus = $name;

  $p = "";
  if ($img)
    $p .= "<a class=\"widget_a_img\""
         ." href=\""
         .$url
         ."\""
         ." target=\""
         .$target
         ."\""
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
         .($tooltip ? "<span>".$tooltip."</span>" : "")
         ."</a>";

  if ($label)
    $p .= "<a class=\"widget_a_label\""
         ." href=\""
         .$url
         ."\""
         ." target=\""
         .$target
         ."\""
         .($flags & WIDGET_DISABLED ? " disabled" : "")
         .">"
         ."<nobr>"
         .$label
         ."</nobr>"
         .($tooltip ? "<span>".$tooltip."</span>" : "")
         ."</a>";

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
      .($this->css_flags & WIDGET_CSS_IMG ? "<span>".$tooltip."</span>" : "")
      ."\n";

  return $p;
}


function
widget_trans ($w, $h, $flags)
{
  if ($flags & WIDGET_FOCUS)
    $this->focus = $name;

  return "<img class=\"widget_trans\""
        ." src=\"images/widget_trans.png\""
        .($w != -1 ? " width=\"".$w."\"" : "")
        .($h != -1 ? " height=\"".$h."\"" : "")
        ." border=\"0\""
        ." alt=\"images/widget_trans.png not found\">";
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
widget_panel ($url_array, $img_array, $w, $h, $tooltip)
{
?>
<script language="JavaScript">
<!--

//var test_array = new Array  (<?php

$p = "";
$i_max = sizeof ($img_array);  
for ($i = 0; $i < $i_max; $i++)
  {
    if ($i)
      $p .= ", ";
    $p .= "widget_panel_".$i;
  }

echo $p;
?>);

var img_w = <?php echo $w; ?>;
var img_h = <?php echo $h; ?>;
var img_n = <?php echo sizeof ($img_array); ?>;


function
js_panel_get_img_array ()
{
  var img = new Array (<?php

$p = "";
$i_max = sizeof ($img_array);
for ($i = 0; $i < $i_max; $i++)
  {
    if ($i)
      $p .= ", ";
    $p .= "widget_panel_".$i;
  }

echo $p;

?>);
  return img;
}

//-->
</script><?

  $p = "<table width=\"100%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n"
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
      ."<nobr>\n";

  $i_max = min (sizeof ($url_array), sizeof ($img_array));
  for ($i = 0; $i < $i_max; $i++)
    $p .= "<a href=\""
         .$url_array[$i]
         ."\" target=\"_blank\"><img name=\"widget_panel_"
         .$i
         ."\" src=\""
         .$img_array[$i]
         ."\" width=\""
         .$w
         ."\" height=\""
         .$h
         ."\" border=\"0\"></a>\n";

  $p .= "</nobr>\n"
       ."    </td>\n"
       ."    <td width=\"10\" valign=\"bottom\" onMouseOver=\"js_mouse_callback_func (js_panel_event_ignore);\">\n"
       ."    </td>\n"
       ."  </tr>\n"
       ."  <tr>\n"
       ."    <td height=\"10\" colspan=\"4\" onMouseOver=\"js_mouse_callback_func (js_panel_event_ignore);\">\n"
       ."    </td> \n"
       ."  </tr>\n"
       ."</table>\n";

  return $p;
}


function
widget_box_start ($img_tl, $img_t, $img_tr, $img_l, $img_r, $img_bl, $img_b, $img_br)
{
  $this->img_r  = $img_r;
  $this->img_bl = $img_bl;
  $this->img_b  = $img_b;
  $this->img_br = $img_br;

  return "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n"
        ."  <tr>\n"
        ."    <td>\n"
        ."<img src=\""
        .$img_tl
        ."\">\n"
        ."    </td>\n"
        ."    <td class=\""
        .$img_t
        ."\">\n"
        ."    </td>\n"
        ."    <td class=\""
        .$img_tr
        ."\">\n"
        ."    </td>\n"
        ."  </tr>\n"
        ."  <tr>\n"
        ."    <td class=\""
        .$img_l
        ."\">\n"
        ."    </td>\n"
        ."    <td valign=\"top\" style=\"background-color:#fff\">\n";
}


function
widget_box_end ()
{
  return "    </td>\n"
        ."    <td class=\""
        .$this->img_r
        ."\">\n"
        ."    </td>\n"
        ."  </tr>\n"
        ."  <tr>\n"
        ."    <td class=\""
        .$this->img_bl
        ."\">\n"
        ."    </td>\n"
        ."    <td class=\""
        .$this->img_b
        ."\">\n"
        ."    </td>\n"
        ."    <td> \n"
        ."<img src=\""
        .$this->img_br
        ."\" border=\"0\">\n"
        ."    </td>\n"
        ."  </tr>\n"
        ."</table>\n";
}


function
widget_audio ($url, $start, $stream, $next_stream)
{
  return "<object"
//  classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000" 
//  codebase="http://fpdownload.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=7,0,0,0" 
//  id="widget_audio"
//  movie="widget_audio.swf"
        ."><embed src=\""
        .dirname ($_SERVER['PHP_SELF'])
        ."/misc/misc_flash.swf?url="
        .$url
        ."&start="
        .$start
        ."&stream="
        .$stream
        ."&next="
        .$next_stream
        ."\""
        ." type=\"application/x-shockwave-flash\""
        ." width=\"1\""
        ." height=\"1\""
//  quality="high"
//  bgcolor="#ffffff"
//  loop="true"
//  align=""
//  allowScriptAccess="sameDomain"
//  swLiveConnect="true"
       ." pluginspace=\"http://www.macromedia.com/go/flashplayer/\""
       ."></embed></object>";
}


function
widget_slider ($name, $value, $tooltip, $vertical, $flags)
{

  return "<div class=\"slider\" id=\"slider-1\" tabIndex=\"1\" style=\"width:auto; margin:10px;\">"
        ."<input class=\"slider-input\" id=\"slider-input-1\"/>"
        ."</div>";
}


function
widget_tabs ($name, $value_array, $label_array, $tooltip, $vertical, $flags)
{
//  return $this->widget_radio ($name, $value_array, $label_array, $tooltip, $vertical, $flags);
  $p = "<table border=\"0\"><tr>";

  $i_max = sizeof ($value_array);
  for ($i = 0; $i < $i_max; $i++)
    $p .= "<td>"
         .$label_array[$i]
         ."</td>";

  $p .= "</tr></table>";

  return $p;
}


function
widget_tree ($name, $path, $mime_type, $flags)
{
  $p = "";
  $dir = opendir ($path);
  while (($file = readdir ($dir)) != false)
    {
      if (is_dir ($file))
        {
          $p .= "<img src=\"images/widget_tree_closed.png\" border=\"0\" alt=\"images/widget_tree_open.png\">"
                 .basename ($file);
        }
      else if (is_file ($file))
        {
          $stat = stat ($file);
          $p .= "<img src=\"images/widget_tree_file.png\" border=\"0\" alt=\"images/widget_tree_file.png\">"
               .basename ($file)
               .$stat['size'];
        }
      else // ?
        {
          $p .= "<img src=\"images/widget_tree_file.png\" border=\"0\" alt=\"images/widget_tree_file.png\">"
               .basename ($file);
        }

      $p .= "<br>\n";
    }
  closedir ($dir);

  return $p;
}


function
widget_relate ($title, $url, $flags)
{
/*
search widget to include in other pages

start page
<a href="#" onclick="this.style.behavior='url(#default#homepage)';this.setHomePage('http://torrent-finder.com');" style="background-image: url(images/home.gif);" title="Start your browser with Torrent Search">Make us your start page </a></li>

link to us
<a href="link-to-us.php" style="background-image: url(images/link.gif);" title="Add our link to your blog or website">Link to us</a></li>

tell a friend
<a href="send-to-friend.php" onclick="return ppup('send-to-friend.php','610','410');" style="background-image: url(images/friend.gif);" title="Send this link to your friends">Tell a friend</a></li>

add site (to many news sites)
function jumpto(trgt)
{
        url = "http://torrent-finder.com/go?id="+trgt;
        window.open(url,'','status=yes,scrollbars=yes,resizable=yes,location=yes,titlebar=yes,toolbar=yes');
        return false;
}


 <h2> Add Torrent Finder</h2>
  <div style="padding-left: 2px;">
<img src="images/addto.jpg" usemap="#AddTo" border="0" height="61" width="163">
    <map name="AddTo" id="AddTo">
<area shape="rect" coords="-1,1,17,16" href="http://torrent-finder.com" onclick="return jumpto('icio');" alt="Add to del.icio.us" title="Add to del.icio.us">
<area shape="rect" coords="16,1,32,16" href="http://torrent-finder.com" onclick="return jumpto('digg');" alt="Digg This!" title="Digg This!">
<area shape="rect" coords="117,47,133,63" href="http://torrent-finder.com" onclick="return jumpto('Maple');" alt="Add to Maple" title="Add to Maple">
<area shape="rect" coords="42,47,59,63" href="http://torrent-finder.com" onclick="return jumpto('AddToAny');" alt="Add to AnY" title="Add to AnY">
<area shape="rect" coords="59,47,72,64" href="http://torrent-finder.com" onclick="return jumpto('Connotea');" alt="Add to Connotea" title="Add to Connotea">
<area shape="rect" coords="71,47,89,63" href="http://torrent-finder.com" onclick="return jumpto('mylinkvault');" alt="Add to mylinkvault" title="Add to mylinkvault">
<area shape="rect" coords="88,47,102,63" href="http://torrent-finder.com" onclick="return jumpto('Nowpublic');" alt="Add to Nowpublic" title="Add to Nowpublic">
<area shape="rect" coords="102,48,117,63" href="http://torrent-finder.com" onclick="return jumpto('Nextaris');" alt="Add to Nextaris" title="Add to Nextaris">
<area shape="rect" coords="150,31,165,48" href="http://torrent-finder.com" onclick="return jumpto('Unalog');" alt="Add to Unalog" title="Add to Unalog">
<area shape="rect" coords="108,31,120,48" href="http://torrent-finder.com" onclick="return jumpto('TailRank');" alt="Add to TailRank" title="Add to TailRank">
<area shape="rect" coords="93,31,108,48" href="http://torrent-finder.com" onclick="return jumpto('Netvouz');" alt="Add to Netvouz" title="Add to Netvouz">
<area shape="rect" coords="76,31,93,47" href="http://torrent-finder.com" onclick="return jumpto('feedmarker');" alt="Add to feedmarker" title="Add to feedmarker">
<area shape="rect" coords="59,29,76,47" href="http://torrent-finder.com" onclick="return jumpto('RawSugar');" alt="Add to RawSugar" title="Add to RawSugar">
<area shape="rect" coords="43,31,59,47" href="http://torrent-finder.com" onclick="return jumpto('ma.gnolia');" alt="Add to ma.gnolia" title="Add to ma.gnolia">
<area shape="rect" coords="1,31,16,47" href="http://torrent-finder.com" onclick="return jumpto('simpy');" alt="Add to Simpy" title="Add to Simpy">
<area shape="rect" coords="33,31,43,47" href="http://torrent-finder.com" onclick="return jumpto('blogmarks');" alt="Add to Blogmarks" title="Add to Blogmarks">
<area shape="rect" coords="119,31,135,47" href="http://torrent-finder.com" onclick="return jumpto('HLOM');" alt="Add to HLOM" title="Add to HLOM">
<area shape="rect" coords="135,31,150,48" href="http://torrent-finder.com" onclick="return jumpto('Onlywire');" alt="Add to Onlywire" title="Add to Onlywire">
<area shape="rect" coords="147,16,164,31" href="http://torrent-finder.com" onclick="return jumpto('spurl');" alt="Spurl This!" title="Spurl This!">
<area shape="rect" coords="107,16,121,31" href="http://torrent-finder.com" onclick="return jumpto('shadows');" alt="Tag to Shadows" title="Tag to Shadows">
<area shape="rect" coords="92,16,107,31" href="http://torrent-finder.com" onclick="return jumpto('Smarking');" alt="Add to Smarking" title="Add to Smarking">
<area shape="rect" coords="77,15,92,31" href="http://torrent-finder.com" onclick="return jumpto('blinklist');" alt="Add to BlinkList" title="Add to BlinkList">
<area shape="rect" coords="61,15,77,31" href="http://torrent-finder.com" onclick="return jumpto('Squidoo');" alt="Add to Squidoo" title="Add to Squidoo">
<area shape="rect" coords="46,16,61,31" href="http://torrent-finder.com" onclick="return jumpto('reddit');" alt="Add to Reddit" title="Add to Reddit">
<area shape="rect" coords="0,16,16,31" href="http://torrent-finder.com" onclick="return jumpto('Taggle');" alt="Add to Taggle.de" title="Add to Taggle.de">
<area shape="rect" coords="30,16,47,31" href="http://torrent-finder.com" onclick="return jumpto('Scuttle');" alt="Add to Scuttle" title="Add to Scuttle">
<area shape="rect" coords="121,16,133,31" href="http://torrent-finder.com" onclick="return jumpto('newsvine');" alt="Add to Newsvine" title="Add to Newsvine">
<area shape="rect" coords="16,16,30,31" href="http://torrent-finder.com" onclick="return jumpto('Wink!');" alt="Add to Wink" title="Add to Wink">
<area shape="rect" coords="133,16,147,31" href="http://torrent-finder.com" onclick="return jumpto('furl');" alt="Add to Furl" title="Add to Furl">
<area shape="rect" coords="146,1,163,16" href="http://torrent-finder.com" onclick="return jumpto('Slashdot');" alt="Add to Slashdot" title="Add to Slashdot">
<area shape="rect" coords="129,1,146,16" href="http://torrent-finder.com" onclick="return jumpto('Linkatopia');" alt="Add to Linkatopia" title="Add to Linkatopia">
<area shape="rect" coords="32,1,48,17" href="http://torrent-finder.com" onclick="return jumpto('Technorati');" alt="Add to Technorati" title="Add to Technorati">
<area shape="rect" coords="48,0,62,17" href="http://torrent-finder.com" onclick="return jumpto('yahoo');" alt="Add to Yahoo!" title="Add to Yahoo!">
<area shape="rect" coords="62,0,78,16" href="http://torrent-finder.com" onclick="return jumpto('google');" alt="Add to Google" title="Add to Google">
<area shape="rect" coords="78,0,95,16" href="http://torrent-finder.com" onclick="return jumpto('Live');" alt="Add to Windows Live" title="Add to Windows Live">
<area shape="rect" coords="95,1,111,16" href="http://torrent-finder.com" onclick="return jumpto('ask');" alt="Add to Jeeves" title="Add to Jeeves">
<area shape="rect" coords="111,1,129,16" href="http://torrent-finder.com" onclick="return jumpto('Protopage');" alt="Add to Protopage" title="Add to Protopage">
<area shape="rect" coords="16,31,33,47" href="http://torrent-finder.com" onclick="return jumpto('Taggly');" alt="Add to Taggly" title="Add to Taggly">
</map>
</div>
*/
  $p = "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" style=\"background-color:#fff;\">\n"
      ."<tr><td>\n"
      ."<font size=\"-1\" face=\"arial,sans-serif\">\n";

//  if ($flags & WIDGET_RELATE_LINKTOUS)
//    $p .= "link to us code";

  if ($flags & WIDGET_RELATE_DELICIOUS)
    $p .= "<img class=\"widget_relate_img\" src=\"images/widget_relate_delicious.png\" border=\"0\">"
         ."<a class=\"widget_relate_label\" href=\"http://del.icio.us/post\">del.icio.us</a><br>\n";

  if ($flags & WIDGET_RELATE_DIGG)
    {
      $p .= "<img class=\"widget_relate_img\" src=\"images/widget_relate_digg.png\" border=\"0\">"
           ."<a class=\"widget_relate_label\" href=\"http://digg.com/submit\">Digg this page</a><br>\n";
/*
?>
<script type="text/javascript" src="http://digg.com/api/diggthis.js"></script>
<?php

  $p .= "<script>\n"
       ."digg_url = '"
       .$url
       ."';\n"
       ."</script>\n"
*/
    }

  if ($flags & WIDGET_RELATE_FRESHMEAT)
    $p .= "<img class=\"widget_relate_img\" src=\"images/widget_relate_fm.png\" border=\"0\">"
         ."<a class=\"widget_relate_label\" href=\"http://freshmeat.net\">Rate this project</a><br>\n";

  if ($flags & WIDGET_RELATE_BOOKMARK)
    $p .= "<img class=\"widget_relate_img\" src=\"images/widget_relate_star.png\" border=\"0\">"
         ."<a class=\"widget_relate_label\" href=\"javascript:js_bookmark('"
         .$title
         ."', '"
         .$url
         ."')\" border=\"0\">Bookmark</a><br>\n";

  if ($flags & WIDGET_RELATE_SEARCH)
    $p .= "<img class=\"widget_relate_img\" src=\"images/widget_relate_star.png\" border=\"0\">"
         ."<a class=\"widget_relate_label\" href=\"javascript:js_bookmark('"
         .$title
         ."', '"
         .$url
         ."')\" border=\"0\">Add search to sidebar</a><br>\n";

  $p .= "</font></td></tr></table>\n";

  return $p; 
}


}


function
widget_test ($w)
{
  $img_array = Array ("images/logo.png", "images/logo.png", "images/logo.png");
  $name_array = Array ("name1", "name2", "name3");
  $value_array = Array ("value1", "value2", "value3");
  $label_array = Array ("label1", "label2", "label3");
  $url_array = Array ("", "", "", "", "");
  $img_array2 = Array ("images/panel1.png", 
                       "images/panel2.png",
                       "images/panel3.png",
                       "images/panel4.png",
                       "images/panel5.png");

  $p = ""
      .$w->widget_start ("widget_start_name", $_SERVER['PHP_SELF'], "POST")
      ."<br>widget_img(): "
      .$w->widget_img ("logo", "images/logo.png", -1, -1, 0, "alt", "tooltip", 0) // w, h, border
      ."<hr>widget_submit(): "
      .$w->widget_submit ("name", "label", "tooltip", 0)
      ."<hr>widget_reset(): "
      .$w->widget_reset ("name", "label", "tooltip", 0)
      ."<hr>widget_image(): "
      .$w->widget_image ("name", "value", "images/logo.png", "label", -1, -1, "tooltip", 0) // w, h
      ."<hr>widget_checkbox(): "
      .$w->widget_checkbox ("name", "tooltip", WIDGET_CHECKED) // checked
      ."<hr>widget_radio(vertical): <br>"
      .$w->widget_radio ("name", $value_array, $label_array, "tooltip", 1, 0)
      ."<hr>widget_text(): "
      .$w->widget_text ("name", "value", "tooltip", 50, 50, WIDGET_FOCUS) // size, maxsize
      ."<hr>widget_upload(): "
      .$w->widget_upload ("upload_file", "label", "tooltip", "/mnt/incoming", 4096, NULL, 0)
      ."<hr>widget_password(): "
      .$w->widget_password ("name", "tooltip", 0)
      ."<hr>widget_hidden(): "
      .$w->widget_hidden ("name", "value", 0)
      ."<hr>widget_textarea(): "
      .$w->widget_textarea ("name", "value", "tooltip", 35, 10, 0) // cols, rows
      ."<hr>widget_textarea(WIDGET_DISABLED): "
      .$w->widget_textarea ("name", "value", "tooltip", 35, 10, WIDGET_DISABLED) // cols, rows
      ."<hr>widget_select(): "
      .$w->widget_select (NULL, "name", $img_array, $name_array, $value_array, "tooltip", 0)
      ."<hr>widget_a(): "
      .$w->widget_a ("url", NULL, "images/logo.png", -1, -1, "label", "tooltip", 0)
      ."<hr>widget_trans(): "
      .$w->widget_trans (100, 100, 0)
      ."<hr>widget_gauge(): "
      .$w->widget_gauge (20, 0)
      ."<hr>widget_panel (): "
      .$w->widget_panel ($url_array, $img_array2, 85, 68, "tooltip")
      ."<hr>widget_box_start() + \"test\" + widget_box_end(): "
      .$w->widget_box_start ("images/box_tl.png",
                             "images/box_t.png",
                             "images/box_tr.png",
                             "images/box_l.png",
                             "images/box_r.png",
                             "images/box_bl.png",
                             "images/box_b.png",
                             "images/box_br.png", 0)
      ."test"
      .$w->widget_box_end ()
      ."<hr>widget_tabs(horizontal): "
      .$w->widget_tabs ("name", $value_array, $label_array, "tooltip", 0, 0)
      ."<hr>widget_tree(): <br>"
      .$w->widget_tree ("name", "/mnt/incoming", NULL, 0)
//      ."<hr>widget_audio(): "
//      .$w->widget_audio ("test.mp3", 0, 0)
//      .$w->widget_map () ? misc/map->png   earth|moon
//      .$w->widget_login () ?
      ."<hr>widget_slider (horizontal): "
      .$w->widget_slider ("name", "value", "tooltip", 0, 0)
      ."<hr>widget_slider (vertical): "
      .$w->widget_slider ("name", "value", "tooltip", 1, 0)
      ."<hr>widget_relate (WIDGET_RELATE_ALL): "
      .$w->widget_relate ("title", "http://localhost", WIDGET_RELATE_ALL)
      .$w->widget_end ();


  echo $p;

?><hr><pre><tt>
$css_flags
  WIDGET_CSS_A      include CSS code for widget_a()
  WIDGET_CSS_SELECT include CSS code for widget_select()
  WIDGET_CSS_IMG    include CSS code for widget_img()
  WIDGET_CSS_BOX    include CSS code for widget_box()
  WIDGET_CSS_SLIDER include CSS code for widget_slider()

$js_flags
  WIDGET_JS_MISC    include miscellaneous JS functions
  WIDGET_JS_EVENT   include JS code for mouse and keyboard events
  WIDGET_JS_WINDOW  include JS code for window manipulation
  WIDGET_JS_SLIDER  include JS code for widget_slider()
  WIDGET_JS_PANEL   include JS code for widget_panel()
  WIDGET_JS_RELATE  include JS code for widget_relate()

$flags
  WIDGET_RO         widget_*() is read-only
  WIDGET_FOCUS      widget_*() has focus
  WIDGET_SUBMIT     widget_*() sends a submit
  WIDGET_CHECKED    widget_checkbox() is checked (ignored by other widgets)
  WIDGET_DISABLED   widget_*() is disabled


$w = new misc_widget;

$w->widget_init ($css_flags, $js_flags);

$w->widget_start ($name, $target, $method);

$w->widget_a ($url, $target, $img, $w, $h, $label, $tooltip, $flags);
$w->widget_audio ($url, $start, $stream);
$w->widget_box_start ($img_tl, $img_t, $img_tr, $img_l, $img_r, $img_bl, $img_b, $img_br, $flags);
$w->widget_box_end ();
$w->widget_checkbox ($name, $tooltip, $flags);
$w->widget_gauge ($percent, $flags);
$w->widget_hidden ($name, $value, $flags);
$w->widget_image ($name, $value, $img, $w, $h, $tooltip, $flags);
$w->widget_img ($name, $img, $w, $h, $border, $alt, $tooltip, $flags);
$w->widget_panel ($url_array, $img_array, $w, $h, $tooltip);
$w->widget_password ($name, $tooltip, $flags);
$w->widget_radio ($name, $value_array, $label_array, $tooltip, $vertical, $flags);
$w->widget_reset ($name, $label, $tooltip, $flags);
$w->widget_select ($img, $name, $img_array, $name_array, $value_array, $tooltip, $flags);
$w->widget_slider ($name, $value, $tooltip, $vertical, $flags);
$w->widget_submit ($name, $label, $tooltip, $flags);
$w->widget_text ($name, $value, $tooltip, $size, $maxlength, $flags);
$w->widget_textarea ($name, $value, $tooltip, $cols, $rows, $flags);
$w->widget_trans ($w, $h, $flags);
$w->widget_upload ($name, $label, $tooltip, $upload_path, $max_file_size, $mime_type, $flags);

$w->widget_end ();

<tt></pre><?php

}


?>