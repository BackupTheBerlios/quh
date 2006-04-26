/*
wavpack.c - new filter wavpack for Quh

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
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "wavpack.h"


static int
quh_wavpack_in_open (st_quh_nfo_t *file)
{
  return 0;
}


static int
quh_wavpack_in_close (st_quh_nfo_t *file)
{
  return 0;
}


static int
quh_wavpack_in_seek (st_quh_nfo_t *file)
{
  return 0;
}


static int
quh_wavpack_in_demux (st_quh_nfo_t *file)
{
  return 0;
}


static int
quh_wavpack_in_write (st_quh_nfo_t *file)
{
  return 0;
}


const st_filter_t quh_wavpack_in =
{
  QUH_WAVPACK_IN,
  "wavpack",
  ".wv",
  -1,
  (int (*) (void *)) &quh_wavpack_in_demux,
  (int (*) (void *)) &quh_wavpack_in_open,
  (int (*) (void *)) &quh_wavpack_in_close,
  NULL,
  (int (*) (void *)) &quh_wavpack_in_write,
  (int (*) (void *)) &quh_wavpack_in_seek,
  NULL,
  NULL,
  NULL
};


const st_getopt2_t quh_wavpack_in_usage =
{
    "wavpack", 1, 0, QUH_WAVPACK,
    "FILE", "FILE is WAVPACK (if it has no .wavpack suffix)",
    (void *) QUH_WAVPACK_IN
};
