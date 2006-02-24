/*
quh_defines.h - definitions for Quh

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
#ifndef QUH_DEFINES_H
#define QUH_DEFINES_H
#ifdef  HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef  __cplusplus
extern "C" {
#endif

#define MAXBUFSIZE 32768

#define QUH_VERSION_S "0.3.0"
#define QUH_VERSION 015

#define ARGS_MAX 128
#define QUH_MAX_ARGS ARGS_MAX

#define QUH_MEGABYTE (1024*1024)

#define QUH_MAX_FILES 999
#define QUH_MAX_VALUES 16
#define QUH_MAXINDEX 99

#define QUH_MAXBUFSIZE (QUH_MEGABYTE/16)

#define QUH_OPTION 1000
#define QUH_INPUT 2000
#define QUH_OUTPUT 3000
#define QUH_FILTER 4000
#define QUH_OBJECT 5000

#define QUH_GETOPT_ERROR ('?')     //getopt() returns 0x3f when a unknown option was given

// version of config file layout
#define QUH_CONFIG_VERSION 3


#define QUH_RECURSIVE (1 << 0)


#define QUH_SOURCE_FILE   (1 << 0)
#define QUH_SOURCE_DEVICE (1 << 1)
#define QUH_SOURCE_STREAM (1 << 2)


enum {
  QUH_NOT_SEEKABLE = 0,
  QUH_SEEKABLE,
  QUH_SEEKABLE_FORWARD_ONLY,
  QUH_SEEKABLE_BACKWARD_ONLY,
  QUH_SEEKABLE_CACHED
};


#if     QUH_MAXINDEX > 999
#define QUH_INDEX_COUNTER_S "%04d"
#elif   QUH_MAXINDEX > 99
#define QUH_INDEX_COUNTER_S "%03d"
#elif   QUH_MAXINDEX > 9
#define QUH_INDEX_COUNTER_S "%02d"
#else
#define QUH_INDEX_COUNTER_S "%01d"
#endif


enum {
  QUH_ALSA = QUH_OPTION + 1,
  QUH_B,
  QUH_BPM,
  QUH_C,
  QUH_CACHE,
  QUH_CDDA,
  QUH_CDDB,
  QUH_CONVERT,
  QUH_DEC,
  QUH_DEBUG,
  QUH_DEMUX,
  QUH_ECHO,
  QUH_ESD,
  QUH_EXAMPLE,
  QUH_FLAC,
  QUH_GEN,
  QUH_HELP,
  QUH_ID3,
  QUH_INTRO,
  QUH_JOY,
  QUH_LEN,
  QUH_LIBAO,
  QUH_LIRC,
  QUH_LOOP,
  QUH_MP3,
  QUH_MS,
  QUH_NEW,
  QUH_CONSOLE,
  QUH_OGG,
  QUH_OSS,
  QUH_PIPE,
  QUH_Q,
  QUH_R,
  QUH_RANDOM,
  QUH_RANDOM2,
  QUH_RANDOM3,
  QUH_RAW,
  QUH_RR,
  QUH_S,
  QUH_SDL,
  QUH_SET,
  QUH_SHUFFLE,
  QUH_SHUFFLE2,
  QUH_SHUFFLE3,
  QUH_SPEAKER,
  QUH_ST,
  QUH_START,
  QUH_STDERR,
  QUH_STDIN,
  QUH_STDOUT,
  QUH_TXT,
  QUH_URL,
  QUH_V,
  QUH_VER,
  QUH_VOL,
  QUH_WAV,
  QUH_XMP,
  QUH_ZERO,
  QUH_669,
  QUH_SNDFILE,
  QUH_AUDIOFILE,
  QUH_LIBMIKMOD,
  QUH_LYRICS
};


enum {
  QUH_AUDIOFILE_IN = QUH_FILTER + 1,
  QUH_CDDA_IN,
  QUH_CDDB_IN,
  QUH_FLAC_IN,
  QUH_LIBMIKMOD_IN,
  QUH_LIBMAD_IN,
  QUH_LIBMPEG_IN,
  QUH_VORBIS_IN,
  QUH_RAW_IN,
  QUH_SID_IN,
  QUH_SNDFILE_IN,
  QUH_SNDLIB_IN,
  QUH_ST_IN,
  QUH_STDIN_IN,
  QUH_TXT_IN,
  QUH_UNKNOWN_IN,
  QUH_WAV_IN,
  QUH_LIBXMP_IN,

  QUH_ALSA_OUT,
  QUH_CACHE_PASS,
  QUH_CDDA_OUT,
  QUH_ESD_OUT,
  QUH_JOY_PASS,
  QUH_LIBAO_OUT,
  QUH_LIRC_OUT,
  QUH_NEW_OUT,
  QUH_CONSOLE_PASS,
  QUH_OSS_OUT,
  QUH_PIPE_PASS,
  QUH_RAW_OUT,
  QUH_SDL_OUT,
  QUH_SET_OUT,
  QUH_SPEAKER_OUT,
  QUH_LIBST_PASS,
  QUH_STDERR_OUT,
  QUH_STDOUT_OUT,
  QUH_WAV_OUT,
  QUH_VOL_PASS,
  QUH_ID3_IN,
  QUH_READ_IN,
  QUH_DEMUX_PASS,
  QUH_LYRICS_IN,
  QUH_DECODE_PASS,
  QUH_ECHO_PASS
};


#ifdef  __cplusplus
}
#endif
#endif  // QUH_DEFINES_H
