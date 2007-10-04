/*
vorbis.c - vorbis support for Quh

Copyright (c) 2005 NoisyB


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
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#ifdef  USE_OGG
#include <vorbis/vorbisfile.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#endif
#include "misc/defines.h"
#include "misc/itypes.h"
#include "misc/bswap.h"
#include "misc/file.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "vorbis.h"


static FILE *fp = NULL;
static OggVorbis_File vf;
static int opened = 0;


static void
quh_fix_vorbis_comment (char *d, char *s)
{
  char *p = s;

  while (*p)
    {
      if (p - s)
        {
          *d = tolower (*p);
          if (*(p - 1) == ' ')
            *d = toupper (*p);
        }
      else
        *d = toupper (*p);

      if (*p == '_')
        *d = ' ';

      if (*p == '=')
        { 
          *d = ':';
          d++;
          *d = ' ';
          p++;
          d++;
          break; // end of name
        }
      p++;
      d++;
    }

  while (*p)
    *d++ = *p++;
    *d = 0;
}


int
quh_vorbis_in_open (st_quh_nfo_t * file)
{
  ogg_int64_t length = 0;
  int bits = 16;
  vorbis_comment *c = NULL;
  char buf[MAXBUFSIZE];
  vorbis_info *nfo = NULL;

  if (!(fp = fopen (file->fname, "rb")))
    return -1;

  if (ov_open (fp, &vf, NULL, 0) < 0)
    {
      fclose (fp);
      return -1;
    }
  opened = 1;

  // comments
  *buf = 0;
  c = vf.vc;
  if (c)
    {
      int x = 0;
      for (; x < c->comments && strlen (buf) < MAXBUFSIZE - 128; x++)
        {
          char *p = strchr (buf, 0);

          if (x)
            strcpy (p, "\n");
                
          p = strchr (buf, 0);
          quh_fix_vorbis_comment (p, c->user_comments[x]);
        }
      quh_set_object_s (quh.filter_chain, QUH_OUTPUT, buf);
    }

  if (ov_seekable (&vf))
    {
      length = ov_pcm_total (&vf, 0);
      file->seekable = QUH_SEEKABLE;
    }

  if ((nfo = ov_info (&vf, 0)))
    {
      file->rate = nfo->rate;
      file->channels = nfo->channels;
      file->min_bitrate = nfo->bitrate_lower ? nfo->bitrate_lower : nfo->bitrate_nominal;
      file->max_bitrate = nfo->bitrate_upper ? nfo->bitrate_upper : nfo->bitrate_nominal;
    }
  else
    {
      file->rate = 44100;
      file->channels = 2;
      file->min_bitrate =
      file->max_bitrate = 0;
    }
  file->is_signed = 1;
  file->size = bits / 8;

  if (length > 0)
    file->raw_size = length * bits / 8 * file->channels;
  file->raw_size = MIN (file->raw_size, 0x7fffffff);

  return 0;
}


int
quh_vorbis_in_close (st_quh_nfo_t * file)
{
  (void) file;

//  if (opened)
//    ov_clear (&vf);
  opened = 0;

  fclose (fp);

  return 0;
}


int
quh_vorbis_in_demux (st_quh_nfo_t * file)
{
  int result = 0;

  if (file->source != QUH_SOURCE_FILE)
    return -1;

  result = quh_vorbis_in_open (file);

  if (!result)
    quh_vorbis_in_close (file);

  return result;
}


int
quh_vorbis_in_seek (st_quh_nfo_t * file)
{
  if (file->seekable)
    ov_pcm_seek_lap (&vf, quh.raw_pos / (file->size * file->channels));

  return 0;
}


int
quh_vorbis_in_write (st_quh_nfo_t * file)
{
  int bs = 0;
#ifdef  WORDS_BIGENDIAN
  int endian = 1;
#else
  int endian = 0;
#endif
  int result = ov_read (&vf, (char *) quh.buffer, QUH_MAXBUFSIZE, endian, file->size, file->is_signed, &bs);

  if (!result)
    return 0;

  if (bs != 0)
    {
      fprintf (stderr, "OGG: Only one logical bitstream currently supported\n");
      return -1;
    }

  // hole in data
  if (result < 0)
    return 0;

  quh.buffer_len = result;

  return 0;
}


QUH_FILTER_FUNC_STUB (quh_vorbis_in_ctrl)
QUH_FILTER_IN(quh_vorbis_in, QUH_VORBIS_IN, "vorbis decode", ".vorbis.ogg")
