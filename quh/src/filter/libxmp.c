/*
xmp.c - xmp support for Quh (using libxmp)

Copyright (c) 2005 NoisyB


using Extended Module Player (the library)
         Copyright (C) 1996-2001 Claudio Matsuoka and Hipolito Carraro Jr
 

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
#ifdef  HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>  // waitpid()
#include <sys/stat.h>
#include "misc/itypes.h"
#include "misc/misc.h"
#include "misc/file.h"
#include "misc/getopt2.h"
#include "misc/string.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "xmp.h"
#include "libxmp.h"


static char temp_filename[FILENAME_MAX];
static FILE *fh = NULL;
static int pid = 0;


static int
quh_libxmp_open (st_quh_filter_t *file)
{
  struct xmp_control opt;
  struct xmp_module_info mi;
//  struct xmp_fmt_info *fmt = NULL;
//  struct xmp_drv_info *drv = NULL;
  char buf[MAXBUFSIZE];
#if     MAXBUFSIZE < FILENAME_MAX
  char fname[FILENAME_MAX];
#else
  char fname[MAXBUFSIZE];
#endif

  xmp_init (0, NULL, &opt);

  opt.drv_id = "file";
  opt.outfile = tmpnam2 (temp_filename);
  opt.verbose = quh.verbose;

  xmp_open_audio (&opt);

#if     MAXBUFSIZE < FILENAME_MAX
  strncpy (fname, file->fname, FILENAME_MAX)[FILENAME_MAX - 1] = 0;
#else
  strncpy (fname, file->fname, MAXBUFSIZE)[MAXBUFSIZE - 1] = 0;
#endif

  if (xmp_load_module (fname) < 0)
    return -1;

  file->rate = opt.memavl ? 0 : opt.freq;
  file->size = opt.resol ? opt.resol / 8 : 1 /* U_LAW */;
  file->channels = opt.outfmt & XMP_FMT_MONO ? 1 : 2;
  file->seekable = QUH_SEEKABLE;
  file->expanding = 1;
      
  xmp_get_module_info (&mi);
//  fmt = xmp_get_fmt_info (&fmt);
//  drv = xmp_get_drv_info (&drv);
  *buf = 0;
  sprintf (buf, "Module Title: %s\n"
                "Module Type: %s\n"
                "Length: %d Patterns\n"
                "Patterns: %d\n"
                "Tracks: %d\n"
                "Instruments: %d\n"
                "Samples: %d\n"
                "Channels: %d",
                mi.name,
                mi.type,
                mi.len,
                mi.pat,
                mi.trk,
                mi.ins,
                mi.smp,
                mi.chn);

  quh_set_object_s (quh.filter_chain, QUH_OUTPUT, buf);

  pid = fork ();

  if (pid < 0) // failed
    return -1;

  if (!pid) // child
    {
      // write temp file
      xmp_play_module ();

      xmp_close_audio ();
      
      exit (0);
    }

//  while (!(fh = fopen (temp_filename, "rb")));
//  while (access (temp_filename, F_OK) != 0);
  wait2 (1500);

  file->raw_size = fsizeof (temp_filename);
//  file->is_big_endian = 0;
  file->is_signed = 1;
    
//  quh_demux_sanity_check (file);

  if (!(fh = fopen (temp_filename, "rb")))
    return -1;

  return 0;
}


static int
quh_libxmp_close (st_quh_filter_t *file)
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


static int
quh_libxmp_demux (st_quh_filter_t *file)
{
  int result = -1;
  
  if (file->source != QUH_SOURCE_FILE)
    return -1;

  result = quh_libxmp_open (file);

  if (!result)
    quh_libxmp_close (file);

  return result;
}


int
quh_libxmp_seek (st_quh_filter_t *file)
{
  fseek (fh, quh.raw_pos, SEEK_SET);
  file->raw_size = fsizeof (temp_filename); // expanding
  return 0;
}


int
quh_libxmp_write (st_quh_filter_t *file)
{
  quh.buffer_len = fread (&quh.buffer, 1, QUH_MAXBUFSIZE, fh);
  file->raw_size = fsizeof (temp_filename); // expanding
  return 0;
}


