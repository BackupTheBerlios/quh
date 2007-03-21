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
misc_exec ($cmdline)
{
  $a = array();

//  exec ("bash -c \"".$cmdline."\"", $a, $res);
  exec ($cmdline, $a, $res);

  $p = $res."\n";

  $i_max = sizeof ($a);
  for ($i = 0; $i < $i_max; $i++)
    $p .= $a[$i]."\n";

  return $p;
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
html_head_tags ($icon, $title, $refresh, $charset,
                $use_dc, $dc_desc, $dc_keywords, $dc_identifier, $dc_lang, $dc_author)
{
  $p = "";

  if ($charset)
    $p .= "<meta http-equiv=\"Content-Type\" content=\"text/html; charset="
         .$charset
         ."\">\n";
  else
    $p .= "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n";

  if ($refresh > 0)
    $p .= "<meta http-equiv=\"refresh\" content=\""
         .$refresh
         ."; URL="
         .$_SERVER['REQUEST_URI']
         ."\">\n";

  if ($icon)
    $p .= "<link rel=\"icon\" href=\""
         .$icon
         ."\" type=\"image/png\">\n";

  if ($title)
    $p .= "<title>"
          .$title
          ."</title>\n";

  if (!$use_dc)
    {
      echo $p;
      return;
    }

  $p .= "<meta name=\"description\" content=\""
       .($dc_desc ? $dc_desc : $title)
       ."\">\n"

       ."<meta name=\"author\" content=\""
       .($dc_author ? $dc_author : "Admin")
       ."\">\n"

       ."<meta name=\"keywords\" content=\""
       .($dc_keywords ? $dc_keywords : "html, php")
       ."\">\n"

       ."<meta name=\"robots\" content=\"follow\">\n"

       ."<!-- Dublin Core -->\n"
       ."<meta name=\"DC.Title\" content=\""
       .($dc_desc ? $dc_desc : $title)
       ."\">\n"

       ."<meta name=\"DC.Creator\" content=\""
       .($dc_author ? $dc_author : "Admin")
       ."\">\n"

       ."<meta name=\"DC.Subject\" content=\""
       .($dc_desc ? $dc_desc : $title)
       ."\">\n"

       ."<meta name=\"DC.Description\" content=\""
       .($dc_desc ? $dc_desc : $title)
       ."\">\n"

       ."<meta name=\"DC.Publisher\" content=\""
       .($dc_author ? $dc_author : "Admin")
       ."\">\n"

//       ."<meta name=\"DC.Contributor\" content=\""
//       ."\">\n"

//       ."<meta name=\"DC.Date\" content=\""
//       ."\">\n"

       ."<meta name=\"DC.Type\" content=\"Software\">\n"

       ."<meta name=\"DC.Format\" content=\"text/html\">\n"

       ."<meta name=\"DC.Identifier\" content=\""
       .($dc_identifier ? $dc_identifier : "localhost")
       ."\">\n"

//       ."<meta name=\"DC.Source\" content=\""
//       ."\">\n"

       ."<meta name=\"DC.Language\" content=\""
       .($dc_lang ? $dc_lang : "en")
       ."\">\n"

//       ."<meta name=\"DC.Relation\" content=\""
//       ."\">\n"
//       ."<meta name=\"DC.Coverage\" content=\""
//       ."\">\n"
//       ."<meta name=\"DC.Rights\" content=\"GPL\">\n"
    ;



/*
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en"><head profile="http://geotags.com/geo"><title>Sniptools | Stash | Trapping keyboard events with Javascript</title>



    
    <meta name="description" content="Trapping keyboard events with Javascript -- in a cross-browser way [Sniptools]">
    <meta name="keywords" content="Javascript keyboard events, keypress, javascript, keyCode, which, repeat, keydown event, Sniptools">
    <meta name="author" content="Shashank Tripathi">
    <meta name="revisit-after" content="1 week">
    <meta name="robots" content="index,all">
    <meta name="revisit-after" content="7 days">
    <meta name="author" content="Shashank Tripathi">
    <meta name="generator" content="Homesite 5.0&nbsp; | &nbsp;  Dreamweaver 6 beta&nbsp; | &nbsp; TopStyle 3&nbsp; | &nbsp; Notepad&nbsp; | &nbsp; Adobe PS 7.0">
    <meta name="resource-type" content="Public">
    <meta name="classification" content="Internet Services">
    <meta name="MSSmartTagsPreventParsing" content="TRUE">
    <meta name="robots" content="ALL">
    <meta name="distribution" content="Global">
    <meta name="rating" content="Safe For Kids">
    <meta name="language" content="English">
    <meta name="doc-type" content="Public">
    <meta name="doc-class" content="Living Document">
    <meta name="doc-rights" content="Copywritten Work">
    <meta name="distribution" content="Global">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <meta http-equiv="PICS-Label" content=" (PICS-1.1 &quot;http://www.gcf.org/v2.5&quot; labels on &quot;2001.11.05T08:15-0500&quot; until &quot;1995.12.31T23:59-0000&quot; for &quot;http://w3.org/PICS/Overview.html&quot; ratings (suds 0.5 density 0 color/hue 1))">
    <meta http-equiv="imagetoolbar" content="no">
    <meta http-equiv="reply-to" content="editor@NOSPAM.sniptools.com">
    <meta http-equiv="MSThemeCompatible" content="Yes">
    <meta http-equiv="Content-Language" content="en">
    <meta http-equiv="Expires" content="Mon, 24 Sep 1976 12:43:30 IST">
    <link rel="shortcut icon" href="http://sniptools.com/favicon.ico">
    <link rel="Stylesheet" href="jskeys_files/site.css" type="text/css" media="screen">
    <link rel="Stylesheet" href="jskeys_files/print.css" type="text/css" media="print">
    <link rel="alternate" type="application/rss+xml" title="RSS" href="http://sniptools.com/rss.xml">
<?php
*/

  return $p;
}


define ("PROXY_SHOW_HEADER", 1); // insert the header as comment into the html
//define ("PROXY_REQ_EDITOR", 2);  // edit requests
define ("PROXY_FORM_FILTER", 4); // pass only form tags
//define ("PROXY_HTML_TO_PDF", 8); // turn the html into pdf
//define ("PROXY_TARGET_COL", 16); // collect all form targets and show them (as comment?)
define ("PROXY_LINK_FILTER", 32);  // pass only the http links

//Remove all cookies (except certain proxy cookies)
//Remove all scripts (recommended for anonymity)
//Remove ads
//Hide referrer information
//Show URL entry form
function
proxy ($url, $flags)
{
  $res_keys = $http_response_header; // deprecated
//  $res = apache_response_headers ();
//  $res_keys = array_keys ($res);
  $req = apache_request_headers ();
  $req_keys = array_keys ($req);


  $fp = fopen ($url, "rb");

  $p = "";
  $i_max = sizeof ($res_keys);
  for ($i = 1; $i < $i_max; $i++)
    {
//      if (!strncasecmp ($res[$res_keys[$i]], "Content-Type: ", 14))
//        {
//          if ($res[$res_keys[$i]] == "Content-Type: audio/mpeg" ||
//              $res[$res_keys[$i]] == "Content-Type: application/octet-stream")
//            $p .= "Content-Disposition: attachment; filename=".$file;
//        }
//      else
//        $p .= $res[$res_keys[$i]];
      $p .= $res_keys[$i];
    }

  header ($p);

  if ($flags & PROXY_SHOW_HEADER)
    {
      $p = "";
      $j_max = sizeof ($req_keys);
      for ($j = 0; $j < $j_max; $j++)
        $p .= $req_keys[$j]
             .": "
             .$req[$req_keys[$j]]
             ."<br>";
      $p .= "<hr>";

      for ($i = 0; $i < $i_max; $i++)
        $p .= $res_keys[$i]
             .": "
//             .$res[$res_keys[$i]]
             ."<br>";
      $p .= "<hr>";

      echo $p;
    }

  fpassthru ($fp);

  fclose ($fp);

  return 0;
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


?>
