/*
sdl.c - sdl support for Quh (using libsdl)

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
#include <stdlib.h>
#include "misc/itypes.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "misc/audio.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "sdl.h"


#warning TODO: fix


static int inited = 0;
static st_audio_t *audio = NULL;


int
quh_sdl_quit (st_quh_filter_t *file)
{
  (void) file;

  audio_close ();

  return 0;
}


int
quh_sdl_open (st_quh_filter_t *file)
{
  (void) file;

  return 0;
}


int
quh_sdl_ctrl (st_quh_filter_t *file)
{
  audio_set_channels (file->channels);
  audio_set_bits (file->size * 8);
  audio_set_freq (file->rate);

  return 0;
}


int
quh_sdl_init (st_quh_filter_t *file)
{
  (void) file;
        
  if (!inited)
    if (!(audio = audio_open (AUDIO_SDL)))
      return -1;
  inited = 1;

  return 0;
}


int
quh_sdl_write (st_quh_filter_t *file)
{
  (void) file;

#if 0
  audio_read_from_mem (quh.buffer, quh.buffer_len);
#else
  audio_read_from_file (file->fname);
#endif
  audio_write ();
        
  return 0;
}



const st_filter_t quh_sdl_out = {
  QUH_SDL_OUT,
  "sdl (audio)",
  NULL,
  0,
  NULL,
  NULL,
  NULL,
  NULL,
  (int (*) (void *)) &quh_sdl_write,
  NULL,
  (int (*) (void *)) &quh_sdl_ctrl,
  (int (*) (void *)) &quh_sdl_init,
  (int (*) (void *)) &quh_sdl_quit 
};


const st_getopt2_t quh_sdl_out_usage =
{
    "sdl", 0, 0, QUH_SDL,
    NULL, "write to soundcard using SDL",
//    "OUT=0 lineout (default)\n"
//    "OUT=1 headphones",
    (void *) QUH_SDL_OUT
};
