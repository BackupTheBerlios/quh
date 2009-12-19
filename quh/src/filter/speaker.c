/*
speaker.c - PC speaker support for Quh

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
#ifdef  HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "misc/itypes.h"
//#include "misc/bswap.h"
#include "misc/getopt2.h"
#include "filter.h"
#include "misc/misc.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "speaker.h"
#ifdef  __linux__
#include <sys/kd.h>
#include <sys/ioctl.h>
#endif  // __linux__


#warning TODO: fix


static int fp = 0;


/*
7.1 Programming the internal speaker

   Believe it or not, your PC speaker is part of the Linux console and thus a character device. Therefore, ioctl()
   requests exist to manipulate it. For the internal speaker the following 2 requests exist:

    1. KDMKTONE
       Generates a beep for a specified time using the kernel timer.
       Example: ioctl (fd, KDMKTONE,(long) argument).
    2. KIOCSOUND
       Generates an endless beep or stops a currently sounding beep.
       Example: ioctl(fd,KIOCSOUND,(int) tone).

   The argument consists of the tone value in the low word and the duration in the high word. The tone value is not the
   frequency. The PC mainboard timer 8254 is clocked at 1.19 MHz and so it's 1190000/frequency. The duration is measured
   in timer ticks. Both ioctl calls return immediately so you can this way produce beeps without blocking the program.
   KDMKTONE should be used for warning signals because you don't have to worry about stopping the tone.
   KIOCSOUND can be used to play melodies as demonstrated in the example program splay (please send more .sng files to
   me). To stop the beep you have to use the tone value 0.
*/


static int
quh_speaker_open (st_quh_nfo_t *file)
{
  (void) file;

  if ((fp = open ("/dev/tty", O_WRONLY)) == -1)
    return -1;

  return 0;
}


static int
quh_speaker_close (st_quh_nfo_t *file)
{
  (void) file;

  ioctl (fp, KIOCSOUND, 0);
  close (fp);

  return 0;
}


static int
quh_speaker_write (st_quh_nfo_t *file)
{
#define QUH_TIMER_MHZ 1190000
  unsigned long pos = 0;

  for (; pos < quh.buffer_len; pos += file->size * file->channels)
    {
      switch (file->size)
        {
          case 1:
            if (file->is_signed)
              {
                int8_t *value_8 = (int8_t *) &quh.buffer[pos];
                ioctl (fp, KIOCSOUND, *value_8);
              }
            else
              {
                uint8_t *value_8 = (uint8_t *) &quh.buffer[pos];
                ioctl (fp, KIOCSOUND, *value_8);
              }
            break;
            
          case 2:
          default:
            if (file->is_signed)
              {
                int16_t *value_16 = (int16_t *) &quh.buffer[pos];
                ioctl (fp, KIOCSOUND, *value_16);
              }
            else
              {
                uint16_t *value_16 = (uint16_t *) &quh.buffer[pos];
                ioctl (fp, KIOCSOUND, *value_16);
              }
            break;
#if 0            
          case 4:
            if (file->is_signed)
              {
                int32_t *value_32 = (int32_t *) &quh.buffer[pos];
                ioctl (fp, KIOCSOUND, *value_32);
              }
            else
              {
                uint32_t *value_32 = (uint32_t *) &quh.buffer[pos];
                ioctl (fp, KIOCSOUND, *value_32);
              }
            break;
#endif
        }

//      ioctl (fp, KIOCSOUND, 0);
    }

  return 0;
}


QUH_FILTER_OUT(quh_speaker_out, QUH_SPEAKER_OUT, "speaker")
#if 0
const st_filter_t quh_speaker_out =
{
  QUH_SPEAKER_OUT,
  "speaker",
  NULL,
  0,
  NULL,
  (int (*) (void *)) &quh_speaker_open,
  (int (*) (void *)) &quh_speaker_close,
  NULL,
  (int (*) (void *)) &quh_speaker_write,
  NULL,
  NULL,
  NULL,
  NULL
};
#endif


const st_getopt2_t quh_speaker_out_usage =
{
    "speaker", 0, 0, QUH_SPEAKER,
    NULL, "write to build-in PC speaker (if present)", (void *) QUH_SPEAKER_OUT
};
