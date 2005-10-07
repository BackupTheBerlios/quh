/*
libao.c - libao support for Quh

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
#include <ao/ao.h>
#include "misc/itypes.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "libao.h"


static int inited = 0;
static ao_device *aodev = NULL;
static ao_sample_format fmt;
  

static int
quh_libao_init (st_quh_filter_t *file)
{
  (void) file;

  if (!quh_get_object_s (quh.filter_chain, QUH_OPTION))
    quh_set_object_s (quh.filter_chain, QUH_OPTION, "0");

  return 0;
}


int
quh_libao_quit (st_quh_filter_t *file)
{
  (void) file;

  ao_shutdown ();

  return 0;
}


int
quh_libao_config (st_quh_filter_t *file)
{
  fmt.bits = file->size * 8;
  fmt.rate = file->rate;
  fmt.channels = file->channels;
  fmt.byte_format = AO_FMT_NATIVE;

  return 0;
}


int
quh_libao_open (st_quh_filter_t *file)
{
  (void) file;

  if (!inited)
    ao_initialize ();
  inited = 1;

  if (!(aodev = ao_open_live (strtol ((const char *) quh_get_object_s (quh.filter_chain, QUH_OPTION), NULL, 10), &fmt, NULL)))
    return -1;

  return 0;
}


int
quh_libao_close (st_quh_filter_t *file)
{
  (void) file;

  ao_close (aodev);

  return 0;
}


int
quh_libao_write (st_quh_filter_t *file)
{
  (void) file;

  ao_play (aodev, (char *) &quh.buffer, quh.buffer_len);
  
  return 0;
}


const st_filter_t quh_libao_out =
{
  QUH_LIBAO_OUT,
  "libao",
  NULL,
  0,
  NULL,
  (int (*) (void *)) &quh_libao_open,
  (int (*) (void *)) &quh_libao_close,
  NULL,
  (int (*) (void *)) &quh_libao_write,
  NULL,
  (int (*) (void *)) &quh_libao_config,
  (int (*) (void *)) &quh_libao_init,
  (int (*) (void *)) &quh_libao_quit
};


const st_getopt2_t quh_libao_out_usage =
{
    "libao", 2, 0, QUH_LIBAO,
    "ID", "write to soundcard with ID using libao (default: 0)",
//    "OUT=0 lineout (default)\n"
//    "OUT=1 headphones",
    (void *) QUH_LIBAO_OUT
};
