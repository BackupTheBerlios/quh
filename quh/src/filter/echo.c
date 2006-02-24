/*
 * August 24, 1998
 * Copyright (C) 1998 Juergen Mueller And Sundry Contributors
 * This source code is freely redistributable and may be used for
 * any purpose.  This copyright notice must be maintained. 
 * Juergen Mueller And Sundry Contributors are not responsible for 
 * the consequences of using this software.
 */

/*
 * This is the "echo.c" while the old "echo.c" from version 12 moves to
 * "reverb.c" satisfying the defintions made in the Guitar FX FAQ.
 *
 *
 * Echo effect for dsp.
 * 
 * Flow diagram scheme for n delays ( 1 <= n <= MAX_ECHOS ):
 *
 *        * gain-in                                              ___
 * ibuff -----------+------------------------------------------>|   |
 *                  |       _________                           |   |
 *                  |      |         |                * decay 1 |   |
 *                  +----->| delay 1 |------------------------->|   |
 *                  |      |_________|                          |   |
 *                  |            _________                      | + |
 *                  |           |         |           * decay 2 |   |
 *                  +---------->| delay 2 |-------------------->|   |
 *                  |           |_________|                     |   |
 *                  :                 _________                 |   |
 *                  |                |         |      * decay n |   |
 *                  +--------------->| delay n |--------------->|___|
 *                                   |_________|                  | 
 *                                                                | * gain-out
 *                                                                |
 *                                                                +----->obuff
 *
 * Usage: 
 *   echo gain-in gain-out delay-1 decay-1 [delay-2 decay-2 ... delay-n decay-n]
 *
 * Where:
 *   gain-in, decay-1 ... decay-n :  0.0 ... 1.0      volume
 *   gain-out :  0.0 ...      volume
 *   delay-1 ... delay-n :  > 0.0 msec
 *
 * Note:
 *   when decay is close to 1.0, the samples can begin clipping and the output
 *   can saturate! 
 *
 * Hint:
 *   1 / out-gain > gain-in ( 1 + decay-1 + ... + decay-n )
 *
*/

/*
 * Sound Tools reverb effect file.
 */

#ifdef  HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef  HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "misc/itypes.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "misc/cache.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "echo.h"


#if 0
#define DELAY_BUFSIZ ( 50L * (50L * 1024) )
#define MAX_ECHOS 7             /* 24 bit x ( 1 + MAX_ECHOS ) = */
                        /* 24 bit x 8 = 32 bit !!!      */

/* Private data for SKEL file */
typedef struct echostuff
{
  int counter;
  int num_delays;
  double *delay_buf;
  float in_gain, out_gain;
  float delay[MAX_ECHOS], decay[MAX_ECHOS];
  int32_t samples[MAX_ECHOS], maxsamples;
  uint32_t fade_out;
} echo_t;

echo_t e;
echo_t *echo = &e;


static st_cache_t *in = NULL;
static st_cache_t *out = NULL;



int32_t st_clip24(int32_t l)
{
    if (l >= ((int32_t)1 << 23))
        return ((int32_t)1 << 23) - 1;
    else if (l <= -((int32_t)1 << 23))
        return -((int32_t)1 << 23) + 1;
    else
        return l;
}


/*
 * Prepare for processing.
 */
int
quh_echo_open (st_quh_filter_t *effp)
{
  int i;
  float sum_in_volume;
  long j;

  echo->maxsamples = 0L;
  if (echo->in_gain < 0.0)
    {
      printf ("echo: gain-in must be positive!\n");
      return -1;
    }
  if (echo->in_gain > 1.0)
    {
      printf ("echo: gain-in must be less than 1.0!\n");
      return -1;
    }
  if (echo->out_gain < 0.0)
    {
      printf ("echo: gain-in must be positive!\n");
      return -1;
    }
  for (i = 0; i < echo->num_delays; i++)
    {
      echo->samples[i] = echo->delay[i] * effp->rate / 1000.0;
      if (echo->samples[i] < 1)
        {
          printf ("echo: delay must be positive!\n");
          return -1;
        }
      if (echo->samples[i] > DELAY_BUFSIZ)
        {
          printf ("echo: delay must be less than %g seconds!\n",
                   DELAY_BUFSIZ / (float) effp->rate);
          return -1;
        }
      if (echo->decay[i] < 0.0)
        {
          printf ("echo: decay must be positive!\n");
          return -1;
        }
      if (echo->decay[i] > 1.0)
        {
          printf ("echo: decay must be less than 1.0!\n");
          return -1;
        }
      if (echo->samples[i] > echo->maxsamples)
        echo->maxsamples = echo->samples[i];
    }
  if (!
      (echo->delay_buf =
       (double *) malloc (sizeof (double) * echo->maxsamples)))
    {
      printf ("echo: Cannot malloc %d bytes!\n",
               sizeof (long) * echo->maxsamples);
      return -1;
    }
  for (j = 0; j < echo->maxsamples; ++j)
    echo->delay_buf[j] = 0.0;
  /* Be nice and check the hint with warning, if... */
  sum_in_volume = 1.0;
  for (i = 0; i < echo->num_delays; i++)
    sum_in_volume += echo->decay[i];
  if (sum_in_volume * echo->in_gain > 1.0 / echo->out_gain)
    printf ("echo: warning >>> gain-out can cause saturation of output <<<");
  echo->counter = 0;
  echo->fade_out = echo->maxsamples;

  in = cache_open (4, QUH_MEGABYTE / 4, CACHE_MALLOC_LIFO);
  out = cache_open (4, QUH_MEGABYTE / 4, CACHE_MALLOC_LIFO);
 
  return 0;
}

