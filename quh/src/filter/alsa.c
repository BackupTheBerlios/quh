/*
alsa.c - alsa support for Quh (Linux only)

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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef  HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <alsa/asoundlib.h>
#include "misc/itypes.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "alsa.h"

#warning TODO: fix


static snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;
static snd_pcm_t *handle = NULL;
static snd_output_t *errlog = NULL;


static int
quh_alsa_init (st_quh_filter_t *file)
{
  (void) file;

  if (!quh_get_object_s (quh.filter_chain, QUH_OPTION))
    quh_set_object_s (quh.filter_chain, QUH_OPTION, "default");
      
  return 0;
}


static int
quh_alsa_config (st_quh_filter_t *file)
{
  (void) file;
#if 0
  snd_pcm_hw_params_t *hw_params;
  snd_pcm_format_t format;
  int rate = 0;
  
  if (snd_pcm_hw_params_malloc (&hw_params) < 0)
    return -1;

  if (snd_pcm_hw_params_any (handle, hw_params) < 0)
    {
      snd_pcm_hw_params_free (hw_params);
      return -1;
    }

  if (snd_pcm_hw_params_set_access (handle, hw_params,
                                    SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
    {
      snd_pcm_hw_params_free (hw_params);
      return -1;
    }

  switch (file->size)
    {
    case 1:
      format = SND_PCM_FORMAT_S8;
      break;
    case 2:
      format = SND_PCM_FORMAT_S16;
      break;
    case 3:
      format = SND_PCM_FORMAT_S24;
      break;
    default:
      format = SND_PCM_FORMAT_S16;
      break;
    }

  if (snd_pcm_hw_params_set_format (handle, hw_params, format) < 0)
    {
      snd_pcm_hw_params_free (hw_params);
      return -1;
    }

  rate = file->rate;
  if (snd_pcm_hw_params_set_rate_near (handle, hw_params, rate, 0) < 0)
    {
      snd_pcm_hw_params_free (hw_params);
      return -1;
    }

  if ((float) rate * 1.05 < file->rate || (float) rate * 0.95 > file->rate)
    {
      snd_pcm_hw_params_free (hw_params);
      return -1;
    }

  if (snd_pcm_hw_params_set_channels (handle, hw_params, file->channels) < 0)
    {
      snd_pcm_hw_params_free (hw_params);
      return -1;
    }

  if (snd_pcm_hw_params (handle, hw_params) < 0)
    {
      snd_pcm_hw_params_free (hw_params);
      return -1;
    }

  snd_pcm_hw_params_free (hw_params);
#endif
  return 0;
}


static int
quh_alsa_open (st_quh_filter_t *file)
{
  (void) file;
  int err;
  
  handle = NULL;
  if ((err = snd_pcm_open (&handle, quh_get_object_s (quh.filter_chain, QUH_OPTION), stream, 0)) < 0)
    {
      fprintf (stderr, "snd_pcm_open: %s\n", snd_strerror (err));
      return -1;
    }

  if (snd_output_stdio_attach (&errlog, stderr, 0) < 0)
    return -1;

  return 0;
}


static int
quh_alsa_close (st_quh_filter_t *file)
{
  (void) file;

  if (handle)
    {
      snd_pcm_drain (handle);
      snd_pcm_close (handle);
      handle = NULL;
    }

  return 0;
}


#if 0
static int
xrun_recovery (snd_pcm_t * handle, int err)
{
  if (err == -EPIPE)
    {                           /* under-run */
      err = snd_pcm_prepare (handle);
      if (err < 0)
        alsaplayer_error ("Can't recovery from underrun, prepare failed: %s",
                          snd_strerror (err));
      return 0;
    }
  else if (err == -ESTRPIPE)
    {
      while ((err = snd_pcm_resume (handle)) == -EAGAIN)
        sleep (1);              /* wait until the suspend flag is released */
      if (err < 0)
        {
          err = snd_pcm_prepare (handle);
          if (err < 0)
            alsaplayer_error
              ("Can't recovery from suspend, prepare failed: %s",
               snd_strerror (err));
        }
      return 0;
    }
  return err;
}
#endif


static int
quh_alsa_write (st_quh_filter_t *file)
{
  (void) file;

  snd_pcm_uframes_t fcount = (snd_pcm_uframes_t) (quh.buffer_len / 4);          

  if (snd_pcm_writei (handle, quh.buffer, fcount) < 0)
    return -1;

  return 0;
}


const st_filter_t quh_alsa_out =
{
  QUH_ALSA_OUT,
  "alsa",
  NULL,
  0,
  NULL,
  (int (*) (void *)) &quh_alsa_open,
  (int (*) (void *)) &quh_alsa_close,
  NULL,
  (int (*) (void *)) &quh_alsa_write,
  NULL,
  (int (*) (void *)) &quh_alsa_config,
  (int (*) (void *)) &quh_alsa_init,
  NULL
};


const st_getopt2_t quh_alsa_out_usage =
{
    "alsa", 2, 0, QUH_ALSA,
    "ID", "write to soundcard with ID using ALSA (default: \"default\")",
//    "OUT=0 lineout (default)\n"
//    "OUT=1 headphones",
    (void *) QUH_ALSA_OUT
};
