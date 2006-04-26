/*
flite.c - flite support for Quh (using flite speech synthesis)

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
#include <flite/flite.h>
#include <flite/flite_version.h>
#include "misc/itypes.h"
#include "misc/misc.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "misc/file.h"
#include "misc/string.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "wav.h"


#define MAX(a,b) ((a)>(b)?(a):(b))


static int init = 0;
static FILE *fp = NULL;


static void
quh_flite_decode_to_wav (st_quh_nfo_t *file, const char *fname)
{
  cst_voice *register_cmu_us_kal ();

//  cst_features *extra_feats = new_features ();
//  cst_voice *v = new_voice();
  cst_voice *v = register_cmu_us_kal ();

//  feat_copy_into (extra_feats, v->features);

  flite_file_to_speech (file->fname, v, fname);

//  delete_features (extra_feats);
  delete_voice (v);
}


static int
quh_flite_open (st_quh_nfo_t *file)
{
  static char buf[MAXBUFSIZE];
  st_wav_header_t wav_header;

  // a device?
  if (file->source != QUH_SOURCE_FILE)
    return -1;

  if (!init)
    {
      flite_init ();
      init = 1;
    }

  if (!quh_forked_wav_decode (file, quh_flite_decode_to_wav))
    return -1;

  if (!(fp = fopen (quh.tmp_file, "rb")))
    return -1;

  fread (&wav_header, 1, sizeof (st_wav_header_t), fp);
  
  if (!memcmp (wav_header.magic, "RIFF", 4))
    {
      quh.raw_pos = sizeof (st_wav_header_t);
      file->raw_size = fsizeof (quh.tmp_file) - quh.raw_pos;
      file->rate = wav_header.freq;
      file->channels = wav_header.channels;
      file->is_signed = TRUE;
      file->size = wav_header.bitspersample / 8;
      file->seekable = QUH_SEEKABLE;
      file->expanding = 1;
    }
  else
    return -1;

  *buf = 0;
#if 0
  sprintf (buf, "Name: %s\n", VOXHUMAN);
  sprintf (strchr (buf, 0), "%*cGender: %s\n",
    misc_digits (FILTER_MAX) + 3, ' ',
    VOXGENDER);
#endif
  sprintf (strchr (buf, 0), "Version: %s (%s %s, %s)",
    FLITE_PROJECT_NAME,
    FLITE_PROJECT_PREFIX,
    FLITE_PROJECT_VERSION,
    FLITE_PROJECT_DATE);

  quh_set_object_s (quh.filter_chain, QUH_OUTPUT, buf);

  return 0;
}


static int
quh_flite_close (st_quh_nfo_t *file)
{
  (void) file;

  fclose (fp);
           
  return 0;
}


static int
quh_flite_seek (st_quh_nfo_t *file)
{
  // skip wav header
  quh.raw_pos = MAX (sizeof (st_wav_header_t), quh.raw_pos);

  fseek (fp, quh.raw_pos, SEEK_SET);
  file->raw_size = fsizeof (quh.tmp_file); // expanding
  
  return 0;
}


static int
quh_flite_write (st_quh_nfo_t *file)
{
  (void) file;
  quh.buffer_len = fread (&quh.buffer, 1, QUH_MAXBUFSIZE, fp);
  file->raw_size = fsizeof (quh.tmp_file); // expanding
  return 0;
}


int
quh_flite_demux (st_quh_nfo_t * file)
{
  int result = 0;

  if (file->source != QUH_SOURCE_FILE)
    return -1;

  if (!stricmp (get_suffix (file->fname), ".txt"))
    result = 0;

  return result;
}


const st_filter_t quh_flite_in =
{
  QUH_TXT_IN,
  "flite (txt)",
  ".txt",
  -1,
  (int (*) (void *)) &quh_flite_demux,
  (int (*) (void *)) &quh_flite_open,
  (int (*) (void *)) &quh_flite_close,
  NULL,
  (int (*) (void *)) &quh_flite_write,
  (int (*) (void *)) &quh_flite_seek,
  NULL,
  NULL,
  NULL
};


#if 0
const st_getopt2_t quh_flite_in_usage =
{
    "wav", 1, 0, QUH_TXT,
    "FILE", "FILE is WAV (if it has no .wav suffix)",
    (void *) QUH_TXT_IN
};
#endif
