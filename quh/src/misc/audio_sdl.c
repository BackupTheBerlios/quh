/*
sdl_audio.c - even more simple SDL wrapper for audio
  
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
#ifdef  HAVE_CONFIG_H
#include "config.h"
#endif
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#if     (defined USE_SDL)
#include <SDL.h>
#include <SDL_audio.h>
#endif
#include "itypes.h"
#include "bswap.h"
#include "misc.h"
#include "cache.h"
#include "audio_sdl.h"


#ifdef  MAXBUFSIZE
#undef  MAXBUFSIZE
#endif
#define MAXBUFSIZE 32768


#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif


unsigned long
audio_ms_to_bytes (int freq, int bits, int channels, unsigned long ms)
{
  unsigned long value_b = 0;

  if (!ms)
    return 0;

  value_b = (unsigned long) ((float) (freq * bits * channels) / 1000) * ms;
  return value_b - (value_b % bits);         // "fix" value
}


unsigned long
audio_bytes_to_ms (int freq, int bits, int channels, unsigned long bytes)
{
  if (!bytes)
    return 0;

  return (unsigned long) (bytes * ((float) 1000 / (freq * bits * channels)));
}


static int
audio_set_wav_header (st_audio_wav_t *header,
                      int freq,
                      int channels,
                      int bytespersecond,
                      int blockalign,
                      int bitspersample,
                      int data_length)
{
  memset (header, 0, sizeof (st_audio_wav_t));

  strncpy ((char *) header->magic, "RIFF", 4);
  header->total_length =           me2le_32 (data_length + sizeof (st_audio_wav_t) - 8);
  strncpy ((char *) header->type,  "WAVE", 4);
  strncpy ((char *) header->fmt,   "fmt ", 4);
  header->header_length =          me2le_32 (16); // always 16
  header->format =                 me2le_16 (1); // WAVE_FORMAT_PCM == default
  header->channels =               me2le_16 (channels);
  header->freq =                   me2le_32 (freq);
  header->bytespersecond =         me2le_32 (bytespersecond);
  header->blockalign =             me2le_16 (blockalign);
  header->bitspersample =          me2le_16 (bitspersample);
  strncpy ((char *) header->data,  "data", 4);
  header->data_length =            me2le_32 (data_length);

  return 0;
}


#if     (defined USE_SDL)


static st_audio_t *audio = NULL;
static st_cache_t *sdl_rb = NULL;
static unsigned char *stream_p = NULL;
static SDL_AudioSpec set, get;


st_audio_wav_t *
audio_get_wav_header (void)
{
  static st_audio_wav_t header;
  
#warning TODO: take care of files here
  audio_set_wav_header (&header, audio->freq, audio->channels,
                        audio->freq * audio->channels * (audio->bits / 8),
                        0, audio->bits, audio->s_len);

  return &header;
}


static int
rb_callback (void *buffer, unsigned long len)
{
  SDL_MixAudio (stream_p, (unsigned char *) buffer, len, SDL_MIX_MAXVOLUME);
  return 0;
}


static void
callback (void *o, unsigned char *stream, int len)
{
  stream_p = stream;
  cache_read_cb (sdl_rb, rb_callback, len);
}


st_audio_t *
audio_open (int flags)
{
  static st_audio_t a;
  
  memset (&a, 0, sizeof (st_audio_t));

  if (!(SDL_WasInit (SDL_INIT_AUDIO) & SDL_INIT_AUDIO))
    if (SDL_InitSubSystem (SDL_INIT_AUDIO) < 0)
      return NULL;

  if (!(sdl_rb = cache_open (16384, CACHE_MEM|CACHE_FIFO)))
    return NULL;

  a.flags = flags;
  audio = &a;

  return &a;
}


void
audio_close (void)
{
  cache_close (sdl_rb);

  if (SDL_WasInit (SDL_INIT_AUDIO) & SDL_INIT_AUDIO)
    SDL_QuitSubSystem (SDL_INIT_AUDIO);
}


void
audio_read_from_mem (const unsigned char *data, int data_len)
{
  (void) data;
  (void) data_len;
}


void
audio_read_from_file (const char *fname)
{
  if (!fname)
    return;

  if (access (fname, R_OK) != 0)
    return;

//  cache_read ()

//  if (wav)
    {
      st_audio_wav_t header;

      if (audio->fs)
        fclose (audio->fs);
      
      if (!(audio->fs = fopen (fname, "rb")))
        return;
    
      fread (&header, 1, sizeof (st_audio_wav_t), audio->fs);

      audio->freq = header.freq;
      audio->channels = header.channels;
      audio->bits = header.bitspersample;

      audio->start = ftell (audio->fs);
    }

  SDL_PauseAudio (1);

  SDL_CloseAudio ();

  set.freq = audio->freq ? audio->freq : 44100;
  set.channels = audio->channels ? audio->channels : 2;        // 1 = mono, 2 = stereo

  switch (audio->bits)
    {
    case 1:
      set.format = audio->is_signed ? AUDIO_S8 : AUDIO_U8;
      break;

    case 2:
    default:
      set.format = audio->is_signed ? AUDIO_S16SYS : AUDIO_U16SYS;
      break;
    }

  set.samples = 1024;
  set.callback = callback;
  set.userdata = audio;

  if (SDL_OpenAudio (&set, &get) < 0)
    {
      SDL_PauseAudio (0); // don't just freeze
        
      return;
    }

#if 0
  switch (get.format)
    {
      case AUDIO_U8:
        audio->soundcard.bits = 1;
        audio->soundcard.is_signed = 0;
        break;

      case AUDIO_S8:
        audio->soundcard.bits = 1;
        audio->soundcard.is_signed = 1;
        break;

      case AUDIO_U16:
#if     AUDIO_U16 != AUDIO_U16LSB
      case AUDIO_U16LSB:
#endif
      case AUDIO_U16MSB:
#if     AUDIO_U16 != AUDIO_U16SYS
      case AUDIO_U16SYS:
#endif
        audio->soundcard.bits = 2;
        audio->soundcard.is_signed = 0;
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
        audio->soundcard.bits = 2;
        audio->soundcard.is_signed = 1;
        break;
    }
  audio->soundcard.freq = get.freq;
  audio->soundcard.channels = get.channels;
  audio->soundcard.buffer_max = get.bits;
#endif

  SDL_PauseAudio (0);
}
  
  
int
audio_get_channels (void)
{
  return audio->channels;
}
  
  
int
audio_get_bits (void)
{
  return audio->bits;
}
  
  
int
audio_get_freq (void)
{
  return audio->freq;
}
  

void
audio_set_channels (int channels)
{
  audio->channels = channels;
}
  
  
void
audio_set_bits (int bits)
{
  audio->bits = bits;
}
  
  
void
audio_set_freq (int freq)
{
  audio->freq = freq;
}
  

void
audio_ctrl_select (int start, int len)
{
  audio->start = start;
  audio->len = len;
}


void
audio_ctrl_select_ms (int start_ms, int len_ms)
{
  audio->start = audio_ms_to_bytes (audio->freq, audio->bits, audio->channels, start_ms);
  audio->len = audio_ms_to_bytes (audio->freq, audio->bits, audio->channels, len_ms);
}


void
audio_ctrl_select_all (void)
{
  audio->start = 0;
  audio->len = audio->s_len;
}


void
audio_ctrl_volume (int left, int right)
{
  audio->left = left;
  audio->right = right;
}


static void
audio_ctrl_scale ()
// "scale" audio data to fit the soundcard settings
{
}


void
audio_write (void)
{
  int buffer_len;
  unsigned char buffer[MAXBUFSIZE];

  fseek (audio->fs, audio->start, SEEK_SET);
  
  while ((buffer_len = fread (buffer, 1, MAXBUFSIZE, audio->fs)))
    CACHE_WRITE (sdl_rb, buffer, buffer_len);   // write to ring buffer
}


void
audio_write_bg (void)
{
  int pid = fork ();
  
  if (!pid) // child process
    {
      audio_write ();
      exit (0);
    }
}


void
audio_sync (void)
{
  while (cache_sizeof (sdl_rb));
}


#ifdef  TEST
//#if 0
void
test (void)
{
  audio_read_from_file ("audiodump.wav");
  
//  audio_write_bg ();
  audio_write ();

  audio_sync ();

  wait2 (2000);
}


int
main (int argc, char ** argv)
{
  audio_open (0);

  test ();

  audio_close ();

  return 0;
}
#endif  // TEST
#endif  // #if     (defined USE_SDL)
