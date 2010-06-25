<?php
/*
widget.php - new HTML widgets

Copyright (c) 2006 - 2008 NoisyB


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
if (!defined ('MISC_WIDGET_PHP'))
{
define ('MISC_WIDGET_PHP', 1);  
include_once ('misc/misc.php');
if (file_exists ('geoip/geoipcity.inc') == TRUE)
  include_once ('geoip/geoipcity.inc'); // widget_geotrace()


/*
  takes image with e.g. 16x16 font tiles and maps them to ascii codes
  generates CSS code to show text with it by using clip()ing
  put CSS code into a span or div

  tile_rows, tile_cols
  16x16 are 256 characters/tiles

  enclose this in a div tag to place it on the page
    e.g. <div style="position:absolute;top:100px;left:200px;">
*/
function
widget_fontiles ($image_url, $image_width, $image_height, $text, $file_cols = 16, $file_rows = 16)
{
  $char_w = $image_width / $file_cols; 
  $char_h = $image_height / $file_rows;

  $p = '';
  for ($i = 0; $i < strlen ($text); $i++)
    {
      $c = ord ($text[$i]);

      $left = $c % $file_cols;
      $left *= $char_w;

      $top = (int) ($c / $file_rows);
      $top *= $char_h;

      $right = $left + $char_w;
      $bottom = $top + $char_h;

      $pos_left = $i * $char_w - $left;
      $pos_top = 0 - $top; 

      $p .= '<img src="'.$image_url.'" style="'
           .'position:absolute;'
           .'clip:rect('.$top.'px,'.$right.'px,'.$bottom.'px,'.$left.'px);'
           .'top:'.$pos_top.'px;left:'.$pos_left.'px;'
           .'width:'.$image_width.';height:'.$image_height.';'
           .'">'."\n";
    }

  return $p;
}


/*
<?xml version="1.0" encoding="UTF-8"?>
<!-- config for aa2map_php -->
<categories>
  <category>
    <id>qscore</id>
    <title>qscore suite</title>
    <tooltip>generate high scores and statistics from game server logs and some other useful game server admin tools</toolti
    <src>aa2map_ascii.php</src>
    <embed>1</embed>
    <new>0</new>
    <lf>1</lf>
  </category>
*/
function
widget_cms ($logo, $config_xml)
{
  $config = simplexml_load_file ($config_xml);

  $q = get_request_value ('q');

  $p = '';

  if ($q)
    $p .= '<a href="."><img src="'.$logo.'" border="0" align="middle" height="50"></a> ';
  else
    $p .= '<br>'
         .'<br>'
         .'<br>'
         .'<br>'
         .'<center>'
         .'<img src="'.$logo.'" border="0">'
         .'<br>'  
         .'<br>';

  for ($i = 0; $config->category[$i]; $i++)
    {
      $p .= '<a href="';

      if ($config->category[$i]->embed == 1)
        $p .= '?q='.$config->category[$i]->id;
      else
        $p .= $config->category[$i]->src;

      $p .= '" title="'
           .$config->category[$i]->tooltip
           .'">'
           .$config->category[$i]->title
           .'</a>'
           .($config->category[$i]->new == 1 ? '<img src="images/new.png">' : '');

      if ($q)
        $p .= '&nbsp;&nbsp;';
      else if ($config->category[$i]->lf > 0)
        {
          $p .= '<br>';
          if ($config->category[$i]->lf > 1)
          for ($j = 0; $j < (int) $config->category[$i]->lf - 1; $j++)
            $p .= '<br>';
        }
    } 
   
  if ($q)
    {
      $p .= '<br>';
      $p .= '<hr>';

      for ($i = 0; $config->category[$i]; $i++)
        if ($q == $config->category[$i]->id)
          {
            if (file_exists ($config->category[$i]->src))
              {
//                $p .= file_get_contents ($config->category[$i]->src);
                ob_start ();
                require_once ($config->category[$i]->src);
                $p .= ob_get_contents ();
                ob_end_clean ();
              }
            else
              $p .= '<iframe width="100%" height="90%" marginheight="0" marginwidth="0" frameborder="0" src="'  
                   .$config->category[$i]->src
                   .'"></iframe>'; 
            break;
          }
    }
  else
    {
      $p .= '</center>';
    }

  return $p;
}  


function
widget_collapse ($label, $s, $collapsed)
{
  $r = rand ();

  $p = '';

  $p .= '<script type="text/javascript">'
       .'<!--'."\n"
       .'function widget_collapse (obj)'."\n"
       .'{'."\n"
       .'  var o = document.getElementById(obj);'."\n"
       .'  o.style.display = (o.style.display != \'none\') ? \'none\' : \'\';'."\n"
       .'}'."\n"
//       .'document.write (\'<a href="javascript:widget_collapse(\''.$r.'\');">'.$label.'</a>\');'."\n"
       .'//-->'
       .'</script>'
       .'<a href="javascript:widget_collapse(\''.$r.'\');">'.$label.'</a>'
       .'<div id="'.$r.'"'
       .($collapsed ? ' style="display:none;"' : '')
       .'>'
       .$s
       .'</div>';

  return $p;
}


function
widget_onhover_link ($url, $image1, $image2)
{
  $name = rand (0, 99999999).crc32 ($url.$image1.$image2);

  $p = '';
  $p .= '<a href="'.$url.'"'
       .' onmouseover="document.'.$name.'.src='.$image1.'"'
       .' onmouseout="document.'.$name.'.src='.$image2.'"'
       .'><img src="'.$image1.'" border="0" name="'.$name.'"></a>';

  return $p;
}


function
widget_window_open ($url, $fullscreen = 0, $window_name = '')
{
  $p = '';

  $p .= '<script type="text/javascript">'."\n";

  $p .= 'function widget_window_open ()'."\n"
       .'{'."\n";

//  $p .= 'var w=screen.width;var h=screen.height;';

//  $p .= 'var win=';

  $p .= 'window.open(\''
       .$url
       .'\',\''
       .str_replace ("'", "\'", $window_name)
       .'\',\'';

// https://developer.mozilla.org/en/Gecko_DOM_Reference
// https://developer.mozilla.org/en/DOM/window.open
  if ($fullscreen)
    $p .= ''
         .'top=0'
         .',left=0'
//         .',width=\'+w+\''
//         .',height=\'+h+\''
         .',fullscreen'
//         .',menubars'
         .',status=0'
//         .',toolbar'
         .',location=0'
//         .',menubar=no'
//         .',directories=no'
//         .',resizable=no'
//         .',scrollbars=no'
//         .',copyhistory'
;
  else
    $p .= ''
         .'width=400'
         .',height=300'
         .',status=no'
         .',toolbar=no'
         .',location=no'
         .',menubar=no'
         .',directories=no'
         .',resizable=yes'
         .',scrollbars=yes'
         .',copyhistory=yes'
;
  $p .= '\').focus();'."\n";

//  $p .= 'window.opener = top;'."\n"; // this will close opener in ie only (not Firefox)

  if ($fullscreen)
  $p .= 'window.moveTo(0,0);'."\n"
       ."\n"
       .'// changing bar states on the existing window)'."\n"
//       .'netscape.security.PrivilegeManager.enablePrivilege("UniversalBrowserWrite");'."\n"
       .'window.locationbar.visible=0;'."\n"
       .'window.statusbar.visible=0;'."\n"
       ."\n"
       .'if (document.all)'."\n"
       .'  window.resizeTo(screen.width, screen.height);'."\n"
       ."\n"
       .'else if (document.layers || document.getElementById)'."\n"
       .'  if (window.outerHeight < screen.height || window.outerWidth < screen.width)'."\n"
       .'    {'."\n"
       .'      window.outerHeight = screen.height;'."\n"
       .'      window.outerWidth = screen.width;'."\n"
       .'    }'."\n"
;

  $p .= '}'."\n";

  $p .= '</script>';

  return $p;
}


