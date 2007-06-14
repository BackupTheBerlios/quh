/*
decode.c - decode input support for Quh


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
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "quh_filter.h"
#include "decode.h"


static int
quh_decode_open (st_quh_nfo_t *file)
{
  if (file->f->open)
    return file->f->open (file);
  return 0;
}


static int
quh_decode_close (st_quh_nfo_t *file)
{
  if (file->f->close)
    return file->f->close (file);
  return 0;
}


static int
quh_decode_write (st_quh_nfo_t *file)
{
  if (file->f->write)
    return file->f->write (file);
  return 0;
}


static int
quh_decode_seek (st_quh_nfo_t *file)
{
  if (file->f->seek)
    return file->f->seek (file);
  return 0;
}


static int
quh_decode_init (st_quh_nfo_t *file)
{
  if (file->f->init)
    return file->f->init (file);
  return 0;
}


static int
quh_decode_quit (st_quh_nfo_t *file)
{
  if (file->f->quit)
    return file->f->quit (file);
  return 0;
}


QUH_FILTER_FUNC_DUMMY (quh_decode_ctrl)


QUH_FILTER_OUT (quh_decode,QUH_DECODE_PASS,"decode")
#if 0
const st_filter_t quh_decode =
{
  QUH_DECODE_PASS,
  "decode",
  NULL,
  0,
  NULL,
  (int (*) (void *)) &quh_decode_open,
  (int (*) (void *)) &quh_decode_close,
  NULL,
  (int (*) (void *)) &quh_decode_write,
  (int (*) (void *)) &quh_decode_seek,
  NULL,
  (int (*) (void *)) &quh_decode_init,
  (int (*) (void *)) &quh_decode_quit,
};
#endif


const st_getopt2_t quh_decode_pass_usage =
{
  "dec", 0, 0, QUH_DEC,
  NULL, "decode (default: enabled)"
};
