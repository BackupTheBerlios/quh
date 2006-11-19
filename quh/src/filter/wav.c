/*
wav.c - WAV support for Quh

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
#include "misc/itypes.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "misc/file.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "wav.h"


#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif


static FILE *wav_out = NULL;
static FILE *wav_in = NULL;


static int
quh_wav_out_init (st_quh_nfo_t *file)
{
  (void) file;

  if (!quh_get_object_s (quh.filter_chain, QUH_OPTION))
    quh_set_object_s (quh.filter_chain, QUH_OPTION, "audiodump.wav");
      
  return 0;
}


static int
quh_wav_out_open (st_quh_nfo_t *file)
{
  (void) file;
  st_wav_header_t wav_header;
    
  if (!(wav_in = fopen (quh_get_object_s (quh.filter_chain, QUH_OPTION), "wb")))
    return -1;

  // reserve space
  memset (&wav_header, 0, sizeof (st_wav_header_t));
  fwrite (&wav_header, 1, sizeof (st_wav_header_t), wav_in);

  return 0;
}


static int
quh_wav_out_close (st_quh_nfo_t *file)
{
  (void) file;
  if (wav_in)
    {
      unsigned long len = 0;

      // re-write header
      fseek (wav_in, 0, SEEK_END);
      len = ftell (wav_in) - sizeof (st_wav_header_t);
      fseek (wav_in, 0, SEEK_SET);

      quh_wav_write_header (wav_in, 2, 44100, 2, len);
                                    
      fclose (wav_in);
    }

  return 0;
}


static int
quh_wav_out_write (st_quh_nfo_t *file)
{
  (void) file;

  if (wav_in)
    fwrite (quh.buffer, 1, quh.buffer_len, wav_in);

  return 0;
}


static int
quh_wav_in_open (st_quh_nfo_t *file)
{
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
}


static int
quh_wav_in_close (st_quh_nfo_t *file)
{
  (void) file;

  fclose (wav_out);

  return 0;
}


static int
quh_wav_in_seek (st_quh_nfo_t *file)
{
  (void) file;

  // skip wav header
  quh.raw_pos = MAX (sizeof (st_wav_header_t), quh.raw_pos);

  fseek (wav_out, quh.raw_pos, SEEK_SET);

  return 0;
}


#if 0
static int
quh_wav_in_demux (st_quh_nfo_t *file)
{
  int result = 0;

  if (file->source != QUH_SOURCE_FILE)
    return -1;

  result = quh_wav_in_open (file);
  
  if (!result)
    quh_wav_in_close (file);
    
  return result;
}
#endif


static int
quh_wav_in_write (st_quh_nfo_t *file)
{
  (void) file;

  quh.buffer_len = fread (&quh.buffer, 1, QUH_MAXBUFSIZE / 4, wav_out);

  return 0;
}


const st_filter_t quh_wav_in =
{
  QUH_WAV_IN,
  "wav read",
  ".wav",
  -1,
//  (int (*) (void *)) &quh_wav_in_demux,
  NULL,
  (int (*) (void *)) &quh_wav_in_open,
  (int (*) (void *)) &quh_wav_in_close,
  NULL,
  (int (*) (void *)) &quh_wav_in_write,
  (int (*) (void *)) &quh_wav_in_seek,
  NULL,
  NULL,
  NULL
};


const st_getopt2_t quh_wav_in_usage =
{
    "wav", 1, 0, QUH_WAV,
    "FILE", "FILE is WAV (if it has no .wav suffix)",
    (void *) QUH_WAV_IN
};


const st_filter_t quh_wav_out =
{
  QUH_WAV_OUT,
  "wav write",
  NULL,
  0,
  NULL,
  (int (*) (void *)) &quh_wav_out_open,
  (int (*) (void *)) &quh_wav_out_close,
  NULL,
  (int (*) (void *)) &quh_wav_out_write,
  NULL,
  NULL,
  (int (*) (void *)) &quh_wav_out_init,
  NULL
};


const st_getopt2_t quh_wav_out_usage =
{
    "wav", 2, 0, QUH_WAV,
    "FILE", "write as wav FILE (default: audiodump.wav)", (void *) QUH_WAV_OUT
};
