<?php
/*
misc.php - miscellaneous functions

Copyright (c) 2006 NoisyB


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


function
time_ms ()
// returns milliseconds since midnight
{
  $tv = gettimeofday ();

  if ($tv)
    {
      $t = $tv['usec'] / 1000;
      $t += ($tv['sec'] % 86400) * 1000;
    }

  return $t;
}


function
window_open ()
{
/*
require_once ("misc/widget.php");


  $w = new misc_widget;
  $w->widget_init (0, // css flags
                   WIDGET_JS_WINDOW); // js flags
?>
<a href="javascript:js_window_open ('ripalot.php',
                                    'mywindow',
                                    'width=450,'
                                   +'height=450,'
                                   +'resizable=no,'
                                   +'scrollbars=no,'
                                   +'toolbar=no,'
                                   +'location=no,'
                                   +'directories=no,'
                                   +'status=no,'
                                   +'menubar=no,'
                                   +'copyhistory=no');">Start</a>
<?php
*/
}


function
islocalhost ()
{
  return $_SERVER['REMOTE_ADDR'] == $_SERVER['SERVER_ADDR'];
}


function
isip ($ip)
{
  // $ip can also be a list of ip's
  return stristr ($ip, $_SERVER['REMOTE_ADDR']);
}


function
get_suffix ($filename)
// get_suffix() never returns NULL
{
  $p = basename ($filename);
  if (!$p)
    $p = filename;

  $s = strchr ($p, '.');
  if (!$s)
    $s = strchr ($p, 0);
  if ($s == $p)
    $s = strchr ($p, 0);

  return $s;
}


function
set_suffix ($filename, $suffix)
{
  // always use set_suffix() and NEVER the code below
  strcpy (get_suffix ($filename), $suffix);

  return $filename;
}


/*
  getfile()           runs callback_func with the realpath() of file/dir as string
                        flags:
  0                           pass all files/dirs with their realpath()
  GETFILE_FILES_ONLY     pass only files with their realpath()
  GETFILE_RECURSIVE      pass all files/dirs with their realpath()'s recursively
  GETFILE_RECURSIVE_ONCE like GETFILE_FILE_RECURSIVE, but only one level deep
  (GETFILE_FILES_ONLY|GETFILE_RECURSIVE)
                           pass only files with their realpath()'s recursively

  callback_func()       getfile() expects the callback_func to return the following
                          values:
                          0 == ok, 1 == skip the rest/break, -1 == failure/break
*/
//define ("GETFILE_FILES_ONLY",     1);
//define ("GETFILE_RECURSIVE",      1<<1);
//define ("GETFILE_RECURSIVE_ONCE", 1<<2);
function
getfile ($path_array, $callback_func, $flags)
{
  $result = 0;
  $i_max = sizeof ($path_array);

  for ($i = 0; $i < $i_max; $i++)
    {
      $dir = opendir ($path_array[$i]);

      if ($dir)
        {
          while (($file = readdir ($dir)) != false)
            if (strcmp ($file, "..") != 0 &&
                strcmp ($file, ".") != 0)
              {
                $result = callback_func ($file);
                if ($result == 1)
                  {
                    closedir ($dir);
                    return 0;
                  }
                if ($result == -1)
                  {
                    closedir (dir);
                    return -1;
                  }
              }
          closedir ($dir);
        }
    }

  return 0;
}


if (!function_exists('sprint_r'))
{
function 
sprint_r ($var)
{
  ob_start();

  print_r ($var);

  $ret = ob_get_contents();

  ob_end_clean();

  return $ret;
}
}


function
force_mozilla ()
{
  if (!stristr ($_SERVER['HTTP_USER_AGENT'], "moz"))
    {
/*
      echo "<script type=\"text/javascript\"><!--\n"
          ."location.href=\"http://www.mozilla.com/firefox/\"\n"
          ."//--></script>\n";
*/
      echo "<meta http-equiv=\"refresh\" content=\"1; URL=http://www.mozilla.com/firefox/\">";

/*
?>
<span style="font-family: arial,sans-serif;">
<table border="0" cellpadding="0" cellspacing="0" width="80%" height="100">
  <tr>
    <td border="0" cellpadding="0" cellspacing="0" bgcolor="#ffff80" align="center">
<font size="-1" face="arial" color="#000000">Your browser is not supported here. Redirecting...</font>
    </td>
  </tr>
</table>
</span>
<?php
*/
      exit ();
    }
}


function
misc_exec ($cmdline, $debug)
{
  if ($debug)
    echo $cmdline."\n";

  if ($debug < 2)
    {
      $a = array();

//      exec ("bash -c \"".$cmdline."\"", $a, $res);
      exec ($cmdline, $a, $res);

      $p = $res."\n";

      $i_max = sizeof ($a);
      for ($i = 0; $i < $i_max; $i++)
        $p .= $a[$i]."\n";

      return $p;
    }

  return "";
}


function
get_request_value ($name)
{
  if (isset ($_POST[$name]))
    return $_POST[$name];

  if (isset ($_GET[$name]))
    return $_GET[$name];

  return NULL;
}


