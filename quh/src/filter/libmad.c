/*
mp3_mad.c - mp3 support for Quh (using libmad)

written by 2005 Dirk (d_i_r_k_@gmx.net)

based on mp3.c from MOC - music on console
   Copyright (C) 2002-2004 Damian Pietras <daper@daper.net>


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
#ifdef  HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef  USE_MAD
#include <mad.h>
#endif // USE_MAD
#include "misc/bswap.h"
#include "misc/itypes.h"
#include "misc/file.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "misc/string.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "libmad.h"
#define USE_XING
#ifdef  USE_XING
#include "xing.h"
#endif


static struct mad_stream stream;
static struct mad_frame frame;
static struct mad_synth synth;

static unsigned long bit_rate = 0;
static signed long raw_size; /* Total time of the file in seconds.
                         used for seeking. */
static off_t size; /* Size of the file */
static int skip_frames = 0;
static int infile = 0;
#ifdef HAVE_MMAP
static char *mapped = NULL;
static int mapped_size;
#endif
/* Used only if mmap() is not available */
#define INPUT_BUFFER    (64 * 1024)
static unsigned char in_buff[INPUT_BUFFER];


/* Fill in the mad buffer, return number of bytes read, 0 on eof or error */
static size_t
fill_buff (void)
{
  size_t remaining;
  ssize_t read_size;
  unsigned char *read_start;

  if (stream.next_frame != NULL)
    {
      remaining = stream.bufend - stream.next_frame;
      memmove (in_buff, stream.next_frame, remaining);
      read_start = in_buff + remaining;
      read_size = INPUT_BUFFER - remaining;
    }
  else
    {
      read_start = in_buff;
      read_size = INPUT_BUFFER;
      remaining = 0;
    }

  read_size = read (infile, read_start, read_size);
  if (read_size < 0)
    {
      printf ("read() failed:\n");
      return 0;
    }
  else if (read_size == 0)
    return 0;

  mad_stream_buffer (&stream, in_buff, read_size + remaining);
  stream.error = 0;

  return read_size;
}


static int
count_time_internal (st_quh_filter_t * file)
{
  char buf[MAXBUFSIZE];
  struct xing xing;
  unsigned long bitrate = 0;
  int has_xing = 0;
  int is_vbr = 0;
  int num_frames = 0;
  mad_timer_t duration = mad_timer_zero;
  struct mad_header header;
  int good_header = 0;          /* Have we decoded any header? */

  mad_header_init (&header);
  xing_init (&xing);

  /* There are three ways of calculating the length of an mp3:
     1) Constant bitrate: One frame can provide the information
     needed: # of frames and duration. Just see how long it
     is and do the division.
     2) Variable bitrate: Xing tag. It provides the number of 
     frames. Each frame has the same number of samples, so
     just use that.
     3) All: Count up the frames and duration of each frames
     by decoding each one. We do this if we've no other
     choice, i.e. if it's a VBR file with no Xing tag.
   */

  while (1)
    {

      /* Fill the input buffer if needed */
      if (stream.buffer == NULL ||
          stream.error == MAD_ERROR_BUFLEN)
        {
#ifdef HAVE_MMAP
          if (mapped)
            break;              /* FIXME: check if the size of file
                                   has't changed */
          else
#endif
          if (!fill_buff ())
            break;
        }

      if (mad_header_decode (&header, &stream) == -1)
        {
          if (MAD_RECOVERABLE (stream.error))
            continue;
          else if (stream.error == MAD_ERROR_BUFLEN)
            continue;
          else
            {
//              printf ("Can't decode header: %s",
//                     mad_stream_errorstr (&stream));
              break;
            }
        }

      good_header = 1;

      /* Limit xing testing to the first frame header */
      if (!num_frames++)
        {
          if (xing_parse (&xing, stream.anc_ptr,
                          stream.anc_bitlen) != -1)
            {
              is_vbr = 1;

//              printf ("Has XING header");

              if (xing.flags & XING_FRAMES)
                {
                  has_xing = 1;
                  num_frames = xing.frames;
                  break;
                }
//              printf ("XING header doesn't contain number of " "frames.");
            }
        }

      /* Test the first n frames to see if this is a VBR file */
      if (!is_vbr && !(num_frames > 20))
        {
          if (bitrate && header.bitrate != bitrate)
            {
//              printf ("Detected VBR after %d frames", num_frames);
              is_vbr = 1;
            }
          else
            bitrate = header.bitrate;
        }

      /* We have to assume it's not a VBR file if it hasn't already
       * been marked as one and we've checked n frames for different
       * bitrates */
      else if (!is_vbr)
        {
//          printf ("Fixed rate MP3");
          break;
        }

      mad_timer_add (&duration, header.duration);
    }

  if (!good_header)
    return -1;

  if (!is_vbr)
    {
      /* time in seconds */
      double time = (size * 8.0) / (header.bitrate);

      double timefrac = (double) time - ((long) (time));

      /* samples per frame */
      long nsamples = 32 * MAD_NSBSAMPLES (&header);

      /* samplerate is a constant */
      num_frames = (long) (time * header.samplerate / nsamples);

      mad_timer_set (&duration, (long) time, (long) (timefrac * 100), 100);
    }
  else if (has_xing)
    {
      mad_timer_multiply (&header.duration, num_frames);
      duration = header.duration;
    }
  else
    {
      /* the durations have been added up, and the number of frames
         counted. We do nothing here. */
//      printf ("Counted duration by counting frames durations in " "VBR file.");
    }


  file->rate = header.samplerate;
  file->channels = MAD_NCHANNELS (&header);
  file->size = 2;
  file->is_signed = TRUE;
  file->raw_size = quh_ms_to_bytes (file, mad_timer_count (duration, MAD_UNITS_MILLISECONDS));
  file->seekable = QUH_SEEKABLE;
  *buf = 0;
  if (has_xing)
    strcpy (buf, "Xing tag: Yes");
  else
    strcpy (buf, "Xing tag: No");
    
  if (is_vbr)
    {
#warning min and max vbr -> st_quh_filter_t
    }
    
  quh_set_object_s (quh.filter_chain, QUH_OUTPUT, buf);
                                                                                                                            
  mad_header_finish (&header);

  return mad_timer_count (duration, MAD_UNITS_SECONDS);
}


