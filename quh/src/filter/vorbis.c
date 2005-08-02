/*
ogg.c - ogg support for Quh

written by 2004 Dirk (d_i_r_k_@gmx.net)


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
quh_vorbis_open (st_quh_filter_t * file)
{
  ogg_int64_t length = 0;
  int size = 0;
  int bits = 16;
  vorbis_comment *c = NULL;
  char buf[MAXBUFSIZE];

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
//      strcpy (buf, "Vorbis comment:\n");
                  
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
      size = bits / 8 * ov_info (&vf, 0)->channels;
      file->seekable = QUH_SEEKABLE;
    }

  file->rate = ov_info (&vf, 0)->rate;
  file->channels = ov_info (&vf, 0)->channels;
  file->is_signed = TRUE;
  file->size = bits / 8;
      
  file->raw_size = 0x7fffffff;

  if (length && length * bits / 8 * file->channels < file->raw_size)
    file->raw_size = (unsigned int) (length * bits / 8 * file->channels);

  return 0;
}


int
quh_vorbis_close (st_quh_filter_t * file)
{
  (void) file;

//  if (opened)
//    ov_clear (&vf);
  opened = 0;

  fclose (fp);

  return 0;
}


int
quh_vorbis_demux (st_quh_filter_t * file)
{
  int result = 0;
  struct stat fstate;

  // a device?
  if (file->fname[0])
    if (!stat (file->fname, &fstate))
      if (!S_ISREG (fstate.st_mode))
        return -1;

  result = quh_vorbis_open (file);

  if (!result)
    quh_vorbis_close (file);

  return result;
}


int
quh_vorbis_seek (st_quh_filter_t * file)
{
  if (file->seekable)
    ov_pcm_seek_lap (&vf, quh.raw_pos / (file->size * file->channels));

  return 0;
}


int
quh_vorbis_write (st_quh_filter_t * file)
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


const st_filter_t quh_vorbis_in = {
  QUH_VORBIS_IN,
  "vorbis decode",
  ".ogg",
  strlen (".ogg"),
  (int (*) (void *)) &quh_vorbis_open,
  (int (*) (void *)) &quh_vorbis_close,
  (int (*) (void *)) &quh_vorbis_demux,
  (int (*) (void *)) &quh_vorbis_write,
  (int (*) (void *)) &quh_vorbis_seek,
  NULL,
  NULL,
  NULL
};


const st_getopt2_t quh_vorbis_in_usage =
{
    "ogg", 1, 0, QUH_OGG,
    "FILE", "FILE is OGG (if it has no .ogg suffix)",
    (void *) QUH_VORBIS_IN
};