/*
The current version recognizes the following module formats:

  Type  Rating	Tracker/packer		Recognized versions

  Protracker and variants

  CHN	****+	Fast/Taketracker	xCHN, xxCH
  EXO	*** 	Startrekker/ADSC	FLT4/8, EXO4/8 (AM unsupported)
  M15	****	Soundtracker		2.2 (UST unsupported)
  MOD	****+	Sound/Noise/Protracker	M.K., M!K!, M&K!, N.T., CD81
  WOW	****+	Mod's Grave		M.K.

  Packed modules

  AC1D  ****+	AC1D Packer		-
  CRB	****+	Heatseeker		mc1.0
  DI	****+	Digital Illusions	-
  FC-M	****+	FC-M Packer		1.0
  KSM	****+	Kefrens Sound Machine	-
  MP	****+	Module Protector	-
  NP	****+	NoisePacker		1.0, 2.0, 3.0
  TP    ****	The Player		6.0a
  PM	***	Power Music		-
  PP10	****+	Pha Packer		-
  PROM	***	Promizer		0.1, 1.0c, 1.8a, 2.0, 4.0
  PRU	****+	ProRunner		1.0, 2.0
  STIM	****+	Slamtilt		-
  UNIC	***	Unic Tracker		1.0, 2.0
  WN	****+	Wanton Packer		-
  XANN	****+	XANN Packer		-
  ZEN	****+	Zen Packer		-

  Other (Amiga)

  DIGI	***	DIGI Booster		1.4, 1.5, 1.6, 1.7
  EMOD  ***	Quadra Composer		0001
  MTN	****	ST 2.6, Ice Tracker	MTN, IT10
  KRIS	****+	ChipTracker		KRIS
  MODL	****+	Protracker 3.59		PTDT
  MED	*	MED		 	MED3, MED4
  MED	**	OctaMED			MMD0, MMD1, MMD2, MMD3
  OKTA	***	Oktalyzer		-
  SFX	***	SoundFX			1.3, 2.0?

  Other (PC)

  669	***	Composer 669/UNIS 669	if, JN
  ALM   ****+	Aley Keptr		1.0, 1.1, 1.2
  AMD	***	Amusic Adlib Tracker	-
  FAR	**	Farandole Composer	1.0
  FNK	**	Funktracker		?
  IMF	*** 	Imago Orpheus		1.0
  IT	****-	Impulse Tracker		1.00, 2.00, 2.14, 2.15
  MDL	**	Digitrakker		0.0, 1.0, 1.1
  MTM	****+	Multitracker		1.0
  PTM	**	Poly Tracker		2.03
  RAD	***	Reality Adlib Tracker	-
  S3M	**** 	Scream Tracker 3	3.00, 3.01+
  STM	****+	Scream Tracker 2	!Scream!, BMOD2STM
  STX	****+	ST Music Interface Kit	1.0, 1.1
  ULT	***	Ultra Tracker		V0001, V0002, V0003, V0004
  XM	**** 	Fast Tracker II		1.02, 1.03, 1.04
*/

const st_filter_t quh_libxmp_in = {
  QUH_LIBXMP_IN,
  "xmp ("
  "669, ac1d, alm, amd, chn, crb, di, digi, "
  "emod, exo, far, fc-m, fnk, imf, it, kris, "
  "ksm, m15, mdl, med, mod, modl, mp, mtm, "
  "mtn, np, okta, pm, pp10, prom, pru, ptm, "
  "rad, s3m, sfx, stim, stm, stx, tp, ult, "
  "unic, wn, wow, xann, xm, zen"
  ")",
  ".669.ac1d.alm.amd.chn.crb.di.digi"
  ".emod.exo.far.fc-m.fnk.imf.it.kris"
  ".ksm.m15.mdl.med.mod.modl.mp.mtm"
  ".mtn.np.okta.pm.pp10.prom.pru.ptm"
  ".rad.s3m.sfx.stim.stm.stx.tp.ult"
  ".unic.wn.wow.xann.xm.zen",
  -1,
  (int (*) (void *)) &quh_libxmp_demux,
  (int (*) (void *)) &quh_libxmp_open,
  (int (*) (void *)) &quh_libxmp_close,
  NULL,
  (int (*) (void *)) &quh_libxmp_write,
  (int (*) (void *)) &quh_libxmp_seek,
  NULL,
  NULL,
  NULL
};


const st_getopt2_t quh_libxmp_in_usage =
{
    "xmp", 1, 0, QUH_XMP,
    "FILE", "FILE is XMP (if it has no .flac suffix)",
    (void *) QUH_LIBXMP_IN
};


