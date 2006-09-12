/*
fade.c - fade filter for Quh

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
#include <ctype.h>
#include <stdlib.h>
#include "misc/itypes.h"
#include "misc/getopt2.h"
#include "misc/misc.h"
#include "misc/file.h"
#include "misc/filter.h"
#include "misc/term.h"
#include "misc/property.h"
#include "quh_defines.h"
#include "quh_filter.h"
#include "quh.h"
#include "quh_misc.h"
#include "fade.h"


#ifdef  MIN
#undef  MIN
#endif
#define MIN(a,b) ((a)<(b)?(a):(b))
#ifdef  MAX
#undef  MAX
#endif
#define MAX(a,b) ((a)>(b)?(a):(b))


enum
{
  GAUGE_MODE_METER,
  GAUGE_MODE_TIME,
  GAUGE_MODE_VOL
};


#if     FILENAME_MAX < MAXBUFSIZE
static char fname[MAXBUFSIZE];
static char old_fname[MAXBUFSIZE];
#else
static char fname[FILENAME_MAX]; 
static char old_fname[FILENAME_MAX];
#endif
static unsigned int display_pos = 0;
static int gauge_mode = GAUGE_MODE_METER;  // default
static unsigned long t = 0, t2 = 0;
static int verbose = 0xff;  // the verbosity level shall never exceed 0xff
static int output_once = 0;
static int quitting = 0;    // quh quits in 2 seconds
static int starting = 0;    // quh is running for the 1st 2 seconds


static int
quh_fade_init (st_quh_nfo_t *file)
{
  (void) file;

//  if (!quh_get_object_s (quh.filter_chain, QUH_OPTION))
//    quh_set_object_s (quh.filter_chain, QUH_OPTION, "0");

  return 0;
}


static int
quh_fade_ctrl (st_quh_nfo_t *file)
{
  (void) file;
  return 0;
}


static int
quh_fade_open (st_quh_nfo_t *file)
{
  (void) file;
  char buf[MAXBUFSIZE];

  *buf = 0;
//  strcpy (buf, "Keys: crsr, pg up/dn, '>', '<', '9', '0', 'p'ause and 'q'uit");

  quh_set_object_s (quh.filter_chain, QUH_OUTPUT, buf);

  return 0;
}


static int
quh_fade_close (st_quh_nfo_t *file)
{
  (void) file;
  
  return 0;
}


static int
quh_fade_write (st_quh_nfo_t *file)
{
  char c = 0;
  static int display_delay = 0;

  t2 = time_ms (0);

  if (t2 - t > 100) // only every 10th second
    {
          if (!starting)
            {
              gauge_mode = GAUGE_MODE_VOL;
              quh.soundcard.vol = 0;
              starting = 1;
            }
          else if (starting == 1)
            {
              gauge_mode = GAUGE_MODE_VOL;
              quh.soundcard.vol = MIN (quh.settings, quh.soundcard.vol + 20);

              if (quh.soundcard.vol == quh.settings)
                starting = 2;
            }

          if (quh.quitting)
            {
              gauge_mode = GAUGE_MODE_VOL;
              quh.soundcard.vol = MAX (0, quh.soundcard.vol - 10);

              if (!quh.soundcard.vol)
                {
                  quh.soundcard.vol = quh.settings; // set system volume again
                  quh.quit = 1;
                }
            }

      t = t2;
    }

  display_delay = 10; // 1 second

  return 0;
}

      
static int
quh_fade_quit (st_quh_nfo_t *file)
{
  (void) file;

  return 0;
}


const st_filter_t quh_fade =
{
  QUH_FADE_PASS,
  "fade",
  NULL,
  0,
  NULL,
  (int (*) (void *)) &quh_fade_open,
  (int (*) (void *)) &quh_fade_close,
  NULL,
  (int (*) (void *)) &quh_fade_write,
  NULL,
  (int (*) (void *)) &quh_fade_ctrl,
  (int (*) (void *)) &quh_fade_init,
  (int (*) (void *)) &quh_fade_quit
};


const st_getopt2_t quh_fade_usage =
{
    "fade", 0, 0, QUH_FADE,
    NULL, "enable fade-in and fade-out (default: disabled)",
    (void *) QUH_FADE_PASS
};

