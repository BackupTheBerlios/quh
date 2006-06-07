/*
wavpack.c - new filter wavpack for Quh

Copyright (c) 2006 NoisyB


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
#include <wavpack/wavpack.h>
#include "misc/itypes.h"
#include "misc/bswap.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "wavpack.h"


static WavpackContext *wpc = NULL;
#define NO_ERROR 0L
#define SOFT_ERROR 1
#define HARD_ERROR 2


// Reformat samples from longs in processor's native endian mode to
// little-endian data with (possibly) less than 4 bytes / sample.
static uchar *
format_samples (int bps, uchar * dst, int32_t * src, uint32_t samcnt)
{
  int32_t temp;

  switch (bps)
    {

    case 1:
      while (samcnt--)
        *dst++ = *src++ + 128;

      break;

    case 2:
      while (samcnt--)
        {
          *dst++ = (uchar) (temp = *src++);
          *dst++ = (uchar) (temp >> 8);
        }

      break;

    case 3:
      while (samcnt--)
        {
          *dst++ = (uchar) (temp = *src++);
          *dst++ = (uchar) (temp >> 8);
          *dst++ = (uchar) (temp >> 16);
        }

      break;

    case 4:
      while (samcnt--)
        {
          *dst++ = (uchar) (temp = *src++);
          *dst++ = (uchar) (temp >> 8);
          *dst++ = (uchar) (temp >> 16);
          *dst++ = (uchar) (temp >> 24);
        }

      break;
    }

  return dst;
}


static int
quh_wavpack_in_open (st_quh_nfo_t * file)
{
  int open_flags = 0;
  char error[80];

  int result = 0, md5_diff =
    FALSE, bytes_per_sample, num_channels, wvc_mode, bps;
  uint32_t outfile_length, output_buffer_size, bcount,
    total_unpacked_samples = 0;
  uchar *output_buffer = NULL, *output_pointer = NULL;
  double dtime, progress = -1.0;
  WavpackContext *wpc;
  int32_t *temp_buffer;
  FILE *outfile;

//  open_flags |= OPEN_WRAPPER;
  open_flags |= OPEN_STREAMING;
//  open_flags |= OPEN_WVC;
//  open_flags |= OPEN_TAGS;

  if (!(wpc = WavpackOpenFileInput (file->fname, error, open_flags, 0)))
    return -1;

  wvc_mode = WavpackGetMode (wpc) & MODE_WVC;
  num_channels = WavpackGetNumChannels (wpc);
  bps = WavpackGetBytesPerSample (wpc);
  bytes_per_sample = num_channels * bps;

#if 0
  if (!WavpackGetWrapperBytes (wpc) && outfilename && !raw_decode)
    {
      error_line ("no wav header, can only decode to raw file (use -r)!");
      WavpackCloseFile (wpc);
      return SOFT_ERROR;
    }


  if (WavpackGetWrapperBytes (wpc))
    {
      if (outfile
          &&
          (!DoWriteFile
           (outfile, WavpackGetWrapperData (wpc),
            WavpackGetWrapperBytes (wpc), &bcount)
           || bcount != WavpackGetWrapperBytes (wpc)))
        {
          error_line ("can't write .WAV data, disk probably full!");
          DoTruncateFile (outfile);
          result = HARD_ERROR;
        }

      WavpackFreeWrapper (wpc);
    }
#endif
  temp_buffer = malloc (4096L * num_channels * 4);

  while (result == NO_ERROR)
    {
      uint32_t samples_to_unpack, samples_unpacked;

      if (output_buffer)
        {
          samples_to_unpack =
            (output_buffer_size -
             (output_pointer - output_buffer)) / bytes_per_sample;

          if (samples_to_unpack > 4096)
            samples_to_unpack = 4096;
        }
      else
        samples_to_unpack = 4096;

      samples_unpacked =
        WavpackUnpackSamples (wpc, temp_buffer, samples_to_unpack);
      total_unpacked_samples += samples_unpacked;

      if (output_buffer)
        {
          if (samples_unpacked)
            output_pointer = format_samples (bps, output_pointer, temp_buffer,
                                             samples_unpacked * num_channels);

          if (!samples_unpacked ||
              (output_buffer_size - (output_pointer - output_buffer)) < bytes_per_sample)
            {
              if (!DoWriteFile
                  (outfile, output_buffer, output_pointer - output_buffer,
                   &bcount) || bcount != output_pointer - output_buffer)
                {
                  error_line ("can't write .WAV data, disk probably full!");
                  DoTruncateFile (outfile);
                  result = HARD_ERROR;
                  break;
                }

              output_pointer = output_buffer;
            }
        }

      if (!samples_unpacked)
        break;

#if 0
      if (WavpackGetProgress (wpc) != -1.0 &&
          progress != floor (WavpackGetProgress (wpc) * 100.0 + 0.5))
        {
          int nobs = progress == -1.0;

          progress = WavpackGetProgress (wpc);
          display_progress (progress);
          progress = floor (progress * 100.0 + 0.5);

          if (!quiet_mode)
            fprintf (stderr, "%s%3d%% done...",
                     nobs ? " " : "\b\b\b\b\b\b\b\b\b\b\b\b", (int) progress);
        }
#endif
    }

  free (temp_buffer);

  return 0;
}


static int
quh_wavpack_in_close (st_quh_nfo_t * file)
{
  return 0;
}


static int
quh_wavpack_in_seek (st_quh_nfo_t * file)
{
  return 0;
}


static int
quh_wavpack_in_demux (st_quh_nfo_t * file)
{
  return 0;
}


static int
quh_wavpack_in_write (st_quh_nfo_t * file)
{
  return 0;
}


const st_filter_t quh_wavpack_in = {
  QUH_WAVPACK_IN,
  "wavpack",
  ".wv",
  -1,
  (int (*)(void *)) &quh_wavpack_in_demux,
  (int (*)(void *)) &quh_wavpack_in_open,
  (int (*)(void *)) &quh_wavpack_in_close,
  NULL,
  (int (*)(void *)) &quh_wavpack_in_write,
  (int (*)(void *)) &quh_wavpack_in_seek,
  NULL,
  NULL,
  NULL
};


const st_getopt2_t quh_wavpack_in_usage = {
  "wavpack", 1, 0, QUH_WAVPACK,
  "FILE", "FILE is WAVPACK (if it has no .wavpack suffix)",
  (void *) QUH_WAVPACK_IN
};
