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


extern unsigned long audio_ms_to_bytes (int freq, int bits, int channels, unsigned long ms);
extern unsigned long audio_bytes_to_ms (int freq, int bits, int channels, unsigned long bytes);


typedef struct
{
  uint8_t magic[4];       // 'RIFF'
  uint32_t total_length;  // length of file minus the 8 byte riff header

  uint8_t type[4];        // 'WAVE'

  uint8_t fmt[4];         // 'fmt '
  uint32_t header_length; // length of format chunk minus 8 byte header
  uint16_t format;        // identifies WAVE_FORMAT_PCM
  uint16_t channels;
  uint32_t freq;          // samples per second per channel
  uint32_t bytespersecond;       
  uint16_t blockalign;    // basic block size
  uint16_t bitspersample;

  // PCM formats then go straight to the data chunk
  uint8_t data[4];        // 'data'
  uint32_t data_length;   // length of data chunk minus 8 byte header
} st_audio_wav_t;


#if     (defined USE_SDL)


enum {
  AUDIO_SDL = 1,
  AUDIO_AO,
  AUDIO_SPEAKER
};


typedef struct
{
  int flags;
  
  int freq;
  int bits;
  int channels;
  int is_signed;

  // mem
  void *s;
  int s_len;
  // stream
  FILE *fs;

  int start;
  int len;

  // volume
  int left;
  int right;
} st_audio_t;


/*
  audio_open()
  flags
    AUDIO_SDL             use SDL_audio
    AUDIO_AO              use libao
    AUDIO_SPEAKER         use PC spreaker

  audio_close()

  audio_read_from_mem()
  audio_read_from_file()
  
  audio_ctrl_channels()   set/get channels
  audio_ctrl_bits()       set/get bits
  audio_ctrl_freq()       set/get freq
  audio_ctrl_wav()        set/get channels, bits, freq, using a wav header
  audio_ctrl_vol()        set/get soundcard volume (value: 0...100)
  audio_ctrl_select()     play only a part of the audio data (in bytes)
  audio_ctrl_select_ms()  play only a part of the audio data (in ms)
  audio_ctrl_select_all() play the whole audio data (default)
  
  audio_write()           write sound to soundcard
  audio_write_bg()        write sound to soundcard in background (uses fork())
                            mixing of simultaneously played sounds is done
                            automatically
  
  audio_sync()            wait for all background tasks
*/
extern st_audio_t *audio_open (int flags);
extern void audio_close (void);

extern void audio_read_from_mem (const unsigned char *data, int data_len);
extern void audio_read_from_file (const char *fname);

extern int audio_ctrl_channels (int *channels);
extern int audio_ctrl_bits (int *bits);
extern int audio_ctrl_freq (int *freq);
extern st_audio_wav_t *audio_ctrl_wav (st_audio_wav_t *wav);
extern int audio_ctrl_vol (int *vol);
extern void audio_ctrl_select (int start, int len);
extern void audio_ctrl_select_ms (int start_ms, int len_ms);
extern void audio_ctrl_select_all (void);

extern void audio_write (void);
extern void audio_write_bg (void);

extern void audio_sync (void);
#endif  // #if     (defined USE_SDL)
#endif  // MISC_SDL_AUDIO_H