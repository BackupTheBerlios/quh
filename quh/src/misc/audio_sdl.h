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
  flags

  audio_sdl_close()

  audio_sdl_read_from_mem()
  audio_sdl_read_from_file()
  
  audio_sdl_ctrl_channels()   set/get channels
  audio_sdl_ctrl_bits()       set/get bits
  audio_sdl_ctrl_freq()       set/get freq
  audio_sdl_ctrl_wav()        set/get channels, bits, freq, using a wav header
  audio_sdl_ctrl_vol()        set/get soundcard volume (value: 0...100)
  audio_sdl_ctrl_select()     play only a part of the audio data (in bytes)
  audio_sdl_ctrl_select_ms()  play only a part of the audio data (in ms)
  audio_sdl_ctrl_select_all() play the whole audio data (default)
  
  audio_sdl_write()           write sound to soundcard
  audio_sdl_write_bg()        write sound to soundcard in background (uses fork())
                            mixing of simultaneously played sounds is done
                            automatically
  
  audio_sdl_sync()            wait for all background tasks
*/
extern st_audio_t *audio_sdl_open (int flags);
extern void audio_sdl_close (void);

extern void audio_sdl_read_from_mem (const unsigned char *data, int data_len);
extern void audio_sdl_read_from_file (const char *fname);

extern int audio_sdl_ctrl_channels (int *channels);
extern int audio_sdl_ctrl_bits (int *bits);
extern int audio_sdl_ctrl_freq (int *freq);
extern st_audio_wav_t *audio_sdl_ctrl_wav (st_audio_wav_t *wav);
extern int audio_sdl_ctrl_vol (int *vol);
extern void audio_sdl_ctrl_select (int start, int len);
//extern void audio_sdl_ctrl_select_ms (int start_ms, int len_ms);
extern void audio_sdl_ctrl_select_all (void);

extern void audio_sdl_write (void);
extern void audio_sdl_write_bg (void);

extern void audio_sdl_sync (void);
#endif  // #if     (defined USE_SDL)
#endif  // MISC_SDL_AUDIO_H