function
html_head_tags_meta ($name, $content)
{
  if ($name && $content)
    return "<meta name=\"".$name."\" content=\"".$content."\">\n";

  return "";
}


function
html_head_tags_http_equiv ($http_equiv, $content)
{
  if ($http_equiv && $content)
    return "<meta http-equiv=\"".$http_equiv."\" content=\"".$content."\">\n";

  return "";
}


function
html_head_tags ($icon, $title, $refresh, $charset,
                $use_dc, $desc, $keywords, $identifier, $lang, $author)
{
  $p = "";

  $p .= html_head_tags_http_equiv ("Content-Type", "text/html; charset=".($charset ? $charset : "UTF-8"));

  if ($refresh > 0)
    $p .= html_head_tags_http_equiv ("refresh", $refresh."; URL=".$_SERVER['REQUEST_URI']);

/*
?>
    <meta http-equiv="imagetoolbar" content="no">
    <meta http-equiv="reply-to" content="editor@NOSPAM.sniptools.com">
    <meta http-equiv="MSThemeCompatible" content="Yes">
    <meta http-equiv="Content-Language" content="en">
    <meta http-equiv="Expires" content="Mon, 24 Sep 1976 12:43:30 IST">
<?php
*/

  if ($icon)
    $p .= "<link rel=\"icon\" href=\""
         .$icon
         ."\" type=\"image/png\">\n";

  if ($title)
    $p .= "<title>"
          .$title
          ."</title>\n";

  if ($use_dc)
    $p .= html_head_tags_meta ("description", $desc ? $desc : $title)
         .html_head_tags_meta ("author", $author ? $author : "Admin")
         .html_head_tags_meta ("keywords", $keywords ? $keywords : "html, php")
         .html_head_tags_meta ("robots", "follow")
         ."<!-- Dublin Core -->\n"
         .html_head_tags_meta ("DC.Title", $desc ? $desc : $title)
         .html_head_tags_meta ("DC.Creator", $author ? $author : "Admin")
         .html_head_tags_meta ("DC.Subject", $desc ? $desc : $title)
         .html_head_tags_meta ("DC.Description", $desc ? $desc : $title)
         .html_head_tags_meta ("DC.Publisher", $author ? $author : "Admin")
//         .html_head_tags_meta ("DC.Contributor", "")
//         .html_head_tags_meta ("DC.Date", "")
         .html_head_tags_meta ("DC.Type", "Software")
         .html_head_tags_meta ("DC.Format", "text/html")
         .html_head_tags_meta ("DC.Identifier", $identifier ? $identifier : "localhost")
//         .html_head_tags_meta ("DC.Source", "")
         .html_head_tags_meta ("DC.Language", $lang ? $lang : "en")
//         .html_head_tags_meta ("DC.Relation", "")
//         .html_head_tags_meta ("DC.Coverage", "")
//         .html_head_tags_meta ("DC.Rights", "GPL")

//       .html_head_tags_meta ("description", "Trapping keyboard events with Javascript -- in a cross-browser way [Sniptools]")
//       .html_head_tags_meta ("keywords", "Javascript keyboard events, keypress, javascript, keyCode, which, repeat, keydown event, Sniptools")
//       .html_head_tags_meta ("author", "Shashank Tripathi")
//       .html_head_tags_meta ("revisit-after", "1 week")
//       .html_head_tags_meta ("robots", "index,all")
//       .html_head_tags_meta ("revisit-after", "7 days")
//       .html_head_tags_meta ("author", "Shashank Tripathi")
//       .html_head_tags_meta ("generator", "Homesite 5.0&nbsp; | &nbsp;  Dreamweaver 6 beta&nbsp; | &nbsp; TopStyle 3&nbsp; | &nbsp; Notepad&nbsp; | &nbsp; Adobe PS 7.0")
//       .html_head_tags_meta ("resource-type", "Public")
//       .html_head_tags_meta ("classification", "Internet Services")
//       .html_head_tags_meta ("MSSmartTagsPreventParsing", "TRUE")
//       .html_head_tags_meta ("robots", "ALL")
//       .html_head_tags_meta ("distribution", "Global")
//       .html_head_tags_meta ("rating", "Safe For Kids")
//       .html_head_tags_meta ("language", "English")
//       .html_head_tags_meta ("doc-type", "Public")
//       .html_head_tags_meta ("doc-class", "Living Document")
//       .html_head_tags_meta ("doc-rights", "Copywritten Work")
//       .html_head_tags_meta ("distribution", "Global")
;

  return $p;
}


