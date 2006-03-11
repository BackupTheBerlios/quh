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
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "quh_filter.h"


static int filters = 0;
st_quh_t quh;
static int32_t quh_configfile (void);


static int
quh_set_fname (const char *fname)
{
  static int once = 0;

#ifdef  DEBUG
  printf ("\n%s\n\n", fname);
  fflush (stdout);
#endif

  if (access (fname, R_OK))
    return 0; // streams are not supported, yet

  if (quh.files >= QUH_MAX_FILES - 1)
    {
      if (!once++)
        fprintf (stderr, "ERROR: can not open more than %d inputs, skipping\n\n", QUH_MAX_FILES);

      return 0; // skipping
    }

#warning m3u and pls support
#if 0
  // playlist
  if (*suffix)
    {
      if (!stricmp (suffix, ".m3u") ||
          !stricmp (suffix, ".pls"))
        {
          printf ("Playlists are not supported, yet");
          return 0; // skip
        }
    }
#endif

  quh.fname[quh.files++] = strdup (fname);

  return 0;
}
  

void
quh_exit (void)
{
  if (quh.filter_chain)
    {
      filter_quit (quh.filter_chain, &quh.demux);
      filter_free_chain (quh.filter_chain);
    }

  while (quh.files--)
    free (quh.fname[quh.files]);

  cache_close (quh.o);

  printf ("\n");

  fflush (stdout);
}


