/*
quh_filter.c - filter for Quh

written by 2005 Dirk (d_i_r_k_@gmx.net)

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
#include <stdlib.h>
#include "misc/itypes.h"
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


#include "filter/alsa.h"
#include "filter/ape.h"
#include "filter/audiofile.h"
#include "filter/cache.h"
#include "filter/cdda.h"
#include "filter/cddb.h"
#include "filter/console.h"
#include "filter/demux.h"
#include "filter/echo.h"
#include "filter/esd.h"
#include "filter/ffmpeg.h"
#include "filter/flac.h"
#include "filter/gui.h"
#include "filter/id3.h"
#include "filter/joy.h"
#include "filter/libao.h"
#include "filter/libflite.h"
#include "filter/libmad.h"
#include "filter/libmikmod.h"
#include "filter/libmpeg.h"
#include "filter/libxmp.h"
#include "filter/lyrics.h"
#include "filter/midi.h"
#include "filter/nsf.h"
#include "filter/oss.h"
#include "filter/pipe.h"
#include "filter/raw.h"
#include "filter/read.h"
#include "filter/remote.h"
#include "filter/sdl.h"
#include "filter/sid.h"
#include "filter/sndfile.h"
#include "filter/sndlib.h"
#include "filter/speaker.h"
#include "filter/stderr.h"
#include "filter/stdin.h"
#include "filter/stdout.h"
#include "filter/vol.h"
#include "filter/vorbis.h"
#include "filter/wav.h"



const st_filter_t *quh_filter_write[] = {
#ifdef  USE_CDDA
  &quh_cdda_in,
#endif
#ifdef  USE_FLAC
  &quh_flac_in,
#endif
#ifdef  USE_MAD
  &quh_libmad_in,
#endif  // USE_MAD
#ifdef  USE_MIKMOD
  &quh_libmikmod_in,
#endif
#ifdef  USE_XMP
  &quh_libxmp_in,
#endif
#ifdef  USE_SPLAY_MPEG
  &quh_libmpeg_in,
#endif
#ifdef  USE_SID
  &quh_sid_in,
#endif
#ifdef  USE_SNDFILE
  &quh_sndfile_in,
#endif
#ifdef  USE_SNDLIB
  &quh_sndlib_in,
#endif
#ifdef  USE_FLITE
  &quh_txt_in,
#endif
#ifdef  USE_OGG
  &quh_vorbis_in,
#endif
#ifdef  USE_AUDIOFILE
//  &quh_audiofile_in,
#endif
  &quh_wav_in,
  &quh_raw_in,

  &quh_alsa_out,
//  &quh_cache,
  &quh_cdda_out,
  &quh_cddb_in,
  &quh_lyrics_in,
  &quh_console,
  &quh_demux,
  &quh_esd_out,
  &quh_id3_in,
  &quh_joystick,
#ifdef  USE_LIBAO
  &quh_libao_out,
#endif
  &quh_oss_out,
  &quh_raw_out,
  &quh_read_in,
#ifdef  USE_SDL
  &quh_sdl_out,
#endif
  &quh_speaker_out,
  &quh_stderr_out,
  &quh_stdin_in,
  &quh_stdout_out,
  &quh_vol,
  &quh_wav_out,
  NULL
};


static const st_getopt2_t title =
{
  NULL, 0, 0, 0,
  NULL, "Filter", NULL
}, quh_new_usage =
{
  "new", 0, 0, QUH_NEW,
  NULL, "start a new filter chain with read and demux, only",
  (void *) QUH_NEW
}, lf =
{
  NULL, 0, 0, 0,
  NULL, "", NULL
};


const st_getopt2_t *quh_filter_write_usage[] =
{
  &title,
  &quh_new_usage,
  &quh_cache_usage,
#ifdef  USE_NETGUI
#endif
#ifdef  USE_GTK
  &quh_gtk_filter_usage,
#endif
#ifdef  USE_FOX
  &quh_fox_filter_usage,
#endif
  &quh_console_usage,
  &quh_console_usage2,
  &quh_id3_in_usage,
  &quh_cddb_in_usage,
  &quh_lyrics_in_usage,
  &quh_joystick_usage,
  &quh_vol_usage,
  &quh_raw_out_usage,
  &quh_wav_out_usage,
  &quh_cdda_out_usage,
  &quh_stdout_out_usage,
  &quh_stderr_out_usage,
  &quh_pipe_usage,
#ifdef  USE_OSS
  &quh_oss_out_usage,
#endif
#ifdef  USE_SDL
  &quh_sdl_out_usage,
#endif  // USE_SDL
#ifdef  USE_ALSA
  &quh_alsa_out_usage,
#endif
#ifdef  USE_ESD
  &quh_esd_out_usage,
#endif
#ifdef  USE_LIBAO
  &quh_libao_out_usage,
#endif
#ifdef  USE_PCSPEAKER
  &quh_speaker_out_usage,
#endif
  &lf,
  NULL
};