/*
  misc_proxy()
    performs many different tasks (see below)
    can be used to include other html pages inline

  $translate_func
    a (optional) callback function that translates foreign text in html
*/
define ("PROXY_SHOW_HEADER",    1);     // insert the header as comment into the html
//define ("PROXY_REQ_EDITOR",     1<<1);  // edit GET/POST requests/urls (shows all targets in a list)
//define ("PROXY_MAKEPDF",        1<<2);  // turn the whole html page into a pdf
define ("PROXY_PASS_FORMS",     1<<3);  // pass only form tags
define ("PROXY_PASS_LINKS",     1<<4);  // pass only the http links
//define ("PROXY_FILTER_COOKIES", 1<<5);  // remove cookies
//define ("PROXY_FILTER_JS",      1<<6);  // remove JavaScript
//define ("PROXY_FILTER_FLASH",   1<<7);  // remove Flash movies
//define ("PROXY_FILTER_CSS",     1<<8);  // remove CSS
//define ("PROXY_FILTER_ADS",     1<<9); // remove ads (content that comes from a different server)
define ("PROXY_FILTER_HTML",    1<<10); // remove all html tags
/*
  A dereferer is a means to strip the details of the referring website from a
  link request so that the target website cannot identify the page which was
  clicked on to originate a request.
*/
//define ("PROXY_DEREFERER",      1<<12);
/*
  PROXY_PUSH_*
    shows only the CAPTCHA dialog of a (news) site
    and a title, url and description prompt (depending on the target)
*/
define ("PROXY_PUSH_DIGG",      1<<13);
define ("PROXY_PUSH_SLASHDOT",  1<<14); // no CAPTCHA
define ("PROXY_PUSH_DELICIOUS", 1<<15);

function
misc_proxy ($url, $translate_func, $flags)
{
//  $res_keys = $http_response_header; // deprecated
  $res = apache_response_headers ();
  $res_keys = array_keys ($res);
  $req = apache_request_headers ();
  $req_keys = array_keys ($req);

  if (($fp = fopen ($url, "rb")) == false)
    return -1;

  $p = "";
  $i_max = sizeof ($res_keys);
/*
  for ($i = 1; $i < $i_max; $i++)
    {
      if (!strncasecmp ($res[$res_keys[$i]], "Content-Type: ", 14))
        {
          if ($res[$res_keys[$i]] == "Content-Type: audio/mpeg" ||
              $res[$res_keys[$i]] == "Content-Type: application/octet-stream")
            $p .= "Content-Disposition: attachment; filename=".$file;
        }
      else
        $p .= $res[$res_keys[$i]];
      $p .= $res_keys[$i];
    }

  header ($p);
*/

  if ($flags & PROXY_SHOW_HEADER)
    {
      $p = "<!--\n";
      $j_max = sizeof ($req_keys);
      for ($j = 0; $j < $j_max; $j++)
        $p .= $req_keys[$j]
             .": "
             .$req[$req_keys[$j]]
             ."\n";

      $p .= "\n\n\n\n";

      for ($i = 0; $i < $i_max; $i++)
        $p .= $res_keys[$i]
             .": "
             .$res[$res_keys[$i]]
             ."\n";
      $p .= "\n//-->";

      echo $p;
    }

  if ($translate_func ||
      $flags & PROXY_PASS_FORMS ||
      $flags & PROXY_PASS_LINKS)
    {
      while (($p = fgets ($fp)))
        echo $translate_func ($p);
    }
  else
    fpassthru ($fp);

  fclose ($fp);

  return 0;
}


class rrdtool
{
  var $rrd;
//  var $w;
//  var $h;


function
rrdtool_open ($rrd)
{
  $this->rrd = $rrd;
  $step = 1;
  $p = "rrdtool create "
      .$rrd
      ." --step "
      .$step
      ." DS:values:ABSOLUTE:"
//      .$step * 2
      ."900"
      .":U:U"
      ." RRA:AVERAGE:0.5:1:9000" 
      ." RRA:AVERAGE:0.5:4:9000" 
      ." RRA:AVERAGE:0.5:24:9000" 
//      ." RRA:AVERAGE:0.5:1:2160" 
//      ." RRA:AVERAGE:0.5:5:2016" 
//      ." RRA:AVERAGE:0.5:15:2880"
//      ." RRA:AVERAGE:0.5:60:8760"
//      ." RRA:MAX:0.5:1:2160" 
//      ." RRA:MAX:0.5:5:2016" 
//      ." RRA:MAX:0.5:15:2880"
//      ." RRA:MAX:0.5:60:8760"
;

//  echo $p;

  // create if necessary
  if (!file_exists ($rrd))
    return misc_exec ($p, 1);
  return 0;
}


function
rrdtool_update ($time, $value)
{
  if (!$time)
//    $time = time ();
    $time = "N";

  return misc_exec ("rrdtool update "
                   .$this->rrd
                   ." "
                   .$time
                   .":"
                   .$value, 1);
}


function
rrdtool_graph ($file, $seconds, $img_w, $img_h)
{
  return misc_exec ("rrdtool graph "
                   .$file
                   ." -s -"
                   .$seconds
                   ." -a PNG"
//                   ." --vertical-label \"Values\""
                   ." -w "
                   .$img_w
                   ." -h "
                   .$img_h
                   ." DEF:show="
                   .$this->rrd
                   .":values:AVERAGE LINE1:show#ff0000:Value", 1);
}


};


?>