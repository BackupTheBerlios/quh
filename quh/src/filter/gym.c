/*
gym.c - Genesis/GYM support for Quh
        inspired by XymMS:  Sega Genesis GYM Input plugin for XMMS
                    Phillip K. Hornung <falcon@pknet.com>

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
#include <stdlib.h>
#include <sys/stat.h>
#include "misc/itypes.h"
#include "misc/misc.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "misc/file.h"
#include "misc/unzip.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "gym/ym2612.h"
#include "gym/ym2151.h"
#include "gym/sn76496.h"
#include "gym.h"


#define DUP_INIT 0x20000000
#define GYM_MAGIC_S "GYMX"

typedef struct
{
  unsigned char gym_id[4];
  char song_title[32];
  char game_title[32];
  char game_publisher[32];
  char ripped_with[32];
  char ripped_by[32];
  char comments[256];
  unsigned int looped;
  unsigned int compressed;
} GYMTAG;


/*
 * Settings from DGen
 * int ym2612_base = 7520000;
 * int sn76496_base = 3478000;
 *
 * Settings from MSP
 * int ym2612_base = 7670442;
 * int sn76496_base = 3579580;
 */
static int ym2612_clock = 100;
static int ym2612_base = 7670442;
static int sn76496_clock = 100;
static int sn76496_base = 3579580;

static int ext_samples_per_sec = 44100;
static int ext_bits_per_sample = 16;
static int ext_channels = 2;
//static int samples_menu[] = { 48000, 44100, 32000, 22050, 0 };
//static int channels_menu[] = { 2, 1, 0 };
//static char *name = NULL;
static unsigned char *ym_data = NULL;
static GYMTAG gymtag;
static int has_gymtag = 0;

//static int audio_error = 0;
static int is_playing = 0;
static int is_eof = 0;
static int seek_to = 0;
static int want_stop = 0;
//static int song_length = -1;
static int samples_per_sec = 44100;
static int bits_per_sample = 16;
static int channels = 2;
static int bitrate = 0;
static int compressed_filesize = 0;
static unsigned long uncompressed_filesize = 0;
//static int file_type = 0;


static short
mix (int a, int b)
{
  int c = (a + b) * 2;
  if (c > 32767)
    c = 32767;
  else if (c < -32768)
    c = -32768;
  return (short) c;
}


static int
get_gym_data_pos (int time_position)
{
  unsigned long loop = 0;
  unsigned int num_zeros = 0, num_zeros_target = 0;

  num_zeros_target = (int) ((time_position / 1000.0) * 60.0);
  for (loop = 0; loop < uncompressed_filesize; loop++)
    {
      if (num_zeros == num_zeros_target)
        break;
      switch (ym_data[loop])
        {
        case (0x00):
          num_zeros++;
          continue;
        case (0x01):
          loop += 2;
          continue;
        case (0x02):
          loop += 2;
          continue;
        case (0x03):
          loop += 1;
          continue;
        }
    }
  return loop;
}


#if 0
static int
calc_gym_time_length (unsigned char *data, int size)
{
  int loop, num_zeros = 0;

  for (loop = 0; loop < size; loop++)
    {
      switch (data[loop])
        {
        case (0x00):
          num_zeros++;
          continue;
        case (0x01):
          loop += 2;
          continue;
        case (0x02):
          loop += 2;
          continue;
        case (0x03):
          loop += 1;
          continue;
        }
    }
  return (int) ((num_zeros / 60.0) * 1000.0);
}


void
stop (void)
{
  is_playing = 0;
  want_stop = 1;
  is_eof = 1;
}


void
seek (int time)
{
  seek_to = (time * 1000);
}
#endif