function
widget_carousel ($xmlfile, $width=200, $height=150)
{
  $p = ''
      .'<span class="carousel_container">'
      .'<span id="carousel1">'
      .'</span>'
      .'</span>'
      .'<script type="text/javascript" src="misc/swfobject.js"></script>'
      .'<script type="text/javascript">'."\n"
      .'swfobject.embedSWF ('."\n"
      .'  "misc/carousel.swf",'."\n"
      .'  "carousel1",'."\n"
      .'  "'.$width.'", "'.$height.'",'."\n"
      .'  "9.0.0",'."\n"
      .'  false,'."\n"
      .'  {'."\n"
      .'    xmlfile:"'.$xmlfile.'",'."\n"
      .'    loaderColor:"0xffffff",'."\n"
      .'    messages:"  ::  ::  ::  "'."\n"
      .'  },'."\n"
      .'  {bgcolor: "#ffffff"});'."\n"
      .'</script>';

  echo $p;
}


function
widget_geotrace ($host, $w = '100%', $h = '100%')
{
//http://maps.google.com/?ie=UTF8&ll=37.0625,-95.677068&spn=31.013085,55.634766&t=h&z=4
  $p = '';

  // GeoLiteCity
  if (file_exists ('GeoLiteCity.dat'))
    {
      geoip_load_shared_mem ('GeoLiteCity.dat');
      $gi = geoip_open ('GeoLiteCity.dat', GEOIP_SHARED_MEMORY);
      $host = gethostbyname ($host);
      $a = GeoIP_record_by_addr ($gi, $host);
      geoip_close ($gi);

      // DEBUG
//      echo '<pre><tt>';
//      print_r ($a);

      $p .= ''
           .'<iframe width="'.$w.'" height="'.$h.'"'
           .' frameborder="0"'
           .' scrolling="no"'
           .' marginheight="0"'
           .' marginwidth="0"'
           .' src="http://www.openstreetmap.org/export/embed.html?bbox='
             .($a->longitude - 0.005).','
             .($a->latitude - 0.005).','
             .($a->longitude + 0.005).','
             .($a->latitude + 0.005).'&layer=mapnik"'
           .'>'
           .'</iframe>'
;
    }

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
widget_upload ($upload_path, $max_file_size, $mime_type, $submit_button_html, $uploaded_html)
{
  $debug = 0;
  $p = '';

  if (!$_FILES)
    return '<form action="'
      .$_SERVER['PHP_SELF']
      .'" method="POST" enctype="multipart/form-data"'
//      .' style="margin:0;"'
      .' style="display:inline;"'
      .'>'
      .'<input type="hidden" name="MAX_FILE_SIZE" value="'
      .$max_file_size
      .'">'
      .'<input type="file"'
      .' name="widget_upload"'
//      .' title="'
//      .$tooltip
//      .'"'
      .($max_file_size ? ' maxlength="'.$max_file_size.'"' : '')
      .($mime_type ? ' accept="'.$mime_type.'"' : '')
      .'>'
      .($submit_button_html ? $submit_button_html :
       '<input type="submit" name="widget_upload" value="Upload"'
//      .' tooltip="'
//      .$tooltip
//      .'"'
)
      .'>'
      .'</form>'
;

  if ($debug)
    {
      $p .= '<pre><tt>'
           .sprint_r ($_FILES);
    }

  $d = $upload_path.'/'
//      .str_replace (' ', '_', basename($_FILES['widget_upload']['name']));
      .basename($_FILES['widget_upload']['name']);

  if (file_exists ($d))
    $p .= 'ERROR: file already exists';
  else if (move_uploaded_file ($_FILES['widget_upload']['tmp_name'], $d) == FALSE)
    $p .= 'ERROR: move_uploaded_file() failed';

  $s = Array (
           UPLOAD_ERR_OK =>         'OK',
           UPLOAD_ERR_INI_SIZE =>   'The uploaded file exceeds the upload_max_filesize directive ('
                                   .ini_get ('upload_max_filesize')
                                   .') in php.ini',
           UPLOAD_ERR_FORM_SIZE =>  'The uploaded file exceeds the MAX_FILE_SIZE directive ('
                                   .$max_file_size
                                   .') that was specified in the HTML form',
           UPLOAD_ERR_PARTIAL =>    'The uploaded file was only partially uploaded',
           UPLOAD_ERR_NO_FILE =>    'No file was uploaded',
           UPLOAD_ERR_NO_TMP_DIR => 'Missing a temporary folder',
//           UPLOAD_ERR_CANT_WRITE => 'Failed to write file to disk',
//           UPLOAD_ERR_EXTENSION =>  'File upload stopped by extension'
         );

  if (!empty ($_FILES['widget_upload']) &&
      $_FILES['widget_upload']['error'] == UPLOAD_ERR_OK)
    {
      $p .= $uploaded_html;
    }
  else
    {
      $e = $s[$_FILES['widget_upload']['error']];
      if (!$e)
        $e .= 'An unknown error occured';
      $p .= 'ERROR: '.$e;
    }

  if ($debug)
    {
      $p .= '<pre><tt>'
           .sprint_r ($s)
           .sprint_r ($_FILES);
    }

  return $p;
}


function
widget_captcha ($captcha_path)
{
  global $tv2_root;

  // use random captcha image
  if (!($handle = opendir ($tv2_root.'/'.$captcha_path)))
    return 'ERROR: problem with CAPTCHA';

  $a = array ();
  $count = 0;
  while (false !== ($p = readdir ($handle)))
    if (get_suffix ($p) == '.jpg')
      $a[$count++] = $p;

  closedir ($handle);

  srand (microtime () * time ());
  $r = rand (0, sizeof ($a) - 1);

  $captcha_md5 = set_suffix ($a[$r], '');
  $widget_captcha_key = md5 ($captcha_md5.$_SERVER['REMOTE_ADDR']); // key
  $img = $captcha_path.'/'.$captcha_md5.'.jpg'; // image name is md5 of the captcha in the image

  $p = '';
  $p .= '<input type="hidden" name="widget_captcha_key" value="'.$widget_captcha_key.'">';
  $p .= '<img src="'.$img.'" border="0" title="enter this CAPTCHA in the field to the right">';
  $p .= '<input type="text" size="3" maxsize="3" name="widget_captcha" title="enter the CAPTCHA you see left from here">';

  return $p;
}


function
widget_captcha_check ()
{
  $widget_captcha = get_request_value ('widget_captcha');
  $widget_captcha_key = get_request_value ('widget_captcha_key');

  // DEBUG
//  echo md5 (md5 ($widget_captcha).$_SERVER["REMOTE_ADDR"]).' == '.$widget_captcha_key.'<br>';

  if (md5 (md5 ($widget_captcha).$_SERVER["REMOTE_ADDR"]) == $widget_captcha_key)
    return TRUE;
  return FALSE;
}


/*
function
widget_table ($title_array, $content_array)
{
  // $cols == number of titles in $title_array 
  $cols = sizeof ($title_array);
  $rows = $cols * 0.5;

  $p = '';

  $p .= '<table class="widget_table" border="0" cellpadding="1" cellspacing="0">';

  // titles
  $p .= '<tr class="widget_table_title">';
  for ($i = 0; $title_array[$i]; $i++)
    $p .= '<td class="widget_table_td">'.$title_array[$i].'</td>';
  $p .= '</tr>';

  // content
  for ($i = 0; $i < $rows; $i++)
    {
      $p .= '<tr class="widget_table_tr'.(($i & 1) + 1).'">';
      for ($j = $rows * $cols; $j < $cols; $j++)
        $p .= '<td class="widget_table_td">'.$content_array[$j].'</td>';
      $p .= '</tr>';
    }

  $p .= '</table>';

  return $p;
}
*/


function
widget_indexof_sort_time ($a, $b)
{
  if ($a[3] == $b[3])
    return 0;
  return ($a[3] < $b[3]) ? 1 : -1;
}


/*
function
widget_index_tree ($name, $path, $mime_type, $flags)
{
  $p = '';
  $dir = opendir ($path);
  while (($file = readdir ($dir)) != false)
    {
      if (is_dir ($file))
        {
          $p .= '<img src="images/widget_tree_closed.png" border="0" alt="images/widget_tree_open.png">'
                 .basename ($file);
        }
      else if (is_file ($file))
        {
          $stat = stat ($file);
          $p .= '<img src="images/widget_tree_file.png" border="0" alt="images/widget_tree_file.png">'
               .basename ($file)
               .$stat['size'];
        }
      else // ?
        {
          $p .= '<img src="images/widget_tree_file.png" border="0" alt="images/widget_tree_file.png">'
               .basename ($file);
        }

      $p .= '<br>'."\n";
    }
  closedir ($dir);

  return $p;
}
*/


function
widget_indexof_func ($b)
{
// DEBUG
//  return '<pre><tt>'.sprint_r ($b).'</tt></pre>';  

  $p = '<table><tr>';
  $j = sizeof ($b);
  for ($i = 0; $i < $j; $i++)
    {
      if ($i > 0)
        $p .= '</tr><tr>';

      $p .= '<td>';

      $p .= '<a href="'
           .$b[$i][0].$b[$i][1]
           .'">'
           .$b[$i][1]
           .'</a>';

      $p .= '</td><td>';

      $p .= $b[$i][2];

      $p .= '</td><td>';

      date_default_timezone_set ('Europe/Berlin');
//      $p .= date ("%a %d-%b-%y %T %Z", strtotime ($b[$i][3]));
      $p .= strftime ("%a %d-%b-%y %T %Z", $b[$i][3]);

      $p .= '</td><td>';

      if ($b[$i][5] == '.flv' || $b[$i][5] == '.mp4')
        $p .= '<a href="?video='.$b[$i][1].'">Play</a>';
      else if ($b[$i][5] == '.mp3')
        $p .= '<a href="?audio='.$b[$i][1].'">Play</a>';

      $p .= '</td>';
    }

  $p .= '</tr></table>';

  return $p;
}


function
widget_indexof ($dir, $suffix, $indexof_func)
{
  $b = array (array ());
 
  $a = array ();
  $a = scandir ($dir, 0);

  // read filenames, sizes and mtime
  $i_max = sizeof ($a);
  for ($i = 0, $j = 0; $i < $i_max; $i++)
    {
      $dirname = str_replace ('//', '/', $dir.'/');
      $basename = str_replace ("\n", '', $a[$i]);
      $path = $dirname.'/'.$basename;

      if (!file_exists ($path))
        continue;

      if ($basename == '.' || $basename == '..')
        continue;

      if (!is_file ($path))
        continue;

      if ($suffix)
        if (get_suffix ($basename) != $suffix)
          continue;

      $b[$j][0] = $dirname;
      $b[$j][1] = $basename;

/*
stat ()

0  	dev  	device number
1 	ino 	inode number *
2 	mode 	inode protection mode
3 	nlink 	number of links
4 	uid 	userid of owner *
5 	gid 	groupid of owner *
6 	rdev 	device type, if inode device
7 	size 	size in bytes
8 	atime 	time of last access (Unix timestamp)
9 	mtime 	time of last modification (Unix timestamp)
10 	ctime 	time of last inode change (Unix timestamp)
11 	blksize 	blocksize of filesystem IO **
12 	blocks 	number of blocks allocated **

* On Windows this will always be 0.

** Only valid on systems supporting the st_blksize type - other systems (e.g. Windows) return -1.

In case of error, stat() returns FALSE
*/


      $s = stat ($path);
      $b[$j][2] = $s['size'];
      $b[$j][3] = $s['mtime'];
      $b[$j][4] = is_file ($path) ? '1' : '0';
      $b[$j][5] = get_suffix ($path);

      $j++;
    }

  // sort by date or size or suffix
  usort ($b, 'widget_indexof_sort_time');

  return $indexof_func ? $indexof_func ($b) : widget_indexof_func ($b);
}


// widget_relate() flags
//define ('WIDGET_RELATE_BOOKMARK',    1<<6);  // browser bookmark
//define ('WIDGET_RELATE_STARTPAGE',   1<<7);  // use as start page
//define ('WIDGET_RELATE_SEARCH',      1<<8);  // add search plugin to browser
//define ('WIDGET_RELATE_LINKTOUS',    1<<9);  // link-to-us code for link sections of other sites
define ('WIDGET_RELATE_TELLAFRIEND', 1<<10); // send tell-a-friend email
define ('WIDGET_RELATE_SBOOKMARKS',  1<<11); // social bookmarks
//define ('WIDGET_RELATE_DIGGTHIS',    1<<12);
//define ('WIDGET_RELATE_DONATE',      1<<13); // donate button (paypal, etc..)
//define ('WIDGET_RELATE_RSSFEED',     1<<14); // generate RSS feed
define ('WIDGET_RELATE_ALL',
//                                     WIDGET_RELATE_BOOKMARK|
//                                     WIDGET_RELATE_STARTPAGE|
//                                     WIDGET_RELATE_SEARCH|
//                                     WIDGET_RELATE_LINKTOUS|
                                     WIDGET_RELATE_TELLAFRIEND|
                                     WIDGET_RELATE_SBOOKMARKS // |
//                                     WIDGET_RELATE_DIGGTHIS|
//                                     WIDGET_RELATE_DONATE|
//                                     WIDGET_RELATE_RSSFEED
);


function
widget_relate ($title, $url, $rss_feed_url, $vertical, $flags)
{
  $p = '';
//  $p .= '<table border="0" cellpadding="0" cellspacing="0" style="background-color:#fff;">'
//       .'<tr><td>';
//  $p .= '<font size="-1" face="arial,sans-serif">';

  $title = trim ($title);
  $url = trim ($url);
  $lf = $vertical ? '<br>' : ' ';

/*
  // digg this button
  if ($flags & WIDGET_RELATE_DIGGTHIS)
    $p .= '<script><!--'."\n"
         .'digg_url = \''
         .$url
         .'\';'
         .'//--></script>'
         .'<script type="text/javascript" src="http://digg.com/api/diggthis.js"></script>'
         .$lf;

  // donate
  if ($flags & WIDGET_RELATE_DONATE)
    $p .= '<img class="widget_relate_img" src="images/widget/widget_relate_paypal.png" border="0">'
         .'<a class="widget_relate_label" href="http://paypal.com">Donate</a>'
.'<pre>* * *   D O N A T I O N S   A R E   A C C E P T E D   * * *</pre><br>'
.'<br>'
.'<img src="images/widget_relate_refrigator.jpg" border="0"><br>'
.'<br>'
.'Individuals and companies can now donate funds to support me and keep me from<br>'
.'writing proprietary software.<br>'
.'<br>'
.'Thank You!<br>'
.'<br>'
.'<pre>* * *   D O N A T I O N S   A R E   A C C E P T E D   * * *</pre><br-->'
.'search widget to include in other pages'
         .$lf;

  // link-to-us code for link sections of other sites
  if ($flags & WIDGET_RELATE_LINKTOUS)
    $p .= '<textarea title="Add this code to your blog or website">Link to us</textarea>'
         .$lf;
*/

  if ($flags & WIDGET_RELATE_TELLAFRIEND)
    $p .= '<img class="widget_relate_img" src="images/widget/widget_relate_tellafriend.png" border="0">'
         .'<a class="widget_relate_label" href="mailto:?body='
         .$url
         .'&subject='
         .$title
         .'"'
         .'>Share</a>'
         .$lf;
/*
  // add browser bookmark
  if ($flags & WIDGET_RELATE_BOOKMARK)
    $p .= '<img class="widget_relate_img" src="images/widget/widget_relate_star.png" border="0">'
         .'<a class="widget_relate_label"'
         .' href="javascript:js_bookmark (\''
         .$url
         .'\', \''
         .$title
         .'\');"'
         .' border="0">Bookmark</a>'
         .$lf;

  // use as startpage
  if ($flags & WIDGET_RELATE_STARTPAGE)
    $p .= '<img class="widget_relate_img" src="images/widget/widget_relate_home.png" border="0">'
         .'<a class="widget_relate_label"'
         .' href="http://"'
         .' onclick="this.style.behavior=\'url(#default#homepage)\';this.setHomePage(\'http://torrent-finder.com\');"'
         .'>'
         .'Make us your start page</a>'
         .$lf;

  // add search plugin to browser
  if ($flags & WIDGET_RELATE_SEARCH)
    $p .= '<img class="widget_relate_img" src="images/widget/widget_relate_search.png" border="0">'
         .'<a class="widget_relate_label"'
         .' href="http://'
//         .' href=\"javascript:js_bookmark('"
//         .$title
//         .'\', \''
//         .$url
//         .'\')"'
         .' border="0">Add search</a>'
         .$lf;

  // generate rss feed
  if ($flags & WIDGET_RELATE_RSSFEED)
    $p .= '<img class="widget_relate_img" src="images/widget/widget_relate_rss.png" border="0">'
         .'<a class="widget_relate_label"'
         .' href="'
         .$rss_feed_url
         .'"'
         .' border="0">RSS feed</a>'
         .$lf;
*/

  // social bookmarks
  if ($flags & WIDGET_RELATE_SBOOKMARKS)
    {
      $a = array (
        array ('Digg',			'digg.png', 'http://digg.com/submit?phase=2&url=', '&bodytext=&tags=&title='),
//        array ('Digg',		'digg.png', 'http://digg.com/submit?phase=2&url=', '&title='),
        array ('Twitter',               'twitter.png', 'http://twitter.com/home?status=', NULL),
        array ('Facebook',              'facebook.png', 'http://www.facebook.com/sharer.php?u=', NULL),
        array ('StumbleUpon',           'stumbleupon.png', 'http://www.stumbleupon.com/submit?url=', '&title=')

//http://twitter.com/home?status=Hardware+Companies+Team+Up+To+Fight+Mobile+Linux+Fragmentation%3A+http%3A%2F%2Fbit.ly%2Fd9DXNF
//http://www.facebook.com/sharer.php?u=http://linux.slashdot.org/story/10/06/05/1327228/Hardware-Companies-Team-Up-To-Fight-Mobile-Linux-Fragmentation

/*
//        array ('30 Day Tags',		'30_day_tags.png', NULL, NULL),
//        array ('AddToAny',		'addtoany.png', NULL, NULL),
//        array ('Ask',			'ask.png', NULL, NULL),
//        array ('BM Access',		'bm_access.png', NULL, NULL),
        array ('Backflip',		'backflip.png', 'http://www.backflip.com/add_page_pop.ihtml?url=', '&title='),
//        array ('BlinkBits',		'blinkbits.png', 'http://www.blinkbits.com/bookmarklets/save.php?v=1&source_url=', '&title='),
        array ('BlinkBits',		'blinkbits.png', 'http://www.blinkbits.com/bookmarklets/save.php?v=1&source_image_url=&rss_feed_url=&rss_feed_url=&rss2member=&body=&source_url=', '&title='),
        array ('Blinklist',		'blinklist.png', 'http://www.blinklist.com/index.php?Action=Blink/addblink.php&Description=&Tag=&Url=', '&Title='),
//        array ('Bloglines',		'bloglines.png', NULL, NULL),
        array ('BlogMarks',		'blogmarks.png', 'http://blogmarks.net/my/new.php?mini=1&simple=1&url=', '&content=&public-tags=&title='),
//        array ('BlogMarks',		'blogmarks.png', 'http://blogmarks.net/my/new.php?mini=1&simple=1&url=', '&title='),
        array ('Blogmemes',		'blogmemes.png', 'http://www.blogmemes.net/post.php?url=', '&title='),
//        array ('Blue Dot',		'blue_dot.png', NULL, NULL),
        array ('Buddymarks',		'buddymarks.png', 'http://buddymarks.com/s_add_bookmark.php?bookmark_url=', '&bookmark_title='),
//        array ('CiteULike',		'citeulike.png', NULL, NULL),
        array ('Complore',		'complore.png', 'http://complore.com/?q=node/add/flexinode-5&url=', '&title='),
//        array ('Connotea',		'connotea.png', NULL, NULL),
        array ('Del.icio.us',		'del.icio.us.png', 'http://del.icio.us/post?v=2&url=', '&notes=&tags=&title='),
//        array ('Del.icio.us',		'del.icio.us.png', 'http://del.icio.us/post?v=2&url=', '&title='),
//        array ('Del.icio.us',		'del.icio.us.png', 'http://del.icio.us/post?url=', '&title='),
        array ('De.lirio.us',		'de.lirio.us.png', 'http://de.lirio.us/bookmarks/sbmtool?action=add&address=', '&title='),
        array ('Digg',			'digg.png', 'http://digg.com/submit?phase=2&url=', '&bodytext=&tags=&title='),
//        array ('Digg',		'digg.png', 'http://digg.com/submit?phase=2&url=', '&title='),
        array ('Diigo',			'diigo.png', 'http://www.diigo.com/post?url=', '&tag=&comments=&title='),
//        array ('Dogear',		'dogear.png', NULL, NULL),
//        array ('Dotnetkicks',		'dotnetkicks.png', 'http://www.dotnetkicks.com/kick/?url=', '&title='),
//        array ('Dude, Check This Out',	'dude_check_this_out.png', NULL, NULL),
//        array ('Dzone',		'dzone.png', NULL, NULL),
//        array ('Eigology',		'eigology.png', NULL, NULL),
        array ('Fark',			'fark.png', 'http://cgi.fark.com/cgi/fark/edit.pl?new_url=', '&title='),
//        array ('Favoor',		'favoor.png', NULL, NULL),
//        array ('FeedMeLinks',		'feedmelinks.png', NULL, NULL),
//        array ('Feedmarker',		'feedmarker.png', NULL, NULL),
        array ('Folkd',			'folkd.png', 'http://www.folkd.com/submit/', NULL),
//        array ('Freshmeat',		'freshmeat.png', NULL, NULL)
        array ('Furl',			'furl.png', 'http://www.furl.net/storeIt.jsp?u=', '&keywords=&t='),
//        array ('Furl',		'furl.png', 'http://www.furl.net/storeIt.jsp?u=', '&t='),
//        array ('Furl',		'furl.png', 'http://www.furl.net/store?s=f&to=0&u=', '&ti='),
//        array ('Givealink',		'givealink.png', NULL, NULL),
        array ('Google',		'google.png', 'http://www.google.com/bookmarks/mark?op=add&hl=en&bkmk=', '&annotation=&labels=&title='),
//        array ('Google',		'google.png', 'http://www.google.com/bookmarks/mark?op=add&bkmk=', '&title='),
//        array ('Humdigg',		'humdigg.png', NULL, NULL),
//        array ('HLOM (Hyperlinkomatic)',		'hlom.png', NULL, NULL),
//        array ('I89.us',		'i89.us.png', NULL, NULL),
        array ('Icio',			'icio.png', 'http://www.icio.de/add.php?url=', NULL),
//        array ('Igooi',		'igooi.png', NULL, NULL),
//        array ('Jots',		'jots.png', NULL, NULL),
//        array ('Link Filter',		'link_filter.png', NULL, NULL),
//        array ('Linkagogo',		'linkagogo.png', NULL, NULL),
        array ('Linkarena',		'linkarena.png', 'http://linkarena.com/bookmarks/addlink/?url=', '&desc=&tags=&title='),
//        array ('Linkatopia',		'linkatopia.png', NULL, NULL),
//        array ('Linklog',		'linklog.png', NULL, NULL),
//        array ('Linkroll',		'linkroll.png', NULL, NULL),
//        array ('Listable',		'listable.png', NULL, NULL),
//        array ('Live',		'live.png', 'https://favorites.live.com/quickadd.aspx?marklet=1&mkt=en-us&url=', '&title='),
//        array ('Lookmarks',		'lookmarks.png', NULL, NULL),
        array ('Ma.Gnolia',		'ma.gnolia.png', 'http://ma.gnolia.com/bookmarklet/add?url=', '&description=&tags=&title='),
//        array ('Ma.Gnolia',		'ma.gnolia.png', 'http://ma.gnolia.com/bookmarklet/add?url=', '&title='),
//        array ('Maple',		'maple.png', NULL, NULL),
//        array ('MrWong',		'mrwong.png', NULL, NULL),
//        array ('Mylinkvault',		'mylinkvault.png', NULL, NULL),
        array ('Netscape',		'netscape.png', 'http://www.netscape.com/submit/?U=', '&T='),
        array ('NetVouz',		'netvouz.png', 'http://netvouz.com/action/submitBookmark?url=', '&popup=yes&description=&tags=&title='),
//        array ('NetVouz',		'netvouz.png', 'http://netvouz.com/action/submitBookmark?url=', '&title='),
        array ('Newsvine',		'newsvine.png', 'http://www.newsvine.com/_tools/seed&save?u=', '&h='),
//        array ('Newsvine',		'newsvine.png', 'http://www.newsvine.com/_wine/save?popoff=1&u=', '&tags=&blurb='),
//        array ('Nextaris',		'nextaris.png', NULL, NULL),
//        array ('Nowpublic',		'nowpublic.png', NULL, NULL),
//        array ('Oneview',		'oneview.png', 'http://beta.oneview.de:80/quickadd/neu/addBookmark.jsf?URL=', '&title='),
//        array ('Onlywire',		'onlywire.png', NULL, NULL),
//        array ('Pligg',		'pligg.png', NULL, NULL),
//        array ('Portachi',		'portachi.png', NULL, NULL),
//        array ('Protopage',		'protopage.png', NULL, NULL),
        array ('RawSugar',		'rawsugar.png', 'http://www.rawsugar.com/pages/tagger.faces?turl=', '&tttl='),
        array ('Reddit',		'reddit.png', 'http://reddit.com/submit?url=', '&title='),
//        array ('Rojo',		'rojo.png', NULL, NULL),
        array ('Scuttle',		'scuttle.png', 'http://www.scuttle.org/bookmarks.php/maxpower?action=add&address=', '&description='),
//        array ('Searchles',		'searchles.png', NULL, NULL),
        array ('Shadows',		'shadows.png', 'http://www.shadows.com/features/tcr.htm?url=', '&title='),
//        array ('Shadows',		'shadows.png', 'http://www.shadows.com/bookmark/saveLink.rails?page=', '&title='),
//        array ('Shoutwire',		'shoutwire.png', NULL, NULL),
        array ('Simpy',			'simpy.png', 'http://simpy.com/simpy/LinkAdd.do?href=', '&tags=&note=&title='),
//        array ('Simpy',		'simpy.png', 'http://simpy.com/simpy/LinkAdd.do?href=', '&title='),
        array ('Slashdot',		'slashdot.png', 'http://slashdot.org/bookmark.pl?url=', '&title='),
        array ('Smarking',		'smarking.png', 'http://smarking.com/editbookmark/?url=', '&tags=&description='),
//        array ('Spurl',		'spurl.png', 'http://www.spurl.net/spurl.php?url=', '&title='),
        array ('Spurl',			'spurl.png', 'http://www.spurl.net/spurl.php?v=3&tags=&url=', '&title='),
//        array ('Spurl',		'.png', 'http://www.spurl.net/spurl.php?v=3&url=', '&title='),
//        array ('Squidoo',		'squidoo.png', NULL, NULL),
        array ('StumbleUpon',		'stumbleupon.png', 'http://www.stumbleupon.com/submit?url=', '&title='),
//        array ('Tabmarks',		'tabmarks.png', NULL, NULL),
//        array ('Taggle',		'taggle.png', NULL, NULL),
//        array ('Tag Hop',		'taghop.png', NULL, NULL),
//        array ('Taggly',		'taggly.png', NULL, NULL),
//        array ('Tagtooga',		'tagtooga.png', NULL, NULL),
//        array ('TailRank',		'tailrank.png', NULL, NULL),
        array ('Technorati',		'technorati.png', 'http://technorati.com/faves?tag=&add=', NULL),
//        array ('Technorati',		'technorati.png', 'http://technorati.com/faves?add=', '&title='),
//        array ('Tutorialism',		'tutorialism.png', NULL, NULL),
//        array ('Unalog',		'unalog.png', NULL, NULL),
//        array ('Wapher',		'wapher.png', NULL, NULL),
        array ('Webnews',		'webnews.png', 'http://www.webnews.de/einstellen?url=', '&title='),
//        array ('Whitesoap',		'whitesoap.png', NULL, NULL),
//        array ('Wink',		'wink.png', NULL, NULL),
//        array ('WireFan',		'wirefan.png', NULL, NULL),
        array ('Wists',			'wists.png', 'http://wists.com/r.php?c=&r=', '&title='),
//        array ('Wists',		'wists.png', 'http://www.wists.com/?action=add&url=', '&title='),
        array ('Yahoo',			'yahoo.png', 'http://myweb2.search.yahoo.com/myresults/bookmarklet?u=', '&d=&tag=&t='),
//        array ('Yahoo',		'yahoo.png', 'http://myweb2.search.yahoo.com/myresults/bookmarklet?u=', '&t='),
//        array ('Yahoo',		'yahoo.png', 'http://myweb.yahoo.com/myresults/bookmarklet?u=', '&t='),
        array ('Yigg',			'yigg.png', 'http://yigg.de/neu?exturl=', NULL),
//        array ('Zumaa',		'zumaa.png', NULL, NULL),
//        array ('Zurpy',		'zurpy.png', NULL, NULL),
*/
      );

      $i_max = sizeof ($a);
      for ($i = 0; $i < $i_max; $i++)
        $p .= '<a class="widget_relate_img" href="'
             .$a[$i][2]
             .urlencode ($url)
             .($a[$i][3] ? $a[$i][3].urlencode ($title) : '')
             .'" alt="Add to '
             .$a[$i][0]
             .'" title="Add to '
             .$a[$i][0]
             .'">'
             .'<img src="images/widget/widget_relate_'
             .$a[$i][1]
             .'" border="0"></a>';

      $p .= $lf;
    }

  return $p; 
}


/*
<a href="javascript:bookmarksite('Bittorrent Search Engine', 'http://yotoshi.com')">Bookmark us!</a>


  .'http://myjeeves.ask.com/mysearch/BookmarkIt?v=1.2&t=webpages&title='+encodeURIComponent(document.title)+'&url='+encodeURIComponent(location.href)+'" title="bookmark to Jeeves" ><img src="http://yotoshi.com/image/96479491.png" alt="askjeeves"  height="16" width="16" /></a>');
  .'http://www.blinklist.com/index.php?Action=Blink/addblink.php&Url='+encodeURIComponent(location.href)+'&Title='+encodeURIComponent(document.title)+'" title="Add To BlinkList"><img src="http://yotoshi.com/image/89442389.png" alt="BlinkList"  height="16" width="16" /></a>');
  .'http://blogmarks.net/my/new.php?mini=1&title='+encodeURIComponent(document.title)+'&url='+encodeURIComponent(location.href)+'" title="Bookmark This to Blogmarks"><img src="http://yotoshi.com/image/7577931.png" alt="Blogmarks"  height="16" width="16" /></a>');
   .'http://buddymarks.com/add_bookmark.php?bookmark_title='+encodeURIComponent(document.title)+'&bookmark_url='+encodeURIComponent(location.href)+'" title="bookmark to Buddymarks" ><img src="http://yotoshi.com/image/69894407.png" alt="Buddymarks"  height="16" width="16" /></a>');
  .'http://del.icio.us/post?url='+encodeURIComponent(location.href)+'&title='+encodeURIComponent(document.title)+'" title="Bookmark This to del.icio.us"><img src="http://yotoshi.com/image/65682475.png" alt="del.icio.us"  height="16" width="16" /></a>');
  .'http://digg.com/submit?phase=2&url='+encodeURIComponent(location.href)+'" title="Digg This!"><img src="http://yotoshi.com/image/61822091.png" alt="digg"  height="16" width="16" /></a>');
  .'http://www.feedmarker.com/admin.php?do=bookmarklet_mark&url='+encodeURIComponent(location.href)+'&title='+encodeURIComponent(document.title)+';" title="Add to feedmarker" ><img src="http://yotoshi.com/image/95971882.png"alt="Feedmarker"  height="16" width="16" /></a>');
  .'http://www.furl.net/storeIt.jsp?u='+encodeURIComponent(location.href)+'&t='+encodeURIComponent(document.title)+'" title="Bookmark To Furl"><img src=http://www.furl.net/i/favicon.gif alt="Furl button"  height="16" width="16" /></a>');
  .'http://www.google.com/bookmarks/mark?op=add&bkmk='+encodeURIComponent(location.href)+'&title='+encodeURIComponent(document.title)+'" title="Bookmark to Google"><img src="http://yotoshi.com/image/35814433.png" alt="Google"  height="16" width="16" /></a>');
   .'http://www.hyperlinkomatic.com/lm2/add.html?LinkTitle='+encodeURIComponent(document.title)+'&LinkUrl='+encodeURIComponent(location.href)+'" title="Add to HLOM" ><img src="http://yotoshi.com/image/67303319.png" alt="HOLM" height="15" width="15" /></a>');
  .'http://ma.gnolia.com/bookmarklet/add?url='+encodeURIComponent(location.href)+'&title='+encodeURIComponent(document.title)+'" title="Add to ma.gnolia"><img src="http://yotoshi.com/image/44917000.png" alt="ma.gnolia"  height="16" width="16" /></a>');
  .'http://www.netvouz.com/action/submitBookmark?url='+encodeURIComponent(location.href)+'&title='+encodeURIComponent(document.title)+'&popup=no" title="Bookmark to Netvouz"><img src="http://yotoshi.com/image/1073885.png" alt="Netvouz"  height="16" width="16" /></a>');
   .'http://www.newsvine.com/_tools/seed&save?u='+encodeURIComponent(location.href)+'&h='+encodeURIComponent(document.title)+'" title="Bookmark to Newsvine" ><img src="http://yotoshi.com/image/61968867.png" alt="Newsvine"  height="16" width="16" /></a>');
    .'http://www.nextaris.com/servlet/com.surfwax.Nextaris.Bookmarklets?cmd=addurlrequest&v=1&url='+encodeURIComponent(location.href)+'&title='+encodeURIComponent(document.title)+'"><img src="http://yotoshi.com/image/92909732.png" alt="Nextaris"  height="16" width="16" /></a>');
  .'http://view.nowpublic.com/?src='+encodeURIComponent(location.href)+'&t='+encodeURIComponent(document.title)+'"><img src="http://yotoshi.com/image/38451081.png" alt="Nowpublic"  height="16" width="16" /></a>');
  .'http://reddit.com/submit?url='+encodeURIComponent(location.href)+'&title='+encodeURIComponent(document.title)+'" title="Add to reddit"><img src="http://yotoshi.com/image/33092456.png" alt="reddit"  height="16" width="16" /></a>');
  .'http://www.rawsugar.com/pages/tagger.faces?turl='+encodeURIComponent(location.href)+'&tttl='+encodeURIComponent(document.title)+'" title="Bookmark to RawSugar"><img src="http://yotoshi.com/image/56440303.png" alt="rawsugar"  height="16" width="16" /></a>');
  .'http://scuttle.org/bookmarks.php/pass?action=add&address='+encodeURIComponent(location.href)+'&title='+encodeURIComponent(document.title)+'" title="Bookmark to Scuttle"><img src="http://yotoshi.com/image/50964829.png" alt="Scuttle"  height="16" width="16" /></a>');
  .'http://www.shadows.com/features/tcr.htm?url='+encodeURIComponent(location.href)+'&title='+encodeURIComponent(document.title)+'" title="Tag to Shadows"><img src="http://yotoshi.com/image/30177473.png" alt="Shadows"  height="16" width="16" /></a>');
  .'http://www.simpy.com/simpy/LinkAdd.do?href='+encodeURIComponent(location.href)+'&title='+encodeURIComponent(document.title)+'" title="Add to Simpy"><img src="http://yotoshi.com/image/70286063.png" alt="Simpy"  height="16" width="16" /></a>');
  .'http://smarking.com/editbookmark/?url='+encodeURIComponent(location.href)+'&description='+encodeURIComponent(document.title)+'"  title="Bookmark This to Smarking"><img src="http://yotoshi.com/image/21598036.png" alt="Smarking"  height="16" width="16" /></a>');
  .'http://www.spurl.net/spurl.php?url='+encodeURIComponent(location.href)+'&title='+encodeURIComponent(document.title)+'" title="Spurl This!"><img src="http://yotoshi.com/image/83344081.png" alt="Spurl"  height="16" width="16" /></a>');
   .'http://www.squidoo.com/lensmaster/bookmark?'+encodeURIComponent(location.href)+'"><img src="http://yotoshi.com/image/53248495.png" alt="Squidoo"  height="16" width="16" /></a>');
  .'http://tailrank.com/share/?text=&link_href='+encodeURIComponent(location.href)+'&title='+encodeURIComponent(document.title)+'"><img src="http://yotoshi.com/image/47382617.png" alt="Tailrank"  height="16" width="16" /></a>');
  .'http://technorati.com/faves?add='+encodeURIComponent(location.href)+'" title="Add to Technorati Favorites" ><img src="http://yotoshi.com/image/78708502.png" alt="Technorati"  height="16" width="16" /></a>');
  .'http://unalog.com/my/stack/link?url='+encodeURIComponent(location.href)+'&title='+encodeURIComponent(document.title)+'" title="Add to Unalog" ><img src="http://yotoshi.com/image/19300886.png" alt="Unalog"  height="16" width="16" /></a>');
  .'http://www.wink.com/_/tag?url='+encodeURIComponent(location.href)+'&doctitle='+encodeURIComponent(document.title)+'" title="Wink This!"><img src="http://yotoshi.com/image/95672969.png" alt="Wink"  height="16" width="16" /></a>');
  .'http://myweb2.search.yahoo.com/myresults/bookmarklet?t='+encodeURIComponent(document.title)+'&u='+encodeURIComponent(location.href)+'" title="Bookmark To Yahoo! MyWeb"><img src="http://yotoshi.com/image/41626225.png" alt="Yahoo! Myweb"  height="16" width="16" /></a>');
   .'http://www.addtoany.com/? linkname='+encodeURIComponent(document.title)+'&linkurl='+encodeURIComponent(location.href)+' &type=page"><img src="http://yotoshi.com/image/10418375.png" alt="AddToAny" height="16" width="16" /></a>');
  .'http://www.onlywire.com/b/?u='+encodeURIComponent(location.href)+'&t='+encodeURIComponent(document.title)+'" title="Bookmark with Onlywire" ><img src="http://yotoshi.com/image/2315000.png" alt="onlywire" height="16" width="16" /></a>');
  .'http://myjeeves.ask.com/mysearch/BookmarkIt?v=1.2&amp;t=webpages&amp;title=Yotoshi%20%3A%20Bittorrent%20Search%20Engine&amp;url=http%3A%2F%2Fwww.yotoshi.com%2F" title="bookmark to Jeeves">

<a href="http://www.blinklist.com/index.php?Action=Blink/addblink.php&amp;Url=http%3A%2F%2Fwww.yotoshi.com%2F&amp;Title=Yotoshi%20%3A%20Bittorrent%20Search%20Engine" title="Add To BlinkList">
<a href="http://blogmarks.net/my/new.php?mini=1&amp;title=Yotoshi%20%3A%20Bittorrent%20Search%20Engine&amp;url=http%3A%2F%2Fwww.yotoshi.com%2F" title="Bookmark This to Blogmarks">
<a href="http://buddymarks.com/add_bookmark.php?bookmark_title=Yotoshi%20%3A%20Bittorrent%20Search%20Engine&amp;bookmark_url=http%3A%2F%2Fwww.yotoshi.com%2F" title="bookmark to Buddymarks">
<a href="http://del.icio.us/post?url=http%3A%2F%2Fwww.yotoshi.com%2F&amp;title=Yotoshi%20%3A%20Bittorrent%20Search%20Engine" title="Bookmark This to del.icio.us">
<a href="http://digg.com/submit?phase=2&amp;url=http%3A%2F%2Fwww.yotoshi.com%2F" title="Digg This!">
<a href="http://www.feedmarker.com/admin.php?do=bookmarklet_mark&amp;url=http%3A%2F%2Fwww.yotoshi.com%2F&amp;title=Yotoshi%20%3A%20Bittorrent%20Search%20Engine;" title="Add to feedmarker">
<a href="http://www.furl.net/storeIt.jsp?u=http%3A%2F%2Fwww.yotoshi.com%2F&amp;t=Yotoshi%20%3A%20Bittorrent%20Search%20Engine" title="Bookmark To Furl">
<a href="http://www.google.com/bookmarks/mark?op=add&amp;bkmk=http%3A%2F%2Fwww.yotoshi.com%2F&amp;title=Yotoshi%20%3A%20Bittorrent%20Search%20Engine" title="Bookmark to Google">
<a href="http://www.hyperlinkomatic.com/lm2/add.html?LinkTitle=Yotoshi%20%3A%20Bittorrent%20Search%20Engine&amp;LinkUrl=http%3A%2F%2Fwww.yotoshi.com%2F" title="Add to HLOM">
<a href="http://ma.gnolia.com/bookmarklet/add?url=http%3A%2F%2Fwww.yotoshi.com%2F&amp;title=Yotoshi%20%3A%20Bittorrent%20Search%20Engine" title="Add to ma.gnolia">
<a href="http://www.netvouz.com/action/submitBookmark?url=http%3A%2F%2Fwww.yotoshi.com%2F&amp;title=Yotoshi%20%3A%20Bittorrent%20Search%20Engine&amp;popup=no" title="Bookmark to Netvouz">
<a href="http://www.newsvine.com/_tools/seed&amp;save?u=http%3A%2F%2Fwww.yotoshi.com%2F&amp;h=Yotoshi%20%3A%20Bittorrent%20Search%20Engine" title="Bookmark to Newsvine">
<a href="http://www.nextaris.com/servlet/com.surfwax.Nextaris.Bookmarklets?cmd=addurlrequest&amp;v=1&amp;url=http%3A%2F%2Fwww.yotoshi.com%2F&amp;title=Yotoshi%20%3A%20Bittorrent%20Search%20Engine">
<a href="http://view.nowpublic.com/?src=http%3A%2F%2Fwww.yotoshi.com%2F&amp;t=Yotoshi%20%3A%20Bittorrent%20Search%20Engine">
<a href="http://reddit.com/submit?url=http%3A%2F%2Fwww.yotoshi.com%2F&amp;title=Yotoshi%20%3A%20Bittorrent%20Search%20Engine" title="Add to reddit">
<a href="http://www.rawsugar.com/pages/tagger.faces?turl=http%3A%2F%2Fwww.yotoshi.com%2F&amp;tttl=Yotoshi%20%3A%20Bittorrent%20Search%20Engine" title="Bookmark to RawSugar">
<a href="http://scuttle.org/bookmarks.php/pass?action=add&amp;address=http%3A%2F%2Fwww.yotoshi.com%2F&amp;title=Yotoshi%20%3A%20Bittorrent%20Search%20Engine" title="Bookmark to Scuttle">
<a href="http://www.shadows.com/features/tcr.htm?url=http%3A%2F%2Fwww.yotoshi.com%2F&amp;title=Yotoshi%20%3A%20Bittorrent%20Search%20Engine" title="Tag to Shadows">
<a href="http://www.simpy.com/simpy/LinkAdd.do?href=http%3A%2F%2Fwww.yotoshi.com%2F&amp;title=Yotoshi%20%3A%20Bittorrent%20Search%20Engine" title="Add to Simpy">
<a href="http://smarking.com/editbookmark/?url=http%3A%2F%2Fwww.yotoshi.com%2F&amp;description=Yotoshi%20%3A%20Bittorrent%20Search%20Engine" title="Bookmark This to Smarking">
<a href="http://www.spurl.net/spurl.php?url=http%3A%2F%2Fwww.yotoshi.com%2F&amp;title=Yotoshi%20%3A%20Bittorrent%20Search%20Engine" title="Spurl This!">
<a href="http://www.squidoo.com/lensmaster/bookmark?http%3A%2F%2Fwww.yotoshi.com%2F">
<a href="http://tailrank.com/share/?text=&amp;link_href=http%3A%2F%2Fwww.yotoshi.com%2F&amp;title=Yotoshi%20%3A%20Bittorrent%20Search%20Engine">
<a href="http://technorati.com/faves?add=http%3A%2F%2Fwww.yotoshi.com%2F" title="Add to Technorati Favorites">
<a href="http://unalog.com/my/stack/link?url=http%3A%2F%2Fwww.yotoshi.com%2F&amp;title=Yotoshi%20%3A%20Bittorrent%20Search%20Engine" title="Add to Unalog">
<a href="http://www.wink.com/_/tag?url=http%3A%2F%2Fwww.yotoshi.com%2F&amp;doctitle=Yotoshi%20%3A%20Bittorrent%20Search%20Engine" title="Wink This!">
<a href="http://myweb2.search.yahoo.com/myresults/bookmarklet?t=Yotoshi%20%3A%20Bittorrent%20Search%20Engine&amp;u=http%3A%2F%2Fwww.yotoshi.com%2F" title="Bookmark To Yahoo! MyWeb">
<a href="http://www.addtoany.com/?%20linkname=Yotoshi%20%3A%20Bittorrent%20Search%20Engine&amp;linkurl=http%3A%2F%2Fwww.yotoshi.com%2F%20&amp;type=page">
<a href="http://www.onlywire.com/b/?u=http%3A%2F%2Fwww.yotoshi.com%2F&amp;t=Yotoshi%20%3A%20Bittorrent%20Search%20Engine" title="Bookmark with Onlywire">

<strong>Primarily CPM Based Ad Networks</strong>
<a href="http://www.121media.com/">121Media</a>
<a href="http://www.247realmedia.com/">24/7 RealMedia</a> </li>
<a href="http://www.accelerator-media.com/">Accelerator-Media </a>
<a href="http://www.asn.com/">Ad Solutions Network </a>
<a href="http://www.adworldnetwork.com/">Ad World Network</a>
<a href="http://www.adagency1.com/">AdAgency1</a>
<a href="http://www.adbonus.com/">AdBonus</a>
<a href="http://www.addynamix.com/">AdDynamix / Pennyweb Networks</a> </li>
<a href="http://www.adorigin.com/">AdOrigin</a>
<a href="http://www.adpepper.com/">AdPepper</a>
<a href="http://www.adsmart.net/">AdSmart</a>
<a href="http://www.adtegrity.com/">Adtegrity</a>
<a href="http://www.adzuba.com/">AdZuba</a>
<a href="http://www.ampiramedia.com/">Ampira Media</a>
<a href="http://www.bannerconnect.net/">Bannerconnect</a>
<a href="http://www.bannerspace.com/">BannerSpace</a>
<a href="http://www.bluelithium.com/">BlueLithium</a>
<a href="http://www.burstmedia.com/">BURST! Media</a>
<a href="http://casalemedia.com/">Casale Media</a>
<a href="http://www.claxon.com/">Claxon Media</a>
<a href="http://clickagents.com/">Click Agents </a>
<a href="http://clickbooth.com/">ClickBooth </a>
<a href="http://www.cpxinteractive.com/">CPX Interactive (Formerly Buds Media)</a> </li>
<a href="http://www.euroclick.com/">EuroClick </a>
<a href="http://experclick.com/">Experclick </a>
<a href="http://www.fastclick.com/">FastClick/ValueClick</a>
<a href="http://www.federatedmedia.net/">Federated Media</a>
<a href="http://gold-group.com/">Gold Group</a>
<a href="http://www.gorillanation.com/">Gorilla Nation Media</a>
<a href="http://www.hurricanedigitalmedia.com/">Hurricane Digital Media</a>
<a href="http://impressionup.com/">Impression|Up</a>
<a href="http://www.interclick.com/">InterClick</a> </li>
<a href="http://www.interevco.com/">Interevco (Interactive Revenue Company Ltd.) </a>
<a href="http://joetec.net/">Joetec</a>
<a href="http://www.mammamediasolutions.com/">Mamma Media / FocusIn</a> </li>
<a href="http://www.maxonline.com/">MaxOnline</a>
<a href="http://oridian.com/">Oridian</a>
<a href="http://www.premiumnetwork.com/">Premium Network</a>
<a href="http://www.quakemarketing.com/">Quake Marketing</a>
<a href="http://www.quinstreet.com/">Quin Street</a>
<a href="http://www.realcastmedia.com/">RealCastMedia</a>
<a href="http://www.realtechnetwork.com/">RealTechNetwork</a>
<a href="http://www.revenue.net/">Revenue.net</a>
<a href="http://www.rightmedia.com/">Right Media</a>
<a href="http://www.rydium.com/">Rydium</a>
<a href="http://www.robertsherman.com/">The Robert Sherman Company</a>
<a href="http://www.tmp.com/">TMP</a>
<a href="http://tribalfusion.com/">Tribal Fusion</a>
<a href="http://valuead.com/">Valuead.com</a>
<a href="http://www.yesadvertising.com/">Yes Advertising</a>

<strong>Primarily CPA/CPL Ad Networks</strong> </p>
<a href="http://advertising.com/">Advertising.com</a>
<a href="http://www.axill.com/">Axill</a>
<a href="http://www.azoogleads.com/">Azoogle Ads</a>
<a href="http://clickbank.com/">ClickBank</a>
<a href="http://www.clickxchange.com/">ClickXChange</a>
<a href="http://www.cj.com/">Commission Junction / BeFree</a>
<a href="http://www.coverclicks.com/">CoverClicks</a>
<a href="http://www.darkblue.com/">DarkBlue</a>
<a href="http://www.drivepm.com/">DrivePM</a>
<a href="https://www.emarketmakers.com/">emarketmakers</a>
<a href="http://www.linkshare.com/">Linkshare</a>
<a href="http://www.maxbounty.com/">Maxbounty</a>
<a href="http://metareward.com/">Meta Reward</a>
<a href="http://profitcenternetwork.com/">ProfitCenter</a>
<a href="http://www.revenue.net/">Revenue.Net</a>
<a href="http://www.shareasale.com/">ShareASale</a>
<a href="http://strategicaffiliates.com/">Strategic Affiliates</a>
<a href="http://websponsors.com/">WebSponsors</a>

<strong>Primarily CPC AND/OR Text Based/Contextual Ad Networks</strong> </p>
<a href="http://https//www.google.com/adsense/">Google AdSense</a>
<a href="http://publisher.yahoo.com/">Yahoo! Publisher Network</a>
<a href="http://www.adforce.com/">AdForce</a>
<a href="http://www.adhearus.org/">AdHearUs</a>
<a href="http://www.adknowledge.com/">AdKnowledge</a>
<a href="http://www.quigo.com/adsonarexchange.htm">AdSonar</a>
<a href="http://www.affiliatesensor.com/">Affiliate Sensor</a>
<a href="http://www.allclicks.com/">All Clicks</a>
<a href="http://www.allfeeds.com/">AllFeeds</a>
<a href="http://www.bannerboxes.com/">BannerBoxes</a>
<a href="http://www.bclick.com/">BClick</a>
<a href="http://www.bidclix.com/">BidClix</a>
<a href="http://www.bidvertiser.com/">Bidvertiser</a>
<a href="http://www.cbprosense.net/">CBprosense</a>
<a href="http://www.clicksor.com/">Clicksor</a>
<a href="http://www.expoactive.com/">ExpoActive</a>
<a href="http://www.industrybrains.com/">IndustryBrains</a>
<a href="http://www.mirago.com/">Mirago</a>
<a href="http://www.miva.com/">Miva</a>
<a href="http://www.nixxie.com/">Nixxie</a>
<a href="http://onemonkey.com/">One Monkey</a>
<a href="http://www.oxado.com/">Oxado</a>
<a href="http://targetpoint.com/">TargetPoint</a>
<a href="http://www.textads.biz/">Textads Dot Biz</a>
<a href="http://www.textwise.com/">TextWise</a>
<a href="http://www.text-link-ads.com/">Text Link Ads</a>
<a href="http://www.vibrantmedia.com/">Vibrant Media</a>
<a href="http://www.webadvertising.ca/">WebAdvertising.ca</a>
<a href="http://www.adbrite.com/">AdBright</a> </li>
<a href="http://www.hyperbidder.com/">HyperBidder</a>


<strong>Shopping/Comparison Networks</strong> </p>
<a href="http://www.ttzmedia.com/">TTZ Media</a>
<a href="http://www.pricegrabber.com/">PriceGrabber</a>
<a href="http://www.chitika.com/">Chitika</a>
<a href="http://www.shopping.com/">Shopping.com</a>
<a href="http://shopper.cnet.com/">CNet Shopper</a>


non-standard
<a href="http://7adpower.com/">7AdPower</a> </li>
<a href="http://www.opt-media.com/">Opt-Media</a> </li>
<a href="http://www.paypopup.com/">PayPopUp</a> </li>
<a href="http://www.pointroll.com/">PointRoll</a> </li>
<a href="http://www.popuptraffic.com/">PopUpTraffic</a>
<a href="http://www.tremornetwork.com/">Tremor Network</a>
<a href="http://www.whenu.com/">WhenU</a>
<a href="http://www.payperpost.com/">PayPerPost</a>
<a href="http://www.reviewme.com/">ReviewMe</a>
<a href="http://www.creamaid.com/">CreamAid</a>

<strong>Specific Demographic Ad Networks </strong>
<a href="http://click.absoluteagency.com/">Absolute Agency</a>
<a href="http://www.avnads.com/index_avn.php">AVN Ads</a> (*****WARNING: ADULT NETWORK*****)</li>
<a href="http://www.blogads.com/">BlogAds</a>
<a href="http://www.crispads.com/">CrispAds Blog Advertising Network</a>
<a href="http://heragency.com/">HerAgency</a>
<a href="http://www.hispanoclick.com/">HispanoClick</a>
<a href="http://www.pheedo.com/">Pheedo RSS &amp; Weblog Marketing Solutions</a>
<a href="http://www.qumana.com/adgenta.htm">Qumana Adgenta Blog Ads</a>
<a href="http://www.waypointcash.com/">WayPointCash</a> (*****WARNING: ADULT NETWORK*****) </li>


<strong>NON-US Primarily CPM Based Ad Networks </strong>
<a href="http://www.clickhype.com/">ClickHype</a>
<a href="http://www.dmoglobal.com/">DMO Global</a>


<strong>NON-US Primarily CPC AND/OR Text Based/Contextual Ad Networks </strong>
<a href="http://responserepublic.net/">Response Republic</a>
<a href="http://www.peakclick.com/">PeakClick</a>


<strong>NON-US Primarily CPA/CPL Ad Networks </strong>
<a href="http://www.tradedoubler.com/">TradeDoubler</a> </li>
<a href="http://commissionmonster.com.au/">Commission Monster</a> </li>
<a href="http://affiliatefuture.co.uk/">Affiliate Future</a>
*/


}

?>