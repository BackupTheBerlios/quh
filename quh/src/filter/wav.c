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
#include "quh_filter.h"
#include "wav.h"


#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif


static FILE *wav_in = NULL;
static FILE *wav_out = NULL;
static int wav_write_header = 0;


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
  const char *fname = quh_get_object_s (quh.filter_chain, QUH_OPTION);

  if (!strcmp (fname, "-"))
    wav_out = stdout;
  else
    if (!(wav_out = fopen (fname, "wb")))
      return -1;

  wav_write_header = 1;

  return 0;
}


static int
quh_wav_out_close (st_quh_nfo_t *file)
{
  (void) file;

  if (wav_out)
    {
      fflush (wav_out);

      if (wav_out != stdout)
        fclose (wav_out);
    }

  return 0;
}


static int
quh_wav_out_write (st_quh_nfo_t *file)
{
  (void) file;

  if (wav_out)
    {
      if (wav_write_header)
        {
          quh_wav_write_header (wav_out, file->channels, file->rate, file->size, file->raw_size);
          wav_write_header = 0;
        }

      fwrite (quh.buffer, 1, quh.buffer_len, wav_out);
    }

  return 0;
}


static int
quh_wav_in_open (st_quh_nfo_t *file)
{
  st_wav_header_t wav_header;

  wav_in = fopen (file->fname, "rb");

  fread (&wav_header, 1, sizeof (st_wav_header_t), wav_in);
  
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

  fclose (wav_in);

  return 0;
}


static int
quh_wav_in_seek (st_quh_nfo_t *file)
{
  (void) file;

  // skip wav header
  quh.raw_pos = MAX (sizeof (st_wav_header_t), quh.raw_pos);

  fseek (wav_in, quh.raw_pos, SEEK_SET);

  return 0;
}


static int
quh_wav_in_demux (st_quh_nfo_t *file)
{
  int result = -1;
#if 0
  if (file->source != QUH_SOURCE_FILE)
    return -1;

  result = quh_wav_in_open (file);
  
  if (!result)
    quh_wav_in_close (file);
#endif
  return result;
}


static int
quh_wav_in_write (st_quh_nfo_t *file)
{
  (void) file;

  quh.buffer_len = fread (&quh.buffer, 1, QUH_MAXBUFSIZE / 4, wav_in);

  return 0;
}


QUH_FILTER_IN(quh_wav_in, QUH_WAV_IN, "wav read", ".wav")
QUH_FILTER_OUT(quh_wav_out, QUH_WAV_OUT, "wav write")


const st_getopt2_t quh_wav_out_usage =
{
    "wav", 2, 0, QUH_WAV,
    "FILE", "write as wav FILE (default: audiodump.wav)"
};