int
quh_gym_open (const char *filename)
{
  FILE *file = NULL;
  int tmp_filesize;
//  int len;
  unsigned char *tmp_buf = NULL;
//  char buf[4];

  if (!(file = fopen (filename, "r")))
    return -1;

  memset (&gymtag, 0, sizeof (GYMTAG));

  tmp_filesize = fsizeof (filename);

  if (!(file = fopen (filename, "r")))
    return 0;

  fread (&gymtag, sizeof (GYMTAG), 1, file);

  if (memcmp (gymtag.gym_id, GYM_MAGIC_S, 4))
    has_gymtag = 1;

  if (has_gymtag)
    tmp_filesize -= sizeof (GYMTAG);

  tmp_buf = (unsigned char *) malloc (tmp_filesize);
  if (!tmp_buf)
    {
      fclose (file);
      return -1;
    }

  fread (tmp_buf, tmp_filesize, 1, file);

  fclose (file);

  if (gymtag.compressed)
    {
      int zerr;

      uncompressed_filesize = gymtag.compressed;
      compressed_filesize = tmp_filesize;
      ym_data = (unsigned char *) malloc (uncompressed_filesize);
      if (!ym_data)
        {
          if (tmp_buf)
            free (tmp_buf);
          tmp_buf = NULL;

          return -1;
        }

      zerr =
        uncompress (ym_data, &uncompressed_filesize,
                    tmp_buf, compressed_filesize);
      if (zerr != Z_OK)
        {
          if (tmp_buf)
            free (tmp_buf);
          tmp_buf = NULL;

          if (ym_data)
            free (ym_data);
          ym_data = NULL;

          return -1;
        }

      gymtag.compressed = uncompressed_filesize;
      if (tmp_buf)
        {
          free (tmp_buf);
          tmp_buf = NULL;
        }
    }
  else
    {
      uncompressed_filesize = tmp_filesize;
      compressed_filesize = 0;
      if (ym_data)
        free (ym_data);

      ym_data = tmp_buf;
    }

  is_eof = 0;
  is_playing = 1;
  want_stop = 0;
  seek_to = -1;
  samples_per_sec = ext_samples_per_sec;
  bits_per_sample = ext_bits_per_sample;
  channels = ext_channels;
#if 0
  if (has_gymtag && (*gymtag.song_title && *gymtag.game_title))
    {
      len =
        strlen (gymtag.game_title) + strlen (" - ") +
        strlen (gymtag.song_title) + 1;
      string = (char *) malloc (len);
      memset (string, 0, len);
      snprintf (string, len, "%s - %s", gymtag.game_title, gymtag.song_title);

    }
  else
    {
      string = (char *) malloc (strlen (filename) + 1);
      string = strdup (strrchr (filename, '/') + 1);
      *strrchr (string, '.') = 0;
      string = realloc (string, strlen (string) + 1);
    }
#endif
  bitrate = samples_per_sec * channels * ((bits_per_sample / 8));

  return 0;
}


/*
 * Most of this was courtesy YMAMP/MSP.
 */
