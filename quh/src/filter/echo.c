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
#include "filter.h"
#include "misc/string.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"


#if 0
typedef int32_t st_sample_t;
typedef uint32_t st_size_t;
typedef int32_t st_ssize_t;
typedef uint32_t st_rate_t;


typedef struct st_effect *eff_t;

typedef struct
{
    char    *name;                  /* effect name */
    char    *usage;
    unsigned int flags;
   
    int (*getopts)(eff_t effp, int argc, char **argv);
    int (*start)(eff_t effp);
    int (*flow)(eff_t effp, st_sample_t *ibuf, st_sample_t *obuf,
                st_size_t *isamp, st_size_t *osamp);
    int (*drain)(eff_t effp, st_sample_t *obuf, st_size_t *osamp);
    int (*stop)(eff_t effp);
} st_effect_t;


typedef struct  st_signalinfo
{
    st_rate_t rate;       /* sampling rate */
    signed char size;     /* word length of data */
    signed char encoding; /* format of sample numbers */
    signed char channels; /* number of sound channels */
    char swap;            /* do byte- or word-swap */
} st_signalinfo_t;


struct st_effect
{
    char            *name;          /* effect name */
    struct st_signalinfo ininfo;    /* input signal specifications */
    struct st_signalinfo outinfo;   /* output signal specifications */
    st_effect_t     *h;             /* effects driver */
    st_sample_t     *obuf;          /* output buffer */
    st_size_t       odone, olen;    /* consumed, total length */
    /* The following is a portable trick to align this variable on
     * an 8-byte bounder.  Once this is done, the buffer alloced
     * after it should be align on an 8-byte boundery as well.
     * This lets you cast any structure over the private area
     * without concerns of alignment.
     */
    double priv1;
    char priv[ST_MAX_EFFECT_PRIVSIZE]; /* private area for effect */
};

extern st_effect_t *st_effects[]; /* declared in handlers.c */
#endif




#define MAX_EFF 16
#define MAX_USER_EFF 14

/* 
 * efftab[0] is a dummy entry used only as an input buffer for
 * reading input data into.
 *
 * If one was to support effects for quad-channel files, there would
 * need to be an effect table for each channel to handle effects
 * that don't set ST_EFF_MCHAN.
 */

//static struct st_effect efftab[MAX_EFF]; /* left/mono channel effects */
//static struct st_effect efftabR[MAX_EFF];/* right channel effects */
//static int neffects;                     /* # of effects to run on data */
//static int input_eff;                    /* last input effect with data */
//static int input_eff_eof;                /* has input_eff reached EOF? */
   
static struct st_effect user_efftab[MAX_USER_EFF];
static int nuser_effects;


static eff_t ep = (eff_t) &user_efftab;
static st_effect_t *effect = NULL;


int
quh_echo_init (st_quh_nfo_t * file)
{
  (void) file;
  int x = 0;
  char *argv[4] = {"0.9", "10", "10", "0.4"};
  struct st_effect * e = &user_efftab[nuser_effects];

  for (; st_effects[x]->name; x++)
    if (!strcmp (st_effects[x]->name, "echo"))
      {
        effect = st_effects[x];
        break;
      }

//  memset (&e, 0, sizeof (struct st_effect));
  e->ininfo.rate = file->rate;
//printf ("%d",  e->ininfo.rate); // = file->rate;
//  e.ininfo.size = file->size;
//  e.ininfo.encoding = 0; //file->encoding;
  e.ininfo.channels = file->channels;
//  e.ininfo.swap = 0; //file->swap;
//  memcpy (&e.outinfo, &e.ininfo, sizeof (st_signalinfo_t));


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


QUH_FILTER_OUT(quh_echo, QUH_ECHO_PASS, "echo (using libst)")


const st_getopt2_t quh_echo_usage = {
  "echo", 0, 0, QUH_ECHO,
  "OPTS", "echo effect"
};
