/*
quh_filter.c - filter for Quh

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
#include <stdlib.h>
#include "misc/cache.h"
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


#ifdef  USE_ALSA
#include "filter/alsa.h"
#endif
#ifdef  USE_APE
#include "filter/ape.h"
#endif
#ifdef  USE_AUDIOFILE
#include "filter/audiofile.h"
#endif
#include "filter/cache.h"
#ifdef  USE_CDDA
#include "filter/cdda.h"
#endif
#ifdef  USE_CDDB
#include "filter/cddb.h"
#endif
//#include "filter/cdxa.h"
#include "filter/console.h"
#include "filter/decode.h"
#include "filter/demux.h"
#ifdef  USE_ESD
#include "filter/esd.h"
#endif
#ifdef  USE_FFMPEG
#include "filter/ffmpeg.h"
#endif
#ifdef  USE_FLAC
#include "filter/flac.h"
#endif
//#include "filter/gbs.h"
//#include "filter/gsf.h"
#ifdef  USE_GYM
#include "filter/gym.h"
#endif
//#include "filter/hes.h"
#ifdef  USE_ID3
#include "filter/id3.h"
#endif
#ifdef  USE_JOYSTICK
#include "filter/joy.h"
#endif
#ifdef  USE_LIBAO
#include "filter/ao.h"
#endif
#ifdef  USE_FLITE
#include "filter/flite.h"
#endif
#ifdef  USE_MAD
#include "filter/mad.h"
#endif
#ifdef  USE_MIKMOD
#include "filter/mikmod.h"
#endif

//#include "filter/mpeg.h"
#ifdef  USE_ST
#include "filter/echo.h"
#endif
#ifdef  USE_XMP
#include "filter/xmp.h"
#endif
//#include "filter/lyrics.h"
//#include "filter/midi.h"
//#include "filter/nsf.h"

#ifdef  USE_OSS
#include "filter/oss.h"
#endif
//#include "filter/psf.h"
#include "filter/raw.h"
#include "filter/read.h"
//#include "filter/remote.h"
#ifdef  USE_SDL
#include "filter/sdl.h"
#endif
#ifdef  USE_SERVER
//#include "filter/server.h"
#endif
#ifdef  USE_SID
#include "filter/sid.h"
#endif
#ifdef  USE_SNDFILE
#include "filter/sndfile.h"
#endif
#ifdef  USE_SNDLIB
#include "filter/sndlib.h"
#endif
//#include "filter/spc.h"
#ifdef  USE_PCSPKR
#include "filter/speaker.h"
#endif
//#include "filter/stdout.h"
//#include "filter/usf.h"
//#include "filter/vgm.h"
#ifdef  USE_OGG
#include "filter/vorbis.h"
#endif
#include "filter/wav.h"
//#include "filter/xmms.h"


const st_filter_t *quh_filter[] = {
#ifdef  USE_GYM
  &quh_gym_in,
#endif
#ifdef  USE_ST
  &quh_echo,
#endif
#ifdef  USE_CDDA
  &quh_cdda_in,
#endif
#ifdef  USE_SPC
  &quh_spc_in,
#endif
#ifdef  USE_FFMPEG
  &quh_ffmpeg_in,
#endif
#ifdef  USE_FLAC
  &quh_flac_in,
#endif
#ifdef  USE_MAD
  &quh_mad_in,
#endif
#ifdef  USE_MIKMOD
  &quh_mikmod_in,
#endif
#ifdef  USE_XMP
  &quh_xmp_in,
#endif
#ifdef  USE_SPLAY_MPEG
  &quh_mpeg_in,
#endif
#ifdef  USE_SID
  &quh_sid_in,
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
  &quh_audiofile_in,
#endif
#ifdef  USE_SNDFILE
  &quh_sndfile_in,
#endif
  &quh_wav_in,
  &quh_raw_in,

#ifdef  USE_ALSA
  &quh_alsa_out,
#endif
  &quh_cache,
#ifdef  USE_CDDA
//  &quh_cdda_out,
#endif
#ifdef  USE_CDDB
  &quh_cddb_in,
#endif
//  &quh_lyrics_in,
  &quh_console,
  &quh_demux,
#ifdef  USE_ESD
  &quh_esd_out,
#endif
#ifdef  USE_ID3
  &quh_id3_in,
#endif 
#ifdef  USE_JOYSTICK
  &quh_joystick,
#endif
#ifdef  USE_LIBAO
  &quh_ao_out,
#endif
#ifdef  USE_OSS
  &quh_oss_out,
#endif
//  &quh_raw_out,
  &quh_read_in,
#ifdef  USE_SDL
  &quh_sdl_out,
#endif
#ifdef  USE_PCSPKR
  &quh_speaker_out,
#endif
//  &quh_stdout_out,
//  &quh_wav_out,
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


const st_getopt2_t *quh_filter_usage[] =
{
  &title,
  &quh_new_usage,
  &quh_cache_usage,
  &quh_demux_usage,
//  &quh_decode_pass_usage,
  &quh_console_usage,
#ifdef  USE_ID3 
  &quh_id3_in_usage,
#endif
#ifdef  USE_CDDB
  &quh_cddb_in_usage,
#endif
//  &quh_lyrics_in_usage,
#ifdef  USE_JOYSTICK 
  &quh_joystick_usage,
#endif
//  &quh_raw_out_usage,
//  &quh_wav_out_usage,
#ifdef  USE_CDDA
//  &quh_cdda_out_usage,
#endif
#ifdef  USE_ST
  &quh_echo_usage,
#endif
//  &quh_stdout_out_usage,
#ifdef  USE_OSS
  &quh_oss_out_usage,
#endif
#ifdef  USE_SDL
  &quh_sdl_out_usage,
#endif
#ifdef  USE_ALSA
  &quh_alsa_out_usage,
#endif
#ifdef  USE_ESD
  &quh_esd_out_usage,
#endif
#ifdef  USE_LIBAO
  &quh_ao_out_usage,
#endif
#ifdef  USE_PCSPKR
  &quh_speaker_out_usage,
#endif
  &lf,
  NULL
};
