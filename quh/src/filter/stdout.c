/*
stdout.c - STDOUT support for Quh

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
#include "quh_defines.h"
#include "quh.h"
#include "stdout.h"


static int
quh_stdout_out_write (st_quh_nfo_t *file)
{
  (void) file;

  fwrite (quh.buffer, 1, quh.buffer_len, stdout);

  return 0;
}


QUH_FILTER_FUNC_STUB (quh_stdout_out_seek)
QUH_FILTER_FUNC_STUB (quh_stdout_out_ctrl)
QUH_FILTER_FUNC_STUB (quh_stdout_out_open)
QUH_FILTER_FUNC_STUB (quh_stdout_out_close)
QUH_FILTER_FUNC_STUB (quh_stdout_out_init)
QUH_FILTER_FUNC_STUB (quh_stdout_out_quit)
QUH_FILTER_OUT(quh_stdout_out, QUH_STDOUT_OUT, "stdout write")


const st_getopt2_t quh_stdout_out_usage =
{
    "stdout", 0, 0, QUH_STDOUT,
    NULL, "write to stdout"
};
