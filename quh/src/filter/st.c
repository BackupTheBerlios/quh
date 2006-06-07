/*
st.h - sox soud tools support for Quh (using st)

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
#include "st.h"


/* called from util.c::st_fail() */
void cleanup (void) {}


int
quh_st_init (st_quh_nfo_t * file)
{
  (void) file;
  int x = 0, y = 0;
#if 0
  struct
  {
    const char *e;
    int argc;
    char **argv;
  } effect_usage[] = {
    {"avg", 0, NULL},
    {"band", 0, NULL},
    {"bandpass", 0, NULL},
    {"bandreject", 0, NULL},
    {"chorus", 0, NULL},
    {"compand", 0, NULL},
    {"copy", 0, NULL},
    {"dcshift", 0, NULL},
    {"deemph", 0, NULL},
    {"earwax", 0, NULL},
    {"echo", 0, NULL},
    {"echos", 0, NULL},
    {"fade", 0, NULL},
    {"filter", 0, NULL},
    {"flanger", 0, NULL},
    {"highp", 0, NULL},
    {"highpass", 0, NULL},
    {"lowp", 0, NULL},
    {"lowpass", 0, NULL},
    {"mask", 0, NULL},
    {"mcompand", 0, NULL},
    {"noiseprof", 0, NULL},
    {"noisered", 0, NULL},
    {"pan", 0, NULL},
    {"phaser", 0, NULL},
    {"pick", 0, NULL},
    {"pitch", 0, NULL},
    {"polyphase", 0, NULL},
    {"rate", 0, NULL},
    {"repeat", 0, NULL},
    {"resample", 0, NULL},
    {"reverb", 0, NULL},
    {"reverse", 0, NULL},
    {"silence", 0, NULL},
    {"speed", 0, NULL},
    {"stat", 0, NULL},
    {"stretch", 0, NULL},
    {"swap", 0, NULL},
    {"synth", 0, NULL},
    {"trim", 0, NULL},
    {"vibro", 0, NULL},
    {"vol", 0, NULL},
    {NULL, 0, NULL}
  };
#endif

  for (; st_effects[x].name; x++)
    {
//      printf ("%s\n", st_effects[x].name);
//      st_effects[x].getopts ((eff_t) &st_effects[x], 0, NULL);
    }

  return 0;
}      
      

int
quh_st_quit (st_quh_nfo_t * file)
{
  (void) file;
                  
  return 0;
}


int
quh_st_open (st_quh_nfo_t * file)
{
  (void) file;

  return 0;
}


static int
quh_st_close (st_quh_nfo_t * file)
{
  (void) file;

  return 0;
}


static int
quh_st_write (st_quh_nfo_t *file)
{
  (void) file;
  
  return 0;
}


const st_filter_t quh_st = {
  QUH_ST_PASS,
  "sox sound tools",
  NULL,
  0,
  NULL,
  (int (*) (void *)) &quh_st_open,
  (int (*) (void *)) &quh_st_close,
  NULL,
  (int (*) (void *)) &quh_st_write,
  NULL,
  NULL,
  (int (*) (void *)) &quh_st_init,
  (int (*) (void *)) &quh_st_quit,
};


const st_getopt2_t quh_st_usage = {
  "st", 1, 0, QUH_ST,
  "EFFECT[:OPTIONS]", "use sox sound tools EFFECT",
  (void *) QUH_ST_PASS
};
