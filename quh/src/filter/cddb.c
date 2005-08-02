/*
cddb.c - cddb filter for quh

Copyright (c) 2005 NoisyB (noisyb@gmx.net)


This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
#include "misc/bswap.h"
#include "misc/itypes.h"
#include "misc/file.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "cddb.h"
#ifdef  HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef  HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef USE_NETGUI
#include <libnetgui.h>
#endif
#include "misc/property.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "cddb.h"


//static int cddb_in = 0;
static unsigned long cddb_id = 0;  
#ifdef  USE_NETGUI
static netgui_t *netgui = NULL;
#endif
static const char *cddb_host = "http://freedb.freedb.org";
static const char *cddb_uri = "/~cddb/cddb.cgi";
      

static int
quh_cddb_in_init (st_quh_filter_t *file)
{
  (void) file;

  if (!quh_get_object_s (quh.filter_chain, QUH_OPTION))
    quh_set_object_s (quh.filter_chain, QUH_OPTION, "http://freedb.freedb.org/~cddb/cddb.cgi");

  netgui = ng_init (cddb_host, NG_TCP|NG_DEBUG, NULL);

  return 0;
}


static int
cddb_sum (int n)
{
  int ret = 0;

  for (;;)
    {
      ret += n % 10;
      n = n / 10;
      if (!n)
        return ret;
    }
}


static int
quh_cddb_in_open (st_quh_filter_t * file)
{
  int x = 0;
  char buf[MAXBUFSIZE];
  int t = 0, n = 0;
  int count = 0;
  char buf2[MAXBUFSIZE];

  *buf = 0;
  if (!file->indices)
    {
      strcpy (buf, "Failed: Input has no indices");

      quh_set_object_s (quh.filter_chain, QUH_OUTPUT, buf);
  
      return 0;
    }

  for (x = 0; x < file->indices; x++)
    {
      // add to cddb_sum
      n += cddb_sum (file->index_pos[x] / file->channels / file->size / file->rate);

      if (x == 0)
        t = (file->raw_size / file->channels / file->size / file->rate) - 
            (file->index_pos[x] / file->channels / file->size / file->rate) -
            ((file->index_pos[x] / file->channels / file->size / file->rate) % 60); 
    }

  cddb_id = (n % 0xff) << 24 | t << 8 | file->indices;

#ifdef  USE_NETGUI
  sprintf (buf, "%s?cmd=cddb+query+%08lx+%d+",
    cddb_uri,
    cddb_id,
    file->indices);
  
  for (x = 0; x < file->indices; x++)
    sprintf (strchr (buf, 0), "%ld+", (file->index_pos[x] / 2352) + 150);

  sprintf (strchr (buf, 0), "%ld&hello=anonymous+localhost+%s+%s&proto=6",
    ((file->raw_size / 2352) + 150) / 75,
    "Quh",
    QUH_VERSION_S);

  ng_open (netgui, buf);
  while (ng_gets (netgui, buf, MAXBUFSIZE))
    {
      if (!(*buf))
        count++;

      if (count == 2) // EOF
        break;

      if (count == 1)
        if (ng_gets (netgui, buf, MAXBUFSIZE))
          if (ng_gets (netgui, buf2, MAXBUFSIZE))
            {
//classical 9b10f50b Wayne Marshall / Symphonie

              char *p = strchr (buf2, ' '), *p2 = NULL;
              if (!p)
                {
                  *buf2 = 0;
                  break;
                }

              *p='+';

              p2 = strchr (p, ' ');
              if (p2)
                *p2 = 0;
 
              break;
            }
    }
  ng_close (netgui);

  sprintf (buf, "%s?cmd=cddb+read+%s&hello=anonymous+localhost+%s+%s&proto=6",
    cddb_uri,
    buf2,
    "Quh",
    QUH_VERSION_S);
                                          
  ng_open (netgui, buf);

  x = 0;
  while (ng_gets (netgui, buf, MAXBUFSIZE))
    {
      if (!strncmp (buf, "TTITLE", 6))
        {
          sprintf (buf2, "TTITLE%d", x);
          get_property_from_string (buf, buf2, '=', file->index_name[x], '#');
          printf ("%s\n\n", file->index_name[x]);
          x++;
        }

      if (x == file->indices)
        break;
    }

  ng_close (netgui);
#endif  // USE_NETGUI

  *buf = 0;
  sprintf (strchr (buf, 0), "DiscId: 0x%08lx\n", cddb_id);

  if (file->indices)
    for (x = 1 ; x < file->indices + 1; x++)
      {
        unsigned long len = (x < file->indices ? file->index_pos[x] : file->raw_size) - file->index_pos[x - 1];

        sprintf (strchr (buf, 0), QUH_INDEX_COUNTER_S ":", x);

        sprintf (strchr (buf, 0), " %s", file->index_name[x - 1]);

        sprintf (strchr (buf, 0), "%s",
          quh_bytes_to_units (file, len, QUH_UNITS_CLOCK));
        
        if (x < file->indices)
          strcat (buf, "\n");
      }

  quh_set_object_s (quh.filter_chain, QUH_OUTPUT, buf);

  return 0;
}


static int
quh_cddb_in_quit (st_quh_filter_t * file)
{
  (void) file;

  ng_quit (netgui);

  return 0;
}  


const st_filter_t quh_cddb_in = {
  QUH_CDDB_IN,
  "cddb",
  NULL,
  0,
  (int (*) (void *)) &quh_cddb_in_open,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  (int (*) (void *)) &quh_cddb_in_init,
  (int (*) (void *)) &quh_cddb_in_quit
};


const st_getopt2_t quh_cddb_in_usage =
{
  "cddb", 2, 0, QUH_CDDB,
  "URL", "read CDDB info from server with URL", (void *) QUH_CDDB_IN
};
