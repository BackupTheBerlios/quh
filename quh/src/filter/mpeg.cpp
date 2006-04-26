/*
mp3_splay.cpp - mp3_splay support for Quh (using splay's libmpeg)

written by 2005 Dirk (d_i_r_k_@gmx.net)


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
#ifdef  HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef  HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef  HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "libmpeg/mpegsound.h"
#include "misc/bswap.h"
#include "misc/itypes.h"
#include "misc/file.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "libmpeg.h"


class quh_libmpeg_player : public Soundplayer
{
public:
  ~quh_libmpeg_player();

  virtual bool
  putblock (void *buffer, int size)
    {
      (void) buffer;
      (void) size;
      return true;
    }
};


static int
quh_libmpeg_open (st_quh_nfo_t *file)
{
  (void) file;
#if 0
  Mpegfileplayer *player;
  
  player=new Mpegfileplayer;
  if(!player->openfile(file->fname,"/dev/dsp"))
    {
      delete player;
      return -1;
    }
  player->setdownfrequency(44100);

  player->playing(0,true, 0);

  delete player;
#endif  

  return 0;
}


static int
quh_libmpeg_close (st_quh_nfo_t *file)
{
  (void) file;
                                
  return 0;
}


static int
quh_libmpeg_seek (st_quh_nfo_t *file)
{
  (void) file;

  return 0;
}


static int
quh_libmpeg_read (st_quh_nfo_t *file)
{
  (void) file;

  return 0;
}


int
quh_libmpeg_demux (st_quh_nfo_t * file)
{
  int result = 0;

  if (file->source != QUH_SOURCE_FILE)
    return -1;

  result = quh_libmpeg_open (file);

  if (!result)
    quh_libmpeg_close (file);

  return result;
}


const st_filter_t quh_libmpeg_in = {
  QUH_LIBMPEG_IN,
  "mp3 (using libmpeg)",
  ".mp3",
  strlen (".mp3"),
  (int (*) (void *)) &quh_libmpeg_demux,
  (int (*) (void *)) &quh_libmpeg_open,
  (int (*) (void *)) &quh_libmpeg_close,
  (int (*) (void *)) &quh_libmpeg_read,
  NULL,
  (int (*) (void *)) &quh_libmpeg_seek,
  NULL,
  NULL,
  NULL
};


const st_getopt2_t quh_libmpeg_in_usage =
{
    "mp3_splay", 1, 0, QUH_MP3,
    "FILE", "FILE is MP3 (if it has no .mp3_splay suffix)",
    (void *) QUH_LIBMPEG_IN
};
