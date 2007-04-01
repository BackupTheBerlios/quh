<?php
/*
php_quh.php - PHP MP3 player and (optional) frontend for Quh

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
<?php
require_once ("misc/widget.php");


  $w = new misc_widget;
  $w->widget_init (0, // css flags
                   WIDGET_JS_WINDOW); // js flags
?>

<a href="javascript:js_window_open ('php_quh.php',
                                    'mywindow',
/*                                    'fullscreen=yes,'
*/                                  'width=415,'
                                   +'height=650,'
                                   +'resizable=no,'
                                   +'scrollbars=no,'
                                   +'toolbar=no,'
                                   +'location=no,'
                                   +'directories=no,'
                                   +'status=no,'
                                   +'menubar=no,'
                                   +'copyhistory=no');">Start</a>
*/
?><html>
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
  $php_quh_prev_file_value = get_request_value ("php_quh_prev_file");
  $php_quh_next_file_value = get_request_value ("php_quh_next_file");

  $php_quh_file_value = get_request_value ("php_quh_file");
  $php_quh_start_value = get_request_value ("php_quh_start");
  $php_quh_stream_value = get_request_value ("php_quh_stream");
  $php_quh_pos_value = get_request_value ("php_quh_pos");

  $php_quh_back_value = get_request_value ("php_quh_back");
  $php_quh_play_value = get_request_value ("php_quh_play");
  $php_quh_pause_value = get_request_value ("php_quh_pause");
  $php_quh_stop_value = get_request_value ("php_quh_stop");
  $php_quh_next_value = get_request_value ("php_quh_next");

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

  $tab_value_array = Array ("0", "1", "2", "3", "4");
  $tab_label_array = Array ("Player", "Add/Upload Song", "Lyrics", "Wiki", "Preferences");

  $p = "";
//  $p = "<iframe src=\"\" name=\"php_quh_ui\">"
//       "Your Browser does not support IFRAME tags.";

  $p .= $w->widget_start ("php_quh_form", "", "POST")
       .$w->widget_select (NULL, "php_quh_tab", NULL, $tab_label_array, $tab_value_array, "Choose function", WIDGET_SUBMIT);
//        $w->widget_tabs ("php_quh_tab", $tab_value_array, $tab_label_array, "Choose function", 0, WIDGET_SUBMIT);


  if ($php_quh_tab_value == 1)
    $p .= php_quh_upload ($w);
  else if ($php_quh_tab_value == 2)
    $p .= php_quh_lyrics ($w);
  else if ($php_quh_tab_value == 3)
    $p .= php_quh_wiki ($w);
  else if ($php_quh_tab_value == 4)
    $p .= php_quh_prefs ($w);
  else
    {
      $p .= php_quh_player ($w,
                            $php_quh_prev_file_value,
                            $php_quh_next_file_value,
                            $php_quh_file_value,
                            $php_quh_start_value,
                            $php_quh_stream_value,
                            $php_quh_pos_value);
    }


  $p .= $w->widget_end ()
//       ."</iframe>"
;
  echo $p;
  
?></font></body></html>