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
digg_me ($url)
{
  return "<script>\n"
        ."digg_url = '"
        .$url
        ."';\n"
        ."</script>\n"
        ."<script src=\"http://digg.com/api/diggthis.js\">\n"
        ."</script>";
}


function
get_firefox ()
{
  //check user-agent and redirect ie users to http://www.mozilla.org/firefox
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


/*
function
traffic ($db, $table_name)
{
  $p = "INSERT INTO `"
      .$table_name
      ."` (`time`,`ip`)"
      ." VALUES ('"
      .time(0)
      ."','"
      .$_SERVER['REMOTE_ADDR']
      ."');";

  $db->sql_write ($p, 0);
}


function
traffic_stats ($db, $table_name)
{
  $t = time (0) - (86400 * 2);

  $p = "SELECT `time`, `ip`"
      ." FROM `"
      .$table_name
      ."`"
      ." WHERE time > "
      .$t
      ." ORDER BY `time` DESC";

  $db->sql_write ($p, 0);
  $stats = $db->sql_read (0);

  $p = "";

  $i_max = sizeof ($stats);
  for ($i = 0; $i < $i_max; $i++)
    $p .= strftime ("%b %02e %T", $stats[$i][0])
         .": "
         .$stats[$i][1]
         . " "
         .get_country_by_ip ($stats[$i][1])
         ."<br>";

  return $p;
}
*/


function
set_request_method_to_get ()
{
  $GLOBALS['misc_method'] = $_GET;
}


function
set_request_method_to_post ()
{
  $GLOBALS['misc_method'] = $_POST;
}


function
get_request_method ()
{
  if (isset ($GLOBALS['misc_method']))
    if ($GLOBALS['misc_method'] == $_POST)
      return "POST";

  return "GET";
}


function
get_request_value ($name)
{
  if (isset ($GLOBALS['misc_method']))
    if (isset ($GLOBALS['misc_method'][$name]))
      return $GLOBALS['misc_method'][$name];

  return NULL;
}


function
html_head_tags ($icon, $title, $refresh, $charset,
                $use_dc, $dc_desc, $dc_keywords, $dc_identifier, $dc_lang, $dc_author)
{
  $p = "";

  if ($charset)
    $p .= "<meta http-equiv=\"content-type\" content=\"text/html; charset="
         .$charset
         ."\">\n";
  else
    $p .= "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n";

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

  return $p;
}


?>
