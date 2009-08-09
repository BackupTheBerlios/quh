/*
sdl.c - sdl support for Quh (using libsdl)

written by 2004 Dirk (d_i_r_k_@gmx.net)

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
#include <stdlib.h>
#include "misc/itypes.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "cache.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#ifdef  USE_SDL
#include <SDL.h>
#include <SDL_audio.h>
#endif
#include "sdl.h"


static int inited = 0;
static st_cache_t *sdl_rb = NULL;
static unsigned char *stream_p = NULL;


int
rb_callback (void *buffer, unsigned long len)
{
  SDL_MixAudio (stream_p, (unsigned char *) buffer, len, SDL_MIX_MAXVOLUME);
  return 0;
}


static void
callback (void *o, unsigned char *stream, int len)
{
  (void) o;
  stream_p = stream;
  cache_read_cb (sdl_rb, rb_callback, len);
}


int
quh_sdl_out_quit (st_quh_nfo_t *file)
{
  (void) file;

  if (SDL_WasInit (SDL_INIT_AUDIO) & SDL_INIT_AUDIO)
    SDL_QuitSubSystem (SDL_INIT_AUDIO);

  if (sdl_rb)
    cache_close (sdl_rb);

  return 0;
}


int
quh_sdl_out_open (st_quh_nfo_t *file)
{
  (void) file;
  if (!inited)
    if (!(SDL_WasInit (SDL_INIT_AUDIO) & SDL_INIT_AUDIO))
      if (SDL_InitSubSystem (SDL_INIT_AUDIO) < 0)
        return -1;
  inited = 1;

  return 0;
}


int
quh_sdl_out_ctrl (st_quh_nfo_t *file)
{
  SDL_AudioSpec spec;
  SDL_AudioSpec obtained;
  
  SDL_PauseAudio (1);

  SDL_CloseAudio ();

  spec.freq = file->rate ? file->rate : 44100;
  spec.channels = file->channels ? file->channels : 2;        // 1 = mono, 2 = stereo

  spec.format = AUDIO_S16SYS;
  switch (file->size)
    {
    case 1:
      spec.format = file->is_signed ? AUDIO_S8 : AUDIO_U8;
      break;

    case 2:
    default:
      spec.format = file->is_signed ? AUDIO_S16SYS : AUDIO_U16SYS;
      break;
    }

  spec.samples = 1024;
  spec.callback = callback;
  spec.userdata = file;

  if (SDL_OpenAudio (&spec, &obtained) < 0)
    {
      SDL_PauseAudio (0); // don't just freeze
        
      return -1;
    }

  switch (obtained.format)
    {
      case AUDIO_U8:
        quh.soundcard.size = 1;
        quh.soundcard.is_signed = 0;
        break;

      case AUDIO_S8:
        quh.soundcard.size = 1;
        quh.soundcard.is_signed = 1;
        break;

      case AUDIO_U16:
#if     AUDIO_U16 != AUDIO_U16LSB
      case AUDIO_U16LSB:
#endif
      case AUDIO_U16MSB:
#if     AUDIO_U16 != AUDIO_U16SYS
      case AUDIO_U16SYS:
#endif
        quh.soundcard.size = 2;
        quh.soundcard.is_signed = 0;
        break;

      case AUDIO_S16:
#if     AUDIO_S16 != AUDIO_S16LSB
      case AUDIO_S16LSB:
#endif
      case AUDIO_S16MSB:
#if     AUDIO_S16 != AUDIO_S16SYS
      case AUDIO_S16SYS:
#endif
      default:
        quh.soundcard.size = 2;
        quh.soundcard.is_signed = 1;
        break;
    }
  quh.soundcard.rate = obtained.freq;
  quh.soundcard.channels = obtained.channels;
  quh.soundcard.buffer_max = obtained.size;

  SDL_PauseAudio (0);

  return 0;
}


int
quh_sdl_out_init (st_quh_nfo_t *file)
{
  (void) file;
        
  if (!(sdl_rb = cache_open (16384, CACHE_MEM|CACHE_FIFO)))
    return -1;

  return 0;
}


int
quh_sdl_out_write (st_quh_nfo_t *file)
{
  (void) file;

  // write to ring buffer
  if (CACHE_WRITE (sdl_rb, quh.buffer, quh.buffer_len) == -1)
    return -1;
        
  return 0;
}


QUH_FILTER_FUNC_STUB (quh_sdl_out_seek)
QUH_FILTER_FUNC_STUB (quh_sdl_out_close)
QUH_FILTER_OUT(quh_sdl_out, QUH_SDL_OUT, "sdl (audio)")


const st_getopt2_t quh_sdl_out_usage =
{
    "sdl", 0, 0, QUH_SDL,
    NULL, "write to soundcard using SDL"
//    "OUT=0 lineout (default)\n"
//    "OUT=1 headphones"
};
