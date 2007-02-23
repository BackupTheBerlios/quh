/*
ape.c - ape support for Quh

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
#ifdef  USE_APE
#include "MACLib.h"
#endif
#include "misc/misc.h"
#include "misc/itypes.h"
#include "misc/file.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "ape.h"


static FILE *fh = NULL;
#if     MAXBUFSIZE < FILENAME_MAX
char tmp_file[FILENAME_MAX];
#else
char tmp_file[MAXBUFSIZE];
#endif


// defines
#define COMPRESS_MODE		0
#define DECOMPRESS_MODE		1
#define VERIFY_MODE			2
#define CONVERT_MODE		3
#define UNDEFINED_MODE		-1

// global variables
TICK_COUNT_TYPE g_nInitialTickCount = 0;


void CALLBACK ProgressCallback(int nPercentageDone)
{
    // get the current tick count
	TICK_COUNT_TYPE  nTickCount;
	TICK_COUNT_READ(nTickCount);

	// calculate the progress
	double dProgress = nPercentageDone / 1.e5;											// [0...1]
	double dElapsed = (double) (nTickCount - g_nInitialTickCount) / TICK_COUNT_FREQ;	// seconds
	double dRemaining = dElapsed * ((1.0 / dProgress) - 1.0);							// seconds

	// output the progress
	fprintf(stderr, "Progress: %.1f%% (%.1f seconds remaining, %.1f seconds total)          \r", 
		dProgress * 100, dRemaining, dElapsed);
}


static void
quh_ape_decode_to_wav (st_quh_nfo_t *file, const char *fname)
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
quh_ape_open (st_quh_nfo_t *file)
{
  int x = 0;
  char buf[MAXBUFSIZE];
#if     MAXBUFSIZE < FILENAME_MAX
  char fname[FILENAME_MAX];
#else
  char fname[MAXBUFSIZE];
#endif
  st_wav_header_t wav_header;

	// variable declares
	CSmartPtr<wchar_t> spInputFilename; CSmartPtr<wchar_t> spOutputFilename;
	int nRetVal = ERROR_UNDEFINED;
	int nMode = UNDEFINED_MODE;
	int nCompressionLevel = COMPRESSION_LEVEL_NORMAL;
	int nPercentageDone;
		
	// store the input file
	spInputFilename.Assign(GetUTF16FromANSI(argv[1]), TRUE);
	
	// store the output file
	spOutputFilename.Assign(GetUTF16FromANSI(argv[2]), TRUE);

	nMode = DECOMPRESS_MODE;

	// set the initial tick count
	TICK_COUNT_READ(g_nInitialTickCount);
	
	// process
	int nKillFlag = 0;
		fprintf(stderr, "Decompressing...\n");
		nRetVal = DecompressFileW(spInputFilename, spOutputFilename, &nPercentageDone, ProgressCallback, &nKillFlag);

	if (nRetVal != ERROR_SUCCESS) 
          return -1;

        return 0;



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

  if (!quh_forked_wav_decode (file, quh_ape_decode_to_wav))
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
quh_ape_close (st_quh_nfo_t *file)
{
  (void) file;

  fclose (fh);

  return 0;
}


static int
quh_ape_seek (st_quh_nfo_t *file)
{
  fseek (fh, quh.raw_pos, SEEK_SET);
  file->raw_size = fsizeof (quh.tmp_file); // expanding
  return 0;
}


static int
quh_ape_write (st_quh_nfo_t *file)
{
  quh.buffer_len = fread (&quh.buffer, 1, QUH_MAXBUFSIZE, fh);
  file->raw_size = fsizeof (quh.tmp_file); // expanding
  return 0;
}


#if 0
static int
quh_ape_demux (st_quh_nfo_t * file)
{
  int result = 0;

  if (file->source != QUH_SOURCE_FILE)
    return -1;

  result = quh_ape_open (file);

  if (!result)
    quh_ape_close (file);

  return result;
}
#endif


static int
quh_ape_quit (st_quh_nfo_t *file)
{
  (void) file;

  remove (quh.tmp_file);
  strcpy (quh.tmp_file, tmp_file);

  return 0;
}


const st_filter_t quh_ape_in = {
  QUH_APE_IN,
  "ape",
  ".ape",
  -1,
//  (int (*) (void *)) &quh_ape_demux,
  NULL,
  (int (*) (void *)) &quh_ape_open,
  (int (*) (void *)) &quh_ape_close,
  NULL,
  (int (*) (void *)) &quh_ape_write,
  (int (*) (void *)) &quh_ape_seek,
  NULL,
  NULL,
  (int (*) (void *)) &quh_ape_quit
};
