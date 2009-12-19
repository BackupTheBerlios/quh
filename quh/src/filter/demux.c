/*
demux.c - demux input support for Quh
          (de-)activates/(re-)sorts the other filters in the current chain

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
#include "misc/string.h"
#include "misc/misc.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "quh_filter.h"
#include "demux.h"


static int
quh_demux_probe (const char *fname, const int id)
{
  int probe_id[2];
    
  probe_id[0] = id;
  probe_id[1] = 0;

  filter_set_chain (quh.filter_chain, probe_id);

  quh.nfo.id = id;
  quh.nfo.id_s = filter_get_id_s (quh.filter_chain, 0);
  quh.nfo.f = filter_get_filter_by_id (quh.filter_chain, id);
  quh.nfo.fname = fname;
      
  if (!filter_demux (quh.filter_chain, &quh.nfo))
    return 0;

  return -1;
}


static int
quh_demux_open (st_quh_nfo_t *file)
{
  (void) file;
  int x = 0;
  const char *suffix = NULL;
  int id_chain[FILTER_MAX];
  int found = 0;
  int id = 0;

  // demux by file suffix
  if ((suffix = get_suffix (basename2 (quh.fname[quh.current_file]))))
    if (*suffix)
      {
#if     FILENAME_MAX > MAXBUFSIZE
        char buf[FILENAME_MAX];
#else
        char buf[MAXBUFSIZE];
#endif
        const st_filter_t *f = NULL;
  
        strcpy (buf, suffix);
        strlwr (buf);

        if ((f = filter_get_filter_by_magic (quh.filter_chain, (const unsigned char *) buf, strlen (buf))))
          {
            if (!quh_demux_probe (quh.fname[quh.current_file], f->id))
              {
                id = f->id;
                found = 1;
              }
          }
      }

  // demux by specific demux function
  if (!found)
    for (x = 0; quh_filter[x]; x++)
      if (quh_filter[x]->demux) // HAS a demux function
        if (!quh_demux_probe (quh.fname[quh.current_file], quh_filter[x]->id))
          {
            id = quh_filter[x]->id;
            found = 1;
            break;
          }

  if (found)
    {
      id_chain[0] = QUH_READ_IN;
      id_chain[1] = QUH_DEMUX_PASS;
#if 1
      id_chain[2] = id;
#else
      id_chain[2] = QUH_DECODE_PASS;
#endif

      for (x = 0; quh.filter_id[x]; x++)
        id_chain[x + 3] = quh.filter_id[x];
      id_chain[x + 3] = 0;

      // re-set filter chain with right codec
      filter_set_chain (quh.filter_chain, id_chain);

      // copy options from cmdline
      for (x = 0; quh.filter_id[x]; x++)
        if (quh.filter_option[x])
          {
            int subkey = QUH_OPTION;
            int id = filter_get_id (quh.filter_chain, x + 3);
            char *key_temp = filter_generate_key (NULL, &id, &subkey);

            cache_write (quh.o, key_temp, strlen (key_temp) + 1,
              (void *) quh.filter_option[x], strlen (quh.filter_option[x]) + 1);
          }

      return 0;
    }

  // unknown format

  return -1;
}


typedef struct
{
  unsigned long bitrate;
  const char *q;
} st_quality_t;


static int
quh_demux_ctrl (st_quh_nfo_t *file)
{
  int x = 0;
  char buf[MAXBUFSIZE], *p = NULL;
  struct
    {
      unsigned long bitrate;
      const char *q;
    } q[] = {
    {0,      "LOSSLESS"},
    {64000,  "LOW!"},
    {96000,  "*------"},
    {128000, "**-----"},
    {160000, "***----"},
    {192000, "****---"},
    {224000, "*****--"},
    {256000, "******-"},
    {320000, "CLASSIC"},
    {0, NULL}
  };

  sprintf (buf, "Format: %s\n", file->id_s);
  
  if (file->source == QUH_SOURCE_FILE)
    {
      if (file->expanding)
        strcat (buf, "Raw/File: Expanding\n");
      else if (file->raw_size > (unsigned long) fsizeof (file->fname))
        sprintf (strchr (buf, 0), "Raw/File: %.4f\n", (float) file->raw_size / fsizeof (file->fname));
    }

  sprintf (strchr (buf, 0), "Rate: %dHz\n", file->rate);
  sprintf (strchr (buf, 0), "Channels: %d\n", file->channels);
  sprintf (strchr (buf, 0), "Bits: %d\n", file->size * 8);

  if (file->min_bitrate != file->max_bitrate)
    sprintf (strchr (buf, 0), "Bitrate: %ld...%ldkbit ", file->min_bitrate / 1000, file->max_bitrate / 1000);
  else // both are the same
    {
      if (file->min_bitrate)
        sprintf (strchr (buf, 0), "Bitrate: %ldkbit ", file->max_bitrate / 1000);
      else // both are zero
        strcat (buf, "Bitrate: ");
    }

  if (quh.ansi_color)
    strcat (buf, "\x1b[1;33m");

  p = strchr (buf, 0);
  for (x = 0; q[x].q; x++)
    if (file->max_bitrate <= q[x].bitrate)
      {
        strcpy (p, q[x].q);
        break;
      }

  if (quh.ansi_color)
    strcat (buf, "\x1b[0m");
  strcat (buf, "\n");

  sprintf (strchr (buf, 0), "Signed: %s\n", file->is_signed ? "Yes" : "No");
  sprintf (strchr (buf, 0), "Endian: %s\n", file->is_big_endian ? "Big" : "Little");

  if (file->indices)
    {
      sprintf (strchr (buf, 0), "Indices: %d\n", file->indices);
        
      for (x = 1 ; x < file->indices + 1; x++)
        {
          unsigned long len = (x < file->indices ? file->index_pos[x] : file->raw_size) - file->index_pos[x - 1];

          sprintf (strchr (buf, 0), "%0*d:", misc_digits (QUH_MAXINDEX), x);

          sprintf (strchr (buf, 0), "%s (%ld Bytes)\n",
            quh_bytes_to_units (file, len, QUH_UNITS_CLOCK), len);
        }
    }
  if (file->expanding)
    strcat (buf, "Total: Expanding");
  else
    {
      sprintf (strchr (buf, 0), "Total: %s (%ld Bytes",
        quh_bytes_to_units (file, file->raw_size, QUH_UNITS_CLOCK), file->raw_size);
#if 0
      sprintf (strchr (buf, 0), "/%s BPS",
        quh_bytes_to_units (file, file->raw_size, QUH_UNITS_BPS));
#endif
      strcat (buf, ")");
    }

  quh_set_object_s (quh.filter_chain, QUH_OUTPUT, buf);
    
  return 0;
}


QUH_FILTER_FUNC_STUB (quh_demux_seek)
QUH_FILTER_FUNC_STUB (quh_demux_close)
QUH_FILTER_FUNC_STUB (quh_demux_write)
QUH_FILTER_FUNC_STUB (quh_demux_init)
QUH_FILTER_FUNC_STUB (quh_demux_quit)
QUH_FILTER_OUT(quh_demux,QUH_DEMUX_PASS,"demux")


const st_getopt2_t quh_demux_usage =
{
#if 0
    "demux", 2, 0, QUH_DEMUX,
    "CODEC", "demux file format (default: auto)\n"
    "CODEC=0 mp3\n"
    "CODEC=1 vorbis\n"
    "CODEC=2 wav"
#else
    "demux", 0, 0, QUH_DEMUX,
    NULL, "demux file format (default: enabled)"
#endif
};
