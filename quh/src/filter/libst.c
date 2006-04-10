/*
libst.h - sox soud tools support for Quh (using libst)

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
#ifdef  HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <string.h>
#include <st.h>
#include "misc/bswap.h"
#include "misc/itypes.h"
#include "misc/file.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "misc/string.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "libst.h"


/* called from util.c::st_fail() */
void
cleanup (void)
{
  return;
}


int
quh_libst_init (st_quh_nfo_t * file)
{
  (void) file;

  return 0;
}      
      

int
quh_libst_quit (st_quh_nfo_t * file)
{
  (void) file;
                  
  return 0;
}


int
quh_libst_open (st_quh_nfo_t * file)
{
  (void) file;

  return 0;
}


static int
quh_libst_close (st_quh_nfo_t * file)
{
  (void) file;

  return 0;
}


static int
quh_libst_write (st_quh_nfo_t *file)
{
  (void) file;
  
  return 0;
}


const st_filter_t quh_libst = {
  QUH_LIBST_PASS,
  "sox sound tools",
  NULL,
  0,
  NULL,
  (int (*) (void *)) &quh_libst_open,
  (int (*) (void *)) &quh_libst_close,
  NULL,
  (int (*) (void *)) &quh_libst_write,
  NULL,
  NULL,
  (int (*) (void *)) &quh_libst_init,
  (int (*) (void *)) &quh_libst_quit,
};


const st_getopt2_t quh_libst_usage = {
  "st", 1, 0, QUH_ST,
  "EFFECT[:OPTIONS]", "use sox sound tools EFFECT",
  (void *) QUH_LIBST_PASS
};
