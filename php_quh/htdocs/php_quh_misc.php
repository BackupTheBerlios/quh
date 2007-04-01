<?php
/*
php_quh_misc.php - miscellaneuos functions for php_quh

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
//phpinfo ();
require_once ("config.php");
require_once ("misc/widget.php");
require_once ("misc/misc.php");
require_once ("MP3/Id.php");


function
php_quh_nfo ($w, $search)
{
  $wikipedia_url = "http://www.wikipedia.org/wiki/";
  $google_url = "http://www.google.com/search?q=";
  $lyrics_url = "http://search.azlyrics.com/cgi-bin/azseek.cgi?q=";

  return $w->widget_a ($wikipedia_url
                      .str_replace (" ", "_", $search),
                       "_blank",
                       "images/wikipedia.png",
                       -1,
                       -1,
                       NULL,
                       "Search Wikipedia for \""
                      .$search
                      ."\"",
                       0)
        .$w->widget_a ($google_url
                      .str_replace (" ", "+", $search),
                       "_blank",
                       "images/google.png",
                       -1,
                       -1,
                       NULL,
                       "Search Google for \""
                      .$search
                      ."\"",
                       0)
        .$w->widget_a ($lyrics_url
                      .str_replace (" ", "+", $search),
                       "_blank",
                       "images/lyrics.png",
                       -1,
                       -1,
                       NULL,
                       "Search lyrics for \""
                      .$search
                      ."\"",
                       0);
}


function
php_quh_player ($w,
                $php_quh_prev_file_value,
                $php_quh_next_file_value,
                $php_quh_file_value,
                $php_quh_start_value,
                $php_quh_stream_value,
                $php_quh_pos_value)
{
  $php_quh_incoming = dirname ($_SERVER['PHP_SELF'])."/incoming/";
  $php_quh_incoming_abs = dirname ($_SERVER['SCRIPT_FILENAME'])."/incoming/";
//  $skin = dirname ($_SERVER['PHP_SELF'])."/skin/";
//  $skin_abs = dirname ($_SERVER['SCRIPT_FILENAME'])."/skin/";

echo "|".$php_quh_incoming."|".$php_quh_incoming_abs."|";


  $php_quh_file = "";
  $php_quh_artist = "";
  $php_quh_title = "";
  $php_quh_album = "";
  $php_quh_year = "";
  $php_quh_track = 0;
  $php_quh_genre = "";
  $php_quh_length = "00:00:00";
  $php_quh_size = 0;  

  $p = "";

  if ($php_quh_file_value)
    {
      $id3 = &new MP3_Id ();
      $id3->read ($php_quh_incoming_abs.basename ($php_quh_file_value));
      $id3->study ($php_quh_incoming_abs.basename ($php_quh_file_value));
//      $stat = stat ($php_quh_incoming_abs.basename ($php_quh_file_value));

      $php_quh_file = basename ($php_quh_file_value);
      $php_quh_artist = $id3->getTag ('artists');
      $php_quh_title = $id3->getTag ('name');
      $php_quh_album = $id3->getTag ('album');
      $php_quh_year = $id3->getTag ('year');
      $php_quh_track = $id3->getTag ('track');
      $php_quh_genre = $id3->getTag ('genre');
      $php_quh_length = $id3->lengthh;
      $php_quh_size = $id3->musicsize;
/*
    * version of mpeg
    var $mpeg_ver = 0;
    * version of layer
    var $layer = 0;
    * version of bitrate
    var $bitrate = 0;
    * Frames are crc protected?
    var $crc = false;
    * frequency
    var $frequency = 0;
    * encoding type (CBR or VBR)
    var $encoding_type = 0;
    * number of samples per MPEG audio frame
    var $samples_per_frame = 0;
    * samples in file
    var $samples = 0;
    * Bytes in file without tag overhead
    var $musicsize = -1;
    * number of MPEG audio frames
    var $frames = 0;
    * quality indicator (0% - 100%)
    var $quality = 0;
    * Frames padded
    var $padding = false;
    * private bit set
    var $private = false;
    * Mode (Stero etc)
    var $mode = '';
    * Copyrighted
    var $copyright = false;
    * On Original Media? (never used)
    var $original = false;
    * Emphasis (also never used)
    var $emphasis = '';
    * Bytes in file
    var $filesize = -1;
    * Byte at which the first mpeg header was found
    var $frameoffset = -1;
    * length of mp3 format hh:mm:ss
    var $lengthh = false;
    * length of mp3 format mm:ss
    var $length = false;
    * length of mp3 in seconds
    var $lengths = false;
*/
    }


  $p .= "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\">"
       ."<tr>"
       ."<td style=\"background-image:url('images/black_opaque80.png');\" valign=\"top\" width=\"400\">"
       ."<font face=\"arial,sans-serif\" size=\"-1\" color=\"00e300\">"
       ."File: "
       .$php_quh_file
       ."<br>\nArtist: "
       .$php_quh_artist
       .($php_quh_artist ? " ".php_quh_nfo ($w, $php_quh_artist) : "")
       ."<br>\nTitle: "
       .$php_quh_title
       .($php_quh_title ? " ".php_quh_nfo ($w, $php_quh_title) : "")
       ."<br>\nAlbum: "
       .$php_quh_album
       .($php_quh_album ? " ".php_quh_nfo ($w, $php_quh_album) : "")
       ."<br>\nYear: "
       .$php_quh_year
       ."<br>\nTrack: "
       .$php_quh_track
       ."<br>\nGenre: "
       .$php_quh_genre
       ."<br>\nLength: "
       .$php_quh_length
       .", "
       .$php_quh_size
       ." Bytes"
       ."</td></tr></table>";

