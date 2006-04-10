/*
vol.c - vol filter for Quh

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
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "misc/itypes.h"
#include "misc/file.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "misc/string.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "vol.h"

#warning TODO: fix

static int
quh_vol_init (st_quh_nfo_t *file)
{
  (void) file;

  if (!quh_get_object_s (quh.filter_chain, QUH_OPTION))
    quh_set_object_s (quh.filter_chain, QUH_OPTION, "100");
      
  return 0;
}


static int
quh_vol_write (st_quh_nfo_t * file)
{
  (void) file;
#if 0
  unsigned long pos = 0;
  float factor = (float) strtol ((const char *) quh_get_object_s (quh.filter_chain, QUH_OPTION), NULL, 10) / 100;

  for (; pos < quh.buffer_len; pos += file->size)// * file->channels)
    {
      switch (file->size)
        {
          case 1:
            if (file->is_signed)
              {
                int8_t *value_8 = (int8_t *) &quh.buffer[pos];
                *value_8 = (int8_t) *value_8 * factor;
              }
            else
              {
                uint8_t *value_8 = (uint8_t *) &quh.buffer[pos];
                *value_8 = (uint8_t) *value_8 * factor;
              }
            break;
            
          case 4:
            if (file->is_signed)
              {
                int32_t *value_32 = (int32_t *) &quh.buffer[pos];
                *value_32 = (int32_t) *value_32 * factor;
              }
            else
              {
                uint32_t *value_32 = (uint32_t *) &quh.buffer[pos];
                *value_32 = (uint32_t) *value_32 * factor;
              }
            break;

          case 2:
          default:
            if (file->is_signed)
              {
                int16_t *value_16 = (int16_t *) &quh.buffer[pos];
                *value_16 = (uint16_t) *value_16 * factor;
              }
            else
              {
                uint16_t *value_16 = (uint16_t *) &quh.buffer[pos];
                *value_16 = (uint16_t) *value_16 * factor;
              }
            break;
        }
    }

#endif
  return 0;
}


#if 0
static int
quh_vol_write (st_quh_nfo_t * file)
{
  unsigned long pos = 0;
  float factor = (float) strtol ((const char *) quh_get_object_s (quh.filter_chain, QUH_OPTION), NULL, 10) / 100;

  for (; pos < quh.buffer_len; pos ++)
    {
       quh.buffer[pos] *= factor;
    }
  return 0;
}
#endif

const st_filter_t quh_vol = {
  QUH_VOL_PASS,
  "vol",
  NULL,
  0,
  NULL,
  NULL,
  NULL,
  NULL,
  (int (*) (void *)) &quh_vol_write,
  NULL,
  NULL,
  (int (*) (void *)) &quh_vol_init,
  NULL
};


const st_getopt2_t quh_vol_usage =
{
    "vol", 1, 0, QUH_VOL,
    "PERCENT", "change volume for all channels in PERCENT", 
    (void *) QUH_VOL_PASS
};
