/*
echo.c - echo filter for Quh

written by 2005 Dirk (d_i_r_k_@gmx.net)

this code is inspired by sox by Chris Bagwell (cbagwell@sprynet.com)
I really, really tried to use libst, but it refused to fit into Quh's
framework. Because util.c::st_fail() does exit() i had to do ugly
"atexit(next_effect_usage++)" hacks to get at least a reasonable usage
output for all sox effects. libst() did also expect a cleanup function
in my code. :-(

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
#ifdef  USE_ST
#include "libst/st.h"
#endif
#include "misc/itypes.h"
#include "misc/file.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "misc/string.h"
#include "quh_defines.h"
#include "quh.h"
#include "st.h"


static struct st_effect eff;
static st_sample_t ibufl[QUH_MAXBUFSIZE/2];    /* Left/right interleave buffers */
static st_sample_t ibufr[QUH_MAXBUFSIZE/2];
static st_sample_t obufl[QUH_MAXBUFSIZE/2];
static st_sample_t obufr[QUH_MAXBUFSIZE/2];


/* called from util.c::st_fail() */
void cleanup (void)
{
#if 0
    int i;

    /* Close the input file and outputfile before exiting*/
    for (i = 0; i < input_count; i++)
    {
        if (file_desc[i] && file_desc[i]->fp)
                fclose(file_desc[i]->fp);
        if (file_desc[i])
            free(file_desc[i]);
    }
    if (writing && file_desc[file_count-1] && file_desc[file_count-1]->fp) {
        fclose(file_desc[file_count-1]->fp);
        /* remove the output file because we failed, if it's ours. */
        /* Don't if its not a regular file. */
        if (filetype(fileno(file_desc[file_count-1]->fp)) == S_IFREG)
            unlink(file_desc[file_count-1]->filename);
        if (file_desc[file_count-1])
            free(file_desc[file_count-1]);
    }
#endif
}


typedef struct
{
  const char *name;
  int argc;
  char *argv[4];
} st_usage_hack_t;


st_usage_hack_t *
get_usage_hack (const char *name)
// it's all util.c::st_fail()'s fault ;-)
{
  static st_usage_hack_t usage_hack[] =
    {
      {"echo",      1, {"a", NULL}},
      {"mask",      1, {"a", NULL}},
      {"mcompand",  1, {"\t", NULL}},
      {"noiseprof", 2, {"a", "a", NULL}},
      {"pitch",     1, {"a", NULL}},
//      {"polyphase", 0, {"a", NULL}}, // sox has no usage for this
      {"repeat",    0, {"a", NULL}},
      {"resample",  3, {"a", "a", "a", NULL}},
//      {"stat",      3, {"a", "a", "a", NULL}}, // sox has no usage for this
      {"stretch",   1, {"a", NULL}},
      {NULL,        0, {NULL}}
    }, def = {NULL, -1, {"a", "a", NULL}};
  int x = 0;

  for (; usage_hack[x].name; x++)
    if (!strcmp (usage_hack[x].name, name))
      return &usage_hack[x];

  return &def;
}


static int usage = 0;


static void
quh_st_usage (void)
{
  fflush (stdout);
        
  if (!st_effects[usage].name)
    {
      printf ("\n\nREMEMBER: You have to replace all spaces between EFFECT name and EFFECT args\n"
                  "          with commas\n"
                  "          example: \"trim start [length]\" becomes \"" OPTION_LONG_S "st=trim,start[,length]\"\n");

      atexit (quh_exit);
      exit (0);
    }
  else
    {
      st_usage_hack_t *hack = get_usage_hack (st_effects[usage].name);

      printf ("\nName: %s\n", st_effects[usage].name);
      fflush (stdout);
                 
      st_geteffect (&eff, st_effects[usage].name);
      usage++;
      atexit (quh_st_usage);

      (*eff.h->getopts) (&eff, hack->argc, (char **) hack->argv);
    }
}


