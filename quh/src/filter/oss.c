/*
oss.c - OSS support for Quh (Linux only)

written by 2004 Dirk (d_i_r_k_@gmx.net)

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
#include <stdlib.h>
#ifdef  __linux__
#include <sys/soundcard.h>
#include <sys/ioctl.h>
#endif  // __linux__
#include "misc/itypes.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "oss.h"


static int
quh_oss_init (st_quh_filter_t *file)
{
  (void) file;

  if (!quh_get_object_s (quh.filter_chain, QUH_OPTION))
    quh_set_object_s (quh.filter_chain, QUH_OPTION, "/dev/dsp");

  return 0;
}


int
quh_oss_quit (st_quh_filter_t * file)
{
  (void) file;

  return 0;
}


static int
quh_oss_ctrl (st_quh_filter_t *file)
{
#ifdef WORDS_BIGENDIAN
  int format = AFMT_S16_BE;
#else
  int format = AFMT_S16_LE;
#endif
  int stereo = 1;
  unsigned long value = 0;
  int oss_out = 0;
  
  quh_get_object (quh.filter_chain, QUH_OBJECT, &oss_out, sizeof (int));
    
  if (ioctl (oss_out, SOUND_PCM_RESET, 0) == -1)
    {
      close (oss_out);
      return -1;
    }

  switch (file->size)
    {
      case 1:
        format = file->is_signed ? AFMT_S8 : AFMT_U8;
        break;
          
      case 2:
      default:
        format =
#ifdef WORDS_BIGENDIAN
          file->is_signed ? AFMT_S16_BE : AFMT_U16_BE;
#else
          file->is_signed ? AFMT_S16_LE : AFMT_U16_LE;
#endif
        break;
    }

//  if (ioctl (oss_out, SNDCTL_DSP_SETFMT, &format) == -1)
  if (ioctl (oss_out, SOUND_PCM_SETFMT, &format) == -1)
    {
      close (oss_out);
      return -1;
    }
    
  switch (format)
    {
      case AFMT_U8:
        quh.soundcard.size = 1;
        quh.soundcard.is_signed = 0;
        break;

      case AFMT_S8:
        quh.soundcard.size = 1;
        quh.soundcard.is_signed = 1;
        break;

      case AFMT_U16_BE:
      case AFMT_U16_LE:
        quh.soundcard.size = 2;
        quh.soundcard.is_signed = 0;
        break;

      case AFMT_S16_BE:
      case AFMT_S16_LE:
      default:
        quh.soundcard.size = 2;
        quh.soundcard.is_signed = 1;
        break;
    }

  value = file->size ? file->size * 8 : 16;
//  if (ioctl (oss_out, SNDCTL_DSP_SAMPLESIZE, &value) == -1)
  if (ioctl (oss_out, SOUND_PCM_WRITE_BITS, &value) == -1)
    {
      close (oss_out);
      return -1;
    }
  quh.soundcard.size = value;
        
  value = file->channels ? file->channels : 1;    // 0 = mono, 1 = stereo
//  if (ioctl (oss_out, SNDCTL_DSP_CHANNELS, &value) == -1)
  if (ioctl (oss_out, SOUND_PCM_WRITE_CHANNELS, &value) == -1)
    {
      close (oss_out);
      return -1;
    }
  quh.soundcard.channels = value;

  stereo = quh.soundcard.channels == 2 ? 1 : 0;
  if (ioctl (oss_out, SNDCTL_DSP_STEREO, &stereo) == -1)
    {
      close (oss_out);
      return -1;
    }

  value = file->rate ? file->rate : 44100;
//  if (ioctl (oss_out, SNDCTL_DSP_SPEED, &value) == -1)
  if (ioctl (oss_out, SOUND_PCM_WRITE_RATE, &value) == -1)
    {
      close (oss_out);
      return -1;
    }
  quh.soundcard.rate = value;

  value = QUH_MAXBUFSIZE;
  if (ioctl (oss_out, SNDCTL_DSP_GETBLKSIZE, &value) == -1)
    {
      close (oss_out);
      return -1;
    }
  quh.soundcard.buffer_max = value;

//  quh_soundcard_sanity_check ();

  quh_set_object (quh.filter_chain, QUH_OBJECT, &oss_out, sizeof (int));
  
  return 0;
}


static int
quh_oss_open (st_quh_filter_t *file)
{
  (void) file;
  const char *p = NULL;
  int oss_out = 0;

  p = quh_get_object_s (quh.filter_chain, QUH_OPTION);

#ifdef __linux__
  if ((oss_out = open (p, O_WRONLY|O_NONBLOCK)) == -1)
#else
  if ((oss_out = open (p, O_WRONLY)) == -1)
#endif
    return -1;

#ifdef __linux__
  // remove the non-blocking flag
  if (fcntl (oss_out, F_SETFL, 0) < 0)
    return -1;
#endif

#if 0
//#ifndef __FreeBSD__
  if (ioctl (oss_out, SNDCTL_DSP_SETFRAGMENT, (16 << 16) | 8) == -1)
    {
      close (oss_out);
      return -1;
    }
#endif

  quh_set_object (quh.filter_chain, QUH_OBJECT, &oss_out, sizeof (int));

  return 0;
}


static int
quh_oss_close (st_quh_filter_t *file)
{
  (void) file;
  int oss_out = 0;

  quh_get_object (quh.filter_chain, QUH_OBJECT, &oss_out, sizeof (int));
    
//  if (ioctl (oss_out, SNDCTL_DSP_SYNC) == -1)
  if (ioctl (oss_out, SOUND_PCM_SYNC, 0) == -1)
    {
      close (oss_out);
      return -1;
    }

  close (oss_out);

  quh_set_object (quh.filter_chain, QUH_OBJECT, &oss_out, sizeof (int));
  
  return 0;
}


static int
quh_oss_write (st_quh_filter_t *file)
{
  (void) file;
  int oss_out = 0;
  
  quh_get_object (quh.filter_chain, QUH_OBJECT, &oss_out, sizeof (int));
    
  write (oss_out, quh.buffer, quh.buffer_len);

//  quh_set_object (quh.filter_chain, QUH_OBJECT, &oss_out, sizeof (int));

  return 0;
}


const st_filter_t quh_oss_out =
{
  QUH_OSS_OUT,
  "oss (linux)",
  NULL,
  0,
  (int (*) (void *)) &quh_oss_open,
  (int (*) (void *)) &quh_oss_close,
  NULL,
  (int (*) (void *)) &quh_oss_write,
  NULL,
  (int (*) (void *)) &quh_oss_ctrl,
  (int (*) (void *)) &quh_oss_init,
  (int (*) (void *)) &quh_oss_quit
};


const st_getopt2_t quh_oss_out_usage =
{
    "oss", 2, 0, QUH_OSS,
    "DEVICE", "write to soundcard DEVICE using OSS (default: /dev/dsp)",
//    "OUT=0 lineout (default)\n"
//    "OUT=1 headphones",
    (void *) QUH_OSS_OUT
};
