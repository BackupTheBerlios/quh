/*
nsf.c - Nintendo/NSF support for Quh

Copyright (c) 2006 NoisyB


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
#include "misc/itypes.h"
#include "misc/bswap.h"
#include "misc/getopt2.h"
#include "filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "wav.h"
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
#include "misc/itypes.h"
#include "misc/misc.h"
#include "misc/getopt2.h"
#include "filter.h"
#include "misc/file.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "nsf.h"


#warning TODO: add support

static int
quh_nsf_in_open (st_quh_nfo_t *file)
{
#if 0
  st_wav_header_t wav_header;

  wav_out = fopen (file->fname, "rb");

  fread (&wav_header, 1, sizeof (st_wav_header_t), wav_out);
  
  if (!memcmp (wav_header.magic, "RIFF", 4))
    {
      quh.raw_pos = sizeof (st_wav_header_t);
      file->raw_size = fsizeof (file->fname) - quh.raw_pos;
      file->rate = wav_header.freq;
      file->channels = wav_header.channels;
      file->is_signed = 1;
      file->size = wav_header.bitspersample / 8;
      file->seekable = QUH_SEEKABLE;
      
      return 0; 
    }

  return -1;
#endif
  return 0;
}


static int
quh_nsf_in_close (st_quh_nfo_t *file)
{
  (void) file;

//  fclose (wav_out);

  return 0;
}


static int
quh_nsf_in_seek (st_quh_nfo_t *file)
{
  (void) file;
#if 0
  // skip wav header
  quh.raw_pos = MAX (sizeof (st_wav_header_t), quh.raw_pos);

  fseek (wav_out, quh.raw_pos, SEEK_SET);
#endif
  return 0;
}


static int
quh_nsf_in_demux (st_quh_nfo_t *file)
{
#if 0
  int result = 0;

  if (file->source != QUH_SOURCE_FILE)
    return -1;

  result = quh_wav_in_open (file);
  
  if (!result)
    quh_wav_in_close (file);
    
  return result;
#endif
  return 0;
}


static int
quh_nsf_in_write (st_quh_nfo_t *file)
{
  (void) file;

//  quh.buffer_len = fread (&quh.buffer, 1, QUH_MAXBUFSIZE / 4, wav_out);

  return 0;
}


QUH_FILTER_IN (quh_nsf_in, QUH_NSF_IN,"nsf", ".nsf")
#if 0
const st_filter_t quh_nsf_in =
{
  QUH_NSF_IN,
  "nsf",
  ".nsf",
  -1,
  (int (*) (void *)) &quh_nsf_in_demux,
  (int (*) (void *)) &quh_nsf_in_open,
  (int (*) (void *)) &quh_nsf_in_close,
  NULL,
  (int (*) (void *)) &quh_nsf_in_write,
  (int (*) (void *)) &quh_nsf_in_seek,
  NULL,
  NULL,
  NULL
};
#endif


const st_getopt2_t quh_nsf_in_usage =
{
    "nsf", 1, 0, QUH_NSF,
    "FILE", "FILE is NSF (if it has no .wav suffix)",
    (void *) QUH_NSF_IN
};
