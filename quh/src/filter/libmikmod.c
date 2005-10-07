/*
mikmod.c - mikmod support for Quh

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
#include "libmikmod/mikmod.h"
#endif
#include "misc/misc.h"
#include "misc/itypes.h"
#include "misc/file.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "libmikmod.h"


/*
  mikmod is newer but supports only a fraction of the formats the
  stalled libxmp does
*/


static char temp_filename[FILENAME_MAX];
static FILE *fh = NULL;
static int pid = 0;
#define QUH_MIKMOD_RAW_DEVICE 4
#define QUH_MIKMOD_WAV_DEVICE 3


static int
quh_mikmod_open (st_quh_filter_t *file)
{
  int x = 0;
  char buf[MAXBUFSIZE];
#if     MAXBUFSIZE < FILENAME_MAX
  char fname[FILENAME_MAX];
#else
  char fname[MAXBUFSIZE];
#endif
    
  UNIMOD *mf = NULL;
  static BOOL  cfg_extspd  = 1,      /* Extended Speed enable */
               cfg_panning = 1,      /* DMP panning enable (8xx effects) */
               cfg_loop    = 0;      /* auto song-looping disable */
  int cfg_maxchn = 64;

//  tmpnam2 (temp_filename);
  strcpy (temp_filename, "music.raw"); // that's the mikmod default
                                  
  // global mikmod defaults
  md_mixfreq      = 44100;            /* standard mixing freq */
  md_dmabufsize   = 32000;            /* standard dma buf size (max 32000) */
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
  MikMod_RegisterDriver(drv_wav);
  MikMod_RegisterDriver(drv_raw);
  MikMod_RegisterDriver(drv_nos);
  MikMod_Init ();

#if     MAXBUFSIZE < FILENAME_MAX
  strncpy (fname, file->fname, FILENAME_MAX)[FILENAME_MAX - 1] = 0;
#else
  strncpy (fname, file->fname, MAXBUFSIZE)[MAXBUFSIZE - 1] = 0;
#endif
    
  if (!(mf = MikMod_LoadSong (fname, cfg_maxchn)))
    return -1;

  mf->extspd  = cfg_extspd;
  mf->panflag = cfg_panning;
  mf->loop    = cfg_loop;

  Player_Start (mf);

  *buf = 0;
  sprintf (buf, "Module Title: %s\n"
                "Module Type: %s\n"
                "Author: %s\n"
                "Comment: %s\n"
                "Length: %d Patterns\n"
                "Patterns: %d\n"
                "Tracks: %d\n"
                "Instruments: %d\n"
                "Samples: %d\n"
                "Channels: %d",
                mf->songname ? mf->songname : "Untitled",
                mf->modtype ? mf->modtype : "Unknown",
                mf->composer ? mf->composer : "Unknown",
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

  pid = fork ();
  
  if (pid < 0) // failed
    return -1;
        
  if (!pid) // child
    {
      // write temp file

      while (Player_Active() && mf->numpos < 256)
        MikMod_Update ();

      Player_Stop ();          /* stop playing */

      MikMod_FreeSong (mf);            /* and free the module */
      MikMod_Exit();

      exit (0);
    }

  wait2 (500);

  file->rate = md_mixfreq;
  file->size = 2;
  file->channels = 2;
  file->raw_size = fsizeof (temp_filename);
  //  file->is_big_endian = FALSE;
  file->is_signed = TRUE;
  file->seekable = QUH_SEEKABLE;
  file->expanding = TRUE;
          
//  quh_demux_sanity_check (file);
   
  if (!(fh = fopen (temp_filename, "rb")))
    return -1;
          
  return 0;
}


static int
quh_mikmod_close (st_quh_filter_t *file)
{
  (void) file;

  if (pid > 0) // parent kills child
    {
      kill (pid, SIGKILL);
      waitpid (pid, NULL, 0);
    }
                      
  fclose (fh);
  remove (temp_filename);
    
  return 0;
}


int
quh_mikmod_seek (st_quh_filter_t *file)
{
  fseek (fh, quh.raw_pos, SEEK_SET);
  file->raw_size = fsizeof (temp_filename); // expanding
  return 0;
}


int
quh_mikmod_write (st_quh_filter_t *file)
{
  quh.buffer_len = fread (&quh.buffer, 1, QUH_MAXBUFSIZE, fh);
  file->raw_size = fsizeof (temp_filename); // expanding
  return 0;
}


const st_filter_t quh_libmikmod_in = {
  QUH_LIBMIKMOD_IN,
  "mikmod (669, dsm, far, it, med, mod, mtm, s3m, stm, ult, xm)",
  ".it.xm.s3m.mod.mtm.stm.dsm.med.far.ult.669",
  strlen (".it.xm.s3m.mod.mtm.stm.dsm.med.far.ult.669"),
  NULL,
  (int (*) (void *)) &quh_mikmod_open,
  (int (*) (void *)) &quh_mikmod_close,
  NULL,
  (int (*) (void *)) &quh_mikmod_write,
  (int (*) (void *)) &quh_mikmod_seek,
  NULL,
  NULL,
  NULL
};


const st_getopt2_t quh_libmikmod_in_usage =
{
    "mikmod", 1, 0, QUH_LIBMIKMOD,
    "FILE", "FILE is FLAC (if it has no .mikmod suffix)",
    (void *) QUH_LIBMIKMOD_IN
};
