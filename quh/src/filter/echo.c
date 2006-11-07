/*
echo.c - echo effect support for Quh (using sox sound tools)

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


static struct st_effect e;
static eff_t ep = (eff_t) &e;
static st_effect_t *effect = NULL;


int
quh_echo_init (st_quh_nfo_t * file)
{
  (void) file;
  int x = 0;
  char *argv[4] = {"0.9", "10", "10", "0.4"};

  for (; st_effects[x]->name; x++)
    if (!strcmp (st_effects[x]->name, "echo"))
      {
        effect = st_effects[x];
        break;
      }

//  memset (ep, 0, sizeof (struct st_effect));
//  ep->ininfo.rate = file->rate;
//  ep->ininfo.size = file->size;
//  ep->ininfo->encoding = file->encoding;
//  ep->ininfo.channels = file->channels;
//  ep->ininfo.swap = file->swap;
//  memcpy (&ep->outinfo, &ep->ininfo, sizeof (st_signalinfo_t));

  effect->getopts (ep, 4, argv);

  return 0;
}      
      

int
quh_echo_quit (st_quh_nfo_t * file)
{
  (void) file;

  effect->stop (ep);
                  
  return 0;
}


int
quh_echo_open (st_quh_nfo_t * file)
{
  (void) file;

  effect->start (ep);

  return 0;
}


static int
quh_echo_close (st_quh_nfo_t * file)
{
  (void) file;
  st_sample_t *obuf = (st_sample_t *) &quh.buffer;
  st_size_t osamp = quh.buffer_len / sizeof (st_sample_t);

  effect->drain (ep, obuf, &osamp);

  return 0;
}


static int
quh_echo_write (st_quh_nfo_t *file)
{
  (void) file;
#if 0
  st_sample_t *ibuf = (st_sample_t *) &quh.buffer;
  st_size_t isamp = quh.buffer_len / sizeof (st_sample_t);
  st_sample_t *obuf = (st_sample_t *) &quh.buffer;
  st_size_t osamp = quh.buffer_len / sizeof (st_sample_t);
#else
  st_sample_t *ibuf = (st_sample_t *) malloc (quh.buffer_len);
  st_size_t isamp = quh.buffer_len / sizeof (st_sample_t);
  st_sample_t *obuf = (st_sample_t *) &quh.buffer;
  st_size_t osamp = QUH_MAXBUFSIZE / sizeof (st_sample_t);

  memcpy (ibuf, obuf, quh.buffer_len);
#endif

  effect->flow (ep, ibuf, obuf, &isamp, &osamp);

  return 0;
}


const st_filter_t quh_echo = {
  QUH_ECHO_PASS,
  "echo (using libst)",
  NULL,
  0,
  NULL,
  (int (*) (void *)) &quh_echo_open,
  (int (*) (void *)) &quh_echo_close,
  NULL,
  (int (*) (void *)) &quh_echo_write,
  NULL,
  NULL,
  (int (*) (void *)) &quh_echo_init,
  (int (*) (void *)) &quh_echo_quit,
};


const st_getopt2_t quh_echo_usage = {
  "echo", 0, 0, QUH_ECHO,
  "OPTS", "echo effect",
  (void *) QUH_ECHO_PASS
};
