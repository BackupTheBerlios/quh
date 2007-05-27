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
#ifdef  USE_SDL
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL.h>
#include <SDL_audio.h>
#include "defines.h"
#include "itypes.h"
#include "misc.h"
#include "cache.h"
#include "audio_sdl.h"


#ifdef  MAXBUFSIZE
#undef  MAXBUFSIZE
#endif
#define MAXBUFSIZE 32768


st_audio_t *p = NULL;


#if 0
static int
audio_init_wavheader (st_audio_wav_t *header,
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
#endif


static int
rb_callback (void *buffer, unsigned long len)
{
printf ("rp_callback()->len: %ld\n", len);
  SDL_MixAudio (p->stream, (unsigned char *) buffer, len, SDL_MIX_MAXVOLUME);
  return 0;
}


static void
callback (void *a, unsigned char *stream, int len)
{
  p = (st_audio_t *) &a;
  p->stream = stream;
printf ("callback()->len: %d\n", len);

  cache_read_cb (p->rb, rb_callback, len);
}


st_audio_t *
audio_open (int flags)
{
  static st_audio_t a;

  if (!(flags & AUDIO_SDL))
    return NULL;
  
  memset (&a, 0, sizeof (st_audio_t));

  a.flags = flags;

  if (!(SDL_WasInit (SDL_INIT_AUDIO) & SDL_INIT_AUDIO))
    if (SDL_InitSubSystem (SDL_INIT_AUDIO) < 0)
      return NULL;

  if (!(a.rb = cache_open (16384, CACHE_MEM|CACHE_FIFO)))
    return NULL;

  return &a;
}


int
audio_close (st_audio_t *a)
{
  cache_close (a->rb);

  if (SDL_WasInit (SDL_INIT_AUDIO) & SDL_INIT_AUDIO)
    SDL_QuitSubSystem (SDL_INIT_AUDIO);

  return 0;
}


int
audio_read_from_mem (st_audio_t *a, const unsigned char *data, int data_len)
{
  (void) data;
  (void) data_len;
  return 0;
}


int
audio_read_from_file (st_audio_t *a, const char *fname)
{
  if (!fname)
    return -1;

  if (access (fname, R_OK) != 0)
    return -1;

  if (a->fh)
    fclose (a->fh);
      
  if (!(a->fh = fopen (fname, "rb")))
    return -1;
    
  fread (&a->wav, 1, sizeof (st_audio_wav_t), a->fh);

  // check magic
#if 0
  if (!memcmp (a->wav.magic, "RIFF", 4) ||
      !memcmp (a->wav.fmt, "WAVE", 4))
    {
      fputs ("Not a WAV file\n", stderr);
      return -1;
    }
#endif

  a->freq = a->wav.freq;
  a->channels = a->wav.channels;
  a->bits = a->wav.bitspersample;

  a->start = ftell (a->fh);

  if (a->freq)
    a->set.freq = a->freq;
  else
    a->set.freq = 44100; // default

  if (a->channels)
    a->set.channels = a->channels;
  else
    a->set.channels = 2;  // default; 1 = mono, 2 = stereo

  switch (a->bits)
    {
    case 1:
      a->set.format = a->is_signed ? AUDIO_S8 : AUDIO_U8;
      break;

    case 2:
    default:
      a->set.format = a->is_signed ? AUDIO_S16SYS : AUDIO_U16SYS;
      break;
    }

  a->set.samples = 1024;
  a->set.callback = callback;
  a->set.userdata = a;

  if (SDL_OpenAudio (&a->set, &a->get) < 0)
    {
      SDL_PauseAudio (0); // don't just freeze
        
      return -1;
    }

#if 0
  switch (a->get.format)
    {
      case AUDIO_U8:
        a->soundcard.bits = 1;
        a->soundcard.is_signed = 0;
        break;

      case AUDIO_S8:
        a->soundcard.bits = 1;
        a->soundcard.is_signed = 1;
        break;

      case AUDIO_U16:
#if     AUDIO_U16 != AUDIO_U16LSB
      case AUDIO_U16LSB:
#endif
      case AUDIO_U16MSB:
#if     AUDIO_U16 != AUDIO_U16SYS
      case AUDIO_U16SYS:
#endif
        a->soundcard.bits = 2;
        a->soundcard.is_signed = 0;
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
        a->soundcard.bits = 2;
        a->soundcard.is_signed = 1;
        break;
    }
  a->soundcard.freq = a->get.freq;
  a->soundcard.channels = a->get.channels;
//  a->soundcard.buffer_max = a->get.bits;
#endif

//  SDL_PauseAudio (1);

//  SDL_CloseAudio ();
//  SDL_PauseAudio (0);
//  cache_read ()

  return 0;
}
  
  
int
audio_write (st_audio_t *a)
{
  int buffer_len;
  unsigned char buffer[MAXBUFSIZE];

  if (!a->fh)
    return -1;

  fseek (a->fh, a->start, SEEK_SET);
  
  while ((buffer_len = fread (buffer, 1, MAXBUFSIZE, a->fh)))
{
printf ("%d\n", buffer_len);
    CACHE_WRITE (a->rb, buffer, buffer_len);   // write to ring buffer
}
  return 0;
}


int
audio_write_bg (st_audio_t *a)
{
  int pid = fork ();
  
  if (!pid) // child process
    {
      audio_write (a);
      exit (0);
    }

  return 0;
}


void
audio_sync (st_audio_t *a)
{
  while (cache_sizeof (a->rb));
}


#ifdef  TEST
//#if 0
int
main (int argc, char ** argv)
{
  st_audio_t *a = audio_open (AUDIO_SDL);

  if (audio_read_from_file (a, "audiodump.wav") == -1)
    return -1;

  audio_ctrl_select_all (a);

//  audio_write_bg (a);
  audio_write (a);

  audio_sync (a);

  wait2 (2000);

  audio_close (a);

  return 0;
}
#endif  // TEST


#endif  // #ifdef  USE_SDL

