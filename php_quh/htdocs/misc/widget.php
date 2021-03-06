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
define ("WIDGET_CSS_START",  1<<6);
define ("WIDGET_CSS_ALL",    WIDGET_CSS_A|
                             WIDGET_CSS_SELECT|
                             WIDGET_CSS_IMG|
                             WIDGET_CSS_BOX|
                             WIDGET_CSS_SLIDER|
                             WIDGET_CSS_RELATE|
                             WIDGET_CSS_START);
// widget_init() js flags
define ("WIDGET_JS_EVENT",  1);
define ("WIDGET_JS_MISC",   1<<1);
define ("WIDGET_JS_WINDOW", 1<<2);
define ("WIDGET_JS_PANEL",  1<<3);
define ("WIDGET_JS_SLIDER", 1<<5);
define ("WIDGET_JS_RELATE", 1<<6);
define ("WIDGET_JS_ALL",    WIDGET_JS_EVENT|
                            WIDGET_JS_MISC|
                            WIDGET_JS_WINDOW|
                            WIDGET_JS_PANEL|
                            WIDGET_JS_SLIDER|
                            WIDGET_JS_RELATE);
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
define ("WIDGET_RELATE_DONATE",    1<<13);
define ("WIDGET_RELATE_RSSFEED",   1<<14);
define ("WIDGET_RELATE_STARTPAGE", 1<<15);
define ("WIDGET_RELATE_DIGG_THIS", 1<<16);
define ("WIDGET_RELATE_ALL",       WIDGET_RELATE_DIGG|
                                   WIDGET_RELATE_DELICIOUS|
                                   WIDGET_RELATE_BOOKMARK|
                                   WIDGET_RELATE_SEARCH|
                                   WIDGET_RELATE_LINK|
                                   WIDGET_RELATE_FRESHMEAT|
                                   WIDGET_RELATE_LINKTOUS|
                                   WIDGET_RELATE_RSSFEED);
define ("WIDGET_ADSENSE_TEXT",  "text");
define ("WIDGET_ADSENSE_IMAGE", "image");
define ("WIDGET_ADSENSE_BOTH",  "text_image");


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

      if ($css_flags & WIDGET_CSS_RELATE)
        $p .= "<link rel=\"stylesheet\" type=\"text/css\" media=\"all\" href=\"css/start.css\">\n";

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

  return "<form class=\"widget_start\" name=\""
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
  $p = "</form>"; // TODO: no line-feed after </form>

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

  $p .= sprint_r ($_FILES); // debug

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

  $p .= "\n\n\n".sprint_r ($s); // debug

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
      ." name=\""
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
        ."misc/misc_flash.swf?url="
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
widget_relate ($title, $url, $vertical, $flags)
{
  $p = "";

//  $p .= "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" style=\"background-color:#fff;\">\n"
//       ."<tr><td>\n";
  $p .= "<font size=\"-1\" face=\"arial,sans-serif\">\n";

  if ($flags & WIDGET_RELATE_DONATE)
    $p .= "<img class=\"widget_relate_img\" src=\"images/widget_relate_donate.png\" border=\"0\">"
         ."<a class=\"widget_relate_label\" href=\"http://paypal.com\">Donate</a>\n"
/*
<pre>* * *   D O N A T I O N S   A R E   A C C E P T E D   * * *</pre><br>
<br>
<img src="images/widget_relate_refrigator.jpg" border="0"><br>
<br>
Individuals and companies can now donate funds to support me and keep me from<br>
writing proprietary software.<br>
<br>
Thank You!<br>
<br>
<pre>* * *   D O N A T I O N S   A R E   A C C E P T E D   * * *</pre><br-->
search widget to include in other pages
*/
         .($vertical ? "<br>" : "");

  if ($flags & WIDGET_RELATE_STARTPAGE)
    $p .= "<img class=\"widget_relate_img\" src=\"images/widget_relate_home.png\" border=\"0\">"
         ."<a class=\"widget_relate_label\""
         ." onclick=\"this.style.behavior='url(#default#homepage)';this.setHomePage('http://torrent-finder.com');\""
         .">Make us your start page</a>"
         .($vertical ? "<br>" : "");
/*
link to us
<a href="link-to-us.php" style="background-image: url(images/link.gif);" title="Add our link to your blog or website">Link to us</a></li>

tell a friend
<a href="send-to-friend.php" onclick="return ppup('send-to-friend.php','610','410');" style="background-image: url(images/friend.gif);" title="Send this link to your friends">Tell a friend</a></li>
*/

//  if ($flags & WIDGET_RELATE_LINKTOUS)
//    $p .= "link to us code";

  if ($flags & WIDGET_RELATE_DELICIOUS)
    $p .= "<img class=\"widget_relate_img\" src=\"images/widget_relate_delicious.png\" border=\"0\">"
         ."<a class=\"widget_relate_label\" href=\"http://del.icio.us/post\">del.icio.us</a>\n"
         .($vertical ? "<br>" : "");

  if ($flags & WIDGET_RELATE_DIGG)
    $p .= "<img class=\"widget_relate_img\" src=\"images/widget_relate_digg.png\" border=\"0\">"
         ."<a class=\"widget_relate_label\" href=\"http://digg.com/submit\">digg it</a>\n"
         .($vertical ? "<br>" : "");

  if ($flags & WIDGET_RELATE_DIGG_THIS)
    $p .= "<img class=\"widget_relate_img\" src=\"images/widget_relate_digg.png\" border=\"0\">"
         ."<a class=\"widget_relate_label\" href=\"http://digg.com/submit\">digg it</a>\n"
         .($vertical ? "<br>" : "");

/*
<script><!--
digg_url = '<?php echo $url; ?>';
//--></script>
<script type="text/javascript" src="http://digg.com/api/diggthis.js"></script>
*/
/*
  if ($flags & WIDGET_RELATE_FRESHMEAT)
    $p .= "<img class=\"widget_relate_img\" src=\"images/widget_relate_fm.png\" border=\"0\">"
         ."<a class=\"widget_relate_label\" href=\"http://freshmeat.net\">Rate project</a>\n"
         .($vertical ? "<br>" : "");

  if ($flags & WIDGET_RELATE_BOOKMARK)
    $p .= "<img class=\"widget_relate_img\" src=\"images/widget_relate_star.png\" border=\"0\">"
         ."<a class=\"widget_relate_label\" href=\"javascript:js_bookmark('"
         .$title
         ."', '"
         .$url
         ."')\" border=\"0\">Bookmark</a>\n"
         .($vertical ? "<br>" : "");

  if ($flags & WIDGET_RELATE_SEARCH)
    $p .= "<img class=\"widget_relate_img\" src=\"images/widget_relate_star.png\" border=\"0\">"
         ."<a class=\"widget_relate_label\" href=\"javascript:js_bookmark('"
         .$title
         ."', '"
         .$url
         ."')\" border=\"0\">Add search</a>\n"
         .($vertical ? "<br>" : "");
*/

/*
  if ($flags & WIDGET_RELATE_RSSFEED)
    $p .= "<img class=\"widget_relate_img\" src=\"images/widget_relate_rss.png\" border=\"0\">"
         ."<a class=\"widget_relate_label\" href=\"javascript:js_bookmark('"
         .$title
         ."', '"
         .$url
         ."')\" border=\"0\">RSS feed</a>\n";
*/
  $p .= "</font>";
//  $p .= "</td></tr></table>\n";

  return $p; 
}