int
quh_libmad_init (st_quh_filter_t * file)
{
  (void) file;

//  mad_stream_init (&stream);
//  mad_frame_init (&frame);
//  mad_synth_init (&synth);

  return 0;
}      
      

int
quh_libmad_quit (st_quh_filter_t * file)
{
  (void) file;

//  mad_stream_finish (&stream);
//  mad_frame_finish (&frame);
//  mad_synth_finish (&synth);
                  
  return 0;
}


int
quh_libmad_open (st_quh_filter_t * file)
{
  if ((infile = open (file->fname, O_RDONLY)) == -1)
    return -1;

  size = fsizeof (file->fname);

  raw_size = count_time_internal (file);

  if (raw_size < 1) // failed == no mp3
    {
      close (infile);
  
      return -1;
    }

  mad_stream_init (&stream);
  mad_frame_init (&frame);
  mad_synth_init (&synth);

  mad_frame_mute (&frame);
  stream.next_frame = NULL;
  stream.sync = 0;
  stream.error = MAD_ERROR_NONE;

      if (lseek (infile, SEEK_SET, 0) == (off_t) - 1)
        {
//          printf ("lseek() failed:");
          close (infile);
          return -1;
        }

      stream.error = MAD_ERROR_BUFLEN;


//  quh_demux_sanity_check (file);

  return 0;
}


static int
quh_libmad_close (st_quh_filter_t * file)
{
  (void) file;

  mad_stream_finish (&stream);
  mad_frame_finish (&frame);
  mad_synth_finish (&synth);
      
  close (infile);

  return 0;
}


/* Scale PCM data to 16 bit unsigned */
static inline signed int
scale (mad_fixed_t sample)
{
  /* round */
  sample += (1L << (MAD_F_FRACBITS - 16));

  /* clip */
  if (sample >= MAD_F_ONE)
    sample = MAD_F_ONE - 1;
  else if (sample < -MAD_F_ONE)
    sample = -MAD_F_ONE;

  /* quantize */
  return sample >> (MAD_F_FRACBITS + 1 - 16);
}


static int
put_output (char *buf, int buf_len, struct mad_pcm *pcm,
            struct mad_header *header)
{
  unsigned int nsamples;
  mad_fixed_t const *left_ch, *right_ch;
  int olen;
  int pos = 0;

  nsamples = pcm->length;
  left_ch = pcm->samples[0];
  right_ch = pcm->samples[1];
  olen = nsamples * MAD_NCHANNELS (header) * 2;

  if (olen > buf_len)
    {
      printf ("PCM buffer to small!");
      return 0;
    }

