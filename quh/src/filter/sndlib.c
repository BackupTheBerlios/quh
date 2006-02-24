/*
sndlib.c - sndlib support for Quh

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
#ifdef  USE_SNDLIB
#include <sndlib.h>
#endif
#include "misc/itypes.h"
#include "misc/file.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "sndlib.h"


static int fd = 0;


static int
quh_sndlib_open (st_quh_filter_t *file)
{
  (void) file;

  if (!(fd = mus_sound_open_input (file->fname)))
    return -1;
    
  file->rate = mus_sound_srate (file->fname);
  file->channels = mus_sound_chans (file->fname);
//  file->size = mus_sound_block_align (file->fname);
//  
  file->size = (unsigned long) mus_sound_frames(file->fname) / mus_sound_fact_samples (file->fname);
  file->raw_size = mus_sound_length (file->fname);
  file->seekable = QUH_SEEKABLE;
  
  quh_set_object_s (quh.filter_chain, (void *) QUH_OPTION, (void *) mus_sound_comment(file->fname));
                    
//quh_demux_sanity_check (file);

  return 0;
}


static int
quh_sndlib_close (st_quh_filter_t *file)
{
  (void) file;
//      mus_sound_close_input(fd);
      
  return 0;
}


static int
quh_sndlib_seek (st_quh_filter_t *file)
{
  (void) file;
//      start = (off_t)(begin_time * srate);
      //        mus_file_seek_frame(fd, start);
        
  return 0;
}


static int
quh_sndlib_demux (st_quh_filter_t * file)
{
  (void) file;

  return -1;
}


static int
quh_sndlib_write (st_quh_filter_t * file)
{
  (void) file;
//            mus_file_read(fd, 0, curframes - 1, chans, read_bufs);
//            
  return 0;
}


const st_filter_t quh_sndlib_in = {
  QUH_SNDLIB_IN,
  "sndlib (scm, rb)",
  ".scm.rb",
  strlen (".scm.rb"),
  (int (*) (void *)) &quh_sndlib_demux,
  (int (*) (void *)) &quh_sndlib_open,
  (int (*) (void *)) &quh_sndlib_close,
  NULL,
  (int (*) (void *)) &quh_sndlib_write,
  (int (*) (void *)) &quh_sndlib_seek,
  NULL,
  NULL,
  NULL
};


const st_getopt2_t quh_sndlib_in_usage =
{
    "sndlib", 1, 0, QUH_FLAC,
    "FILE", "FILE is FLAC (if it has no .sndlib suffix)",
    (void *) QUH_SNDLIB_IN
};
