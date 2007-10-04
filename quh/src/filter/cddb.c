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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "misc/string.h"
#include "misc/itypes.h"
#include "misc/misc.h"
#include "misc/file.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "misc/net.h"
#include "misc/property.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "cddb.h"


static int inited = 0;
static unsigned long cddb_id = 0;  
#ifdef  USE_TCP
static st_net_t *net = NULL;
#endif
static const char *cddb_host = "http://freedb.freedb.org";
static const char *cddb_uri = "/~cddb/cddb.cgi";
      

static int
cddb_sum (int n)
{
  int ret = 0;

  while (1)
    {
      ret += n % 10;
      n = n / 10;
      if (!n)
        return ret;
    }
}


static int
quh_cddb_in_open (st_quh_nfo_t * file)
{
  int x = 0;
  char buf[MAXBUFSIZE];
  int t = 0, n = 0;
  char buf2[MAXBUFSIZE];
  char *p = NULL;

  if (!inited)
    { 
      if (!quh_get_object_s (quh.filter_chain, QUH_OPTION))
        quh_set_object_s (quh.filter_chain, QUH_OPTION, "http://freedb.freedb.org/~cddb/cddb.cgi");

#ifdef  USE_TCP
      net = net_init (0);
#endif
      inited = 1; 
    }

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

#ifdef  USE_TCP
  net_open (net, cddb_host, 80);

  sprintf (buf, "%s%s?cmd=cddb+query+%08lx+%d+",
    cddb_host,
    cddb_uri,
    cddb_id,
    file->indices);
  
  for (x = 0; x < file->indices; x++)
    sprintf (strchr (buf, 0), "%ld+", (file->index_pos[x] / 2352) + 150);

  sprintf (strchr (buf, 0), "%ld&hello=anonymous+localhost+%s+%s&proto=6",
    ((file->raw_size / 2352) + 150) / 75,
    "Quh",
    QUH_VERSION_S);

  p = net_build_http_request (buf, "Quh", 0, NET_METHOD_GET, 0);

  net_write (net, p, strlen (p));

  net_parse_http_response (net);

  // before: classical 9b10f50b Wayne Marshall / Symphonie
  // after: classical+9b10f50b
  while (net_gets (net, buf, MAXBUFSIZE))
    {
      // 200 classical 9e10cb0b Wayne Marshall / Organ Transcriptions
      if (!strncmp (buf, "200 ", 4))
        {
          strtrim_s (buf, "200 ", NULL);
          p = strchr (buf, ' ');

          if (p)
            *p = '+';

          p = strchr (buf, ' ');

          if (p)
            *p = 0;

          strncpy (buf2, buf, MAXBUFSIZE)[MAXBUFSIZE - 1] = 0;

          break;
        }

      // 211 Found inexact matches, list follows (until terminating .')
      // classical 9b10f50b Wayne Marshall / Symphonie
      // .
      if (!strncmp (buf, "211 ", 4))
        if (net_gets (net, buf, MAXBUFSIZE))
          {
            p = strchr (buf, ' ');

            if (p)
              *p = '+';

            p = strchr (buf, ' ');

            if (p)
              *p = 0;

            strncpy (buf2, buf, MAXBUFSIZE)[MAXBUFSIZE - 1] = 0;

            break;
          }
    }

  // we don't use the http keep alive flag
  net_close (net);
  net_open (net, cddb_host, 80);

  sprintf (buf, "%s%s?cmd=cddb+read+%s&hello=anonymous+localhost+%s+%s&proto=6",
    cddb_host,
    cddb_uri,
    buf2,
    "Quh",
    QUH_VERSION_S);

  p = net_build_http_request (buf, "Quh", 0, NET_METHOD_GET, 0);
  
  net_write (net, p, strlen (p));
  
  net_parse_http_response (net);

  x = 0;
  while (net_gets (net, buf, MAXBUFSIZE))
    if (!strncmp (buf, "TTITLE", 6))
      {
        sprintf (buf2, "TTITLE%d", x);
        p = (char *) get_property_from_string (buf, buf2, '=', '#');
        if (p)
          strncpy (file->index_name[x], p, QUH_INDEX_NAME_LEN)[QUH_INDEX_NAME_LEN - 1] = 0;
        else
          *(file->index_name[x]) = 0;

#ifdef  DEBUG
        printf ("%s\n\n", file->index_name[x]);
#endif

        x++;

        if (x == file->indices)
          break;
      }

  net_close (net);  
#endif  // USE_TCP

  *buf = 0;
  sprintf (strchr (buf, 0), "DiscId: 0x%08lx\n", cddb_id);

  if (file->indices)
    for (x = 1 ; x < file->indices + 1; x++)
      {
        unsigned long len = (x < file->indices ? file->index_pos[x] : file->raw_size) - file->index_pos[x - 1];

        sprintf (strchr (buf, 0), "%0*d:", misc_digits (QUH_MAXINDEX), x);

        sprintf (strchr (buf, 0), " %s", file->index_name[x - 1]);

        sprintf (strchr (buf, 0), "%s",
          quh_bytes_to_units (file, len, QUH_UNITS_CLOCK));
        
        if (x < file->indices)
          strcat (buf, "\n");
      }

  quh_set_object_s (quh.filter_chain, QUH_OUTPUT, buf);

  return 0;
}


#if 0
static int
quh_cddb_in_quit (st_quh_nfo_t * file)
{
  (void) file;

#ifdef  USE_TCP
  net_quit (net);
#endif

  return 0;
}  
#endif


QUH_FILTER_FUNC_STUB (quh_cddb_in_seek)
QUH_FILTER_FUNC_STUB (quh_cddb_in_ctrl)
QUH_FILTER_FUNC_STUB (quh_cddb_in_demux) 
QUH_FILTER_FUNC_STUB (quh_cddb_in_close) 
QUH_FILTER_FUNC_STUB (quh_cddb_in_write) 
QUH_FILTER_IN(quh_cddb_in, QUH_CDDB_IN, "cddb", NULL)


const st_getopt2_t quh_cddb_in_usage =
{
#if 0
  "cddb", 2, 0, QUH_CDDB,
  "URL", "read CDDB info from server with URL"
#else
  "cddb", 0, 0, QUH_CDDB,
  NULL, "read CDDB info from freedb.org"
#endif
};