/*
   $next =
           $_SERVER['PHP_SELF']
           ."?php_quh_file="
           .$php_quh_incoming
           ."/"  
           .$next_file
           ."&php_quh_start="
           ."0"
           ."&php_quh_stream="
           ."1"
           ."&php_quh_pos="
           .$php_quh_pos_value + 1
*/

  $p .= $w->widget_audio ($php_quh_file_value, $php_quh_start_value, $php_quh_stream_value, $php_quh_next_file_value)

       .$w->widget_image ("php_quh_back", NULL, "images/back.png", -1, -1, "Previous Song", 0)
       .$w->widget_image ("php_quh_play", NULL, "images/play.png", -1, -1, "Play Song", 0)
       .$w->widget_image ("php_quh_pause", NULL, "images/pause.png", -1, -1, "Pause Song", 0)
       .$w->widget_image ("php_quh_stop", NULL, "images/stop.png", -1, -1, "Stop Song", 0)
       .$w->widget_image ("php_quh_next", NULL, "images/forward.png", -1, -1, "Next Song", 0)

//       .$w->widget_slider ("php_quh_volume", "value", "tooltip", 0, 0)
//       .$w->widget_slider ("php_quh_pos", "value", "tooltip", 0, 0)

       ."<br>"
       ."<div>";

  $next_file = "";
  $dir = opendir ($php_quh_incoming_abs);
  for ($pos = 0; ($file = readdir ($dir)) != false; $pos++)
    if (get_suffix ($file) == ".mp3" ||
        get_suffix ($file) == ".wav")
    {
      $stat = stat ($php_quh_incoming_abs.$file);
      $curr = basename ($php_quh_file_value) == basename ($file);
      if ($pos == $php_quh_pos_value + 1)
        $next_file = $file;

      $p.= "<a class=\"widget_a_label\" href=\""
          .$_SERVER['PHP_SELF']
          ."?php_quh_file="
          .$php_quh_incoming
          ."/"  
          .$file
          ."&php_quh_start="
          ."0"
          ."&php_quh_stream="
          ."1"
          ."&php_quh_pos="
          .$pos
          ."\""
          .($curr ? " style=\"color:#fff;\"" : "")
          .">"
          .$file
//          ." "
//          .$stat['size']
          ."</a> "
          ."<br>\n";
    }
  closedir ($dir);

  $p .= "</div>"
       .$w->widget_end ();

  return $p;
  
}


function
php_quh_upload ($w)
{
  $php_quh_incoming = dirname ($_SERVER['PHP_SELF'])."/incoming/";
  $php_quh_incoming_abs = dirname ($_SERVER['SCRIPT_FILENAME'])."/incoming/";

  $p = "<br>"
      .$w->widget_upload ("php_quh_file", "Add", "Add a MP3 or WAV song to the playlist (max.size: 10MB)", $php_quh_incoming_abs, 10000000, "audio/mpeg", 0);
//       .$w->widget_upload ("php_quh_skin", "Skin", "Change Skin (max.size: 250K)", $php_quh_incoming_abs, 250000, NULL, 0);

  return $p;
}


function
php_quh_prefs ($w)
{
  return "<br><br>Preferences<br><br>Skin<br>"
//        .$w->widget_checkbox ();
//       "Winamp classic"
//       "Default"
;
//proxy ($url, PROXY_FORM_FILTER);

}


function
php_quh_lyrics ($w)
{
  return "<br><br>Preferences<br><br>Skin<br>"
//        .$w->widget_checkbox ();
//       "Winamp classic"
//       "Default"
;
//proxy ($url, PROXY_FORM_FILTER);

}


function
php_quh_wiki ($w)
{
  return "<br><br>Preferences<br><br>Skin<br>"
//        .$w->widget_checkbox ();
//       "Winamp classic"
//       "Default"
;
//proxy ($url, PROXY_FORM_FILTER);
}


?>