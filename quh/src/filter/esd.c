/*
oss.c - OSS support for Quh

Copyright (c) 2005 NoisyB
inspired by ESD output plugin Copyright (C) 1999 Andy Lo A Foe <andy@alsa-project.org>

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
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#ifdef  USE_ESD
#include <esd.h>
#endif  // USE_ESD
#include "misc/itypes.h"
#include "misc/getopt2.h"
#include "filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "esd.h"


static int esound_socket = -1;
static int inited = 0;


static int
quh_esd_out_open (st_quh_nfo_t * file)
{
  (void) file;
  char *host = NULL;
  char *name = NULL;
  esd_format_t format = ESD_BITS16 | ESD_STEREO | ESD_STREAM | ESD_PLAY;
  int rate = 44100;

  if (!inited)
    {
//  if (global_session_name)
//    {
//      name = global_session_name;
//    }
  esound_socket = esd_play_stream_fallback (format, rate, host, name);
  if (esound_socket < 0)
    {
      /* printf("ESD: could not open socket connection\n"); */
      return -1;
    }
      inited = 1;
    }
  if (esound_socket >= 0)
    return 0;

  return -1;
}


static int
quh_esd_out_write (st_quh_nfo_t * file)
{
  (void) file;

  write (esound_socket, quh.buffer, quh.buffer_len);

  return 0;
}


#if 0
static int
quh_esd_set_buffer (int *fragment_size, int *fragment_count, int *channels)
{
  printf ("ESD: fragments fixed at 256/256, stereo\n");
  return 1;
}


static unsigned int
quh_esd_set_sample_rate (unsigned int rate)
{
  printf ("ESD: rate fixed at 44100Hz\n");
  return rate;
}

static int
quh_esd_get_latency (void)
{
  return 256 * 256;         // Hardcoded, but esd sucks ass
}

output_plugin esound_output;

output_plugin *
output_plugin_info (void)
{
  memset (&esound_output, 0, sizeof (output_plugin));
  esound_output.version = OUTPUT_PLUGIN_VERSION;
  esound_output.name = "ESD output v1.0 (broken for mono output)";
  esound_output.author = "Andy Lo A Foe";
  esound_output.init = esound_init;
  esound_output.open = esound_open;
  esound_output.close = esound_close;
  esound_output.write = esound_write;
  esound_output.set_buffer = esound_set_buffer;
  esound_output.set_sample_rate = esound_set_sample_rate;
  esound_output.get_latency = esound_get_latency;
  return &esound_output;
}
#endif


QUH_FILTER_FUNC_STUB (quh_esd_out_seek)
QUH_FILTER_FUNC_STUB (quh_esd_out_ctrl)
QUH_FILTER_FUNC_STUB (quh_esd_out_init)
QUH_FILTER_FUNC_STUB (quh_esd_out_close)
QUH_FILTER_FUNC_STUB (quh_esd_out_quit)
QUH_FILTER_OUT (quh_esd_out, QUH_ESD_OUT, "esound")


const st_getopt2_t quh_esd_out_usage =
{
    "esd", 0, 0, QUH_ESD,
    NULL, "write to soundcard using EsounD"
};
