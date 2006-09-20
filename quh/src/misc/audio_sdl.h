/*
sdl_audio.h - even more simple SDL wrapper for audio

Copyright (c) 2005 Dirk


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
#ifndef MISC_SDL_AUDIO_H
#define MISC_SDL_AUDIO_H
#ifdef  HAVE_CONFIG_H
#include "config.h"
#endif
#if     (defined USE_SDL)
#include "audio.h"


/*
  audio_sdl_open()

  audio_sdl_close()

  audio_sdl_read_from_mem()
  audio_sdl_read_from_file()
  
  audio_sdl_write()           write sound to soundcard
  audio_sdl_write_bg()        write sound to soundcard in background (uses fork())
                                mixing of simultaneously played sounds is done
                                automatically
  
  audio_sdl_sync()            wait for all background tasks
*/
extern st_audio_t *audio_sdl_open (st_audio_t *a);
extern int audio_sdl_close (st_audio_t *a);

extern int audio_sdl_read_from_mem (st_audio_t *a, const unsigned char *data, int data_len);
extern int audio_sdl_read_from_file (st_audio_t *a, const char *fname);

extern int audio_sdl_write (st_audio_t *a);
extern int audio_sdl_write_bg (st_audio_t *a);

extern void audio_sdl_sync (st_audio_t *a);
#endif  // #if     (defined USE_SDL)
#endif  // MISC_SDL_AUDIO_H