int
quh_gym_write (void)
{
  int loop = 0, loop2 = 0, update = 0, adjtime = 0, time_pos = 0, srate =
    0, srate152 = 0, srate15 = 0, srate30 = 0, numsamples = 0;
  short *samp_buf = NULL, *ym_buf[2] = { NULL, NULL }, *snbuf = NULL;
  unsigned char *dac_data = NULL;
  unsigned long data_pos = 0;
  int dac_max = 0;

  srate = samples_per_sec;
  srate152 = (srate / 15) << 1;
  srate15 = srate / 15;
  srate30 = srate / 30;

  dac_data = (unsigned char *) malloc (srate30);
  if (!dac_data)
    {
      free (ym_data);
      want_stop = 1;
      return 0;
    }
  ym_buf[0] = (short *) malloc (srate15);
  if (!ym_buf[0])
    {
      free (ym_data);
      free (dac_data);
      want_stop = 1;
      return 0;
    }
  ym_buf[1] = (short *) malloc (srate15);
  if (!ym_buf[1])
    {
      free (ym_data);
      free (ym_buf[0]);
      want_stop = 1;
      return 0;
    }
  snbuf = (short *) malloc (srate15);
  if (!snbuf)
    {
      free (ym_data);
      free (ym_buf[0]);
      free (ym_buf[1]);
      want_stop = 1;
      return 0;
    }
  samp_buf = (short *) malloc (srate152 << 1);
  if (!samp_buf)
    {
      free (ym_data);
      free (ym_buf[0]);
      free (ym_buf[1]);
      free (snbuf);
      want_stop = 1;
      return 0;
    }
  do
    {
      loop =
        YM2612Init (1, ym2612_clock * ym2612_base / 100, srate, NULL, NULL);
      if (loop == DUP_INIT)
        YM2612Shutdown ();
      else if (loop)
        {
          YM2612Shutdown ();
          free (ym_data);
          free (ym_buf[0]);
          free (ym_buf[1]);
          free (snbuf);
          free (samp_buf);
          return 0;
        }
    }
  while (loop);
  if (SN76496_init (0, sn76496_clock * sn76496_base / 100, srate))
    {
      YM2612Shutdown ();
      free (ym_data);
      free (ym_buf[0]);
      free (ym_buf[1]);
      free (snbuf);
      free (samp_buf);
      return 0;
    }
  while (!want_stop && (data_pos < uncompressed_filesize))
    {
      if (seek_to != -1)
        {
          time_pos = seek_to - (seek_to % 60);
          data_pos = get_gym_data_pos (time_pos);
          adjtime = 0;
          dac_max = 0;
          seek_to = -1;
          continue;
        }
      switch (ym_data[data_pos])
        {
        case (0x00):
          data_pos++;
          if (update)
            {
              update = 0;
              if (dac_max)
                {
                  short *dac_buf[2];
                  double update_cycle = (double) srate30 / dac_max;     /* Dac port write cycle */
                  double stream_cnt = 0;
                  for (loop = 0; loop < dac_max; loop++)
                    {
                      int old_cnt = (int) stream_cnt;
                      int step;
                      /* Update DAC port */
                      YM2612Write (0, 0, 0x2a);
                      YM2612Write (0, 1, dac_data[loop]);
                      if (loop == dac_max - 1)
                        step = srate30 - old_cnt;
                      else
                        {
                          stream_cnt += update_cycle;
                          step = (int) stream_cnt - old_cnt;
                        }
                      /* Update stream */
                      dac_buf[0] = &ym_buf[0][old_cnt];
                      dac_buf[1] = &ym_buf[1][old_cnt];
                      YM2612UpdateOne (0, (void **) dac_buf, step);
                    }
                  dac_max = 0;
                }
              else
                YM2612UpdateOne (0, (void **) ym_buf, srate30);

              SN76496Update_16 (0, snbuf, srate30);
              if (channels == 2)
                {
                  loop2 = 0;
                  for (loop = 0; loop < srate30; loop++)
                    {
                      samp_buf[loop2] =
                        mix (ym_buf[0][loop], snbuf[loop] >> 1);
                      loop2++;
                      samp_buf[loop2] =
                        mix (ym_buf[1][loop], snbuf[loop] >> 1);
                      loop2++;
                    }
                }
              else
                for (loop = 0; loop < srate30; loop++)
                  samp_buf[loop] =
                    mix ((ym_buf[0][loop] + ym_buf[1][loop]) >> 1,
                         snbuf[loop] >> 1);

//                  xmms_gym_ip.add_vis_pcm(xmms_gym_ip.output->written_time(), (bits_per_sample == 16)
//                                          ? FMT_S16_LE : FMT_U8, channels, srate, samp_buf);
              numsamples = srate30;
              if (channels == 1)
                numsamples >>= 1;
              if (adjtime == 2)
                {
                  time_pos += 34;
                  adjtime = 0;
                }
              else
                {
                  time_pos += 33;
                  adjtime++;
                }
              if (numsamples > srate30)
                {
                  numsamples -= srate30;
                  fwrite (&samp_buf[srate15], 1, numsamples << 2, stdout);
                }
              else
                {
                  fwrite (samp_buf, 1, numsamples << 2, stdout);
                }
            }
          else
            update = 1;
          break;
        case (0x01):
          data_pos++;
          if (ym_data[data_pos] == 0x2a)
            {
              data_pos++;
              if (dac_max < srate30)
                dac_data[dac_max++] = ym_data[data_pos];

              data_pos++;
            }
          else
            {
              YM2612Write (0, 0, ym_data[data_pos++]);
              YM2612Write (0, 1, ym_data[data_pos++]);
            }
          break;
        case (0x02):
          data_pos++;
          YM2612Write (0, 2, ym_data[data_pos++]);
          YM2612Write (0, 3, ym_data[data_pos++]);
          break;
        case (0x03):
          data_pos++;
          SN76496Write (0, ym_data[data_pos++]);
          break;

        default:               /* Saves us from bad data */
          data_pos++;
          break;
        }
    }
  YM2612Shutdown ();
  free (ym_data);
  free (ym_buf[0]);
  free (ym_buf[1]);
  free (snbuf);
  free (samp_buf);
  ym_buf[0] = ym_buf[1] = snbuf = NULL;
  ym_data = NULL;

  time_pos = 0;
  want_stop = 1;
  return 0;
}


static int
quh_gym_in_open (st_quh_nfo_t *file)
{
  quh_gym_open (file->fname);
  quh_gym_write();

  return 0;
}


static int
quh_gym_in_close (st_quh_nfo_t *file)
{
  (void) file;

  return 0;
}


static int
quh_gym_in_seek (st_quh_nfo_t *file)
{
  (void) file;

  return 0;
}


static int
quh_gym_in_demux (st_quh_nfo_t *file)
{
  (void) file;

  return 0;
}


static int
quh_gym_in_write (st_quh_nfo_t *file)
{
  (void) file;

//  quh.buffer_len = fread (&quh.buffer, 1, QUH_MAXBUFSIZE / 4, wav_out);

  return 0;
}


const st_filter_t quh_gym_in =
{
  QUH_GYM_IN,
  "gym (Genesis)",
  ".gym",
  -1,
  (int (*) (void *)) &quh_gym_in_demux,
  (int (*) (void *)) &quh_gym_in_open,
  (int (*) (void *)) &quh_gym_in_close,
  NULL,
  (int (*) (void *)) &quh_gym_in_write,
  (int (*) (void *)) &quh_gym_in_seek,
  NULL,
  NULL,
  NULL
};


const st_getopt2_t quh_gym_in_usage =
{
    "gym", 1, 0, QUH_GYM,
    "FILE", "FILE is GYM (if it has no .wav suffix)",
    (void *) QUH_GYM_IN
};
