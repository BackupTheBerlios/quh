/*
spc.c - Super Nintendo/SPC support for Quh

Copyright (c) 2006 NoisyB


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
#ifdef  USE_OPENSPC
#ifdef  HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <OpenSPC.h>
#include "misc/itypes.h"
#include "misc/bswap.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "wav.h"


static char fn2[64];
static FILE *wavtmp;
static int wavcnt, freq, stereo, bit8;
static int wavout = 0, pipein = 0, pipeout = 0;


// this function is needed by libOpenSPC
void Shutdown(void)
{
}


static int
quh_spc_in_open (st_quh_nfo_t *file)
{
  int urate;                    // int cursx,cursy;
  int seconds, limit, nlimit, ntime, done, voices, v;
  int i;

  SNDmix = 0;
  urate = 100;                  // Default 100Hz update rate
  freq = 44100;                 // Default 44100Hz freq (WAV only)
  stereo = 1;                   // Default stereo
  bit8 = 0;                     // Default 8 bit
  ITrows = 200;                 // Default 200 rows/pattern (IT only)
  limit = 0;                    // Default no time limit
  nlimit = 5;                   // Default 5s null-time limit
  voices = 0;                   // Default no voices disabled

  if (SPCInit (file->fname))            //Reset SPC and load state
    {
      fprintf (stderr, "Failed to initialize emulation!\n");
      return -1;
    }
  if (SNDInit (freq, urate, 1, stereo, bit8))
    {
      fprintf (stderr, "Init Sound failed!\n");
      return -1;
    }
  SNDvmask = ~voices;
//  if (SPCname[0] || SPCtname[0] || SPCdname[0] || SPCcomment[0] || SPCdate[0])
    {
      printf ("ID info:\n");
      printf ("        Song name:              %s\n", SPCname);
      printf ("        Game name:              %s\n", SPCtname);
      printf ("        Person who Dumped SPC:  %s\n", SPCdname);
      printf ("        Comments:               %s\n", SPCcomment);
      printf ("        Dumped on date:         %s\n", SPCdate);
    }
#if 0
  seconds = SNDratecnt = done = 0;
  SNDNoteOn (SPC_DSP_Buffer[0x4c]);     //Turn on voices that were on when saved
  SNDcrc = 0;
  ntime = 0;
  while (!done)
    {
      SNDMix ();
      if (ITdump)
        {
          if (ITUpdate ())
            break;
        }
      if (SNDmix)
        WAVUpdate ();

      if (SNDratecnt >= urate)
        {
          if (tty_getchar () == 32)
            done = 1;
          SNDratecnt -= urate;
          seconds++;
          if (SNDcrc == 0)
            {
              ntime++;
            }
          else
            {
              ntime = 0;
            }
          //                        gotoxy(cursx,cursy);
          if (nlimit && (ntime == nlimit))
            done = 1;
          if (limit)
            {
              if (seconds == limit)
                done = 1;
              fprintf (stderr, "%3d%%\r", seconds * 100 / limit);
            }
          else
            fprintf (stderr, "%2d:%02d\r", seconds / 60, seconds % 60);
          //                        fflush(stdout);
          SNDcrc = 0;
        }
    }
  Shutdown ();
  return (0);
#endif



#if 0
  st_wav_header_t wav_header;

  wav_out = fopen (file->fname, "rb");

  fread (&wav_header, 1, sizeof (st_wav_header_t), wav_out);
  
  if (!memcmp (wav_header.magic, "RIFF", 4))
    {
      quh.raw_pos = sizeof (st_wav_header_t);
      file->raw_size = fsizeof (file->fname) - quh.raw_pos;
      file->rate = wav_header.freq;
      file->channels = wav_header.channels;
      file->is_signed = 1;
      file->size = wav_header.bitspersample / 8;
      file->seekable = QUH_SEEKABLE;
      
      return 0; 
    }

  return -1;
#endif
  return 0;
}


static int
quh_spc_in_close (st_quh_nfo_t *file)
{
  (void) file;

//  fclose (wav_out);

  return 0;
}


static int
quh_spc_in_seek (st_quh_nfo_t *file)
{
  (void) file;
#if 0
  // skip wav header
  quh.raw_pos = MAX (sizeof (st_wav_header_t), quh.raw_pos);

  fseek (wav_out, quh.raw_pos, SEEK_SET);
#endif
  return 0;
}


static int
quh_spc_in_demux (st_quh_nfo_t *file)
{
#if 0
  int result = 0;

  if (file->source != QUH_SOURCE_FILE)
    return -1;

  result = quh_wav_in_open (file);
  
  if (!result)
    quh_wav_in_close (file);
    
  return result;
#endif
  return 0;
}


static int
quh_spc_in_write (st_quh_nfo_t *file)
{
  (void) file;

//  quh.buffer_len = fread (&quh.buffer, 1, QUH_MAXBUFSIZE / 4, wav_out);

  return 0;
}


const st_filter_t quh_spc_in =
{
  QUH_SPC_IN,
  "spc",
  ".spc",
  -1,
  (int (*) (void *)) &quh_spc_in_demux,
  (int (*) (void *)) &quh_spc_in_open,
  (int (*) (void *)) &quh_spc_in_close,
  NULL,
  (int (*) (void *)) &quh_spc_in_write,
  (int (*) (void *)) &quh_spc_in_seek,
  NULL,
  NULL,
  NULL
};


const st_getopt2_t quh_spc_in_usage =
{
    "spc", 1, 0, QUH_SPC,
    "FILE", "FILE is SPC (if it has no .wav suffix)",
    (void *) QUH_SPC_IN
};

#endif  // #ifdef  USE_OPENSPC
