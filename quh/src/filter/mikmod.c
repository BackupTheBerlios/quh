/*
mikmod.c - mikmod support for Quh

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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef  HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>  // waitpid()
#ifdef  USE_MIKMOD
#include <mikmod.h>
#endif
#include "misc/misc.h"
#include "misc/itypes.h"
#include "misc/file.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "mikmod.h"


#define QUH_MIKMOD_RAW_DEVICE 4
#define QUH_MIKMOD_WAV_DEVICE 3


static FILE *fh = NULL;
static MODULE *mf = NULL;
#if     MAXBUFSIZE < FILENAME_MAX
char tmp_file[FILENAME_MAX];
#else
char tmp_file[MAXBUFSIZE];
#endif


static void
quh_mikmod_decode_to_wav (st_quh_nfo_t *file, const char *fname)
{
  (void) file;
  (void) fname;

  while (Player_Active() && mf->numpos < 256)
    MikMod_Update ();

  Player_Stop ();  // stop playing

  Player_Free (mf);            // and free the module
  MikMod_Exit();
}


static int
quh_mikmod_open (st_quh_nfo_t *file)
{
  int x = 0;
  char buf[MAXBUFSIZE];
#if     MAXBUFSIZE < FILENAME_MAX
  char fname[FILENAME_MAX];
#else
  char fname[MAXBUFSIZE];
#endif
  st_wav_header_t wav_header;
  static BOOL  cfg_extspd  = 1,      /* Extended Speed enable */
               cfg_panning = 1,      /* DMP panning enable (8xx effects) */
               cfg_loop    = 0;      /* auto song-looping disable */

  // global mikmod defaults
  md_mixfreq      = 44100;            /* standard mixing freq */
//  md_dmabufsize   = 32000;            /* standard dma buf size (max 32000) */
  md_device       = QUH_MIKMOD_RAW_DEVICE;
  md_volume       = 128;              /* driver volume (max 128) */
  md_musicvolume  = 128;              /* music volume (max 128) */
  md_sndfxvolume  = 128;              /* sound effects volume (max 128) */
  md_pansep       = 128;              /* panning separation (0 = mono, 128 = full stereo) */
/*  md_stereodelay  = 10;               // Stereo Delay (max 15) */
  md_reverb       = 0;                /* Reverb (max 15) */
  md_mode = DMODE_16BITS | DMODE_STEREO | DMODE_SOFT_MUSIC;  /* default mixing mode */
 
  MikMod_RegisterAllLoaders ();
  MikMod_RegisterAllDrivers ();
  MikMod_RegisterDriver(&drv_wav);
//  MikMod_RegisterDriver(&drv_raw);
//  MikMod_RegisterDriver(&drv_stdout);

  // dirty hack...
  strcpy (tmp_file, quh.tmp_file);
  strcpy (quh.tmp_file, "music.wav");

  if (MikMod_Init (""))
    return -1;

#if     MAXBUFSIZE < FILENAME_MAX
  strncpy (fname, file->fname, FILENAME_MAX)[FILENAME_MAX - 1] = 0;
#else
  strncpy (fname, file->fname, MAXBUFSIZE)[MAXBUFSIZE - 1] = 0;
#endif

  if (!(mf = Player_Load (fname, 64, 1)))
    return -1;

  mf->extspd  = cfg_extspd;
  mf->panflag = cfg_panning;
  mf->loop    = cfg_loop;

  Player_Start (mf);

  *buf = 0;
  sprintf (buf, "Module Title: %s\n"
                "Module Type: %s\n"
                "Comment: %s\n"
                "Length: %d Patterns\n"
                "Patterns: %d\n"
                "Tracks: %d\n"
                "Instruments: %d\n"
                "Samples: %d\n"
                "Channels: %d",
                mf->songname ? mf->songname : "Untitled",
                mf->modtype ? mf->modtype : "Unknown",
                mf->comment ? mf->comment : "None",
                mf->numpos + 1,
                mf->numpat,
                mf->numtrk,
                mf->numins,
                mf->numsmp,
                mf->numchn);

  // instrument list
  if (mf->instruments)
    for (x = 0; x < mf->numins && strlen (buf) < MAXBUFSIZE - 40; x++)
      {
        INSTRUMENT *i = &mf->instruments[x];
       
        if (i)
          sprintf (strchr (buf, 0), "\nInstrument[%d]: %-40.40s", x, i->insname ? i->insname : "Unknown");
      }

  quh_set_object_s (quh.filter_chain, QUH_OUTPUT, buf);

  if (!quh_forked_wav_decode (file, quh_mikmod_decode_to_wav))
    return -1;

  if (!(fh = fopen (quh.tmp_file, "rb")))
    return -1;

  fread (&wav_header, 1, sizeof (st_wav_header_t), fh);

  if (!memcmp (wav_header.magic, "RIFF", 4))
    {
      quh.raw_pos = sizeof (st_wav_header_t);
      file->raw_size = fsizeof (quh.tmp_file) - quh.raw_pos;
      file->rate = wav_header.freq;
      file->channels = wav_header.channels;
      file->is_signed = 1;
      file->size = wav_header.bitspersample / 8;
      file->seekable = QUH_SEEKABLE;
      file->expanding = 1;
    }
  else
    return -1;
          
//  quh_demux_sanity_check (file);
   
  return 0;
}


static int
quh_mikmod_close (st_quh_nfo_t *file)
{
  (void) file;

  fclose (fh);

  return 0;
}


static int
quh_mikmod_seek (st_quh_nfo_t *file)
{
  fseek (fh, quh.raw_pos, SEEK_SET);
  file->raw_size = fsizeof (quh.tmp_file); // expanding
  return 0;
}


static int
quh_mikmod_write (st_quh_nfo_t *file)
{
  quh.buffer_len = fread (&quh.buffer, 1, QUH_MAXBUFSIZE, fh);
  file->raw_size = fsizeof (quh.tmp_file); // expanding
  return 0;
}


#if 0
static int
quh_mikmod_demux (st_quh_nfo_t * file)
{
  int result = 0;

  if (file->source != QUH_SOURCE_FILE)
    return -1;

  result = quh_mikmod_open (file);

  if (!result)
    quh_mikmod_close (file);

  return result;
}
#endif


static int
quh_mikmod_quit (st_quh_nfo_t *file)
{
  (void) file;

  remove (quh.tmp_file);
  strcpy (quh.tmp_file, tmp_file);

  return 0;
}


const st_filter_t quh_mikmod_in = {
  QUH_MIKMOD_IN,
  "mikmod (669, amf, dsm, far, imf, it, med, mod, mtm, s3m, stm, ult, xm)",
  ".amf.imf.it.xm.s3m.mod.mtm.stm.dsm.med.far.ult.669",
  -1,
//  (int (*) (void *)) &quh_mikmod_demux,
  NULL,
  (int (*) (void *)) &quh_mikmod_open,
  (int (*) (void *)) &quh_mikmod_close,
  NULL,
  (int (*) (void *)) &quh_mikmod_write,
  (int (*) (void *)) &quh_mikmod_seek,
  NULL,
  NULL,
  (int (*) (void *)) &quh_mikmod_quit
};


const st_getopt2_t quh_mikmod_in_usage =
{
    "mikmod", 1, 0, QUH_MIKMOD,
    "FILE", "FILE is FLAC (if it has no .mikmod suffix)",
    (void *) QUH_MIKMOD_IN
};
