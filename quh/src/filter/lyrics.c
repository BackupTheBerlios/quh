/*
lyrics.c - lyrics3 and lyrics from the new filter for quh

Copyright (c) 2005 NoisyB (noisyb@gmx.net)


This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifdef  HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef  HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "misc/bswap.h"
#include "misc/itypes.h"
#include "misc/file.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "misc/misc.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "lyrics.h"
#ifdef  HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef  HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "misc/property.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "lyrics.h"


#warning TODO: fix


//static int lyrics_in = 0;
static unsigned long lyrics_id = 0;  
#ifdef  USE_NETGUI
static netgui_t *netgui = NULL;
#endif
//static const char *lyrics_host = "http://www.google.com";
//static const char *lyrics_uri = "/~lyrics/lyrics.cgi";
static char index_name[QUH_MAXINDEX][80];
      

static int
quh_lyrics_in_init (st_quh_nfo_t *file)
{
  (void) file;

  if (!quh_get_object_s (quh.filter_chain, QUH_OPTION))
    quh_set_object_s (quh.filter_chain, QUH_OPTION, (void *) "http://freedb.freedb.org/~lyrics/lyrics.cgi");

#ifdef  USE_NETGUI
//  netgui = ng_init (lyrics_host, NG_TCP|NG_DEBUG, NULL);
#endif

  return 0;
}


static int
lyrics_sum (int n)
{
  int ret = 0;

  for (;;)
    {
      ret += n % 10;
      n = n / 10;
      if (!n)
        return ret;
    }
}


static int
quh_lyrics_in_open (st_quh_nfo_t * file)
{
  int x = 0;
  static char buf[MAXBUFSIZE];
  int t = 0, n = 0;

  *buf = 0;
  if (!file->indices)
    {
      strcpy (buf, "Failed: Input has no indices");

      quh_set_object_s (quh.filter_chain, QUH_OUTPUT, buf);
  
      return 0;
    }

  for (x = 0; x < file->indices; x++)
    {
      // add to lyrics_sum
      n += lyrics_sum (file->index_pos[x] / file->channels / file->size / file->rate);

      if (x == 0)
        t = (file->raw_size / file->channels / file->size / file->rate) - 
            (file->index_pos[x] / file->channels / file->size / file->rate) -
            ((file->index_pos[x] / file->channels / file->size / file->rate) % 60); 
    }

  lyrics_id = (n % 0xff) << 24 | t << 8 | file->indices;

#ifdef  USE_NETGUI
#if 0
  sprintf (buf, "%s?cmd=lyrics+query+%08x+%ld+",
    lyrics_uri,
    lyrics_id,
    file->indices);
  
  for (x = 0; x < file->indices; x++)
    sprintf (strchr (buf, 0), "%ld+", (file->index_pos[x] / 2352) + 150);

  sprintf (strchr (buf, 0), "%ld&hello=anonymous+localhost+%s+%s&proto=6",
    ((file->raw_size / 2352) + 150) / 75,
    "Quh",
    QUH_VERSION_S);

//  ng_open (netgui, buf);
//  ng_read (netgui, buf, MAXBUFSIZE);

printf ("%s\n\n", buf);
fflush (stdout);

//  ng_close (netgui);

// build next query genre+other_id

//#endif
  sprintf (buf, "%s?cmd=lyrics+read+%s+%08x&hello=anonymous+localhost+%s+%s&proto=6",
    lyrics_uri,
    "classical",
    0x9b10f50b,
    "Quh",
    QUH_VERSION_S);
#endif

  strcpy (buf, "http://www.google.com/index.htm");
                                          
//  ng_open (netgui, buf);
//  ng_read (netgui, buf, MAXBUFSIZE);

printf ("%s\n\n", buf);
fflush (stdout);

//  ng_close (netgui);

#if 0
  for (x = 0 ; x < file->indices; x++)
    {
      char propname[MAXBUFSIZE];
      sprintf (propname, "TTITLE%d", x);
      ng_get_property (buf, propname, index_name[x]);
      printf ("%s\n\n", index_name[x]);
    }
#endif
#endif  // USE_NETGUI

  *buf = 0;
  sprintf (strchr (buf, 0), "DiscId: 0x%08lx\n", lyrics_id);

  if (file->indices)
    for (x = 1 ; x < file->indices + 1; x++)
      {
        unsigned long len = (x < file->indices ? file->index_pos[x] : file->raw_size) - file->index_pos[x - 1];

        sprintf (strchr (buf, 0), "%0*d:", misc_digits (QUH_MAXINDEX), x);

        sprintf (strchr (buf, 0), " %s", index_name[x - 1]);

        sprintf (strchr (buf, 0), "%s",
          quh_bytes_to_units (file, len, QUH_UNITS_CLOCK));
        
        if (x < file->indices)
          strcat (buf, "\n");
      }

  quh_set_object_s (quh.filter_chain, QUH_OUTPUT, buf);

  return 0;
}


static int
quh_lyrics_in_quit (st_quh_nfo_t * file)
{
  (void) file;

#ifdef  USE_NETGUI
//  ng_quit (netgui);
#endif

  return 0;
}  


const st_filter_t quh_lyrics_in = {
  QUH_LYRICS_IN,
  "lyrics",
  NULL,
  0,
  NULL,
  (int (*) (void *)) &quh_lyrics_in_open,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  (int (*) (void *)) &quh_lyrics_in_init,
  (int (*) (void *)) &quh_lyrics_in_quit
};


const st_getopt2_t quh_lyrics_in_usage =
{
  "lyrics", 2, 0, QUH_LYRICS,
  "URL", "read lyrics from server with URL", (void *) QUH_LYRICS_IN
};


#if 0
http://blueslyrics.tripod.com#$#<BR><BR></FONT>#$#<A HREF#$##$##$##$#
http://cifraclub.terra.com.br#$#</select></td></tr></table>#$#<br>Contribui#$##$##$##$#
http://homepage.ntlworld.com#$#</p><p>#$#<a#$##$##$##$#
http://hudba.hradiste.cz#$#<HR SIZE=1>#$#</table>#$##$##$##$#
http://hudba.hradiste.cz#$#<HR SIZE=1></td>#$#</table>#$##$##$##$#
http://letras.mus.br#$#></a><p>#$#</div>#$##$##$##$#
http://letras.terra.com.br#$#</td></tr></table></div><p>#$#</p></div>#$##$##$##$#
http://letrasdecanciones.tomamusica.com#$#<div align=center>#$#</div#$##$##$##$#
http://lirik.mania.or.id#$#<hr /><br>#$#<hr />#$##$##$##$#
http://lovelyrics.com#$#<pre>#$#</pre>#$##$##$##$#
http://ly-rics.com#$#</a><br></td>#$#<tr>#$##$##$##$#
http://lyrics.blueaudio.com#$#</a> ]<p>#$#</td>#$##$##$##$#
http://lyrics.dct.sk#$#</b><br><br>#$#<br><b>#$##$##$##$#
http://lyrics.duble.com#$#></td></tr></table></td></tr><tr><td>#$#<center>#$##$##$##$#
http://lyrics.mp3s.ru#$#</b></td></tr><tr><td valign=top><#$#<table>#$##$##$##$#
http://lyrics.psp.ee#$#]<BR><br>#$#<br>Added by:#$##$##$##$#
http://lyrics.rare-lyrics.com#$#<a id="lyrid">#$#</a#$##$##$##$#
http://lyrics.rockmagic.net#$#</a><br></font>#$#RockMagic.net#$##$##$##$#
http://lyrics.teenage.cz#$#<hr />#$#</p>#$##$##$##$#
http://lyricsheaven.topcities.com#$#</table>#$#</body>#$##$##$##$#
http://lyricsspot.com#$#</h3>#$#</font>#$##$##$##$#
http://natali.mine.nu#$#</tr><tr><td>#$#<br/>#$##$##$##$#
http://ourlyrics.host.sk#$#</h2>#$#<div #$##$##$##$#
http://pesenki.ru#$#</a></h2><font>#$#<hr>#$##$##$##$#
http://skaponk.com#$#<br><br>#$#</center>#$##$##$##$#
http://teenage.cz#$#</a></span>#$#</p>#$##$##$##$#
http://therudewords.com#$#<td><span>#$#Return to <a #$##$##$##$#
http://www.247lyrics.com#$#</a></b>#$#<a#$##$##$##$#
http://www.2famouslyrics.com#$#*</font>#$#<div#$##$##$##$#
http://www.absolutedivas.com#$#</center>#$#<center>#$##$##$##$#
http://www.absolutedivas.com#$#Arial" size=3>#$#</font>#$##$##$##$#
http://www.absolutelyric.com#$#</noscript></td></tr></table>#$#</tr><tr><td class=e1#$##$##$##$#
http://www.akordy.sk#$#size="4"><b>#$#</p>#$##$##$##$#
http://www.aler.us#$#</a>]#$#<table#$##$##$##$#
http://www.all-good-tabs.com#$#0><br><p>#$#</p>#$##$##$##$#
http://www.alloflyrics.com#$#<per class=s>#$#<table #$##$##$##$#
http://www.allthelyrics.com#$#--><br>#$#</td>#$##$##$##$#
http://www.allthelyrics.com#$#</a></h3>#$#</td>#$##$##$##$#
http://www.allthelyrics.com#$#</font><br>#$#</td>#$##$##$##$#
http://www.allthelyrics.com#$#active -->#$#<font size="1">#$##$##$##$#
http://www.allthelyrics.com#$#class="song">#$#[Thanks to#$##$##$##$#
http://www.alwaysontherun.net#$#</table><HR>#$#<HR><p#$##$##$##$#
http://www.angolotesti.it#$#<font class=testo>#$#</font>#$##$##$##$#
http://www.animelyrics.com#$#Version#$#</p>#$##$##$##$#
http://www.anysonglyrics.com#$#</iframe>#$#</div>#$##$##$##$#
http://www.anysonglyrics.com#$#Lyrics :#$#</td>#$##$##$##$#
http://www.anysonglyrics.com#$#Lyrics:#$#</td>#$##$##$##$#
http://www.anysonglyrics.com#$#songtext" >#$#</div>#$##$##$##$#
http://www.apluslyrics.com#$#users</font></b></td>#$#</table>#$##$##$##$#
http://www.areademusica.com#$#<td class="letra">#$#</td>#$##$##$##$#
http://www.assbach.de#$#<bR><bR><span class=txt2>#$#<!--#$##$##$##$#
http://www.azlyrics.com#$#"</b><BR><BR>#$#[ <a href#$##$##$##$#
http://www.azlyrics.com#$#</font><br>#$#[ <a#$##$##$##$#
http://www.azlyrics.com#$#</font><br>#$#[Thanks to#$##$##$##$#
http://www.azlyrics.us#$#</font><br></td></tr><tr>#$#</td>#$##$##$##$#
http://www.bestofparoles.com#$#<span class="texteparoles">#$#<center>#$##$##$##$#
http://www.bleedingmetal.com#$#color=ffffff>#$#<font#$##$##$##$#
http://www.bollywoodblitz.com#$#Lyrics</font></strong>#$#</table>#$##$##$##$#
http://www.canalquo.com#$#</H3><table><tr><td class=letrasm>#$#</td>#$##$##$##$#
http://www.celebrityscreenshots.com#$#</a><br><font size="2">#$#</font>#$##$##$##$#
http://www.christianrocklyrics.com#$#</b></td></tr></table>#$#</table>#$##$##$##$#
http://www.codehot.co.uk#$#sans-serif"><br>#$#</font>#$##$##$##$#
http://www.completealbumlyrics.com#$#<hr><ul>#$#</ul>#$##$##$##$#
http://www.completealbumlyrics.com#$#><ul>#$#</ul><hr#$##$##$##$#
http://www.cowboylyrics.com#$#></H2>#$#<b>Album Lyrics#$##$##$##$#
http://www.crolinks.com#$#<hr noshade>#$#</ul>#$##$##$##$#
http://www.dapslyrics.com#$#class=borderblkbold>#$#<center>#$##$##$##$#
http://www.darklyrics.com#$#<a name=1>#$#[ <a#$##$##$##$#
http://www.digitalsounds.de#$#</b><br><br>#$#<a #$##$##$##$#
http://www.elyrics.net#$#Lyrics</A><br>#$#</table>#$##$##$##$#
http://www.elyrics4u.com#$#<BR><BR>#$#</td>#$##$##$##$#
http://www.emuzyka.pl#$#<div class="text5"><br>#$#</div><br>#$##$##$##$#
http://www.findanylyrics.com#$#<START>#$#<END>#$##$##$##$#
http://www.findlyrics.com#$#<div class=white>#$#</div>#$##$##$##$#
http://www.fretplay.com#$#</H2>#$#<A #$##$##$##$#
http://www.ftpk.net#$#</option></select>#$#<a #$##$##$##$#
http://www.g-sistah.com#$#Version</a></td>#$#<a#$##$##$##$#
http://www.get-lyrics.net#$#</table><br><br>#$#</div>#$##$##$##$#
http://www.getlyrical.com#$#<td>--------</td></tr><tr><td>#$#</table>#$##$##$##$#
http://www.getthatsong.org#$#users</th>#$#<!--#$##$##$##$#
http://www.getyourlyrics.com#$#*</font>#$#</table>#$##$##$##$#
http://www.go2lyrics.com#$#</div><br>#$#</font>#$##$##$##$#
http://www.goldlyrics.com#$#</h1><br><br>#$#</td>#$##$##$##$#
http://www.guitarboard.com#$#Subject:#$#www.Guitar#$##$##$##$#
http://www.hamienet.com#$#</TD></TR></FORM><tr>#$#</table>#$##$##$##$#
http://www.hindilyrix.com#$#Lyrics:#$#<a#$##$##$##$#
http://www.hiphoptexter.com#$#<hr>#$#<hr>#$##$##$##$#
http://www.hit-haven.com#$#end button -->#$#</td></tr></table><br>#$##$##$##$#
http://www.hotlyrics.net#$#</font><br>#$#<div#$##$##$##$#
http://www.hottest-lyrics.com#$#height="86"></td></tr><tr>#$#</td>#$##$##$##$#
http://www.houseoflyrics.com#$#</A><p>#$#<SMALL>#$##$##$##$#
http://www.hugelyrics.com#$#Lyrics</font></b></td>#$#<center>#$##$##$##$#
http://www.icelyrics.com#$#</a></font>#$#<a#$##$##$##$#
http://www.iloveblue.com#$#</span> ]#$#<center>#$##$##$##$#
http://www.ilyric.net#$#</h1></font></a><br>#$#</font>#$##$##$##$#
http://www.indiafm.com#$#</b></font><BR>#$#<p align#$##$##$##$#
http://www.inlyrics.com#$#<br><td><br>#$#</font>#$##$##$##$#
http://www.inlyrics.com#$#class=text2 valign=top>#$#Some Other Lyrics by#$##$##$##$#
http://www.intothemusic.net#$#Chords</span></P>#$#</span>#$##$##$##$#
http://www.italianissima.net#$#</H5>#$#<SPAN#$##$##$##$#
http://www.kronawithleitner.at#$#</i></font><BR>#$#<font#$##$##$##$#
http://www.lap.ttu.ee#$#</a>]#$#</table>#$##$##$##$#
http://www.laplazavirtual.com#$#</b><br>#$#<a #$##$##$##$#
http://www.leoslyrics.com#$#Arial" size=-1>#$#</font></p><a name#$#sid#$##$##$#
http://www.letrasmania.com#$#INIZIO TESTO-->#$#><!--FINE TESTO-->#$##$##$##$#
http://www.letssingit.com#$#</I></TD></TR></TABLE><BR><BR>#$#</CENTER>#$##$##$##$#
http://www.letssingit.com#$#</TD></TR></TABLE><BR><BR>#$#</TABLE>#$##$##$##$#
http://www.lexode.com#$#</table></td></tr><tr>#$#<a#$##$##$##$#
http://www.links2love.com#$#<BLOCKQUOTE>#$#</BLOCKQUOTE>#$##$##$##$#
http://www.liriklagu.com#$#</strong></i>#$#<a#$##$##$##$#
http://www.loglar.com#$#Lyrics:#$#</font>#$##$##$##$#
http://www.lyrc.com.ar#$#</u></font>#$#<a#$##$##$##$#
http://www.lyred.com#$#</DIV></DIV><BR><P>#$#<P class#$##$##$##$#
http://www.lyred.com#$#</DIV></DIV><BR><P>#$#<P class=endvypisbuy>#$##$##$##$#
http://www.lyricalcontent.com#$#<tr><td><br>#$#</table>#$##$##$##$#
http://www.lyricalfantasia.com#$#views</font>#$#</font>#$##$##$##$#
http://www.lyricallysquared.com#$#<td colspan="2"><font>#$#</font>#$##$##$##$#
http://www.lyricart.net#$#<br><small>#$#</small>#$##$##$##$#
http://www.lyricattack.com#$#</a><br></font>#$#</font>#$##$##$##$#
http://www.lyricattack.com#$#</a><br></font>#$#</td>#$##$##$##$#
http://www.lyricmall.com#$#FCFCFC">#$#</font>#$##$##$##$#
http://www.lyricmall.com#$#users</font></b>#$#<BR></font>#$##$##$##$#
http://www.lyricmania.com#$#><hr size=1 noshade color=000033>#$#</td></tr>#$##$##$##$#
http://www.lyrics-heaven.com#$#</b><BR>#$#[ <#$##$##$##$#
http://www.lyrics.jp#$#TDSONG>#$#</td>#$##$##$##$#
http://www.lyrics.ly#$#zoomed=0;}">#$#</div>#$##$##$##$#
http://www.lyrics.net.ua#$#<p class=text>#$#</table>#$##$##$##$#
http://www.lyrics.nl#$#Songtekst</b>#$#<form method#$##$##$##$#
http://www.lyrics007.com#$#</td><td><br>#$#<br>Fans who visit #$##$##$##$#
http://www.lyrics007.com#$#DominoForm">#$#</FORM>#$##$##$##$#
http://www.lyrics007.com#$#Rank</a>')#$#<br>The hottest songs#$##$##$##$#
http://www.lyrics007.com#$#Rank</a>')#$#Notice</b>#$##$##$##$#
http://www.lyrics007.com#$#lyricstext>#$#</font>#$##$##$##$#
http://www.lyrics007.com#$#lyricstext>#$#[<br><lu>#$##$##$##$#
http://www.lyrics2lyrics.com#$#<blockquote><br>#$#</block#$##$##$##$#
http://www.lyrics4me.com#$#<td valign="top" width="500">#$#</td>#$##$##$##$#
http://www.lyricsbase.com#$#</b><BR><BR>#$#</font>#$##$##$##$#
http://www.lyricsbox.com#$#</H2>#$#<IMG name#$##$##$##$#
http://www.lyricscafe.com#$#<br><br>#$#EOD;$page#$##$##$##$#
http://www.lyricscafe.com#$#<br>Comments:#$#<a#$##$##$##$#
http://www.lyricscafe.com#$#<p class="red">#$#</td>#$##$##$##$#
http://www.lyricscrawler.com#$#</font></td></tr>#$#</table>#$##$##$##$#
http://www.lyricsdatabase.gen.tr#$#Lyrics</font>#$#<strong>#$##$##$##$#
http://www.lyricsdepot.com#$#</i><p>#$#</td>#$##$##$##$#
http://www.lyricsdepot.com#$#</i><p>#$#<b>#$##$##$##$#
http://www.lyricsdepot.com#$#</i><p>#$#<p>#$##$##$##$#
http://www.lyricsdir.com#$#</h2>#$#</td>#$##$##$##$#
http://www.lyricsdomain.com#$#]<br><br>#$#<br>[ <#$##$##$##$#
http://www.lyricsdot.ru#$#</table></td></tr></table>#$#</span>#$##$##$##$#
http://www.lyricsdownload.com#$#Lyrics&nbsp;&nbsp;</font></b></td>#$#</div>#$##$##$##$#
http://www.lyricsdownload.com#$#Song                            Lyrics#$#</div>#$##$##$##$#
http://www.lyricseek.net#$#</h1><BR>#$#</table#$##$##$##$#
http://www.lyricsfind.com#$#</h2>#$#<a #$##$##$##$#
http://www.lyricsfreak.com#$#</form></tr></table>#$#</td>#$##$##$##$#
http://www.lyricsfreak.com#$#</script></div></td>#$#</div>#$##$##$##$#
http://www.lyricsfreak.com#$#</select>#$#<map#$##$##$##$#
http://www.lyricsfreak.com#$#<td valign="top" class="body_large">#$#</table>#$##$##$##$#
http://www.lyricsfreak.com#$#Lyrics:</b></font></td>#$#</td></tr></form></table>#$##$##$##$#
http://www.lyricsgalaxy.de#$#</td></tr></table><br>#$#</table>#$##$##$##$#
http://www.lyricsmania.com#$#</b><br><br>#$#</TABLE>#$##$##$##$#
http://www.lyricsmansion.com#$#</b><br>#$#<a#$##$##$##$#
http://www.lyricsmp3.net#$#</strong></td></tr><tr><td>#$#</strong>#$##$##$##$#
http://www.lyricsmp3.net#$#</tr><tr><td height="500">#$#</strong#$##$##$##$#
http://www.lyricsnet.net#$#</p><b>Lyrics:</b>#$#<FORM>#$##$##$##$#
http://www.lyricsplanet.com#$#</font><BR><BR>#$#</div>#$##$##$##$#
http://www.lyricsprovider.com#$#class=clsLyricHR>#$#<hr noshade#$##$##$##$#
http://www.lyricsrock.com#$#td valign=top align=left>#$#</td>#$##$##$##$#
http://www.lyricsspot.com#$#</h3>#$#</font>#$##$##$##$#
http://www.lyricsstyle.com#$#</font><!-- FreeFind Begin No Index --><br>#$#</h6>#$##$##$##$#
http://www.lyricstar.com#$#Lyrics)<br>#$#<a#$##$##$##$#
http://www.lyricstime.com#$#</b></h1><p>#$#</p>#$##$##$##$#
http://www.lyricstop.com#$#</b></font><br>#$#[ <a#$##$##$##$#
http://www.lyricsxp.com#$#<BR><BR>#$#<a #$##$##$##$#
http://www.lyricz.net#$#<div id="dlmenu">#$#</div>#$##$##$##$#
http://www.lyriks.de#$#</form></td></tr></table>#$#<font#$##$##$##$#
http://www.lyrix.at#$#<td colspan='2' align='left'>#$#<a #$##$##$##$#
http://www.marcolino.com#$#<font size="2" face="Verdana">#$#</font>#$##$##$##$#
http://www.maroconline.com#$#<br>#$#</FONT>#$##$##$##$#
http://www.mathematik.uni-ulm.de#$#</h3>#$#</body>#$##$##$##$#
http://www.metalyou.com#$#</h2>#$#</td>#$##$##$##$#
http://www.metrolyrics.com#$#<br><br>#$#</font>#$##$##$##$#
http://www.miditext.ru#$#</h2><br>#$#<br></td>#$##$##$##$#
http://www.mitos.co.il#$#Black12">#$#</font>#$##$##$##$#
http://www.mlyrics.com#$#Rating: N/A#$#</tr></table>#$##$##$##$#
http://www.mlyrics.com#$#users</font></b>#$#</font>#$##$##$##$#
http://www.modernrocklyrics.com#$#</h3>#$#<h3#$##$##$##$#
http://www.moron.nl#$#</b><br><br>#$#<form#$##$##$##$#
http://www.mp3lyrics.org#$#--><BR><BR>#$#<!--#$##$##$##$#
http://www.mp3music.co.il#$#'right' class='bmenu'>#$#</table>#$##$##$##$#
http://www.musica.ru#$#</blockquote>#$#</ol><ol>#$##$##$##$#
http://www.musicfactory2.co.uk#$#<P align="left">#$#</font>#$##$##$##$#
http://www.musiclinks.nl#$#</b><br><br>#$#</font>#$##$##$##$#
http://www.musicmademe.com#$#<td valign="top" align="left">#$#</table>#$##$##$##$#
http://www.musicsonglyrics.com#$#</font></td></tr><tr>#$#</table>#$##$##$##$#
http://www.musicsonglyrics.com#$#lyrics</font></td></tr><tr>#$#</table>#$##$##$##$# 
http://www.musicsonglyrics.com#$#lyrics</h5><p align='justify'>#$#</table>#$##$##$##$#
http://www.musikeiro.com.ar#$#<p>#$#</p>#$##$##$##$#
http://www.nomorelyrics.net#$#</font></b></br>#$#<font #$##$##$##$#
http://www.oldielyrics.com#$#</B></font><br><br>#$#</font>#$##$##$##$#
http://www.onlylyrics.com#$#<br><br>#$#<a #$##$##$##$#
http://www.ostlyrics.com#$#</b></td></tr><tr>#$#<b><a#$##$##$##$#
http://www.paroles.net#$#<SMALL>#$#</SMALL>#$##$##$##$#
http://www.parolesrock.com#$#</p></td>#$#</div>#$##$##$##$#
http://www.patleck.com#$#<center>#$#</table>#$##$##$##$#
http://www.planetadeletras.com#$#</span><br></td></tr>#$#</table>#$##$##$##$#
http://www.plyrics.com#$#</b><BR><BR>#$#[#$##$##$##$#
http://www.popex.com#$#</h2><p>#$#</p><p#$##$##$##$#
http://www.poplyrics.net#$#<br>#$#<span #$##$##$##$#
http://www.portal.is#$#</h2>#$#<div#$##$##$##$#
http://www.progg.se#$#</a>] <br>#$#[<#$##$##$##$#
http://www.punkhardcore.com#$#<p class="tab">#$#</td>#$##$##$##$#
http://www.purelyrics.com#$#capitalFont">#$#</font>#$##$##$##$#
http://www.puroparty.com#$#<h3>#$#<span#$##$##$##$#
http://www.quicklyrics.com#$#<p align="left" class="lyrics">#$#<a#$##$##$##$#
http://www.raplyricssearch.com#$#lyricstext">#$#</td>#$##$##$##$#
http://www.readlyrics.com#$#<p><br>#$#</td>#$##$##$##$#
http://www.reallyrics.com#$#TDSONG>#$#</td>#$##$##$##$#
http://www.remrock.com#$#</H6><P>#$#</P></TD></TR>#$##$##$##$#
http://www.rlyrics.com#$#</h2>#$#</Table>#$##$##$##$#
http://www.robhoran.com#$#friend</a>#$#<small>#$##$##$##$#
http://www.rocklyricsonline.com#$#</h2>#$#</table>#$##$##$##$#
http://www.seeklyrics.com#$#</script><br><br><b>#$#<b #$##$##$##$#
http://www.sing365.com#$#);</script><BR>#$#<hr size#$##$##$##$#
http://www.song-lyrics-library.com#$#Lyrics</h1>#$#<h2>#$##$##$##$#
http://www.song-text.com#$#<td width="468">#$#</font>#$##$##$##$#
http://www.songlyrics.co.nz#$#songtext" >#$#</td>#$##$##$##$#
http://www.songlyrics.co.nz#$#songtext">#$#<img src#$##$##$##$#
http://www.songlyrics.com#$#div><div class=white>#$#</div#$##$##$##$#
http://www.songlyrics4u.com#$#<br><br>#$#<b>#$##$##$##$#
http://www.songlyricscollection.com#$#</font></ol></td>#$#</p>#$##$##$##$#
http://www.songlyricsworld.com#$#<font face="ms sans serif">#$#</font>#$##$##$##$#
http://www.songme.com#$#<a name="1">#$#</table>#$##$##$##$#
http://www.songmeanings.net#$#<br></p><p>#$#<a#$##$##$##$#
http://www.soundlyrics.com#$#</font></b><!-- FreeFind Begin No Index -->#$#<br><!-- #$##$##$##$#
http://www.sputnik.de#$#<span class="content_blau"></span>#$#<br clear#$##$##$##$#
http://www.spynets.net#$#<td><div align="center">#$#</td>#$##$##$##$#
http://www.stlyrics.com#$#</b><p>#$#<br><b>#$##$##$##$#
http://www.stlyrics.com#$#<br></H1>#$#<a#$##$##$##$#
http://www.stlyrics.com#$#] <br>#$#<br><br><table#$##$##$##$#
http://www.stmlyrics.com#$#</b><br><p>#$#<!--#$##$##$##$#
http://www.supermusic.sk#$#<sup>#$#<script #$##$##$##$#
http://www.tabcrawler.com#$#</center><center>#$#</td>#$##$##$##$#
http://www.tabfind.de#$#</u></td>#$#</table>#$##$##$##$#
http://www.tablaturas.nsites.com.ar#$#<font size="4" face="Courier New, Courier, mono"><pre><font size="2">#$#</font></pre></font>#$##$##$##$#
http://www.tabpower.com#$#-----#$#</td>#$##$##$##$#
http://www.teksty.org#$#</b><br><Br>#$#</td>#$##$##$##$#
http://www.testimania.com#$#</b><br><br>#$#<table #$##$##$##$#
http://www.testimania.com#$#INIZIO TESTO-->#$#<!--FINE TESTO#$##$##$##$#
http://www.thesonglyrics.com#$#</h1>#$#<div #$##$##$##$#
http://www.thetabworld.com#$#Tab</th></tr>#$#<a#$##$##$##$#
http://www.tonguide.com#$#</font></b></td></tr><tr>#$#</table>#$##$##$##$#
http://www.tonsoflyrics.com#$#<per class=lyrics>#$#</table></div>#$##$##$##$#
http://www.top40-charts.com#$#Info from Amazon#$#<img#$##$##$##$#
http://www.tophitsonline.com#$#</font></strong>#$#<!--#$##$##$##$#
http://www.tophitsonline.com#$#Lyrics:</font>#$#<font face#$##$##$##$#
http://www.tophitsonline.com#$#arial"><br><br>#$#</tr></table>#$##$##$##$#
http://www.toplyrics.net#$#</a></td></tr></table><br>#$#</td>#$##$##$##$#
http://www.tosalsa.com#$#</font></font></td>#$#<a href#$##$##$##$#
http://www.ultimate-guitar.com#$#<!-- Tab -->#$#<!-- / CONTENT -->#$##$##$##$#
http://www.upseros.net#$#<TR><TD height=5></TD></TR>#$#</TABLE>#$##$##$##$#
http://www.vers.go.ro#$#<td class="song">#$#<p>#$##$##$##$#
http://www.vlyrics.com#$#>]#$#<table#$##$##$##$#
http://www.woutersen.info#$#</b><br><br>#$#<br>Requested #$##$##$##$#
http://www.yimpan.com#$#<!-- Lyric -->#$#</font>#$##$##$##$#
http://www.yimpan.com#$#LYRICS</b>#$#</td></tr>#$##$##$##$#
http://www.zap-letras.com#$#<p>#$#</div>#$##$##$##$#
http://www.zeneszoveg.hu#$#</font></b></p></b>#$#<hr#$##$##$##$#
http://www28.brinkster.com#$#veces.</td>#$#</p>#$##$##$##$#
http://xlyrictracker.com#$#content1">#$#</td>#$##$##$##$#
http://www.todotango.com#$#<p class="texto">#$#</p>#$##$##$##$#
http://argentina.informatik.uni-muenchen.de#$#<!--X-Body-of-Message-->#$#<!--X-Body-of-Message-End-->#$##$##$##$#
#endif