function
widget_adsense ($client, $type, $border_color, $flags)
{
/*

"text_image"
"text"
"image"

<option value="728x90"> 728 x 90 Leaderboard 
<option value="468x60"> 468 x 60 Banner 
<option value="234x60"> 234 x 60 Half Banner 
<option value="120x600"> 120 x 600 Skyscraper 
<option value="160x600"> 160 x 600 Wide Skyscraper 
<option value="120x240"> 120 x 240 Vertical Banner 
<option value="336x280"> 336 x 280 Large Rectangle 
<option value="300x250"> 300 x 250 Medium Rectangle 
<option value="250x250"> 250 x 250 Square 
<option value="200x200"> 200 x 200 Small Square 
<option value="180x150"> 180 x 150 Small Rectangle 
<option value="125x125"> 125 x 125 Button 

format:
WxH_as

<option value="728x15"> 728 x 15 
<option value="468x15"> 468 x 15 
<option value="200x90"> 200 x 90 
<option value="180x90"> 180 x 90 
<option value="160x90"> 160 x 90 
<option value="120x90"> 120 x 90 

format:
WxH_0ads_al (4 lines)
WxH_0ads_al_s (5 lines)

*/

  $p = explode ("x", $flags, 2);
  $w = $p[0];
  $p = explode ("_", $p[1], 2);
  $h = $p[0];

  return "<script type=\"text/javascript\"><!--\n"
        ."google_ad_client = \""
        .$client
        ."\";\n"
        ."google_ad_width = "
        .$w
        .";\n"
        ."google_ad_height = "
        .$h
        .";\n"
        ."google_ad_format = \""
        .$flags
        ."\";\n"
        .($type ? "google_ad_type = \"".$type."\";\n" : "")
        ."google_ad_channel = \"\";\n"
        ."google_color_border = \""
        .$border_color
        ."\";\n"
        ."//google_color_bg = \"000000\";\n"
        ."//google_color_link = \"0000EF\";\n"
        ."//google_color_text = \"FFFFFF\";\n"
        ."//google_color_url = \"FFFFFF\";\n"
        ."//-->\n"
        ."</script>\n"
        ."<script type=\"text/javascript\" src=\"http://pagead2.googlesyndication.com/pagead/show_ads.js\">"
        ."</script>\n";
}


/*
function
widget_adsense2 ($client, $w, $h, $border_color, $flags)
{
  // sizes in w and h
  $size = Array (
      728 = 90,
      468 = 60,
      728 = 90,
      468 = 60,
      234 = 60,
      120 = 600,
      160 = 600,
      120 = 240,
      336 = 280,
      300 = 250,
      250 = 250,
      200 = 200,
      180 = 150,
      125 = 125
    );

  return "<script type=\"text/javascript\"><!--\n"
        ."google_ad_client = \""
        .$client
        ."\";\n"
        ."google_ad_width = "
        .$w
        .";\n"
        ."google_ad_height = "
        .$h
        .";\n"
        ."google_ad_format = \"".$w."x".$h."_as\";\n"
        .($flags ? "google_ad_type = \"".$flags."\";\n" : "")
        ."google_ad_channel = \"\";\n"
        ."google_color_border = \""
        .$border_color
        ."\";\n"
        ."//google_color_bg = \"000000\";\n"
        ."//google_color_link = \"0000EF\";\n"
        ."//google_color_text = \"FFFFFF\";\n"
        ."//google_color_url = \"FFFFFF\";\n"
        ."//-->\n"
        ."</script>\n"
        ."<script type=\"text/javascript\" src=\"http://pagead2.googlesyndication.com/pagead/show_ads.js\">"
        ."</script>\n";
}
*/


function
widget_pos_start ($x, $y)
{
}


function
widget_pos_end ()
{
}


function
widget_hidden_previous_request ()
{
// inserts all names and values of the previous request
// that are not used in this form as hidden names and values
}


function
widget_maps ()
{
// shows google maps by ip(geoip?), country, city, or long/lat
}


};



?>