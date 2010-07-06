<?php
//phpinfo();
require_once ("config.php");
require_once ("misc/misc.php");
require_once ("misc/widget.php");

  if ($use_gzip)
    ob_start ("ob_gzhandler"); // enable gzip

?>
<html>
<head>
<title>Quh aims to play everything that makes noise!</title>
</head>
<body bgcolor="#000000" text="#ffffff">
<br><!--table border="0" cellpadding="0" cellspacing="0" width="80%" height="100">
  <tr>
    <td border="0" cellpadding="0" cellspacing="0" bgcolor="#ffff80" align="center">
<font size="-1" face="arial" color="#000000">If you're looking for WEBAMP you may scroll to the <a href="#geotool">bottom</a> of this page</font>
    </td>
  </tr>
</table-->
<span style="font-family: monospace;">
<br>
<center>
<br>
<br>
<br>
<br>
<br>
<table>
  <tr>
    <td>
      <img src="images/left.png" border="0">
    </td>
    <td>
<table width="640">
  <tr>
    <td>
<img src="images/quh_logo2.png" border="0"><br><br><br>
<font size="-1">
Having ears makes sense again.
<br>
<br>
<img src="images/white.png" width="620" height="15" border="0">
<br>
<br>
<a href="images/ss0.png"><img src="images/ss0_small.png"></a>
<a href="images/ss1.png"><img src="images/ss1_small.png"></a>
<br>
<br>
    </td>
  </tr>
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
20061125 Quh 0.3.2 is available for download<br>
20061114 Quh 0.3.1 is available for download<br>
20060320 Quh 0.3.0 is available from CVS<br>
    </td>
  </tr>
</table>
<br>
<img src="images/white.png" width="620" height="15" border="0">
<br>
<br>
<img src="images/q3.png" height="20" border="0"> aims to replace all other audio 
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
<img src="images/q2.png" height="20" border="0"> uses keyboard shortcuts similar
to those of MPlayer
<br>
<br>
<img src="images/q1.png" height="20" border="0"> does not require a desktop or GUI toolkit
<br>
<br>
<img src="images/q3.png" height="20" border="0"> is available as ~90kBytes (with all support
enabled) cmdline player ("Quh") or as network version ("php_quh") using PHP, CSS, JS and Flash
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
<!--img src="images/white.png" width="620" height="15" border="0"-->
</center>
    </td>
  </tr>
</table>
    </td>
    <td>
      <img src="images/right.png" border="0">
    </td>
  </tr>
</table>
</center>
<br>
<br>
<br>
<table border="0" bgcolor="#ffffff">
  <tr>
    <td valign="top">
<?php 

  echo widget_pr_berlios().'<br>';
  echo widget_relate ($relate_site_title_s, $relate_site_url_s)
        ."<br>";

  require_once ('project_links.php');

?></td></tr></table><br>
<br>
</span>
<!--a href="http://developer.berlios.de" title="BerliOS Developer"> <img src="http://developer.berlios.de/bslogo.php?group_id=4373" width="124px" height="32px" border="0" alt="BerliOS Developer Logo"></a>
<br-->
</body>
</html>
<?php

  if ($use_gzip)
    ob_end_flush ();

?>