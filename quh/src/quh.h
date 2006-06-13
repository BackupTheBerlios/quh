/*
quh.h - Quh plays everything that makes noise

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
#ifndef QUH_H
#define QUH_H
#ifdef  __cplusplus
extern "C" {
#endif
#include "misc/cache.h"


typedef struct
{
  int id;                 // id of filetype
  const char *id_s;       // id of filetype as a string

  const char *fname;
  const st_filter_t *f;         // decode filter

  int source;             // QUH_SOURCE_FILE, QUH_SOURCE_DEVICE, QUH_SOURCE_STREAM
  int seekable;           // !seekable == ignore raw_size, raw_pos, start and indices
  int expanding;          // if the input gets bigger while playing it

//  unsigned long fsize;  // (Bytes) total size of (compressed) file  if (seekable && !expanding)
  unsigned long raw_size; // (Bytes) total size of (uncompressed) file

  // indices (raw)
  unsigned long index_pos[QUH_MAXINDEX]; // (Bytes (uncompressed)) cdda:tracks, sid:tunes, etc..
  int indices;            // # of indices
  char index_name[QUH_MAXINDEX][QUH_INDEX_NAME_LEN];

  int rate;               // default: 44100
  int channels;           // default: 2 (stereo)
  int size;               // default: 2 (16 bit)
  int is_big_endian;      // default: FALSE
  int is_signed;          // default: TRUE

  unsigned long min_bitrate;
  unsigned long max_bitrate;
} st_quh_nfo_t;


//typedef int32_t quh_sample_t;
typedef unsigned char quh_sample_t;


typedef struct
{
//  int device;  // for SDL, ... default: 0
//  char device_s[FILENAME_MAX]; // default: "/dev/dsp"

  // returned values after st_quh_nfo_t values were set by ioctl (oss) or API
  int rate;
  int channels;
  int size;
//  int is_big_endian;
  int is_signed;
  int vol;                  // volume (in percent)

  // IMPORTANT: max. bytes soundcard can handle at once
  // all soundcard write functions must take care of this
  // if it is smaller than QUH_MAXBUFSIZE
  unsigned long buffer_max;

//  void (*callback) (void);
} st_soundcard_t;


typedef struct
{
  int argc;
  char **argv;

  int pid;

  unsigned long flags;
  int ansi_color;

  int verbose;
  int quiet;

  int quit;                 // quit quh (but save current playlist before)

  int random;
  int loop;                 // loop == 1 for endless; loop > n > 1 for n - 1 times
  int shuffle;
  const char *start_optarg; // start optarg as string
  const char *len_optarg;   // len optarg as string

  unsigned long start;      // start (Bytes) playing of current (uncompressed) file
  unsigned long len;        // play len (Bytes) of current (uncompressed) file
                            //   len == 0  means play until EOF (incl. expanding)

  // input
  char *fname[QUH_MAX_FILES];
  int files;
  int current_file;
  unsigned long raw_pos;  // (Bytes) current pos in (uncompressed) file
  int next_file;
  st_quh_nfo_t demux;

  // filter chain
  int filter_id[FILTER_MAX];
  const char *filter_option[FILTER_MAX];
#if 1
  st_filter_chain_t *filter_chain;
#else
  st_filter_chain_t *fc;
#endif
  st_cache_t *o;      // objects for ALL filters
  
  quh_sample_t buffer[QUH_MAXBUFSIZE];
  unsigned long buffer_len;

  st_soundcard_t soundcard; // soundcard specifications; used by filters
                            // that write to a soundcard hardware

  const char *id3; // used by the id3 filter (for now)

  // config
  char configdir[FILENAME_MAX];
  char configfile[FILENAME_MAX];
//  char skindir[FILENAME_MAX];
  char tmp_file[FILENAME_MAX];
  char cmdline[ARGS_MAX];
} st_quh_t;

extern st_quh_t quh;
extern void quh_exit (void);
  

#ifdef  __cplusplus
}
#endif
#endif // QUH_H
