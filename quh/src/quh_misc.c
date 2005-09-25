/*
quh_misc.c - miscellaneous functions for Quh

written by 2002 Dirk (d_i_r_k_@gmx.net)

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
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/time.h>
#ifdef  HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef  USE_NETGUI
#include <libnetgui.h>
#endif
#include "misc/itypes.h"
#include "misc/bswap.h"
#include "misc/getopt.h"
#include "misc/getopt2.h"
#include "misc/file.h"
#include "misc/misc.h"
#include "misc/string.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "quh_filter.h"


unsigned long
time_ms (unsigned long *ms)
// returns milliseconds since midnight
{
  unsigned long t = 0;
  struct timeval tv;

  if (!gettimeofday (&tv, NULL))
    {
      t = (unsigned long) (tv.tv_usec / 1000);
      t += (unsigned long) ((tv.tv_sec % 86400) * 1000);
    }

  return ms ? *ms = t : t;
}


//#ifdef  DEBUG
void
quh_demuxer_sanity_check (st_quh_demux_t *file)
{
  int x = 0;

  printf ("\nid: %d\n", file->id);
  printf ("id_s: %s\n", file->id_s);

  printf ("indices: %d\n", file->indices);
  for (; x < file->indices; x++)
    printf ("index_pos[%d]: %ld\n", x, file->index_pos[x]);

  printf ("fname: %s\n", file->fname);
  printf ("raw_size: %ld\n", file->raw_size);
  printf ("source: %d\n", file->source);
  printf ("seekable: %d\n", file->seekable);

  printf ("rate: %d\n", file->rate);
  printf ("channels: %d\n", file->channels);
  printf ("is_big_endian: %s\n", file->is_big_endian ? "TRUE" : "FALSE");
  printf ("is_signed: %s\n", file->is_signed ? "TRUE" : "FALSE");
  printf ("size: %d\n", file->size);
  
//  printf ("misc: %s\n", file->misc ? "!NULL" : "NULL");

  fflush (stdout);
}


void
quh_soundcard_sanity_check (void)
{
  printf ("Rate: %d\n", quh.soundcard.rate);
  printf ("Channels: %d\n", quh.soundcard.channels);
//  printf ("is_big_endian: %s\n", quh.soundcard.is_big_endian ? "Yes" : "No");
  printf ("Signed: %s\n", quh.soundcard.is_signed ? "Yes" : "No");
  printf ("Size: %d\n", quh.soundcard.size);
  printf ("Max. Buffer: %ld\n", quh.soundcard.buffer_max);

  fflush (stdout);
}
//#endif


void
quh_set_object (st_filter_chain_t *fc, int o_id, void *o, int o_size)
{
  filter_object_write_by_id (fc, o_id, (void *) o, o_size);
}


void *
quh_get_object (st_filter_chain_t *fc, int o_id, void *o, int o_size)
{
  if (!filter_object_read_by_id (fc, o_id, o, o_size))
    return o;

  return NULL;
}


void
quh_set_object_s (st_filter_chain_t *fc, int o_id, const char *o)
{
  char key[MAXBUFSIZE];
  int pos = filter_get_pos (fc);
      
  if (o_id == QUH_OPTION) // QUH_OPTION fix
    {
      sprintf (key, "%d:%d", filter_get_id (fc, pos), o_id);
      filter_object_write (fc, key, strlen (key) + 1, (void *) o, strlen (o) + 1);
    }
  else
    filter_object_write_by_id (fc, o_id, (void *) o, strlen (o) + 1);
}


const char *
quh_get_object_s (st_filter_chain_t *fc, int o_id)
{
  static char o[MAXBUFSIZE];
  char key[MAXBUFSIZE];
  int pos = filter_get_pos (fc);
    
  if (o_id == QUH_OPTION) // QUH_OPTION fix
    {
      sprintf (key, "%d:%d", filter_get_id (fc, pos), o_id);
      if (!filter_object_read (fc, key, strlen (key) + 1, o, MAXBUFSIZE))
        return o;
    }
  else
    if (!filter_object_read_by_id (fc, o_id, o, MAXBUFSIZE))
      return o;

  return NULL;
}


int
quh_wav_write_header (FILE *fh, int channels, int freq, int size, unsigned long raw_size)
{
  st_wav_header_t wav_header;
  memset (&wav_header, 0, sizeof (st_wav_header_t));

  strncpy ((char *) wav_header.magic, "RIFF", 4);
  wav_header.total_length =           me2le_32 (raw_size + sizeof (st_wav_header_t) - 8);
  strncpy ((char *) wav_header.type,  "WAVE", 4);
  strncpy ((char *) wav_header.fmt,   "fmt ", 4);
  wav_header.header_length =          me2le_32 (16); // always 16
  wav_header.format =                 me2le_16 (1); // WAVE_FORMAT_PCM == default
  wav_header.channels =               me2le_16 (channels);
  wav_header.freq =                   me2le_32 (freq);
  wav_header.bytespersecond =         me2le_32 (freq * channels * size);
  wav_header.blockalign =             me2le_16 (channels * size);
  wav_header.bitspersample =          me2le_16 (size * 8);
  strncpy ((char *) wav_header.data,  "data", 4);
  wav_header.data_length =            me2le_32 (raw_size);

  return fwrite (&wav_header, 1, sizeof (st_wav_header_t), fh);
}


int
quh_get_index (st_quh_filter_t *file)
{
  int x = 0;

  if (!file->indices)
    return 0; // no indices

  if (quh.raw_pos > file->index_pos[0])
    for (; x < file->indices; x++)
      if (quh.raw_pos < file->index_pos[x])
        return x - 1;

  return file->indices - 1;
}


unsigned long
quh_random (unsigned long a, unsigned long b)
{
  static int init = 0;

  if (!init)
    {
      srand (time (0));
      init = 1;
    }

  if (a == b)
    return a;
  else
    {
      unsigned long min = MIN (a, b),
                    max = MAX (a, b);
  
      return (rand () % (max - min)) + min;
    }
}


#if 0
void
quh_sort_values (unsigned long *a, unsigned long *b)
{
  if (*a > *b)
    {
      unsigned long v = *a;
      *a = *b;
      *b = v;
    }
}
#endif


unsigned long
quh_ms_to_bytes (st_quh_filter_t *file, unsigned long ms)
{
  unsigned long value_b = 0;

  if (ms)
    {
      value_b = (unsigned long) ((float) (file->rate * file->size * file->channels) / 1000) * ms;
      return value_b - (value_b % file->size);         // "fix" value
    }
  else
    return 0;
}


unsigned long
quh_bytes_to_ms (st_quh_filter_t *file, unsigned long bytes)
{
  if (bytes)
    return (unsigned long) (bytes * ((float) 1000 / (file->rate * file->size * file->channels)));
  else
    return 0;
}


const char *
quh_bytes_to_units (st_quh_filter_t *file, unsigned long bytes, int units)
{
  static char buf[MAXBUFSIZE];
  unsigned long ms = 0;

  *buf = 0;
  switch (units)
    {
      case QUH_UNITS_MS:
        sprintf (buf, "%ld", quh_bytes_to_ms (file, bytes));
        break;

      case QUH_UNITS_BYTES:
        sprintf (buf, "%ld", bytes);
        break;

      case QUH_UNITS_CLOCK:
      default:
        ms = quh_bytes_to_ms (file, bytes);
        sprintf (buf, "%3ld:%02ld.%03ld",
                 (unsigned long) ms / 60000,
                 (unsigned long) (ms % 60000) / 1000,
                 (unsigned long) ms % 1000);
        break;
    }

  return buf;
}


unsigned long
quh_parse_optarg (st_quh_filter_t *f, const char *p)
{
  unsigned long value = 0;

  if (stristr (p, "eof"))
    {
      value = f->raw_size;
    }
  else if (stristr (p, "min") || stristr (p, "max"))
    {
      unsigned long min_ms = 0;
      unsigned long max_ms = quh_bytes_to_ms (f, f->raw_size);
      char buf[MAXBUFSIZE];
      const char *s = NULL;

      strcpy (buf, p);
      if ((s = stristr (buf, "min")))
        {
          s = strchr (s, '=') + 1;
          if (strchr (s, ':'))
            *(strchr (s, ':')) = 0;
          min_ms = strtol (s, NULL, 10);
        }

      strcpy (buf, p);
      if ((s = stristr (buf, "max")))
        {
          s = strchr (s, '=') + 1;
          if (strchr (s, ':'))
            *(strchr (s, ':')) = 0;
          max_ms = strtol (s, NULL, 10);
        }

      value = quh_ms_to_bytes (f, quh_random (min_ms, max_ms));
    }
  else
    {
      value = quh_ms_to_bytes (f, strtol (p, NULL, 10));
    }

  return value;
}


int
quh_play (void)
{
// sort files like 10-bla.mp3 1-bla.mp3 to 1-bla.mp3 10-bla.mp3!!!!!!!! because it always sucked

  if (quh.shuffle)
    {
//      int file = 0;
//      for (; file < quh.files; file++)
    }

  while (TRUE)
    {
      const char *fname = NULL;
      st_quh_filter_t *file = &quh.demux;
      int id_chain[FILTER_MAX];

      memset (file, 0, sizeof (st_quh_filter_t));

      if (quh.random)
        quh.current_file = quh_random (0, quh.files);

      // select file
      fname = quh.fname[quh.current_file];

      quh.next_file = quh.current_file + 1;

      // set active filters in chain
      id_chain[0] = QUH_READ_IN;
      id_chain[1] = QUH_DEMUX_PASS;

      filter_set_chain (quh.filter_chain, id_chain);

      if (!filter_open (quh.filter_chain, file))
        {
          if (quh.start_optarg)
            quh.start = quh_parse_optarg (file, quh.start_optarg);
          else
            quh.start = 0; // default
    
          quh.raw_pos = quh.start;
    
          if (quh.len_optarg)
            quh.len = MIN (quh_parse_optarg (file, quh.len_optarg), file->raw_size - quh.raw_pos);
          else
            quh.len = 0; // default (play until EOF)
    
          // play
          filter_seek (quh.filter_chain, file); // seek to quh.raw_pos
    
          filter_ctrl (quh.filter_chain, file);
    
          while (quh.raw_pos <= quh.start + (quh.len ? quh.len : (file->raw_size - quh.start)))
            { 
              quh.buffer_len = 0;
    
              if (filter_write (quh.filter_chain, file) != 0)
                break;

              quh.raw_pos = MIN (quh.raw_pos + quh.buffer_len, file->raw_size);

              if (quh.quit || quh.raw_pos >= file->raw_size)
                break;
            }

          filter_close (quh.filter_chain, file);
        }

      if (quh.quit)
        break;

      quh.current_file = quh.next_file;
      if (quh.current_file >= quh.files)
        {
          if (quh.loop)
            quh.current_file = 0;
          else // stop
            break;
        }
    }

 // TODO: save current playlist

  return 0;
}


const st_getopt2_t quh_options_usage[] =
{
  {
    NULL, 0, 0, 0,
    NULL, "Usage: quh [OPTION]... [PLAYLIST|FILE"
#ifdef  USE_NETGUI
    "|URL"
#endif
    "|-]... [FILTER]...\n", NULL
  },
  {
    NULL, 0, 0, 0,
    NULL, "Option", NULL
  },
  {
    "start", 1, 0, QUH_START,
    "MS", "define start of playback in MS (milliseconds (1/1000 s))\n"
    "MS=15000    play from 15 s\n"
    "MS=min=1000 play from rnd pos between 1 s and eof INPUT\n"
    "MS=max=2500 play from rnd pos between 0 s and 2.5 s\n"
    OPTION_LONG_S "start min=1000:max=2500 would start\n"
    "playing from random pos between 1 s and 2.5 s",
    NULL
  },
  {
    "len", 1, 0, QUH_LEN,
    "MS", "define length of playback in MS\n"
    "MS=15000    play for 15 s\n"
    "MS=min=1000 play for random time between 1 s and eof INPUT\n"
    "MS=max=2500 play for random time between 0 s and 2.5 s\n"
    OPTION_LONG_S "len min=1000:max=2500 would play for\n"
    "random time between 1 s and 2.5 s",
    NULL
  },
  {
    "bpm", 1, 0, QUH_BPM,
    "BPM", "define BPM (Beats Per Minute) of playback\n"
    "exactly like " OPTION_LONG_S "len but takes BPM instead of MS\n"
    "and turns them into MS internally",
    NULL
  },
  {
    "shuffle", 0, 0, QUH_SHUFFLE,
    NULL, "shuffle song-list before playing", NULL
  },
  {
    "random", 0, 0, QUH_RANDOM,
    NULL, "random choice from song-list while playing", NULL
  },
#if 0
  {
    "keep", 0, 0, QUH_KEEP,
    NULL, "use this together with " OPTION_LONG_S "shuffle or " OPTION_LONG_S "random\n"
    "to keep files that belong to the same serie together", NULL
  },
  {
    "keep2", 0, 0, QUH_KEEP2,
    NULL, "like " OPTION_LONG_S "keep but respect files\n"
    "that belong to the same sub-directory", NULL
  },
#endif
  {
    "loop", 2, 0, QUH_LOOP,
    "N", "loops song playback N times (default: 0 (forever))", NULL
  },
  {
    "intro", 0, 0, QUH_INTRO,
    NULL, "play only 10 s of every song\n"
    "same as " OPTION_LONG_S "start=60000 and " OPTION_LONG_S "len=10000",
    NULL
  },
#if 0
  {
    "rr", 2, 0, QUH_RR,
    "MAX[:MIN]", "Random Radio (idea by Benno Hoelle)\n"
    "MAX segment length in ms (default: 1500)\n"
    "MIN segment length in ms (default: 500)\n"
    "same as " OPTION_LONG_S "start=min=0:max=eof " OPTION_LONG_S "len=min=MIN:max=MAX\n"
    OPTION_LONG_S "random " OPTION_LONG_S "loop", NULL
  },
#endif
  {
    "R", 0, 0, QUH_R,
    NULL, "play subdirectories recursively", (void *) QUH_RECURSIVE
  },
  {
    "q", 0, 0, QUH_Q,
    NULL, "be quiet", NULL
  },
  {
    "v", 0, 0, QUH_V,
    NULL, "be more verbose", NULL
  },
#ifdef  DEBUG
  {
    "debug", 0, 0, QUH_DEBUG,
    NULL, "enable debugging messages", NULL
  },
#endif
  {
    "version", 0, 0, QUH_VER,
    NULL, "output version information and exit", NULL
  },
  {
    "ver", 0, 0, QUH_VER,
    NULL, NULL, NULL
  },
  {
    "s", 0, 0, QUH_S,
    NULL, "output supported formats information and exit", NULL
  },
  {
    "help", 0, 0, QUH_HELP,
    NULL, "display this help and exit", NULL
  },
  {
    "h", 0, 0, QUH_HELP,
    NULL, NULL, NULL
  },
  {NULL, 0, 0, 0, NULL, NULL, NULL}
};


const st_getopt2_t quh_options_usage_bottom[] =
{
  {
    NULL, 0, 0, 0,
    NULL, "Please report any problems/ideas/fixes to d_i_r_k_@gmx.net or surf to\n"
          "                                          http://quh.berlios.de", NULL
  },
  {NULL, 0, 0, 0, NULL, NULL, NULL}
};