/*
 * Processed signed long samples from ibuf to obuf.
 * Return number of samples processed.
 */
int
quh_echo_write2 (st_quh_filter_t *effp, int32_t * ibuf, int32_t * obuf,
                uint32_t * isamp, uint32_t * osamp)
{
  int len, done;
  int j;
  double d_in, d_out;
  int32_t out;

  len = ((*isamp > *osamp) ? *osamp : *isamp);
  for (done = 0; done < len; done++)
    {
      /* Store delays as 24-bit signed longs */
      d_in = (double) *ibuf++ / 256;
      /* Compute output first */
      d_out = d_in * echo->in_gain;
      for (j = 0; j < echo->num_delays; j++)
        {
          d_out += echo->delay_buf[(echo->counter + echo->maxsamples -
                                    echo->samples[j]) % echo->maxsamples] *
            echo->decay[j];
        }
      /* Adjust the output volume and size to 24 bit */
      d_out = d_out * echo->out_gain;
      out = st_clip24 ((int32_t) d_out);
      *obuf++ = out * 256;
      /* Store input in delay buffer */
      echo->delay_buf[echo->counter] = d_in;
      /* Adjust the counter */
      echo->counter = (echo->counter + 1) % echo->maxsamples;
    }
  /* processed all samples */
  return 0;
}


int
quh_echo_write (st_quh_filter_t *effp)
{
  cache_write (in, effp->buffer, effp->buffer_len);
  
  if (cache_sizeof (in) > ?)
    {
      int32_t ibuf[?];
      int32_t obuf[?];
      
      cache_read (in, ibuffer, ? / sizeof (int32_t));
      quh_echo_write2 (effp, &ibuf, &obuf, effp->size?, effp->size?);
      
      cache_write (out, obuf, ?);
    }

  if (cache_sizeof (out) > effp->buffer_len)
    {
      cache_read (out, effp->buffer, effp->buffer_len);
    }

  return 0;
}               


#if 0
/*
 * Drain out reverb lines. 
 */
int
quh_echo_sync (st_quh_filter_t *effp, int32_t * obuf, uint32_t * osamp)
{
  double d_in, d_out;
  int32_t out;
  int j;
  uint32_t done;

  done = 0;
  /* drain out delay samples */
  while ((done < *osamp) && (done < echo->fade_out))
    {
      d_in = 0;
      d_out = 0;
      for (j = 0; j < echo->num_delays; j++)
        {
          d_out += echo->delay_buf[(echo->counter + echo->maxsamples -
                                    echo->samples[j]) % echo->maxsamples] *
            echo->decay[j];
        }
      /* Adjust the output volume and size to 24 bit */
      d_out = d_out * echo->out_gain;
      out = st_clip24 ((int32_t) d_out);
      *obuf++ = out * 256;
      /* Store input in delay buffer */
      echo->delay_buf[echo->counter] = d_in;
      /* Adjust the counters */
      echo->counter = (echo->counter + 1) % echo->maxsamples;
      done++;
      echo->fade_out--;
    };
  /* samples played, it remains */
  *osamp = done;
  return 0;
}
#endif


/*
 * Clean up reverb effect.
 */
int
quh_echo_close (st_quh_filter_t *effp)
{
//  quh_echo_sync (effp);
  free ((char *) echo->delay_buf);
  echo->delay_buf = (double *) -1;      /* guaranteed core dump */

  cache_close (in);
  cache_close (out);

  return 0;
}


const st_filter_t quh_echo = {
  QUH_ECHO_PASS,
  "echo",
  NULL,
  0,
  NULL,
  (int (*)(void *)) &quh_echo_open,
  (int (*)(void *)) &quh_echo_close,
  NULL,
  (int (*)(void *)) &quh_echo_write,
  NULL,
  NULL,
  NULL,
  NULL
};
#else
const st_filter_t quh_echo = {
  QUH_ECHO_PASS,
  "echo",
  NULL,
  0,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};
#endif


const st_getopt2_t quh_echo_usage = {
  "echo", 0, 0, QUH_ECHO,
  NULL, "echo",
  (void *) QUH_ECHO_PASS
};


#if 0
/*
 * Process options
 */
int
st_echo_getopts (st_quh_filter_t *effp, int n, char **argv)
{
  echo_t echo = (echo_t) effp->priv;
  int i;

  echo->num_delays = 0;

  if ((n < 4) || (n % 2))
    {
      printf
        ("Usage: echo gain-in gain-out delay decay [ delay decay ... ]");
      return -1;
    }

  i = 0;
  sscanf (argv[i++], "%f", &echo->in_gain);
  sscanf (argv[i++], "%f", &echo->out_gain);
  while (i < n)
    {
      if (echo->num_delays >= MAX_ECHOS)
        printf ("echo: to many delays, use less than %i delays", MAX_ECHOS);
      /* Linux bug and it's cleaner. */
      sscanf (argv[i++], "%f", &echo->delay[echo->num_delays]);
      sscanf (argv[i++], "%f", &echo->decay[echo->num_delays]);
      echo->num_delays++;
    }
  return 0;
}
#endif
