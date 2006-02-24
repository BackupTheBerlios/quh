/*
txt.c - txt support for Quh (using flite speech synthesis)

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
#include <signal.h>
#include <sys/wait.h>  // waitpid()
#include "libflite/flite.h"
#include "libflite/flite_version.h"
#ifdef  HAVE_VOXDEFS_H
#include "libflite/voxdefs.h"
#else
#include "voxdefs.h"
#endif
#include "misc/itypes.h"
#include "misc/misc.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "misc/file.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "wav.h"


static int init = 0;
static FILE *fp = NULL;
static int pid = 0;
static char temp_filename[FILENAME_MAX];


static int
quh_flite_open (st_quh_filter_t *file)
{
  static char buf[MAXBUFSIZE];
  cst_voice *REGISTER_VOX(const char *voxdir);
  cst_voice *UNREGISTER_VOX(cst_voice *vox);
  cst_features *extra_feats = NULL;
  cst_voice *v = NULL;
  st_wav_header_t wav_header;

  // a device?
  if (file->source != QUH_SOURCE_FILE)
    return -1;

  if (!init)
    {
      flite_init ();
      init = 1;
    }

//  tmpnam2 (temp_filename);
  strcpy (temp_filename, "test.wav");

  pid = fork ();
  
  if (pid < 0) // failed
    return -1;
        
  if (!pid) // child
    {
      // write temp file
      extra_feats = new_features ();
      v = REGISTER_VOX(NULL);
      feat_copy_into (extra_feats, v->features);

      flite_file_to_speech (file->fname, v, temp_filename);

      delete_features (extra_feats);
      UNREGISTER_VOX(v);
      exit (0);
    }

  wait2 (500);

  if (!(fp = fopen (temp_filename, "rb")))
    return -1;

  fread (&wav_header, 1, sizeof (st_wav_header_t), fp);
  
  if (!memcmp (wav_header.magic, "RIFF", 4))
    {
      quh.raw_pos = sizeof (st_wav_header_t);
      file->raw_size = fsizeof (temp_filename) - quh.raw_pos;
      file->rate = wav_header.freq;
      file->channels = wav_header.channels;
      file->is_signed = TRUE;
      file->size = wav_header.bitspersample / 8;
      file->seekable = QUH_SEEKABLE;
    }
  else
    return -1;

  *buf = 0;
  sprintf (buf, "Name: %s\n", VOXHUMAN);
  sprintf (strchr (buf, 0), QUH_INPUT_COLUMN_S "Gender: %s\n", VOXGENDER);
  sprintf (strchr (buf, 0), QUH_INPUT_COLUMN_S "Version: %s (%s %s, %s)",
    FLITE_PROJECT_NAME,
    FLITE_PROJECT_PREFIX,
    FLITE_PROJECT_VERSION,
    FLITE_PROJECT_DATE);
  file->misc = buf;

  return 0;
}


static int
quh_flite_close (st_quh_filter_t *file)
{
  (void) file;

  if (pid > 0) // parent kills child
    {
      kill (pid, SIGKILL);
      waitpid (pid, NULL, 0);
    }

  fclose (fp);
  remove (temp_filename);
           
  return 0;
}


static int
quh_flite_seek (st_quh_filter_t *file)
{
  // skip wav header
  quh.raw_pos = MAX (sizeof (st_wav_header_t), quh.raw_pos);

  fseek (fp, quh.raw_pos, SEEK_SET);
  file->raw_size = fsizeof (temp_filename); // expanding
  
  return 0;
}


static int
quh_flite_read (st_quh_filter_t *file)
{
  (void) file;
  quh.buffer_len = fread (&quh.buffer, 1, QUH_MAXBUFSIZE, fp);
  file->raw_size = fsizeof (temp_filename); // expanding
  return 0;
}


const st_filter_t quh_txt_in =
{
  INPUT_FLITE,
  "txt",
  ".txt",
  strlen (".txt"),
  NULL,
  (int (*) (void *)) &quh_flite_open,
  (int (*) (void *)) &quh_flite_close,
  (int (*) (void *)) &quh_flite_read,
  NULL,
  (int (*) (void *)) &quh_flite_seek,
  NULL,
  NULL,
  NULL
};


const st_filter_t quh_pdf_in =
{
  INPUT_FLITE2,
  "pdf",
  ".pdf",
  strlen (".txt"),
  (int (*) (void *)) &quh_flite_open,
  (int (*) (void *)) &quh_flite_close,
  (int (*) (void *)) &quh_flite_read,
  NULL,
  (int (*) (void *)) &quh_flite_seek,
  NULL,
  NULL,
  NULL
};


const st_filter_t quh_doc_in =
{
  INPUT_FLITE,
  "txt",
  ".txt",
  strlen (".txt"),
  (int (*) (void *)) &quh_flite_open,
  (int (*) (void *)) &quh_flite_close,
  (int (*) (void *)) &quh_flite_read,
  NULL,
  (int (*) (void *)) &quh_flite_seek,
  NULL,
  NULL,
  NULL
};


const st_getopt2_t quh_flite_in_usage =
{
    "wav", 1, 0, QUH_TXT,
    "FILE", "FILE is WAV (if it has no .wav suffix)",
    (void *) INPUT_TXT
};
