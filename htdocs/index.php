<?php
//phpinfo();
require_once ("config.php");
require_once ("misc/misc.php");

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
<img src="images/new.png" border="0"> 20060320 Quh 0.3.0 is available from CVS<br>
<br>
<img src="images/white.png" width="620" height="15" border="0">
<br>
<br>
<img src="images/q3.png" height="20" border="0"> will replace all other audio 
players because there are too many :-)
<br> 
<br>
<img src="images/q2.png" height="20" border="0"> supports 100+ different noise
containing file, stream or data formats/sources/filters like:<br>
cdda read, ffmpeg (3g2, 3gp, 4xm, RoQ, aac, ac3, adts, aiff, alaw, asf, au,
avi, avs, crc, daud, dts, dv, dv1394, dvd, ea, ffm, film_cpk, flic, flv,
gif, h261, h263, h264, idcin, ingenient, ipmovie, m4v, matroska, mjpeg, mm,
mmf, mov, mov,mp4,m4a,3gp,3g2,mj2, mp2, mp3, mp4, mpeg, mpeg1video,
mpeg2video, mpegts, mpegvideo, mpjpeg, mulaw, nsv, nut, nuv, ogg, psp,
psxstr, rawvideo, redir, rm, rtp, rtsp, s16be, s16le, s8, sdp, shn, smk,
sol, svcd, swf, tta, u16be, u16le, u8, vcd, video4linux, video4linux2, vmd,
vob, voc, wav, wc3movie, wsaud, wsvqa, yuv4mpegpipe), flac decode, mp3
decode (using libmad), mikmod (669, amf, dsm, far, imf, it, med, mod, mtm,
s3m, stm, ult, xm), xmp (669, ac1d, alm, amd, chn, crb, di, digi, emod, exo,
far, fc-m, fnk, imf, it, kris, ksm, m15, mdl, med, mod, modl, mp, mtm, mtn,
np, okta, pm, pp10, prom, pru, ptm, rad, s3m, sfx, stim, stm, stx, tp, ult,
unic, wn, wow, xann, xm, zen), libsidplay2 (sid, dat, inf, c64, prg, p00,
info, data, str, mus), sndfile (aiff, au, avr, htk, iff, mat, paf, pvf, raw,
sds, sf, voc, w64, wav, xi), flite (txt), vorbis decode, audiofile (aifc,
aiff, au, avr, bicsf, iff, mpeg, nist, raw, sd1, sd2, sf2, smp, snd, voc,
wav), wav read, raw read, alsa, cdda write (tracks and cue), cddb, lyrics,
console, demux, esound, id3 (using libid3tag), joystick (using libsdl),
libao, oss (linux), raw write, read, sdl (audio), speaker, stdin read,
stdout write, vol, wav write
<br>
<br>
<img src="images/q1.png" height="20" border="0"> supports 10+ different ways of
producing noise like Alsa, OSS, ESD, libao, SDL, PC speaker (if present), ...
<br>
<br>
<img src="images/q2.png" height="20" border="0"> (optionally) takes advantage of
libraries like audiofile, sndfile, xmp (Extended Module Player), mikmod, sidplay,
SDL, festival, libxmms, ...
<br>
<br>
<img src="images/q3.png" height="20" border="0"> can be controlled with a joystick
<!-- or remote control (lirc)-->
<br>
<br>
<img src="images/q1.png" height="20" border="0"> is like an reference example
on how to cultivate bloated API's into an file-operations analogy
<br>
<br>
<img src="images/q2.png" height="20" border="0"> uses keyboard shortcuts similar
to those of MPlayer
<br>
<br>
<img src="images/q3.png" height="20" border="0"> speaks text files in different 
languages using speech synthesis (festival)
<br>
<br>
<img src="images/q2.png" height="20" border="0"> does NOT require an installed 
desktop enviroment or GUI toolkit
<br>
<br>
<img src="images/q1.png" height="20" border="0"> does NOT require any plugins
<br>
<br>
<img src="images/q2.png" height="20" border="0"> is only ~100kBytes in size with all support enabled<br>
(~50 with upx compression)
<br>
<br>
<img src="images/q3.png" height="20" border="0"> sources utilize for the first
time a real autoconf replacement
<br>
<br>
<br>
<img src="images/white.png" width="620" height="15" border="0">
<br>
<!--img src="images/download.png" border="0" height="160"></a><br-->
<a href="http://developer.berlios.de/cvs/?group_id=4373"><img src="images/cvs.png" border="0" height="40"></a>
<a href="quh/changes.html"><img src="images/changes.png" border="0" height="40"></a>
<a href="quh/readme.html"><img src="images/readme.png" border="0" height="40"></a>
<a href="quh/license.html"><img src="images/license.png" border="0" height="40"></a>
<a href="mailto:noisyb@gmx.net"><img src="images/contact.png" border="0" height="40"></a>
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

