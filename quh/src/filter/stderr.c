/*
stderr.c - STDERR support for Quh

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
#include "misc/itypes.h"
#include "misc/bswap.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "stderr.h"


static int
quh_stderr_write (st_quh_filter_t *file)
{
  (void) file;

  fwrite (quh.buffer, 1, quh.buffer_len, stderr);

  return 0;
}


const st_filter_t quh_stderr_out =
{
  QUH_STDERR_OUT,
  "stderr write",
  NULL,
  0,
  NULL,
  NULL,
  NULL,
  (int (*) (void *)) &quh_stderr_write,
  NULL,
  NULL,
  NULL,
  NULL
};


const st_getopt2_t quh_stderr_out_usage =
{
    "stderr", 0, 0, QUH_STDERR,
    NULL, "write to stderr", (void *) QUH_STDERR_OUT
};