  while (nsamples--)
    {
      signed int sample;

      /* output sample(s) in 16-bit signed little-endian PCM */
      sample = scale (*left_ch++);

#ifdef WORDS_BIGENDIAN
      buf[pos++] = (sample >> 8) & 0xff;
      buf[pos++] = (sample >> 0) & 0xff;
#else
      buf[pos++] = (sample >> 0) & 0xff;
      buf[pos++] = (sample >> 8) & 0xff;
#endif

      if (MAD_NCHANNELS (header) == 2)
        {
          sample = scale (*right_ch++);
#ifdef WORDS_BIGENDIAN
          buf[pos++] = (sample >> 8) & 0xff;
          buf[pos++] = (sample >> 0) & 0xff;
#else
          buf[pos++] = (sample >> 0) & 0xff;
          buf[pos++] = (sample >> 8) & 0xff;
#endif
        }
    }

//  return olen;
  quh.buffer_len = olen;
  return 0;
}


static int
quh_libmad_demux (st_quh_filter_t *file)
{
  int result = -1;
  struct stat fstate;
  
  if (file->fname[0])
    if (!stat (file->fname, &fstate))
      if (!S_ISREG (fstate.st_mode))
        return -1;

  result = quh_libmad_open (file);

  if (!result)
    quh_libmad_close (file);

  return result;
}


static int
quh_libmad_write (st_quh_filter_t *file)
{
  while (1)
    {

      /* Fill the input buffer if needed */
      if (stream.buffer == NULL ||
          stream.error == MAD_ERROR_BUFLEN)
        {
          if (!fill_buff ())
            return -1;
        }

      if (mad_frame_decode (&frame, &stream))
        {
          if (MAD_RECOVERABLE (stream.error))
            {

              /* Ignore LOSTSYNC */
              if (stream.error == MAD_ERROR_LOSTSYNC)
                continue;
//return 0; // skip

//              if (options_get_int ("ShowStreamErrors") && !skip_frames)
//                error ("Broken frame: %s",
//                       mad_stream_errorstr (&stream));

              continue;
//return 0; // skip
            }
          else if (stream.error == MAD_ERROR_BUFLEN)
            continue;
//return 0; // skip
          else
            {
//              if (options_get_int ("ShowStreamErrors"))
//                error ("Broken frame: %s",
//                       mad_stream_errorstr (&stream));
              return 0;
            }
        }

      if (skip_frames)
        {
          skip_frames--;
          continue;
//return 0; // skip
        }

      /* Sound parameters. */
      if (!(file->rate = frame.header.samplerate))
        {
//          error ("Broken file: information about the frequency "
//                 "couldn't be read.");
          return 0;
        }
      /* Change of the bitrate? */
      if (frame.header.bitrate != bit_rate)
        {
          if ((bit_rate = frame.header.bitrate) == 0)
            {
//              error ("Broken file: information "
//                     "about the bitrate couldn't " "be read.\n");
               return 0;
            }
        }

      mad_synth_frame (&synth, &frame);
      mad_stream_sync (&stream);

      put_output ((char *) quh.buffer, QUH_MAXBUFSIZE, &synth.pcm, &frame.header);

      return 0;
    }
}


static int
quh_libmad_seek (st_quh_filter_t *file)
{
  int new_position;

  new_position = ((double) quh.raw_pos / (double) file->raw_size) * size;
  
  if (new_position < 0)
    new_position = 0;
  else if (new_position >= size)
    return -1;

      if (lseek (infile, new_position, SEEK_SET) == -1)
        {
//          error ("Failed to seek to: %d", new_position);
          return -1;
        }
      stream.error = MAD_ERROR_BUFLEN;

      mad_frame_mute (&frame);
      mad_synth_mute (&synth);

      stream.sync = 0;
      stream.next_frame = NULL;


  skip_frames = 2;

  return 0;
}


const st_filter_t quh_libmad_in = {
  QUH_LIBMAD_IN,
  "mp3 decode (using libmad)",
  ".mp3",
  strlen (".mp3"),
  (int (*) (void *)) &quh_libmad_open,
  (int (*) (void *)) &quh_libmad_close,
  (int (*) (void *)) &quh_libmad_demux,
  (int (*) (void *)) &quh_libmad_write,
  (int (*) (void *)) &quh_libmad_seek,
  NULL,
  (int (*) (void *)) &quh_libmad_init,
  (int (*) (void *)) &quh_libmad_quit,
};


#if 0
const st_getopt2_t quh_libmad_in_usage = {
  "mp3_mad", 1, 0, QUH_MP3,
  "FILE", "FILE is MP3 (if it has no .mp3_mad suffix)",
  (void *) QUH_LIBMAD_IN
};
#endif
