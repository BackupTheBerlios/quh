/*
sid.c - sid support for Quh (using libsidplay2)

written by 2002 Dirk (d_i_r_k_@gmx.net)


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
#include <stdio.h>
#ifdef  HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef  HAVE_STDLIB_H
#include <stdlib.h>
#endif
//#include <sidplay/sidplay2.h>
#include "misc/itypes.h"
#include "misc/file.h"
#include "misc/getopt2.h"
#include "misc/string.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "sid.h"


//static FILE *fp = NULL;


int
quh_sid_open (st_quh_filter_t *file)
{
//  SidTuneMod         m_tune;
//  sidplay2           m_engine;
//const char *suffices[] = {"sid", NULL};
//  sidplay2 m_engine = sidplay2 ();
////  
//  m_engine.load (file->fname);
//  SidTune fp = SidTune((const char *) file->fname, (const char **) NULL, (bool) false);
  
//  SidTune fp = SidTune(file->fname, suffices, true);

//fp.load(file->fname, true);


  file->rate = 44100;
  file->channels = 2;
  file->is_signed = 1;
  file->size = 2;

  return -1;
}


int
quh_sid_close (st_quh_filter_t *file)
{
  (void) file;
  
//  fclose (fp);
  
  return 0;
}


int
quh_sid_seek (st_quh_filter_t *file)
{
  (void) file;

  return 0;
} 


int
quh_sid_demux (st_quh_filter_t *file)
{
  (void) file;

  return -1;
}


int
quh_sid_write (st_quh_filter_t *file)
{
  (void) file;

  return 0;
}


const st_filter_t quh_sid_in = {
  QUH_SID_IN,
  "libsidplay2 (sid, dat, inf, c64, prg, p00, info, data, str, mus)",
  ".sid.dat.inf.c64.prg.p00.info.data.str.mus",
  42,
  (int (*) (void *)) &quh_sid_demux,
  (int (*) (void *)) &quh_sid_open,
  (int (*) (void *)) &quh_sid_close,
  (int (*) (void *)) &quh_sid_demux,
  (int (*) (void *)) &quh_sid_write,
  (int (*) (void *)) &quh_sid_seek,
  NULL,
  NULL,
  NULL
};
