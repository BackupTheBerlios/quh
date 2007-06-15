/*
cache.c - cache filter for Quh

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
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "misc/itypes.h"
#include "misc/getopt2.h"
#include "misc/misc.h"
#include "misc/filter.h"
#include "misc/cache.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "cache.h"


static st_cache_t *c = NULL;


static int
quh_cache_init (st_quh_nfo_t *file)
{
  (void) file;

  c = cache_open (QUH_MAXBUFSIZE * 4, CACHE_MEM|CACHE_FIFO);

  return 0;
}


static int
quh_cache_open (st_quh_nfo_t *file)
{
  char buf[MAXBUFSIZE];

  sprintf (buf, "Size: %ld ms (%d Bytes)", quh_bytes_to_ms (file, QUH_MAXBUFSIZE * 4), QUH_MAXBUFSIZE * 4);

  quh_set_object_s (quh.filter_chain, QUH_OUTPUT, buf);

  return 0;
}


static int
quh_cache_quit (st_quh_nfo_t *file)
{
  (void) file;

  cache_close (c);

  return 0;
}


#if 0
static int
quh_cache_write (st_quh_nfo_t *file)
{
  if (CACHE_WRITE (c, quh.buffer, quh.buffer_len) == -1)
    return -1;

  quh.buffer_len = 0;
  if (cache_sizeof (c) > QUH_MAXBUFSIZE)
    {
      if (CACHE_READ (c, quh.buffer, QUH_MAXBUFSIZE) == -1)
        return -1;
      quh.buffer_len = QUH_MEGABYTE;
    }

  return 0;
}
#else
QUH_FILTER_FUNC_DUMMY (quh_cache_write) 
#endif


QUH_FILTER_FUNC_DUMMY (quh_cache_seek)
QUH_FILTER_FUNC_DUMMY (quh_cache_close)
QUH_FILTER_FUNC_DUMMY (quh_cache_ctrl)
QUH_FILTER_OUT(quh_cache, QUH_CACHE_PASS, "cache")


const st_getopt2_t quh_cache_usage =
{
    "cache", 1, 0, QUH_CACHE,
    NULL, "enable cache for smooth playback (default: enabled)"
};
