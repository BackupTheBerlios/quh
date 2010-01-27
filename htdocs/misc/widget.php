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
    $p .= '<img class="widget_relate_img" src="images/widget_relate_paypal.png" border="0">'
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
    $p .= '<img class="widget_relate_img" src="images/widget_relate_tellafriend.png" border="0">'
         .'<a class="widget_relate_label" href="mailto:?body='
         .$url
         .'&subject='
         .$title
         .'"'
         .' title="Send this link to your friends">Tell a friend</a>'
         .$lf;
/*
  // add browser bookmark
  if ($flags & WIDGET_RELATE_BOOKMARK)
    $p .= '<img class="widget_relate_img" src="images/widget_relate_star.png" border="0">'
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
    $p .= '<img class="widget_relate_img" src="images/widget_relate_home.png" border="0">'
         .'<a class="widget_relate_label"'
         .' href="http://"'
         .' onclick="this.style.behavior=\'url(#default#homepage)\';this.setHomePage(\'http://torrent-finder.com\');"'
         .'>'
         .'Make us your start page</a>'
         .$lf;

  // add search plugin to browser
  if ($flags & WIDGET_RELATE_SEARCH)
    $p .= '<img class="widget_relate_img" src="images/widget_relate_search.png" border="0">'
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
    $p .= '<img class="widget_relate_img" src="images/widget_relate_rss.png" border="0">'
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
             .'<img src="images/widget_relate_'
             .$a[$i][1]
             .'" border="0"></a>';

      $p .= $lf;
    }

  return $p; 
}


/*
function
widget_panel ($url_array, $img_array, $w, $h, $tooltip)
{
?>
<script type="text/javascript">
<!--

//var test_array = new array  (<?php

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
  var img = new array (<?php

$p = '';
$i_max = sizeof ($img_array);
for ($i = 0; $i < $i_max; $i++)
  {
    if ($i)
      $p .= ', ';
    $p .= 'widget_panel_'.$i;
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
*/


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
  $size = array (
      728 => 90,
      468 => 60,
      728 => 90,
      468 => 60,
      234 => 60,
      120 => 600,
      160 => 600,
      120 => 240,
      336 => 280,
      300 => 250,
      250 => 250,
      200 => 200,
      180 => 150,
      125 => 125
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


// originally in rss.php
function
rss_to_array ($tag, $array, $url)
{
  $doc = new DOMdocument();
  $doc->load($url);

  $rss_array = array();
  $items = array();

  foreach($doc->getElementsByTagName($tag) AS $node)
    {
      foreach($array AS $key => $value)
        {
          $items[$value] = $node->getElementsByTagName($value)->item(0)->nodeValue;
        }
      array_push ($rss_array, $items);
    }

  return $rss_array;
}


function
widget_shoutbox ($rssfeed, $submit_shout_func)
{
  /*
    display rssfeed with shouts
    title: shout
    link: to user profile
    desc: user name and date
  */
  $rss = simplexml_load_file ($rssfeed);

  // executes submit_shout_func(shout) on submit
}


}

?>