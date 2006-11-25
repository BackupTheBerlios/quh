<?php
//phpinfo();
require_once ("config.php");
require_once ("misc/misc.php");
require_once ("misc/tags.php");

  if ($use_gzip)
    ob_start ("ob_gzhandler"); // enable gzip

?>
<html>
<head>
<?php

  echo html_head_tags ("images/icon.png",    // icon
                       "Quh aims to play everything that makes noise!",
                       "0",   // refresh
                       "utf-8", // charset
                       1,       // use dublin core
                       NULL,    // default dc-desc
                       "mp3, audio, console, linux",
                       "http://quh.berlios.de",
                       "en",
                       "NoisyB");

?>
</head>
<body bgcolor="#000000" text="#ffffff">
<span style="font-family: monospace;">
<br>
<br>
<br>
<center><img src="images/q1.png" height="150"><img src="images/hemsa.png" height="110">
<br>
<br>
<br>
<br>
<img src="images/bg3.png" border="0">
<br>
<br>
<br>
<br>
<table width="640">
  <tr>
    <td>
<center>
<font size="-1">
<img src="images/white.png" width="620" height="15" border="0">
<br>
<br>
<table>
  <tr>
    <td>
<font size="-1">
20061125 Quh 0.3.2 is available for download <img src="images/new.png"> <a href="http://freshmeat.net/rate/59213/"><img src="images/fm_rate_it_small2.png" border="0"></a><?php

//  digg_me ("http://www.digg.com/videos_people/The_most_beautiful_news_anchor_in_the_world");

?><br>
20061114 Quh 0.3.1 is available for download<br>
20060320 Quh 0.3.0 is available from CVS<br>
    </td>
  </tr>
</table>
<br>
<img src="images/white.png" width="620" height="15" border="0">
<br>
<br>
<img src="images/q3.png" height="20" border="0"> will replace all other audio 
players because there are too many :-)
<br> 
<br>
<img src="images/q1.png" height="20" border="0"> is made of many different filters:<br>
cdda read, flac decode, mp3 decode (mad), vorbis decode, audiofile (aifc,
aiff, au, avr, bicsf, iff, mpeg, nist, raw, sd1, sd2, sf2, smp, snd, voc,
wav), sndfile (aiff, au, avr, htk, iff, mat, paf, pvf, raw, sds, sf, voc,
w64, wav, xi), wav read, raw read, cache, cddb, console, demux, esound, id3,
libao, oss (linux), read<br>
support for much other stuff like reading text files, etc. will be included in future releases
<br>
<br>
<!--img src="images/q3.png" height="20" border="0"> can be controlled with a joystick
<br>
<br-->
<img src="images/q1.png" height="20" border="0"> is like an reference example
on how to cultivate bloated API's into an file-operations analogy
<br>
<br>
<img src="images/q2.png" height="20" border="0"> uses keyboard shortcuts similar
to those of MPlayer
<br>
<br>
<!--img src="images/q3.png" height="20" border="0"> speaks text files in different 
languages using speech synthesis (festival)
<br>
<br-->
<img src="images/q1.png" height="20" border="0"> does not require a desktop or GUI toolkit
<br>
<br>
<img src="images/q2.png" height="20" border="0"> is currently only ~90kBytes in size with all support enabled
<br>
<br>
<br>
<img src="images/white.png" width="620" height="15" border="0">
<br>
<a href="https://developer.berlios.de/project/showfiles.php?group_id=4373"><img src="images/download.png" border="0" height="160"></a><br>
<a href="http://developer.berlios.de/cvs/?group_id=4373"><img src="images/cvs.png" border="0" height="40"></a>
<a href="quh/changes.html"><img src="images/changes.png" border="0" height="40"></a>
<a href="quh/readme.html"><img src="images/readme.png" border="0" height="40"></a>
<a href="quh/license.html"><img src="images/license.png" border="0" height="40"></a>
<!--a href="mailto:noisyb@gmx.net"><img src="images/contact.png" border="0" height="40"></a-->
<br>
<img src="images/white.png" width="620" height="15" border="0">
</center>
    </td>
  </tr>
</table>
</center>
<a href="http://developer.berlios.de" title="BerliOS Developer"> <img src="http://developer.berlios.de/bslogo.php?group_id=4373" width="124px" height="32px" border="0" alt="BerliOS Developer Logo"></a>
<br>
<a href="http://www.webstats4u.com/stats?ADxgsAYFdTpD4fs/wmFkKysebBaA"><img src="http://m1.webstats4u.com/n?id=ADxgsAYFdTpD4fs/wmFkKysebBaA" border="0" width="18" height="18"></a>
</span>
</body>
</html>
<?php

  if ($use_gzip)
    ob_end_flush ();

?>