static int
quh_st_init (st_quh_filter_t *file)
{
  (void) file;

  if (!quh.filter_option[filter_get_pos (quh.write)])
      quh.filter_option[filter_get_pos (quh.write)] = "vol";
  else
    if (!stricmp (quh.filter_option[filter_get_pos (quh.write)], "help"))
      {
          myname = "Args"; // for the usage output
          quh_st_usage ();
      }

  myname = quh.argv[0];

  return 0;
}


static int
quh_st_open (st_quh_filter_t *file)
{
  (void) file;
  int x = 0;
  int argc;
  char *argv[128];
  char buf[MAXBUFSIZE];
      
  strcpy (buf, quh.filter_option[filter_get_pos (quh.write)]);
  argc = strarg (argv, buf, ",", 128);
  
//#ifdef  DEBUG
  for (; x < argc; x++)
    printf ("argv[%d]: %s\n", x, argv[x]);
  fflush (stdout);
//#endif

  eff.ininfo.rate = (st_rate_t) file->rate;
  eff.ininfo.size = file->size;
  eff.ininfo.encoding = ST_ENCODING_UNSIGNED;
  eff.ininfo.channels = file->channels;

  argc = st_geteffect_opt (&eff, argc, &argv[0]);
                                                
    
  if (argc == ST_EOF)
    return -1; // unknown effect
  
  (*eff.h->getopts) (&eff, argc, &argv[1]);

            /* Copy format info to effect table */
            effects_mask = st_updateeffect(&eff,
                                           &file_desc[0]->info,
                                           &file_desc[file_count-1]->info, 
                                           effects_mask);

            /* Rate can't handle multiple channels so be sure and
             * account for that.
             */
            if (eff.ininfo.channels > 1)
            {
                memcpy(&effR, &eff, sizeof(struct st_effect));
            }


  (*eff.h->start) (&eff);

  if (!(eff.obuf = (st_sample_t *) malloc (QUH_MAXBUFSIZE * sizeof (st_sample_t)))) // ST_BUFSIZ
    return -1;
  
  return 0;
}


static int
quh_st_close (st_quh_filter_t *file)
{
  (void) file;

  free (eff.obuf);
  (*eff.h->stop) (&eff);

  return 0;
}


static int
drain_effect ()
{
  st_ssize_t i, olen, olenl, olenr;
  st_sample_t *obuf;

  olen = QUH_MAXBUFSIZE;
  eff.olen = QUH_MAXBUFSIZE;

  (* eff.h->drain) (&eff, eff.obuf, &eff.olen);
  
  return eff.olen;
}


static int
quh_st_write (st_quh_filter_t * file)
{
  (void) file;

  eff.olen = // quh.buffer_len;
  eff.odone = 0;
          
  drain_effect ();

  memcpy (&quh.buffer, eff.obuf, eff.olen);
  quh.buffer_len = eff.olen;

  return 0;
}


const st_filter_t st_filter_write = {
  FILTER_ST,
  "sox sound tools effects",
  NULL,
  0,
  NULL,
  (int (*) (void *)) &quh_st_open,
  (int (*) (void *)) &quh_st_close,
  NULL,
  (int (*) (void *)) &quh_st_write,
  NULL,
  NULL,
  NULL,
  (int (*) (void *)) &quh_st_init,
  NULL
};


const st_getopt2_t st_filter_usage =
{
    "st", 1, 0, QUH_ST,
    "EFFECT", "insert sox sound tools EFFECT by name\n"
    "EFFECT=help output list of EFFECTs with usage and exit\n"
    "in opposite to sox itself, the EFFECT name and the\n"
    "EFFECT args must be separated by commas and not\n"
    "spaces; example: \"trim start [length]\" becomes\n"
    "                 \"" OPTION_LONG_S "st=trim,start[,length])\"",
    (void *) FILTER_ST
};
