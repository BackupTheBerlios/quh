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
*/
?><html>
<head>
<title>php_quh 0.1.0 alpha</title>
<?php
//phpinfo ();
require_once ("config.php");
require_once ("misc/widget.php");
require_once ("misc/misc.php");
require_once ("MP3/Id.php");
require_once ("php_quh_misc.php");

class php_quh
{
  var $tab;

  // player
  var $prev_file;
  var $next_file;

  var $file;
  var $start;
  var $stream;
  var $pos;

  var $back;
  var $play;
  var $pause;
  var $stop;
  var $next;

  var $incoming;
  var $incoming_abs;
  var $incoming_url;

//  var $skin;
//  var $skin_abs;
};

  $w = new misc_widget;
  $w->widget_init (WIDGET_CSS_A, // css flags
                   0); // js flags

  $php_quh = new php_quh;

  $php_quh->view = get_request_value ("view");
  $php_quh->tab = get_request_value ("tab");

  $php_quh->prev_file = get_request_value ("prev_file");
  $php_quh->next_file = get_request_value ("next_file");

  $php_quh->file = get_request_value ("file");
  $php_quh->start = get_request_value ("start");
  $php_quh->stream = get_request_value ("stream");
  $php_quh->pos = get_request_value ("pos");

  $php_quh->back = get_request_value ("back");
  $php_quh->play = get_request_value ("play");
  $php_quh->pause = get_request_value ("pause");
  $php_quh->stop = get_request_value ("stop");
  $php_quh->next = get_request_value ("next");

  $php_quh->incoming = $incoming;
  $php_quh->incoming_abs = $incoming_abs;
  $php_quh->incoming_url = $incoming_url;
//  $skin = dirname ($_SERVER['PHP_SELF'])."/skin/";
//  $skin_abs = dirname ($_SERVER['SCRIPT_FILENAME'])."/skin/";

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
//  background-color:AppWorkspace;
  background-color:#dedbd6;
}
</style>
</head>
<body link="#00e300" vlink="#00e300" alink="#00e300"><!-- oncontextmenu="return false;"-->
<font face="arial,sans-serif" size="-1">
<?php
  $p = "";

      $tab_label_array = Array ("Player", "Add/Upload Song", "Info");
      $p .= $w->widget_start ("form", "", "POST");
//           .$w->widget_select_int (NULL, "tab", NULL, $tab_label_array, "Choose function", WIDGET_SUBMIT)
//           .$w->widget_tabs ("tab", $tab_value_array, $tab_label_array, "Choose function", 0, WIDGET_SUBMIT);
      $p .= $w->widget_audio ($php_quh->incoming_url.$php_quh->file, $php_quh->start, $php_quh->stream, $php_quh->next_file);
//echo "|".$php_quh->incoming."|".$php_quh->incoming_abs."|";

//      switch ($php_quh->tab)
        {
//          case 1:
            $p .= php_quh_upload ($w, $php_quh);
//            break;

//          case 2:
//            $p .= php_quh_info ($w, $php_quh);
//            break;

//          default:
            $p .= php_quh_player ($w, $php_quh);
//        }

      $p .= $w->widget_end ();
    }

  echo $p;

?></font></body></html>