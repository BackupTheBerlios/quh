/*
ffmpeg.c - ffmpeg support for Quh

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
#include <sys/stat.h>
#include <ctype.h>
#ifdef  USE_FFMPEG
#include <ffmpeg/avformat.h>
#include <ffmpeg/avcodec.h>
#endif
#include "misc/itypes.h"
#include "misc/bswap.h"
#include "misc/file.h"
#include "misc/getopt2.h"
#include "filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "ffmpeg.h"


#define MIN(a,b) ((a)<(b)?(a):(b))
// # number of samples to decode
#define FFMPEG_SAMPLES 100
#define INBUF_SIZE 4096

static AVOutputFormat *of = NULL;
static AVFormatContext *fc = NULL; // format context
static AVCodecContext *cc = NULL;  // codec context
static AVCodec *c = NULL;          // codec
static AVFrame *f = NULL;          // frame
static AVStream *audio_st;
static int audio = -1;             // # of stream
static uint8_t *rawData = NULL;
static int bytesDecoded = 0;
static FILE *fh = NULL;


int
quh_ffmpeg_init (st_quh_nfo_t * file)
{
  (void) file;

  // register all codecs, demux and protocols
  avcodec_init();

  avcodec_register_all();

  return 0;
}


int
quh_ffmpeg_open (st_quh_nfo_t * file)
{
  int i = 0;

  if (!(of = guess_format(NULL, file->fname, NULL)))
    return -1;

  if (!(fc = av_alloc_format_context ()))
    return -1;

  fc->oformat = of;
#if 0
  audio_st = NULL;
  if (fc->audio_codec != CODEC_ID_NONE)
    audio_st = add_audio_stream(of, fc->audio_codec);

  if (av_set_parameters(oc, NULL) < 0) // must be done
    return -1;

//#ifdef  DEBUG
  dump_format(of, 0, file->fname, 1);
//#endif

#endif


  if (av_open_input_file (fc, file->fname, NULL, 0, NULL) != 0)
    return -1;

  if (av_find_stream_info (fc) < 0)
    return -1;

  // find the first audio stream
  // TODO: support indexes for multiple audio streams
  audio = -1;
  for(; i < fc->nb_streams; i++)
    if(fc->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO)
      {
        audio = i;
        break;
      }

  if(audio == -1)
    return -1;

  // get a pointer to the codec context for the video stream
  cc = fc->streams[audio]->codec;

  // find the decoder
  if (!(c = avcodec_find_decoder (cc->codec_id)))
    return -1;

  // inform the codec that we can handle truncated bitstreams -- i.e.,
  // bitstreams where frame boundaries can fall in the middle of packets
  if (c->capabilities & CODEC_CAP_TRUNCATED)
    cc->flags|=CODEC_FLAG_TRUNCATED;

  // open codec
  if (avcodec_open (cc, c) < 0)
    return -1;

  if (!(fh = fopen (file->fname, "rb")))
    return -1;
  
#if 0
  if (0)
    {
      file->rate = nfo->rate;
      file->channels = nfo->channels;
      file->min_bitrate = nfo->bitrate_lower ? nfo->bitrate_lower : nfo->bitrate_nominal;
      file->max_bitrate = nfo->bitrate_upper ? nfo->bitrate_upper : nfo->bitrate_nominal;
    }
  else
#endif
    {
      file->rate = 44100;
      file->channels = 2;
      file->min_bitrate =
      file->max_bitrate = 0;
    }
  file->is_signed = 1;
//  file->size = bits / 8;
  file->size = 2;

//  if (length > 0)
//    file->raw_size = length * bits / 8 * file->channels;
//  file->raw_size = MIN (file->raw_size, 0x7fffffff);
  file->raw_size = 1024*1024;
  return 0;
}


int
quh_ffmpeg_close (st_quh_nfo_t * file)
{
  (void) file;

  fclose (fh);

  if (cc)
    avcodec_close (cc);

  av_close_input_file (fc);

  return 0;
}


int
quh_ffmpeg_demux (st_quh_nfo_t * file)
{
  if (file->source != QUH_SOURCE_FILE)
    return -1;

  if (!(of = guess_format(NULL, file->fname, NULL)))
    return -1;

  return 0;
}


int
quh_ffmpeg_seek (st_quh_nfo_t * file)
{
  (void) file;
  return 0;
}


static int
GetNextFrame (void)
{
  static AVPacket packet;
  static int bytesRemaining = 0;
  static int first = 1;
  int frameFinished = 0;

  // First time we're called, set packet.data to NULL to indicate it
  // doesn't have to be freed
  if (first)
    {
      first = 0;
      packet.data = NULL;
    }

  // Decode packets until we have decoded a complete frame
  while (1)
    {
      // Work on the current packet until we have decoded all of it
      while (bytesRemaining > 0)
        {
          // Decode the next chunk of data
          bytesDecoded = avcodec_decode_audio (cc, NULL, NULL, rawData, bytesRemaining);

          // Was there an error?
          if (bytesDecoded < 0)
            {
              fprintf (stderr, "Error while decoding frame\n");
              return 0;
            }

          bytesRemaining -= bytesDecoded;
          rawData += bytesDecoded;

          // Did we finish the current frame? Then we can return
          if (frameFinished)
            return 1;
        }

      // read the next packet, skipping all packets that aren't for this
      // stream
      do
        {
          // Free old packet
          if (packet.data != NULL)
            av_free_packet (&packet);

          // Read new packet
          if (av_read_packet (fc, &packet) < 0)
            goto loop_exit;
        }
      while (packet.stream_index != audio);

      bytesRemaining = packet.size;
      rawData = packet.data;
    }

loop_exit:

  // Decode the rest of the last frame
  bytesDecoded = avcodec_decode_audio (cc, NULL, NULL, rawData, bytesRemaining);

  // Free last packet
  if (packet.data != NULL)
    av_free_packet (&packet);

  return frameFinished != 0;
}

static     int out_size, size, len;

int
quh_ffmpeg_write (st_quh_nfo_t * file)
{
  (void) file;
    char inbuf[INBUF_SIZE], *inbuf_ptr = inbuf;
    uint8_t *outbuf = malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE);

#if 0
  if (!GetNextFrame())
    return -1;
#else
    for(;;) {
        size = fread(inbuf, 1, INBUF_SIZE, fh);
        if (size == 0)
            break;

        inbuf_ptr = inbuf;
        while (size > 0) {
            len = avcodec_decode_audio(cc, (short *)outbuf, &out_size,
                                       quh.buffer, size);
            if (len < 0) {
                fprintf(stderr, "Error while decoding\n");
                return -1;
            }
            if (out_size > 0) {
                /* if a frame has been decoded, output it */
