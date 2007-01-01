/*
quh_misc.c - miscellaneous functions for Quh

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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>  // waitpid()
#include <sys/time.h>
#ifdef  HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "misc/defines.h"
#include "misc/itypes.h"
#include "misc/bswap.h"
#include "misc/cache.h"
#include "misc/getopt.h"
#include "misc/getopt2.h"
#include "misc/file.h"
#include "misc/misc.h"
#include "misc/string.h"
#include "misc/filter.h"
#include "misc/audio_sdl.h"
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
quh_demuxer_sanity_check (st_quh_nfo_t *file)
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


static char *
quh_QUH_OPTION_key (st_filter_chain_t *fc, int subkey)
{
  int pos = filter_get_pos (fc);
  int id = filter_get_id (fc, pos);

  return filter_generate_key (NULL, &id, &subkey);
}


void
quh_set_object (st_filter_chain_t *fc, int subkey, void *o, int o_size)
{
  char *key_temp = NULL;

  if (subkey == QUH_OPTION) // QUH_OPTION fix
    key_temp = quh_QUH_OPTION_key (fc, subkey);
  else
    key_temp = filter_get_key (fc, &subkey);

  cache_write (quh.o, key_temp, strlen (key_temp) + 1, o, o_size);
}


void
quh_set_object_s (st_filter_chain_t *fc, int subkey, const char *o)
{
  quh_set_object (fc, subkey, (void *) o, strlen (o) + 1);
}


void *
quh_get_object (st_filter_chain_t *fc, int subkey, void *o, int o_size)
{
  char *key_temp = NULL;

  if (subkey == QUH_OPTION) // QUH_OPTION fix
    key_temp = quh_QUH_OPTION_key (fc, subkey);
  else
    key_temp = filter_get_key (fc, &subkey);

  if (cache_read (quh.o, key_temp, strlen (key_temp) + 1, o, o_size))
    return o;

  return NULL;
}


const char *
quh_get_object_s (st_filter_chain_t *fc, int subkey)
{
  static char o[MAXBUFSIZE];

  *o = 0;
  return (const char *) quh_get_object (fc, subkey, &o, MAXBUFSIZE);
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


const char *
quh_forked_wav_decode (st_quh_nfo_t *file, void (*decode) (st_quh_nfo_t *, const char *))
{
  int count = 0;

//  remove (quh.tmp_file);

  quh.pid = fork ();
  
  if (quh.pid < 0) // failed
    return NULL;
        
  if (!quh.pid) // child
    {
      decode (file, quh.tmp_file);

      exit (0);
    }

  // wait max. 5 seconds for that file
  count = 5;
  while (access (quh.tmp_file, F_OK) == -1)
    {
      wait2 (1000);
      count--;

      if (!count)
        {
#ifdef  DEBUG
          fputs ("ERROR: decode() callback function failed\n", stderr);
#endif
          return NULL;
        }
    }

  wait2 (2000); // gracetime

#ifdef  DEBUG
  printf ("%s %d\n", quh.tmp_file, fsizeof (quh.tmp_file));
  fflush (stdout);
#endif

  return quh.tmp_file;
}


int
quh_get_index (st_quh_nfo_t *file)
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
      srand (time_ms (0));
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

static unsigned long
quh_ms_to_bytes_int (int freq, int bits, int channels, unsigned long ms)
{
  unsigned long value_b = 0;

  if (!ms)
    return 0;

  value_b = (unsigned long) ((float) (freq * bits * channels) / 1000) * ms;
  return value_b - (value_b % bits);         // "fix" value
}


static unsigned long
quh_bytes_to_ms_int (int freq, int bits, int channels, unsigned long bytes)
{
  if (!bytes)
    return 0;

  return (unsigned long) (bytes * ((float) 1000 / (freq * bits * channels)));
}


unsigned long
quh_ms_to_bytes (st_quh_nfo_t *file, unsigned long ms)
{
  return quh_ms_to_bytes_int (file->rate, file->size, file->channels, ms);
}


unsigned long
quh_bytes_to_ms (st_quh_nfo_t *file, unsigned long bytes)
{
  return quh_bytes_to_ms_int (file->rate, file->size, file->channels, bytes);
}


const char *
quh_bytes_to_units (st_quh_nfo_t *file, unsigned long bytes, int units)
{
  static char buf[MAXBUFSIZE];
  unsigned long ms = quh_bytes_to_ms (file, bytes);

  *buf = 0;
  switch (units)
    {
      case QUH_UNITS_MS:
        sprintf (buf, "%ld", ms);
        break;

      case QUH_UNITS_BPS:
//        sprintf (buf, "%ld", bytes / MAX ((ms / 1000), 1));
//        break;

      case QUH_UNITS_BYTES:
        sprintf (buf, "%ld", bytes);
        break;

      case QUH_UNITS_CLOCK:
      default:
        sprintf (buf, "%3ld:%02ld.%03ld",
                 (unsigned long) ms / 60000,
                 (unsigned long) (ms % 60000) / 1000,
                 (unsigned long) ms % 1000);
        break;
    }

  return buf;
}


unsigned long
quh_parse_optarg (st_quh_nfo_t *f, const char *p)
{
  unsigned long value = 0;
  unsigned long min_ms = 0;
  unsigned long max_ms = quh_bytes_to_ms (f, f->raw_size);
  char buf[MAXBUFSIZE];
  const char *s = NULL;

  if (stristr (p, "eof"))
    {
      value = f->raw_size;
    }
  else if (stristr (p, "min") || stristr (p, "max"))
    {
      strcpy (buf, p);

      if ((s = stristr (buf, "min")))
        if ((s = strchr (s, '=')))
          min_ms = strtol (s + 1, NULL, 10);

      if ((s = stristr (buf, "max")))
        if ((s = strchr (s, '=')))
          max_ms = strtol (s + 1, NULL, 10);

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
  int shuffle[QUH_MAX_FILES];

  if (quh.shuffle) // unlike random, shuffle is done only _once_
    {
      int x = 0, tmp[QUH_MAX_FILES];

      for (; x < quh.files; x++)
        tmp[x] = 1;
      
      for (x = 0; x < quh.files; x++)
        {
          int y;

          do 
            y = quh_random (0, quh.files);
          while (!tmp[y]);

          shuffle[x] = y;
          tmp[y] = 0;
        }
    }

  while (1)
    {
      const char *fname = NULL;
      st_quh_nfo_t *file = &quh.nfo;
      int id_chain[FILTER_MAX];
      struct stat fstate;

      memset (file, 0, sizeof (st_quh_nfo_t));

      if (quh.random)
        quh.current_file = quh_random (0, quh.files);
      else if (quh.shuffle)
        quh.current_file = shuffle[quh.next_file];

      // select file
      fname = quh.fname[quh.current_file];

      // TODO: streams are not supported, yet 
      if (access (fname, R_OK))
        continue;

      file->source = QUH_SOURCE_STREAM;
      if (!stat (fname, &fstate))
        {
          if (S_ISREG (fstate.st_mode))
            file->source = QUH_SOURCE_FILE;
          else if (S_ISBLK (fstate.st_mode))
            file->source = QUH_SOURCE_DEVICE;
        }

      quh.next_file = quh.next_file + 1;

      // set active filters in chain
      id_chain[0] = QUH_READ_IN;
      id_chain[1] = QUH_DEMUX_PASS;

      filter_set_chain (quh.filter_chain, id_chain);

//filter_st_filter_chain_t_sanity_check (quh.filter_chain);

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

              if (quh.quit || quh.raw_pos >= file->raw_size || !quh.buffer_len)
                break;
            }

          filter_close (quh.filter_chain, file);

          if (quh.pid > 0) // parent kills child
            {
              kill (quh.pid, SIGKILL);
              waitpid (quh.pid, NULL, 0);
            }
        }
      else
        {
          fprintf (stderr, "QUH: unknown filetype %s\n", fname);
          fflush (stderr);
        }

      if (quh.quit)
        break;

      quh.current_file = quh.next_file;
      if (quh.current_file >= quh.files)
        {
          if (quh.loop)
            {
              quh.current_file = 0;
              quh.next_file = 0;
            }
          else if (!quh.random)
            break; // stop
        }
    }

// TODO: save current playlist and memorize current_file and quh.raw_pos in rc file

  return 0;
}


const st_getopt2_t quh_options_usage[] =
{
  {
    NULL, 0, 0, 0,
    NULL, "Usage: quh [OPTION]... ["
//          "PLAYLIST|"
          "FILE|"
#ifdef  USE_TCP
//          "URL|"
#endif
#ifdef  USE_CDDB
            "CDROM|"
#endif
            "-]... [FILTER]...\n"
  },
  {
    NULL, 0, 0, 0,
    NULL, "Option"
  },
  {
    "start", 1, 0, QUH_START,
    "MS", "define start of playback in MS (milliseconds (1/1000 s))\n"
    "MS=15000    play from 15 s\n"
    "MS=min=1000 play from rnd pos between 1 s and eof INPUT\n"
    "MS=max=2500 play from rnd pos between 0 s and 2.5 s\n"
    OPTION_LONG_S "start min=1000:max=2500 would start\n"
    "playing from random pos between 1 s and 2.5 s"
  },
  {
    "len", 1, 0, QUH_LEN,
    "MS", "define length of playback in MS\n"
    "MS=15000    play for 15 s\n"
    "MS=min=1000 play for random time between 1 s and eof INPUT\n"
    "MS=max=2500 play for random time between 0 s and 2.5 s\n"
    OPTION_LONG_S "len min=1000:max=2500 would play for\n"
    "random time between 1 s and 2.5 s"
  },
  {
    "bpm", 1, 0, QUH_BPM,
    "BPM", "define BPM (Beats Per Minute) of playback\n"
    "exactly like " OPTION_LONG_S "len but takes BPM instead of MS\n"
    "and turns them into MS internally"
  },
  {
    "shuffle", 0, 0, QUH_SHUFFLE,
    NULL, "shuffle song-list before playing"
  },
  {
    "random", 0, 0, QUH_RANDOM,
    NULL, "random choice from song-list while playing"
  },
#if 0
  {
    "keep", 0, 0, QUH_KEEP,
    NULL, "use this together with " OPTION_LONG_S "shuffle or " OPTION_LONG_S "random\n"
    "to keep files that belong to the same serie together"
  },
  {
    "keep2", 0, 0, QUH_KEEP2,
    NULL, "like " OPTION_LONG_S "keep but respect files\n"
    "that belong to the same sub-directory"
  },
#endif
  {
    "loop", 2, 0, QUH_LOOP,
    "N", "loops song playback N times (default: 0 (forever))"
  },
  {
    "intro", 0, 0, QUH_INTRO,
    NULL, "play only 10 s of every song\n"
    "same as " OPTION_LONG_S "start=60000 and " OPTION_LONG_S "len=10000"
  },
#if 0
  {
    "rr", 2, 0, QUH_RR,
    "MAX[:MIN]", "Random Radio (idea by Benno Hoelle)\n"
    "MAX segment length in ms (default: 1500)\n"
    "MIN segment length in ms (default: 500)\n"
    "same as " OPTION_LONG_S "start=min=0:max=eof " OPTION_LONG_S "len=min=MIN:max=MAX\n"
    OPTION_LONG_S "random " OPTION_LONG_S "loop"
  },
#endif
  {
    "R", 0, 0, QUH_R,
    NULL, "play subdirectories recursively"
  },
  {
    "q", 0, 0, QUH_Q,
    NULL, "be quiet"
  },
  {
    "v", 0, 0, QUH_V,
    NULL, "be more verbose"
  },
  {
    "version", 0, 0, QUH_VER,
    NULL, "output version information and exit"
  },
  {
    "ver", 0, 0, QUH_VER,
    NULL, NULL
  },
  {
    "s", 0, 0, QUH_S,
    NULL, "output supported formats information and exit"
  },
  {
    "help", 0, 0, QUH_HELP,
    NULL, "display this help and exit"
  },
  {
    "h", 0, 0, QUH_HELP,
    NULL, NULL
  },
  {NULL, 0, 0, 0, NULL, NULL}
};


const st_getopt2_t quh_options_usage_bottom[] =
{
  {
    NULL, 0, 0, 0,
    NULL, "Please report any problems/ideas/fixes to noisyb@gmx.net or surf to\n"
          "                                          http://quh.berlios.de"
  },
  {NULL, 0, 0, 0, NULL, NULL}
};
