/*
raw.c - raw support for Quh

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
#include <stdio.h>
#include "misc/itypes.h"
#include "misc/getopt2.h"
#include "filter.h"
#include "misc/file.h"
#include "quh_defines.h"
#include "quh.h"
#include "raw.h"
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
#include "filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "raw.h"


static FILE *raw_out = NULL;
static FILE *raw_in = NULL;


static int
quh_raw_in_open (st_quh_nfo_t * file)
{
  if (!(raw_in = fopen (file->fname, "rb")))
    return -1;

  quh.raw_pos = 0;
  file->raw_size = fsizeof (file->fname) - quh.raw_pos;
  file->rate = 44100;
  file->channels = 2;
  file->is_signed = 1;
  file->size = 2;
  file->seekable = QUH_SEEKABLE;

  quh_set_object_s (quh.filter_chain, QUH_OUTPUT, "Raw: using default values");

  return 0;
}


static int
quh_raw_in_close (st_quh_nfo_t * file)
{
  (void) file;

  fclose (raw_in);

  return 0;
}


static int
quh_raw_in_seek (st_quh_nfo_t * file)
{
  (void) file;

  fseek (raw_in, quh.raw_pos, SEEK_CUR);

  return 0;
}


static int
quh_raw_in_write (st_quh_nfo_t * file)
{
  (void) file;

  quh.buffer_len = fread (&quh.buffer, 1, QUH_MAXBUFSIZE, raw_in);

  return 0;
}


static int
quh_raw_out_init (st_quh_nfo_t *file)
{
  (void) file;

  if (!quh_get_object_s (quh.filter_chain, QUH_OPTION))
    quh_set_object_s (quh.filter_chain, QUH_OPTION, "audiodump.raw");
                
  return 0;
}


static int
quh_raw_out_open (st_quh_nfo_t *file)
{
  (void) file;

  if (!(raw_out = fopen (quh_get_object_s (quh.filter_chain, QUH_OPTION), "wb")))
    return -1;

  return 0;
}


static int
quh_raw_out_close (st_quh_nfo_t *file)
{
  (void) file;

  fclose (raw_out);

  return 0;
}


static int
quh_raw_out_write (st_quh_nfo_t *file)
{
  (void) file;

  fwrite (quh.buffer, 1, quh.buffer_len, raw_out);

  return 0;
}


QUH_FILTER_FUNC_STUB (quh_raw_out_seek)
QUH_FILTER_FUNC_STUB (quh_raw_out_ctrl)
QUH_FILTER_FUNC_STUB (quh_raw_out_quit) 
QUH_FILTER_OUT(quh_raw_out, QUH_RAW_OUT, "raw write")


const st_getopt2_t quh_raw_out_usage =
{
    "raw", 2, 0, QUH_RAW,
    "FILE", "write as raw (1:1) FILE (default: audiodump.raw)"
};


QUH_FILTER_FUNC_STUB (quh_raw_in_demux)
QUH_FILTER_FUNC_STUB (quh_raw_in_ctrl)
QUH_FILTER_IN(quh_raw_in, QUH_RAW_IN, "raw read", NULL)
