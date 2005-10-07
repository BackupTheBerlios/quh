/*
stdin.c - stdin support for Quh

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
#include <stdio.h>
#include "misc/itypes.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "stdin.h"


static int
quh_stdin_demux (st_quh_filter_t * file)
{
  (void) file;
  
// fname == "-"

  return -1;
}


static int
quh_stdin_write (st_quh_filter_t * file)
{
  (void) file;
  
  quh.buffer_len = fread (&quh.buffer, 1, QUH_MAXBUFSIZE, stdin);

  return 0;
}


const st_filter_t quh_stdin_in = {
  QUH_STDIN_IN,
  "stdin read",
  NULL,
  0,
  (int (*) (void *)) &quh_stdin_demux,
  NULL,
  NULL,
  NULL,
  (int (*) (void *)) &quh_stdin_write,
  NULL,
  NULL,
  NULL,
  NULL
};


const st_getopt2_t quh_stdin_in_usage = {
  "stdin", 0, 0, QUH_STDIN,
  NULL, "read from stdin", (void *) QUH_STDIN_IN
};