//                fwrite(outbuf, 1, out_size, outfile);
break;
            }
            size -= len;
            inbuf_ptr += len;
        }
    }
#endif

  memcpy (quh.buffer, &outbuf, out_size);
  quh.buffer_len = out_size;

  return 0;
}


QUH_FILTER_IN (quh_ffmpeg_in, QUH_FFMPEG_IN, "ffmpeg (many)", ".mp3")
#if 0
const st_filter_t quh_ffmpeg_in = {
  QUH_FFMPEG_IN,
  "ffmpeg (many)",
  ".mp3",
  -1,
//  (int (*) (void *)) &quh_ffmpeg_demux,
  NULL,
  (int (*) (void *)) &quh_ffmpeg_open,
  (int (*) (void *)) &quh_ffmpeg_close,
  NULL,
  (int (*) (void *)) &quh_ffmpeg_write,
  (int (*) (void *)) &quh_ffmpeg_seek,
  NULL,
  (int (*) (void *)) &quh_ffmpeg_init,
  NULL
};
#endif


#if 0
const st_getopt2_t quh_ffmpeg_in_usage =
{
    "ogg", 1, 0, QUH_OGG,
    "FILE", "FILE is OGG (if it has no .ogg suffix)",
    (void *) QUH_VORBIS_IN
};
#endif
