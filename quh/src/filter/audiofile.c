/*
audiofile.c - audiofile support for Quh

written by 2005 Dirk (d_i_r_k_@gmx.net)

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
#ifdef  USE_AUDIOFILE
#include <audiofile.h>
#endif
#include "misc/itypes.h"
#include "misc/file.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "audiofile.h"


static AFfilehandle affp;
static float framesize = 0;
  

static int
quh_audiofile_open (st_quh_filter_t *file)
{
  AFframecount frames;
  int format;

  if (!(affp = afOpenFile(file->fname, "r", NULL)))
    return -1;

  frames = afGetFrameCount (affp, AF_DEFAULT_TRACK);

  file->channels = afGetVirtualChannels (affp, AF_DEFAULT_TRACK);

  afGetVirtualSampleFormat (affp, AF_DEFAULT_TRACK, &format, &file->size);

  file->size /= 8;
  file->is_signed = 1;
  switch (format)
    {
//       case AF_SAMPFMT_TWOSCOMP:
//         file->size = 2;
//         break;
         
       case AF_SAMPFMT_UNSIGNED:
         file->is_signed = 0;
         break;
    }

  file->rate = (unsigned long) afGetRate (affp, AF_DEFAULT_TRACK);
  framesize = afGetVirtualFrameSize (affp, AF_DEFAULT_TRACK, 1);
  file->raw_size = (unsigned long) (frames * framesize);
  file->seekable = QUH_SEEKABLE;
      
  return 0;
}


static int
quh_audiofile_close (st_quh_filter_t *file)
{
  (void) file;

  afCloseFile (affp);

  return 0;
}


static int
quh_audiofile_seek (st_quh_filter_t *file)
{
  (void) file;

  afSeekFrame (affp, AF_DEFAULT_TRACK, (unsigned long) (quh.raw_pos / framesize));

  return 0;
}


static int
quh_audiofile_write (st_quh_filter_t * file)
{
  (void) file;

  quh.buffer_len = afReadFrames (affp, AF_DEFAULT_TRACK, quh.buffer,
                                 (int) (QUH_MAXBUFSIZE / (int) framesize)) * (int) framesize;
  return 0;
}


const st_filter_t quh_audiofile_in = {
  QUH_AUDIOFILE_IN,
  "audiofile (aifc, aiff, au, avr, bicsf, iff, mpeg, nist, raw, sd1, sd2, sf2, smp, snd, voc, wav)",
  ".aifc.aiff.au.avr.bicsf.iff.mpeg.nist.raw.sd1.sd2.sf2.smp.snd.voc.wav",
  strlen (".aifc.aiff.au.avr.bicsf.iff.mpeg.nist.raw.sd1.sd2.sf2.smp.snd.voc.wav"),
  (int (*) (void *)) &quh_audiofile_open,
  (int (*) (void *)) &quh_audiofile_close,
  NULL,
  (int (*) (void *)) &quh_audiofile_write,
  (int (*) (void *)) &quh_audiofile_seek,
  NULL,
  NULL,
  NULL
};


const st_getopt2_t quh_audiofile_in_usage =
{
    "audiofile", 1, 0, QUH_AUDIOFILE,
    "FILE", "FILE is FLAC (if it has no .audiofile suffix)",
    (void *) QUH_AUDIOFILE_IN
};
