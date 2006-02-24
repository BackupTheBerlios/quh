/*
midi.c - midi support for Quh

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
#ifdef  USE_TIMIDITY
#include <midi.h>
#endif
#include "misc/itypes.h"
#include "misc/file.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
//#include "midi.h"


static int
midi_open (st_quh_filter_t *file)
{
  (void) file;

  return 0;
}


static int
midi_close (st_quh_filter_t *file)
{
  (void) file;

  return 0;
}


static int
midi_seek (st_quh_filter_t *file)
{
  (void) file;

  return 0;
}


static int
midi_read (st_quh_filter_t * file, int (*write_func) (st_quh_filter_t *))
{
  (void) file;
  (void) write_func;
  
  return 0;
}


const st_filter_t midi_filter_read = {
  INPUT_MIDI,
  "mid, midi",
  ".mid.midi",
  strlen (".mid.midi"),
  NULL,
  (int (*) (void *)) &midi_open,
  (int (*) (void *)) &midi_close,
  NULL,
  NULL,
  (int (*) (void *)) &midi_seek,
  NULL,
  (int (*) (void *, int (*) (void *))) &midi_read,
  NULL,
  NULL
};


const st_getopt2_t midi_filter_usage =
{
    "midi", 1, 0, QUH_FLAC,
    "FILE", "FILE is FLAC (if it has no .midi suffix)",
    (void *) INPUT_FLAC
};
