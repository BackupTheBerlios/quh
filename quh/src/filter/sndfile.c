/*
sndfile.c - sndfile support for Quh

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
#include <sys/stat.h>
#ifdef  USE_SNDFILE
#include <sndfile.h>
#endif
#include "misc/itypes.h"
#include "misc/file.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "sndfile.h"


SNDFILE *sndfile = NULL;
SF_INFO snd_info;


static int
quh_sndfile_open (st_quh_nfo_t *file)
{
  if (!(sndfile = sf_open (file->fname, SFM_READ, &snd_info)))
    return -1;

#if 0
  if (snd_info.channels > 2)
    {
      // more than 2 channels are not supported
      sf_close (sndfile);
      return -1;
    }
#endif

  switch (snd_info.format)
    {
      case SF_FORMAT_PCM_S8: /* Signed 8 bit data */
        file->size = 1;
        file->is_signed = 1;
        break;

      case SF_FORMAT_PCM_24: /* Signed 24 bit data */
        file->size = 3;
        file->is_signed = 1;
        break;

      case SF_FORMAT_PCM_32:  /* Signed 32 bit data */
        file->size = 4;
        file->is_signed = 1;
        break;

      case SF_FORMAT_PCM_U8: /* Unsigned 8 bit data (WAV and RAW only) */
        file->size = 1;
        file->is_signed = 0;
        break;

      case SF_FORMAT_PCM_16: /* Signed 16 bit data */
      default:
        file->size = 2;
        file->is_signed = 1;
        break;
    }
  file->channels = snd_info.channels;
  file->rate = snd_info.samplerate;
  file->raw_size = snd_info.frames * file->size * file->channels;
  file->seekable = QUH_SEEKABLE;
                                            
  return 0;
}


static int
quh_sndfile_close (st_quh_nfo_t *file)
{
  (void) file;

  sf_close (sndfile);

  return 0;
}


static int
quh_sndfile_seek (st_quh_nfo_t *file)
{
  (void) file;
  int result;

  result = sf_seek (sndfile, quh.raw_pos, SEEK_SET);

  if (result < 0)
    return -1;

  return 0;
}


int
quh_sndfile_demux (st_quh_nfo_t * file)
{
  int result = 0;

  if (file->source != QUH_SOURCE_FILE)
    return -1;

  result = quh_sndfile_open (file);

  if (!result)
    quh_sndfile_close (file);

  return result;
}


static int
quh_sndfile_write (st_quh_nfo_t * file)
{
  (void) file;

  quh.buffer_len = sf_read_raw (sndfile, quh.buffer, QUH_MAXBUFSIZE);

  return 0;
}


/*
Version : libsndfile-1.0.11

AIFF (Apple/SGI)  (extension "aiff")

AU (Sun/NeXT)  (extension "au")

AVR (Audio Visual Research)  (extension "avr")

HTK (HMM Tool Kit)  (extension "htk")

IFF (Amiga IFF/SVX8/SV16)  (extension "iff")

MAT4 (GNU Octave 2.0 / Matlab 4.2)  (extension "mat")

MAT5 (GNU Octave 2.1 / Matlab 5.0)  (extension "mat")

PAF (Ensoniq PARIS)  (extension "paf")

PVF (Portable Voice Format)  (extension "pvf")

RAW (header-less)  (extension "raw")

SDS (Midi Sample Dump Standard)  (extension "sds")

SF (Berkeley/IRCAM/CARL)  (extension "sf")

VOC (Creative Labs)  (extension "voc")

W64 (SoundFoundry WAVE 64)  (extension "w64")

WAV (Microsoft)  (extension "wav")

WAV (NIST Sphere)  (extension "wav")

WAVEX (Microsoft)  (extension "wav")

XI (FastTracker 2)  (extension "xi")
*/


const st_filter_t quh_sndfile_in = {
  QUH_SNDFILE_IN,
  "sndfile (aiff, au, avr, htk, iff, mat, paf, pvf, raw, sds, sf, voc, w64, wav, xi)",
  ".aiff.au.avr.htk.iff.mat.paf.pvf.raw.sds.sf.voc.w64.wav.xi",
  -1,
//  (int (*) (void *)) &quh_sndfile_demux,
  NULL,
  (int (*) (void *)) &quh_sndfile_open,
  (int (*) (void *)) &quh_sndfile_close,
  NULL,
  (int (*) (void *)) &quh_sndfile_write,
  (int (*) (void *)) &quh_sndfile_seek,
  NULL,
  NULL,
  NULL
};


const st_getopt2_t quh_sndfile_in_usage =
{
    "sndfile", 1, 0, QUH_SNDFILE,
    "FILE", "FILE is FLAC (if it has no .sndfile suffix)",
    (void *) QUH_SNDFILE_IN 
};
