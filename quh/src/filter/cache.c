/*
cache.c - cache filter for Quh

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


static st_cache_t *in = NULL;
static st_cache_t *out = NULL;
static int forked = 0;
static int pid = 0;


static int
quh_cache_init (st_quh_filter_t *file)
{
  (void) file;
  int mb = 0;

  if (!quh_get_object_s (quh.filter_chain, QUH_OPTION))
    quh_set_object_s (quh.filter_chain, QUH_OPTION, "2");

  mb = strtol (quh_get_object_s (quh.filter_chain, QUH_OPTION), NULL, 10);

  in = cache_open (4, mb * (QUH_MEGABYTE / 4), CACHE_MALLOC_LIFO);
  out = cache_open (4, mb * (QUH_MEGABYTE / 4), CACHE_MALLOC_LIFO);
  
  return 0;
}


static int
quh_cache_quit (st_quh_filter_t *file)
{
  (void) file;

  if (!pid)
    return 0;

  kill (pid, SIGKILL);
  waitpid (pid, NULL, 0);

  cache_close (in);
  cache_close (out); 

  return 0;
}


#if 0
static int
quh_cache_close (st_quh_filter_t *file)
{
  (void) file;
  return 0;
}


static int
quh_cache_open (st_quh_filter_t *file)
{
  (void) file;

  return 0;
}


static int
quh_cache_ctrl (st_quh_filter_t *file)
{
  (void) file;
  return 0;
}
#endif


static int
quh_cache_write (st_quh_filter_t *file)
{
  (void) file;

  if (!forked)
    {
      forked = 1;
      pid = fork();

      if (pid < 0) // failed
        return -1;
    }

  if (!pid) // child
    {
//      unsigned char buf[MAXBUFSIZE];
//      int result = 0;
      
      while (TRUE)
        {
//          result = cache_read (in, buf, MAXBUFSIZE);
          // process?
//          cache_write (out, buf, result);
          wait2 (50);
        }
    }

  if (pid) // parent
    {
//      quh.buffer_len = cache_write (in, quh.buffer, quh.buffer_len);
//      cache_read (out, quh.buffer, quh.buffer_len);
    }

  return 0;
}


const st_filter_t quh_cache =
{
  QUH_CACHE_PASS,
  "cache",
  NULL,
  0,
  NULL,
  NULL,
  NULL,
  NULL,
//  (int (*) (void *)) &quh_cache_write,
  NULL,
  NULL,
  NULL,
  (int (*) (void *)) &quh_cache_init,
  (int (*) (void *)) &quh_cache_quit
};


const st_getopt2_t quh_cache_usage =
{
    "cache", 2, 0, QUH_CACHE,
    "MB", "enable cache with MBytes size (default: 2)",
    (void *) QUH_CACHE_PASS
};