static void
quh_signal_handler (int signum)
{
  switch (signum)
    {
      case SIGINT:
      case SIGTERM:
        if (quh.filter_chain)
          filter_close (quh.filter_chain, &quh.demux);
              
        fflush (stdout);
        fprintf (stderr, "Quh: break");
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
  uint32_t *flags = NULL;
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
      if (!optarg)
        quh.len_optarg = "min=500:max=1500";
#warning parse --rr into optarg
//      else
//        quh.len_optarg = "min=500:max=1500";
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

#ifdef  DEBUG
      case QUH_DEBUG:
        quh.debug = 1;
        break;
#endif

      case QUH_R:
        flags = (uint32_t *) &getopt2_get_index_by_val (options, c)->object;
        if (flags)
          quh.flags |= *flags;
        break;

      case QUH_HELP:
        getopt2_usage (options);
        exit (0);

      case QUH_VER:
        printf ("quh version: %s\n", QUH_VERSION_S);
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
#ifdef  USE_FOX
      case QUH_FOX: 
#endif
      case QUH_CACHE:
      case QUH_PIPE:
#ifdef  USE_SDL
      case QUH_SDL:
#endif  // USE_SDL
#ifdef  USE_OSS
      case QUH_OSS:
#endif
      case QUH_RAW:
      case QUH_CDDB:
      case QUH_ID3:
      case QUH_WAV:
      case QUH_JOY:
#ifdef  USE_PCSPEAKER
      case QUH_SPEAKER:
#endif
#ifdef  USE_ALSA
      case QUH_ALSA:
#endif
#ifdef  USE_ESD
      case QUH_ESD:
#endif
      case QUH_CONSOLE:
      case QUH_CDDA:
#ifdef  USE_LIBAO
      case QUH_LIBAO:
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

  memset (&quh, 0, sizeof (st_quh_t));

  if(!(quh.o = cache_open (MAXBUFSIZE, CACHE_MEM|CACHE_LIFO)))
    {
      fprintf (stderr, "ERROR: Could not malloc %d bytes\n", MAXBUFSIZE);
      exit (1);
    }

  quh_configfile ();

  signal (SIGINT, quh_signal_handler);
  signal (SIGTERM, quh_signal_handler);

  atexit (quh_exit);

#ifdef  USE_ANSI_COLOR
  quh.ansi_color = get_property_int (quh.configfile, "ansi_color");
#endif
  if (quh.ansi_color)
    ansi_init ();
    
  quh.argc = argc;
  quh.argv = argv;

  // set default filter chain
  filters = 0;
//  quh.filter_id[filters++] = QUH_CACHE;
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

  while ((c = getopt_long_only (argc, argv, "", long_only_options, &option_index)) != -1)
    quh_opts (c);

  if (!quh.quiet)
    printf ("Quh " QUH_VERSION_S " 'Having ears makes sense again' 2005 by NoisyB (noisyb@gmx.net)\n"
            "This may be freely redistributed under the terms of the GNU Public License\n\n");

  if (argc < 2) // || !optind)
    {
      getopt2_usage (options);
      exit (-1);
    }

  if (!quh.filter_id[0])
    {
      fprintf (stderr, "ERROR: you haven't specified any filters\n");
      fflush (stderr);
      exit (-1);
    }

  if (!getopt2_file (argc, argv, quh_set_fname, (GETOPT2_FILE_FILES_ONLY | 
                     (quh.flags & QUH_RECURSIVE ? GETOPT2_FILE_RECURSIVE : 0)))) // recursively?
    {
      getopt2_usage (options);
      exit (-1);
    }

  if (!quh.files)
    {
      fprintf (stderr, "ERROR: you haven't specified any files to play\n");
      fflush (stderr);
      exit (-1);
    }

  if (!(quh.filter_chain = filter_malloc_chain (quh_filter)))
    return -1;

  if (filter_init (quh.filter_chain, NULL, NULL) == -1)
    return -1;

  quh_play ();

  return 0;
}


int
quh_configfile (void)
{
  char buf[FILENAME_MAX + 1];
  const st_property_t props[] =
    {
      {
        "ansi_color", "1",
        "use ANSI colors in output? (1=yes; 0=no)"
      },
#if 0
      {
        "quh_configdir",
#if     defined __MSDOS__ || defined __CYGWIN__ || defined _WIN32
        "~",                                    // realpath2() expands the tilde
#elif   defined __unix__ || defined __BEOS__ || defined __APPLE__ // Mac OS X actually
        "~/.quh",
#else
        "",
#endif
        "directory with additional config files"
      },
#if 0
      {
        "quh_skindir",
#if     defined __MSDOS__ || defined __CYGWIN__ || defined _WIN32
        "~",                                    // realpath2() expands the tilde
#elif   defined __unix__ || defined __BEOS__ || defined __APPLE__ // Mac OS X actually
        "~/.quh/skin",
#else
        "",
#endif
        "directory with skin for the GUI"
      },
#endif
#endif
      {NULL, NULL, NULL}
    };


  sprintf (quh.configfile, "%s" FILE_SEPARATOR_S
#ifdef  __MSDOS__
           "quh.cfg"
#else
           /*
              Use getchd() here too. If this code gets compiled under Windows the compiler has to be
              Cygwin. So, it will be a Win32 executable which will run only in an environment
              where long filenames are available and where files can have more than three characters
              as "extension". Under Bash HOME will be set, but most Windows people will propably run
              it under cmd or command where HOME is not set by default. Under Windows XP/2000
              (/NT?) USERPROFILE and/or HOMEDRIVE+HOMEPATH will be set which getchd() will "detect".
            */
           ".quhrc"
#endif
           , getenv2 ("HOME"));


  if (!access (quh.configfile, F_OK))
    {
      if (get_property_int (quh.configfile, "version") < QUH_CONFIG_VERSION)
        {
          strcpy (buf, quh.configfile);
          set_suffix (buf, ".OLD");

          printf ("NOTE: updating config: will be renamed to %s...", buf);

          rename (quh.configfile, buf);

          sync ();
        }
      else return 0;
    }
  else printf ("WARNING: %s not found: creating...", quh.configfile);

  sprintf (buf, "%d", QUH_CONFIG_VERSION);
  set_property (quh.configfile, "version", buf, "Quh configuration");

  set_property_array (quh.configfile, props);

  printf ("OK\n\n");

  return 0;
}
