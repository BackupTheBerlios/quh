/*
pipe.c - pipe filter for Quh

written by 2004 Dirk (d_i_r_k_@gmx.net)

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
#include "misc/itypes.h"
#include "misc/getopt2.h"
#include "misc/misc.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "pipe.h"


static FILE *fh = NULL;


static int
quh_pipe_init (st_quh_filter_t *file)
{
  (void) file;
  
  if (!quh_get_object_s (quh.filter_chain, QUH_OPTION))
    quh_set_object_s (quh.filter_chain, QUH_OPTION, "pipe_example");
        
  return 0;
}


static int
quh_pipe_ctrl (st_quh_filter_t *file)
{
  (void) file;
  char buf[MAXBUFSIZE];
  const char *p = quh_get_object_s (quh.filter_chain, QUH_OPTION);

  if (!p)
    return -1;

  sprintf (buf, "%s %d %d %d %d", p, file->rate, file->channels, file->size * 8, file->is_signed ? 1 : 0);

  if (!(fh = popen (buf, "w")))
    return -1;

printf ("OK");
fflush (stdout);

  return 0;
}


static int
quh_pipe_close (st_quh_filter_t *file)
{
  (void) file;

//  fclose (fh);
  pclose (fh);

  return 0;
}


static int
quh_pipe_write (st_quh_filter_t *file)
{
  (void) file;

  fwrite (quh.buffer, 1, quh.buffer_len, fh);
    
  return 0;
}


const st_filter_t quh_pipe =
{
  QUH_PIPE_PASS,
  "pipe",
  NULL,
  0,
//  (int (*) (void *)) &quh_pipe_open,
  NULL,
  (int (*) (void *)) &quh_pipe_close,
  NULL,
  (int (*) (void *)) &quh_pipe_write,
  NULL,
  (int (*) (void *)) &quh_pipe_ctrl,
  (int (*) (void *)) &quh_pipe_init,
  NULL
};


const st_getopt2_t quh_pipe_usage =
{
    "pipe", 2, 0, QUH_PIPE,
    "PATH", "pipe through external FILTER with PATH as filename\n"
    "The FILTER should (at least) read from STDIN and accept\n"
    "the following optargs:\n"
    "argv[0]=PATH\n" 
    "argv[1]=Rate\n"
    "argv[2]=Channels\n"
    "argv[3]=Bits\n"
    "argv[4]=Is signed (value: 0 or 1)",
//    "WinAmp and QBase/VST plug-ins are detected and\n"
//    "supported automatically",
    (void *) QUH_PIPE_PASS
};
