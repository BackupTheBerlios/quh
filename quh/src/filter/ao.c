/*
libao.c - libao support for Quh

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
#include <ctype.h>
#include <stdlib.h>
#include <ao/ao.h>
#include "misc/itypes.h"
#include "misc/getopt2.h"
#include "misc/misc.h"
#include "misc/file.h"
#include "misc/filter.h"
#include "misc/term.h"
#include "quh_defines.h"
#include "quh_filter.h"
#include "quh.h"
#include "quh_misc.h"
#include "ao.h"


static int inited = 0;
static ao_device *aodev = NULL;
static ao_sample_format fmt;
  

static int
quh_ao_out_init (st_quh_nfo_t *file)
{
  (void) file;

  if (!quh_get_object_s (quh.filter_chain, QUH_OPTION))
    quh_set_object_s (quh.filter_chain, QUH_OPTION, "oss");

  return 0;
}


int
quh_ao_out_quit (st_quh_nfo_t *file)
{
  (void) file;

  ao_shutdown ();

  return 0;
}


int
quh_ao_out_ctrl (st_quh_nfo_t *file)
{
  fmt.bits = file->size * 8;
  fmt.rate = file->rate;
  fmt.channels = file->channels;
  fmt.byte_format = AO_FMT_NATIVE;

  return 0;
}


int
quh_ao_out_open (st_quh_nfo_t *file)
{
  int driver_id = ao_driver_id (quh_get_object_s (quh.filter_chain, QUH_OPTION));

  if (!inited)
    {
      ao_initialize ();
      quh_ao_out_ctrl (file);
      inited = 1;
    }

  if (!(aodev = ao_open_live (driver_id, &fmt, NULL)))
    if (!(aodev = ao_open_live (ao_default_driver_id (), &fmt, NULL)))
      return -1;

  return 0;
}


int
quh_ao_out_close (st_quh_nfo_t *file)
{
  (void) file;

  ao_close (aodev);

  return 0;
}


int
quh_ao_out_write (st_quh_nfo_t *file)
{
  (void) file;

  ao_play (aodev, (char *) &quh.buffer, quh.buffer_len);
  
  return 0;
}


QUH_FILTER_FUNC_DUMMY (quh_ao_out_seek)
QUH_FILTER_OUT(quh_ao_out,QUH_AO_OUT,"libao")


const st_getopt2_t quh_ao_out_usage =
{
    "ao", 2, 0, QUH_AO,
    "DRIVER", "write to soundcard using libao DRIVER (default: auto);\n"
              "DRIVER=\"oss\" for OSS\n"
              "DRIVER=\"alsa\" for ALSA"
//    "OUT=0 lineout (default)\n"
//    "OUT=1 headphones"
};
