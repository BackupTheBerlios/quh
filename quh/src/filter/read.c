/*
read.c - input read support for Quh
         reads from file or stream or device

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
#include "filter.h"
#include "misc/file.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "read.h"


int fp = 0;


#if 0
static int
quh_read_in_open (st_quh_nfo_t *file)
{
  // device, stream or file?
  if ((file->f = open (file->fname, O_RDONLY)) == -1)
    return -1;

  return 0;
}
#endif


static int
quh_read_in_ctrl (st_quh_nfo_t *file)
{
  char buf[MAXBUFSIZE];

  sprintf (buf, "Input: %s\n", basename2 (file->fname));

  strcat (buf, "Source: ");
  switch (file->source)
    {
      case QUH_SOURCE_STREAM:
        strcat (buf, "Stream\n");
        break;

      case QUH_SOURCE_DEVICE:
        strcat (buf, "Device\n");
        break;

      case QUH_SOURCE_FILE:
      default:
        strcat (buf, "File\n");
        break;
    }

  strcat (buf, "Seekable: ");
  switch (file->seekable)
    {
      case QUH_SEEKABLE:
        strcat (buf, "Yes\n");
        break;

      case QUH_SEEKABLE_FORWARD_ONLY:
        strcat (buf, "Yes, forward only\n");
        break;

      case QUH_SEEKABLE_BACKWARD_ONLY:
        strcat (buf, "Yes, backward only\n");
        break;

//      case QUH_SEEKABLE_CACHED:
//        sprintf (strchr (buf, 0), "Yes, cache only (\"Window\" size: %ld Bytes)\n", quh.cache_size);
//        break;

      default:
      case QUH_NOT_SEEKABLE:
        strcat (buf, "No\n");
        break;
    }

  sprintf (strchr (buf, 0), "Expanding: %s", file->expanding ? "Yes" : "No");

  if (quh.start != 0)
    sprintf (strchr (buf, 0), "\nStart: %s (%ld Bytes)",
      quh_bytes_to_units (file, quh.start, QUH_UNITS_CLOCK),
      quh.start);

  if (quh.len != 0)
    sprintf (strchr (buf, 0), "\nLength:%s (%ld Bytes, %.4f BPM)",
      quh_bytes_to_units (file, quh.len, QUH_UNITS_CLOCK),
      quh.len,
      (float) 60000 / quh_bytes_to_ms (file, quh.len));

  quh_set_object_s (quh.filter_chain, QUH_OUTPUT, buf);
  
  return 0;
}


QUH_FILTER_FUNC_STUB (quh_read_in_seek)
QUH_FILTER_FUNC_STUB (quh_read_in_close)
QUH_FILTER_FUNC_STUB (quh_read_in_demux)
QUH_FILTER_FUNC_STUB (quh_read_in_write)
QUH_FILTER_FUNC_STUB (quh_read_in_open)
QUH_FILTER_IN(quh_read_in, QUH_READ_IN, "read", NULL)
