/*
quh.c - Quh plays everything that makes noise

Copyright (c) 2005 Dirk


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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "misc/cache.h"
#include "misc/itypes.h"
#include "misc/file.h"
#include "misc/misc.h"
#include "misc/getopt2.h"
#include "misc/property.h"
#include "misc/filter.h"
#include "misc/term.h"
#include "misc/string.h"
#include "misc/defines.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "quh_filter.h"


static int filters = 0;
st_quh_t quh;


static int
quh_set_fname2 (const char *fname)
{
  static int once = 0;

#ifdef  DEBUG
  printf ("%s\n", fname);
  fflush (stdout);
#endif

  if (quh.files >= QUH_MAX_FILES - 1)
    {
      if (!once++)
        fprintf (stderr, "ERROR: can not open more than %d inputs, skipping\n\n", QUH_MAX_FILES);

      return 1; // skipping
    }

  quh.fname[quh.files] = strdup (fname);
  quh.files++;

  return 0;
}


static int
quh_set_fname (const char *fname)
{
//  const char *suffix = NULL;

#ifdef  DEBUG
  printf ("\n%s\n\n", fname);
  fflush (stdout);
#endif

#if 0
  // playlist? 
  suffix = get_suffix (fname);
  if (*suffix)
    if (!stricmp (suffix, ".m3u"))// ||
//        !stricmp (suffix, ".pls"))
      {
        FILE *fh = fopen (fname, "r");
        char buf[MAXBUFSIZE];

        if (fh)
          {
            while (fgets (buf, MAXBUFSIZE, fh))
              quh_set_fname2 (buf);
            fclose (fh);
          }

        return 0; // failed
      }
#endif

  if (!access (fname, R_OK))
    return quh_set_fname2 (fname);
  return 0;
}
  

void
quh_exit (void)
{
  if (quh.pid)
    {
      if (quh.filter_chain)
        {
          filter_quit (quh.filter_chain, &quh.nfo);
          filter_free_chain (quh.filter_chain);
        }
    
      while (quh.files--)
        free (quh.fname[quh.files]);
    
      cache_close (quh.o);
    
      remove (quh.tmp_file);
    
      set_property_int (quh.configfile, "settings", quh.soundcard.vol, NULL);
 
      fputc ('\n', stdout);
    
      fflush (stdout);
    }
}


static void
quh_signal_handler (int signum)
{
  switch (signum)
    {
      case SIGINT:
      case SIGTERM:
        if (quh.filter_chain)
          filter_close (quh.filter_chain, &quh.nfo);
              
        fflush (stdout);
        fputs ("Quh: break", stderr);
        exit (0);
        break;
        
      default:
        break;
    }
}


static st_getopt2_t options[QUH_MAX_ARGS];
static const st_getopt2_t lf[] =
  {
    {NULL, 0, 0, 0, NULL, "", NULL},
    {NULL, 0, 0, 0, NULL, NULL, NULL}
  },
  *option[] =
  {
    quh_options_usage,
    lf,
    NULL
  },
  *option2[] =
  {
    quh_options_usage_bottom,
    NULL
  };


static int
quh_opts (int c)
{
  uint32_t flags = 0;
  int value = 0;
  static char buf[MAXBUFSIZE];
  const st_getopt2_t *p = NULL;

  switch (c)
    {
      case QUH_START:
        if (optarg)
          quh.start_optarg = optarg;
        break;

      case QUH_LEN:
        if (optarg)
          quh.len_optarg = optarg;
        break;

      case QUH_INTRO:
        quh.start_optarg = "60000";
        quh.len_optarg = "10000";
        break;

      case QUH_RR:
      quh.start_optarg = "min=0:max=eof";
      if (optarg)
        quh.len_optarg = optarg;
      else
        quh.len_optarg = "min=500:max=1500";
      quh.random = 1;
      quh.loop = 1;
      break;

      case QUH_BPM:
        if (optarg)
          {
            value = strtol (optarg, NULL, 10);

            // turn BPM into ms
            sprintf (buf, "%ld", (unsigned long) 60000 / value);
            quh.len_optarg = buf;
          }
        break;

      case QUH_V:
        quh.verbose = 1;
        break;

      case QUH_Q:
        quh.quiet = 1;
        break;

      case QUH_R:
        p = getopt2_get_index_by_val (options, c);
        if (p)
          if (p->object)
            {
              flags = (uint32_t) p->object;
              if (flags)
                quh.flags |= flags;
            }
        break;

      case QUH_HELP:
        getopt2_usage (options);
        exit (0);

      case QUH_VER:
        fputs ("quh version: " QUH_VERSION_S "\n", stdout);
      case QUH_S:
        printf ("filter: %s\n", filter_get_all_id_s_in_array (quh_filter));
        exit (0);

      case QUH_RANDOM:
        quh.random = 1;
        break;

      case QUH_LOOP:
        quh.loop = optarg ? strtol (optarg, NULL, 10) + 1 : 1;
        break;

      case QUH_SHUFFLE:
        quh.shuffle = 1;
        break;

      // filter
      case QUH_NEW:
        quh.filter_id[0] = 0;
        filters = 0;
        break;

      case QUH_STDOUT:
      case QUH_STDERR:
        quh.verbose = 0;
        quh.quiet = 1;
#ifdef  USE_GTK
      case QUH_GTK:
#endif
      case QUH_CACHE:
//      case QUH_PIPE:
#ifdef  USE_ST
      case QUH_ST:
#endif
#ifdef  USE_SDL
      case QUH_SDL:
#endif
#ifdef  USE_OSS
      case QUH_OSS:
#endif
      case QUH_RAW:
#ifdef  USE_CDDB
      case QUH_CDDB:
#endif
#ifdef  USE_ID3
      case QUH_ID3:
#endif
      case QUH_WAV:
#ifdef  USE_JOYSTICK
      case QUH_JOY:
#endif
#ifdef  USE_PCSPKR
      case QUH_SPEAKER:
#endif
#ifdef  USE_ALSA
      case QUH_ALSA:
#endif
#ifdef  USE_ESD
      case QUH_ESD:
#endif
      case QUH_CONSOLE:
#ifdef  USE_CDDA
      case QUH_CDDA:
#endif
#ifdef  USE_LIBAO
      case QUH_AO:
#endif
      case QUH_VOL:
      case QUH_LYRICS:
        p = getopt2_get_index_by_val (options, c);

        if (p)
          { 
            quh.filter_id[filters] = (int) p->object;
 
            if (quh.filter_id[filters])
              {
                if (optarg)
                  if (*optarg)
                    quh.filter_option[filters] = optarg;

                quh.filter_id[++filters] = 0;
              }
          }
        break;

      default:
        printf ("Try '%s --help' for more information.\n", quh.argv[0]);
        exit (-1);
    }

  return 0;
}


int
main (int argc, char **argv)
{
  int c = 0, option_index = 0;
  int x = 0, y = 0;
  struct option long_only_options[ARGS_MAX];
  int result = 0;
//  const char *p = NULL; 
  const st_property_t props[] =
    {
      {
        "ansi_color", "1",
        "use ANSI colors in output? (1=yes; 0=no)"
      },
      {
        "default_cmdline", "",
        "will be used when quh is started w/o args"
      },
      {
        "settings", "100",
        "internal settings like volume, etc."
      },
#if 0
      {
        "quh_configdir",
        PROPERTY_MODE_DIR ("quh"),
        "directory with additional config files"
      },
#endif
      {NULL, NULL, NULL}
    };

  memset (&quh, 0, sizeof (st_quh_t));

  // defaults
  quh.pid = 1;
  tmpnam2 (quh.tmp_file);
  set_suffix (quh.tmp_file, ".wav");

  if(!(quh.o = cache_open (MAXBUFSIZE, CACHE_MEM|CACHE_LIFO)))
    {
      fprintf (stderr, "ERROR: Could not malloc %d bytes\n", MAXBUFSIZE);
      return -1;
    }

  realpath2 (PROPERTY_HOME_RC ("quh"), quh.configfile);

  result = property_check (quh.configfile, QUH_CONFIG_VERSION, 1);
  if (result == 1) // update needed
    result = set_property_array (quh.configfile, props);
  if (result == -1) // property_check() or update failed
    return -1;

  signal (SIGINT, quh_signal_handler);
  signal (SIGTERM, quh_signal_handler);

  atexit (quh_exit);

#ifdef  USE_ANSI_COLOR
  quh.ansi_color = get_property_int (quh.configfile, "ansi_color");
#endif
  if (quh.ansi_color)
    ansi_init ();
    
  quh.settings = get_property_int (quh.configfile, "settings");

  quh.argc = argc;
  quh.argv = argv;

  // set default filter chain
  filters = 0;
  quh.filter_id[filters++] = QUH_CACHE_PASS;
  quh.filter_id[filters++] = QUH_CONSOLE_PASS;
#ifdef  USE_ID3
//  quh.filter_id[filters++] = QUH_ID3_IN;
#endif
//  quh.filter_id[filters++] = QUH_CDDB_IN;
#ifdef  USE_OSS
  quh.filter_id[filters++] = QUH_OSS_OUT;
#elif   defined USE_SDL
  quh.filter_id[filters++] = QUH_SDL_OUT;
#endif

  // convert (st_getopt2_t **) to (st_getopt2_t *)
  memset (&options, 0, sizeof (st_getopt2_t) * QUH_MAX_ARGS);
  for (c = x = 0; option[x]; x++)
    for (y = 0; option[x][y].name || option[x][y].help; y++)
      if (c < QUH_MAX_ARGS)
        {
          memcpy (&options[c], &option[x][y], sizeof (st_getopt2_t));
          c++;
        }

#if 0
  for (x = 0; quh_decode_usage[x]; x++)
    if (c < QUH_MAX_ARGS)
      {
        memcpy (&options[c], quh_decode_usage[x], sizeof (st_getopt2_t));
        c++;
      }
#endif

  for (x = 0; quh_filter_usage[x]; x++)
    if (c < QUH_MAX_ARGS)
      {
        memcpy (&options[c], quh_filter_usage[x], sizeof (st_getopt2_t));
        c++;
      }

  for (x = 0; option2[x]; x++)
    for (y = 0; option2[x][y].name || option2[x][y].help; y++)
      if (c < QUH_MAX_ARGS)
        {
          memcpy (&options[c], &option2[x][y], sizeof (st_getopt2_t));
          c++;
        }

  getopt2_long_only (long_only_options, options, ARGS_MAX);

#if 0
  // if no options or filenames were specified we use a default cmdline
  if (argc < 2) // || !optind)
    {
      p = get_property (quh.configfile, "default_cmdline", PROPERTY_MODE_TEXT);
      if (p)
        {
          strncpy (quh.cmdline, p, ARGS_MAX)[ARGS_MAX - 1] = 0;
          quh.argc = strarg (quh.argv, quh.cmdline, " ", QUH_MAX_ARGS);
        }
    }
  else // store cmdline
    {
      strcpy (quh.cmdline, argv[0]);
      for (x = 1; x < argc; x++)
        sprintf (strchr (quh.cmdline, 0), " \"%s\"", quh.argv[x]);
      set_property (quh.configfile, "default_cmdline", quh.cmdline, NULL);
    }

  for (x = 0; x < quh.argc; x++)
    printf ("quh.argv[%d] == %s\n", x, quh.argv[x]);
  fflush (stdout);
#endif

  while ((c = getopt_long_only (quh.argc, quh.argv, "", long_only_options, &option_index)) != -1)
    quh_opts (c);

//  if (quh.verbose)
  if (!quh.quiet)
    fputs ("Quh " QUH_VERSION_S " 'Having ears makes sense again' 2005-2006 by NoisyB\n"
           "This may be freely redistributed under the terms of the GNU Public License\n\n", stdout);

  if (quh.argc < 2) // || !optind)
    {
      getopt2_usage (options);
      return -1;
    }

  if (!getfile (quh.argc, quh.argv, quh_set_fname,
                (GETFILE_FILES_ONLY | (quh.flags & QUH_RECURSIVE ? GETFILE_RECURSIVE : 0)))) // recursively?
    {
      if (!quh.quiet)
        getopt2_usage (options);
      return -1;
    }

  if (!quh.filter_id[0])
    {
      fputs ("ERROR: you haven't specified any filters\n", stderr);
      fflush (stderr);
      return -1;
    }

  if (!quh.files)
    {
      fputs ("ERROR: you haven't specified any files to play\n", stderr);
      fflush (stderr);
      return -1;
    }

  if (!(quh.filter_chain = filter_malloc_chain (quh_filter)))
    {
      fputs ("ERROR: filter_malloc_chain() failed\n", stderr);
      fflush (stderr);
      return -1;
    }

  if (filter_init (quh.filter_chain, NULL, NULL) == -1)
    {
      fputs ("ERROR: filter_init() failed\n", stderr);
      fflush (stderr);
      return -1;
    }

  quh_play ();

  return 0;
}
