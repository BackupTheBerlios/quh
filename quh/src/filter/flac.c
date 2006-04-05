/*
flac.h - flac support for Quh

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
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#ifdef  USE_FLAC
#include <FLAC/all.h>
#endif
#include "misc/bswap.h"
#include "misc/itypes.h"
#include "misc/file.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "flac.h"


#define BITRATE_HIST_SEGMENT_MSEC	500
/* 500ms * 50 = 25s should be enough */
#define BITRATE_HIST_SIZE		50
#define MAX_SUPPORTED_CHANNELS		2
#define SAMPLES_PER_WRITE		512
#define SAMPLE_BUFFER_SIZE ((FLAC__MAX_BLOCK_SIZE + SAMPLES_PER_WRITE) * MAX_SUPPORTED_CHANNELS * (24/8))


static int flac_abort = 0;
static FLAC__FileDecoder *decoder = NULL;


static void
metadata_callback (const FLAC__FileDecoder * decoder,
                   const FLAC__StreamMetadata * metadata, void *client_data)
{
  (void) decoder;
  st_quh_filter_t *file = (st_quh_filter_t *) client_data;

  if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO)
    {
      unsigned long total_samples =
        (unsigned) (metadata->data.stream_info.total_samples & 0xffffffff);

      file->size = metadata->data.stream_info.bits_per_sample / 8;
      file->channels = metadata->data.stream_info.channels;
      file->rate = metadata->data.stream_info.sample_rate;
      file->raw_size = total_samples * file->size * file->channels;
#ifdef  DEBUG
      printf ("%d samples: %d\n", file->raw_size, total_samples);
      fflush (stdout);
#endif
      file->min_bitrate = file->max_bitrate = 0; // flac is lossless
      file->is_signed = 1;
      file->seekable = QUH_SEEKABLE;
    }
}


// convert FLAC big-endian data into PCM little-endian.
static size_t
pack_pcm_signed (FLAC__byte * data,
                 const FLAC__int32 * const input[], unsigned wide_samples,
                 unsigned channels, unsigned bps)
{
  FLAC__byte *const start = data;
  FLAC__int32 sample;
  const FLAC__int32 *input_;
  unsigned samples, channel;
  const unsigned bytes_per_sample = bps / 8;
  const unsigned incr = bytes_per_sample * channels;

  for (channel = 0; channel < channels; channel++)
    {
      samples = wide_samples;
      data = start + bytes_per_sample * channel;
      input_ = input[channel];

      while (samples--)
        {
          sample = *input_++;

          switch (bps)
            {
            case 8:
              data[0] = sample ^ 0x80;
              break;

#ifdef WORDS_BIGENDIAN
            case 16:
              data[0] = (FLAC__byte) (sample >> 8);
              data[1] = (FLAC__byte) sample;
              break;
            case 24:
              data[0] = (FLAC__byte) (sample >> 16);
              data[1] = (FLAC__byte) (sample >> 8);
              data[2] = (FLAC__byte) sample;
              break;
#else
            case 24:
              data[2] = (FLAC__byte) (sample >> 16);
              /* fall through */
            case 16:
              data[1] = (FLAC__byte) (sample >> 8);
              data[0] = (FLAC__byte) sample;
#endif
            }

          data += incr;
        }
    }

  return wide_samples * channels * (bps / 8);
}


static FLAC__StreamDecoderWriteStatus
write_callback (const FLAC__FileDecoder * decoder,
                const FLAC__Frame * frame,
                const FLAC__int32 * const buffer[], void *client_data)
{
  (void) decoder;
  st_quh_filter_t *file = (st_quh_filter_t *) client_data;

  if (flac_abort)
    return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

  quh.buffer_len =
    pack_pcm_signed (quh.buffer, buffer, frame->header.blocksize,
                     file->channels, file->size * 8);

  return quh.buffer_len ? FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE : FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
}


static void
error_callback (const FLAC__FileDecoder * decoder,
                FLAC__StreamDecoderErrorStatus status, void *client_data)
{
  (void) decoder;
  (void) client_data;

  if (status != FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC)
    flac_abort = 1;
}


static int
quh_flac_open (st_quh_filter_t *file)
{
  if (!(decoder = FLAC__file_decoder_new ()))
    return -1;

  FLAC__file_decoder_set_md5_checking (decoder, false);
  if (!FLAC__file_decoder_set_filename (decoder, file->fname))
    return -1;

  FLAC__file_decoder_set_metadata_ignore_all (decoder);
  FLAC__file_decoder_set_metadata_respond (decoder, FLAC__METADATA_TYPE_STREAMINFO);
  FLAC__file_decoder_set_client_data (decoder, file);
  FLAC__file_decoder_set_metadata_callback (decoder, metadata_callback);
  FLAC__file_decoder_set_write_callback (decoder, write_callback);
  FLAC__file_decoder_set_error_callback (decoder, error_callback);

  if (FLAC__file_decoder_init (decoder) != FLAC__FILE_DECODER_OK)
    return -1;

  if (!FLAC__file_decoder_process_until_end_of_metadata (decoder))
    return -1;

  return 0;
}


static int
quh_flac_close (st_quh_filter_t *file)
{
  (void) file;

  FLAC__file_decoder_finish (decoder);
  FLAC__file_decoder_delete (decoder);
  
  return 0;
}


static int
quh_flac_seek (st_quh_filter_t *file)
{
  FLAC__uint64 target_sample = (FLAC__uint64) (quh.raw_pos / (file->size * file->channels));

#ifdef  DEBUG
  printf ("%d sample: %d\n", quh.raw_pos, target_sample);
  fflush (stdout);
#endif

  FLAC__file_decoder_seek_absolute (decoder, target_sample);

  return 0;
}


static int
quh_flac_write (st_quh_filter_t * file)
{
  (void) file;

  FLAC__uint64 decode_position;

  if (FLAC__file_decoder_get_state (decoder) == FLAC__FILE_DECODER_END_OF_FILE)
    return 0;

  if (!FLAC__file_decoder_process_single (decoder))
    return -1;

  if (!FLAC__file_decoder_get_decode_position (decoder, &decode_position))
   return 0;

  return 0;
}


int
quh_flac_demux (st_quh_filter_t * file)
{
  int result = 0;

  if (file->source != QUH_SOURCE_FILE)
    return -1;

  result = quh_flac_open (file);

  if (!result)
    quh_flac_close (file);

  return result;
}


const st_filter_t quh_flac_in = {
  QUH_FLAC_IN,
  "flac decode",
  ".flac.flc",
  -1,
  (int (*) (void *)) &quh_flac_demux,
  (int (*) (void *)) &quh_flac_open,
  (int (*) (void *)) &quh_flac_close,
  NULL,
  (int (*) (void *)) &quh_flac_write,
  (int (*) (void *)) &quh_flac_seek,
  NULL,
  NULL,
  NULL
};


const st_getopt2_t quh_flac_in_usage =
{
    "flac", 1, 0, QUH_FLAC,
    "FILE", "FILE is FLAC (if it has no .flac suffix)",
    (void *) QUH_FLAC_IN
};
