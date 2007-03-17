<html>
<head>
<title>php_quh 0.1.0</title>
<?php
//phpinfo ();
require_once ("config.php");
require_once ("misc/widget.php");
require_once ("misc/misc.php");
require_once ("MP3/Id.php");
require_once ("php_quh_misc.php");

  $w = new misc_widget;
  $w->widget_init (WIDGET_CSS_A, // css flags
                   0); // js flags

  $php_quh_value = get_request_value ("php_quh");
  $php_quh_tab_value = get_request_value ("php_quh_tab");

  // player
  $php_quh_file_value = get_request_value ("php_quh_file");
  $php_quh_start_value = get_request_value ("php_quh_start");
  $php_quh_stream_value = get_request_value ("php_quh_stream");
  $php_quh_pos_value = get_request_value ("php_quh_pos");

?>
<style type="text/css">
div
{
  background-image:url('images/black_opaque80.png');
//  background-color:#000;
  width:400px;
  height:400px;
  overflow:scroll;
}

body
{ 
//  background-color:#39385a;
//  color:#00e300;
//  background-image:url('images/bg2.png');
//  background-repeat:repeat-y;
//  background-position:top center;
  background-color:AppWorkspace;
}
</style>
</head>
<body link="#00e300" vlink="#00e300" alink="#00e300"><!-- oncontextmenu="return false;"-->
<font face="arial,sans-serif" size="-1">
<?php

  $tab_value_array = Array ("0", "1", "2");
  $tab_label_array = Array ("Player", "Add/Upload Song", "Preferences");

  $p = "";

  $p .= $w->widget_start ("php_quh_form", "", "POST")
       .$w->widget_select (NULL, "php_quh_tab", NULL, $tab_label_array, $tab_value_array, "Choose function", WIDGET_SUBMIT);
//        $w->widget_tabs ("php_quh_tab", $tab_value_array, $tab_label_array, "Choose function", 0, WIDGET_SUBMIT);


  switch ($php_quh_tab_value)
    {
      case 2:
        $p .= php_quh_prefs ($w);
        break;

      case 1:
        $p .= php_quh_upload ($w);
        break;

      default:
        $p .= php_quh_player ($w, $php_quh_file_value, $php_quh_start_value, $php_quh_stream_value, $php_quh_pos_value);
    }


  $p .= $w->widget_end ();

  echo $p;
  
?></font></body></html>