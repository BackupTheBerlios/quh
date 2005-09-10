/*
gui.c - gui filter for Quh

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
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <libnetgui.h>
#include "misc/itypes.h"
#include "misc/getopt2.h"
#include "misc/misc.h"
#include "misc/file.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh_filter.h"
#include "quh.h"
#include "quh_misc.h"
#include "gui.h"


#if     FILTER_MAX > 999
#define QUH_FILTER_COLUMN_S "       "
#define QUH_FILTER_COUNTER_S "%04d"
#elif   FILTER_MAX > 99
#define QUH_FILTER_COLUMN_S "      "
#define QUH_FILTER_COUNTER_S "%03d"
#elif   FILTER_MAX > 9
#define QUH_FILTER_COLUMN_S "     "
#define QUH_FILTER_COUNTER_S "%02d"
#else
#define QUH_FILTER_COLUMN_S "    "
#define QUH_FILTER_COUNTER_S "%01d"
#endif


#define NETGUI_MAX_ARGS 0xff


static netgui_t *netgui = NULL;


#if     FILENAME_MAX < MAXBUFSIZE
static char fname[MAXBUFSIZE];
static char old_fname[MAXBUFSIZE];
#else
static char fname[FILENAME_MAX]; 
static char old_fname[FILENAME_MAX];
#endif
static unsigned int display_pos = 0;
static unsigned long t = 0, t2 = 0;
static int verbose = 0xff; // the verbosity level shall never exceed 0xff
static int output_once = 0;

enum {
  NETGUI_GUI_ROOT = 1,
  NETGUI_GUI_CONSOLE
};

const struct option example_gui_options[] = {
  {"root", 2, 0, NETGUI_GUI_ROOT},
  {"console", 2, 0, NETGUI_GUI_CONSOLE},
  {NULL, 0, 0, 0}
};

static int example_gui_sub = 0;


static void
example_gui_test_widgets (netgui_t *ng)
{
  const char *values[] = {"value1", "value2", "value3", "value4", NULL};
  char buf[FILENAME_MAX];

  ng_html (ng, 0, 0, 0);

  ng_head (ng);

  sprintf (buf, "%s" FILE_SEPARATOR_S "icon.png", quh.skindir);
  ng_title (ng, "example", buf);

  ng_head_end (ng);
    
  ng_body (ng, NULL, "#000000", "#d5d6d5");
  ng_form (ng, quh.argv[0]);
  
  sprintf (buf, "%s" FILE_SEPARATOR_S "logo.png", quh.skindir);
  ng_print (ng, "ng_img(): "); ng_img (ng, buf, -1, -1, 0, "tooltip");

  ng_hr (ng);

  ng_print (ng, "ng_submit(): "); ng_submit (ng, "ng_submit", "value", "label", "tooltip");

  ng_hr (ng);

  ng_print (ng, "ng_print(): "); ng_print (ng, "blablabla");

  ng_hr (ng);

  ng_print (ng, "ng_textarea(): "); ng_textarea (ng, "ng_textarea", "value", "tooltip", 10, 10);

  ng_hr (ng);

  ng_print (ng, "ng_text(): ");   ng_text (ng, "ng_text", "value", "tooltip", -1, -1);

  ng_hr (ng);

  ng_print (ng, "ng_select(): ");   ng_select (ng, "ng_select", values, "tooltip");

  ng_hr (ng);

  ng_print (ng, "ng_checkbox(): ");   ng_checkbox (ng, "ng_checkbox", "tooltip", TRUE);

  ng_hr (ng);

  ng_print (ng, "ng_radio(): ");   ng_radio (ng, "ng_radio", values, "tooltip");

  ng_hr (ng);

  ng_print (ng, "ng_file(): ");   ng_file (ng, "ng_file", "value", "tooltip");

  ng_hr (ng);

  ng_print (ng, "ng_file_server(): ");   ng_file_server (ng, "ng_file_server", "value", "tooltip");

  ng_hr (ng);

  sprintf (buf, "%s" FILE_SEPARATOR_S "icon.png", quh.skindir);
  ng_print (ng, "ng_image(): ");   ng_image (ng, "ng_image","value", buf, "label", -1, -1,"tooltip");

  ng_hr (ng);
  
  ng_print (ng, "ng_a(): ");   ng_a (ng, "ng_a", NULL, "value", "tooltip");

  ng_form_end (ng);
  ng_body_end (ng);
  ng_html_end (ng);
}
//#endif


int
example_gui_main (netgui_t *ng)
{
//  FILE *fh = NULL;
  int x = 0;
  int c = 0;
  int argc = 0;
  char *argv[NETGUI_MAX_ARGS];
#if 0
  netgui_strurl_t url;

  if (!(ng->url_s))
    return -1;
  if (!(*(ng->url_s)))
    return -1;
  if (!ng_strurl (&url, ng->url_s))
    return -1;
#endif

#ifdef  DEBUG
  printf ("example: ng->url_s: %s\n", ng->url_s);
  printf ("example: argc == %d\n", url.argc);
  for (x = 0; x < url.argc; x++)
    printf ("example: argv[%d]==%s\n", x, url.argv[x]);
  fflush (stdout);
#endif
          
  if (!argc)
    {
      example_gui_test_widgets (ng);
      return 0;
    }

  // argv -> getopt()
  optind = 0;
  while ((c = getopt_long_only (argc, argv, "", example_gui_options, NULL)) != -1)
    {
      switch (c)
        {
          default:
            example_gui_test_widgets (ng);
            break;
        }
#if 0
      if ((fh = popen ("example", "r"))) 
        {
          while ((*buf = getc (fh)))
            ng_write (buf, 1, 1, ng);
          close (fileno (fh));
        }
#endif 
    }

  optind = 0;
  while ((c = getopt_long_only (argc, argv, "", example_gui_options, NULL)) != -1)
    {
      switch (c)
        {
          default:
          case NETGUI_GUI_ROOT:
            example_gui_sub = 0;
//            example_gui_root (ng);
            example_gui_test_widgets (ng);
            break;
        }
    }

  return 0;
}


static int
quh_gui_init (st_quh_filter_t *file)
{
  return 0;
}


static int
quh_gui_ctrl (st_quh_filter_t *file)
{

  return 0;
}


static int
quh_gui_open (st_quh_filter_t *file)
{
  (void) file;
  int pid = 0;
  char *optarg = NULL;
  char buf[MAXBUFSIZE];
  output_once = 0;

  *buf = 0;
  strcat (buf, "Keyboard: crsr, page up/down, '>', '<', 'p' and 'q'");

  quh_set_object_s (quh.filter_chain, QUH_OUTPUT, buf);

  if (!quh_get_object_s (quh.filter_chain, QUH_OPTION))
    quh_set_object_s (quh.filter_chain, QUH_OPTION, "0");

//  if (!quh_load_netgui())
//    return -1;
  int flags = NG_GUI;

  if (!optarg)
    optarg = "netgui://localhost";
//  else
//    flags |= NG_TCP;
  flags |= NG_JOYSTICK;
//  flags |= NG_3D;

pid = fork ();
          if (!pid)
          {
  if (!(netgui = ng_init (optarg, flags, example_gui_main)))
{
printf ("TEST");
fflush (stdout);
    return -1;
}
//  ng_set_logfile (netgui, NULL);

  // set external browser for viewing help and/or the homepage
//  ng_set_external_browser (netgui, "mozilla-bin");
}

  return 0;
}


static int
quh_gui_close (st_quh_filter_t *file)
{
  (void) file;
//  quh_gui_gauge (file);

  if (netgui) 
    ng_close (netgui);

  return 0;
}


static int
quh_gui_write (st_quh_filter_t *file)
#if 1
{
  example_gui_main (netgui);
  
  return 0;
}
#else
{
  char c = 0;

  if (!output_once)
    quh_filter_output ();
  output_once = 1;

  if ((t2 = time_ms (0)) - t > 100) // only every 10th second
    {
      quh_gui_gauge (file);

      t = t2;
    }

#if 1
  if (!kbhit ())
    return 0;

  c = getch();
  switch (c)
    {
      case 'q': // quit
        quh.quit = 1;
        break;
        
      case 'p': // pause
      case ' ':
        getch ();
        break;

      case 'o': // verbosity level
        // store old value
        if (verbose == 0xff)
          verbose = quh.verbose;
        if (quh.verbose == 3)
          quh.verbose = verbose;
        else
          quh.verbose++;
        break;

      case '>': // one index/file forward
        if (file->indices)
          quh.raw_pos = file->index_pos[MIN (quh_get_index (file) + 1, file->indices)];
        else
          quh.raw_pos = file->raw_size;
        filter_seek (quh.filter_chain, file);
        break;

      case '<': // one index/file backward
        if (file->indices)
          quh.raw_pos = file->index_pos[MAX (quh_get_index (file) - 1, 0)];
        else
          {
            quh.next_file = MAX (quh.next_file - 2, 0); // one file back
            quh.raw_pos = file->raw_size;
          }
        filter_seek (quh.filter_chain, file);
        break;

      case 0x1b: // esc
        if (getch () == 0x5b) // '['
          switch (getch ())
            {
              case 0x35: // page up +10 min
                if (getch () == 0x7e)
                  {
                    quh.raw_pos += MIN (quh_ms_to_bytes (file, 600000), file->raw_size - quh.raw_pos);
                    filter_seek (quh.filter_chain, file);
                  }
                break;

              case 0x36: // page down -10 min
                if (getch () == 0x7e)
                  {
                    quh.raw_pos -= MIN (quh_ms_to_bytes (file, 600000), quh.raw_pos);
                    filter_seek (quh.filter_chain, file);
                  }
                break;

              case 0x41: // crsr up +1 min
                quh.raw_pos += MIN (quh_ms_to_bytes (file, 60000), file->raw_size - quh.raw_pos);
                filter_seek (quh.filter_chain, file);
                break;

              case 0x42: // crsr down -1 min
                quh.raw_pos -= MIN (quh_ms_to_bytes (file, 60000), quh.raw_pos);
                filter_seek (quh.filter_chain, file);
                break;
                
              case 0x43: // crsr right +10 sec
                quh.raw_pos += MIN (quh_ms_to_bytes (file, 10000), file->raw_size - quh.raw_pos);
                filter_seek (quh.filter_chain, file);
                break;

              case 0x44: // crsr left -10 sec
                quh.raw_pos -= MIN (quh_ms_to_bytes (file, 10000), quh.raw_pos);
                filter_seek (quh.filter_chain, file);
                break;
                
              default:
                printf ("\nUnknown key: 0x%02x\n", c);
//                putc (c, stdout);
                break;
            }
        break;

      default:
        printf ("\nUnknown key: 0x%02x\n", c);
//        putc (c, stdout);
        break;
    }
#endif
  return 0;
}
#endif
      

static int
quh_gui_quit (st_quh_filter_t *file)
{
  (void) file;
        
//  deinit_conio ();
  return 0;
}


const st_filter_t quh_gui =
{
  QUH_GUI_PASS,
  "gui",
  NULL,
  0,
  (int (*) (void *)) &quh_gui_open,
  (int (*) (void *)) &quh_gui_close,
  NULL,
  (int (*) (void *)) &quh_gui_write,
  NULL,
  (int (*) (void *)) &quh_gui_ctrl,
  (int (*) (void *)) &quh_gui_init,
  (int (*) (void *)) &quh_gui_quit
};


const st_getopt2_t quh_gui_usage =
{
    "gui", 2, 0, QUH_GUI,
    NULL, "enable GUI (default: enabled)",
    (void *) QUH_GUI_PASS
};


#if 0


const st_getopt2_t quh_filter_write_usage[] =
{
  {
    NULL, 0, 0, 0,
    NULL, "Filter", NULL
  },
  {
    "example", 0, 0, QUH_EXAMPLE,
    NULL, "example filter that does nothing", (void *) FILTER_EXAMPLE
  },
#ifdef  USE_NETGUI
  {
    "gui", 0, 0, QUH_GUI,
    "URL", "start httpd with host and port from URL;\n"
    "quh: "OPTION_LONG_S "gui localhost:8081\n"
    "or just start 'quhgui' for a non-httpd GUI", (void *) FILTER_GUI
  },
#endif  // USE_NETGUI
//    "(default: enable build-in Osci and Spectrum analyzer)",
  {
    "ctrl", 0, 0, QUH_CTRL,
    NULL, "enable control keys (default: enabled)", (void *) FILTER_CTRL
  },
  {
    "progress", 0, 0, QUH_PROGRESS,
    NULL, "enable progress display (default: enabled)", (void *) FILTER_PROGRESS
  },
  {
    "joy", 2, 0, QUH_JOY,
    "DEVICE", "enable native joystick DEVICE control (default: /dev/js0)", (void *) FILTER_JOY
  },
#ifdef  USE_SDL
  {
    "joysdl", 2, 0, QUH_JOY,
    "N", "enable joystick N control using SDL (default: 0)", (void *) FILTER_JOYSDL
  },
#endif  // USE_SDL
  {
    "lirc", 2, 0, QUH_LIRC,
    "DEVICE", "enable remote control (default: /dev/video0)", (void *) FILTER_LIRC
  },
  {
    "convert", 1, 0, QUH_CONVERT,
    "FREQ[:BIT[:SIG[:SWAP]]]",
    "\nconvert INPUT to FREQuenzy, BITs, SIGned and SWAP bytes\n"
    "FREQ=N   set FREQuenzy to N Hz\n"
    "FREQ=def default from INPUT\n"
    "BIT=N    set to N BITs\n"
    "BIT=def  default from INPUT\n"
    "SIG=0    is unSIGned\n"
    "SIG=1    is SIGned\n"
    "SIG=def  default from INPUT\n"
    "SWAP=1   SWAP bytes (depending on BIT size)\n"
    "SWAP=def default from INPUT\n"
    OPTION_LONG_S "convert 44100:def:0 would change FREQ and SIG only",
    (void *) FILTER_CONVERT
  },
#if 0
  {
    "pitch", 1, 0, QUH_PITCH,
    "N",   "pitchs the output by factor N", (void *) FILTER_PITCH
  },
#endif
  {
    "raw", 2, 0, QUH_RAW,
    "FILE", "write as raw (1:1) FILE (default: audiodump.raw)", (void *) FILTER_RAW
  },
  {
    "wav", 2, 0, QUH_WAV,
    "FILE", "write as wav FILE (default: audiodump.wav)", (void *) FILTER_WAV
  },
#if 0
  {
    "img", 2, 0, QUH_IMG,
    "MB", "write as CD-DA image(s) with MB size and cue sheet\n"
    "(default: 700)", (void *) FILTER_IMG
  },
#endif
  {
    "stdout", 0, 0, QUH_STDOUT,
    NULL, "write to stdout", (void *) FILTER_STDOUT
  },
  {
    "stderr", 0, 0, QUH_STDERR,
    NULL, "write to stderr", (void *) FILTER_STDERR
  },
  {
    "pipe", 2, 0, QUH_PIPE,
    "PATH", "pipe through external FILTER with PATH as filename",
#if 0
    "WinAmp and QBase/VST plug-ins are detected and\n"
    "supported automatically",
#endif
    (void *) FILTER_PIPE
  },
  {
    "set", 1, 0, QUH_SET,
    "FREQ[:BIT[:SIG[:SWAP]]]", "\nlike " OPTION_LONG_S "convert but changes not the INPUT",
    (void *) FILTER_SET
  },
// TODO: OUT=... dynamical
  {
    "libao", 2, 0, QUH_LIBAO,
    "N[:OUT]", "write to soundcard N using libao (default: 0)\n"
    "OUT=0 lineout (default)\n"
    "OUT=1 headphones",
    (void *) FILTER_LIBAO
  },
  {
    "alsa", 2, 0, QUH_ALSA,
    "DEVICE[:OUT]", "write to soundcard DEVICE using ALSA (default: /dev/dsp)\n"
    "OUT=0 lineout (default)\n"
    "OUT=1 headphones",
    (void *) FILTER_ALSA
  },
  {
    "oss", 2, 0, QUH_OSS,
    "DEVICE[:OUT]", "write to soundcard DEVICE using OSS (default: /dev/dsp)\n"
    "OUT=0 lineout (default)\n"
    "OUT=1 headphones",
    (void *) FILTER_OSS
  },
#ifdef  USE_SDL
  {
    "sdl", 2, 0, QUH_SDL,
    "N[:OUT]", "write to soundcard N using SDL (default: 0)\n"
    "OUT=0 lineout (default)\n"
    "OUT=1 headphones",
    (void *) FILTER_SDL
  },
#endif  // USE_SDL
  {
    "speaker", 0, 0, QUH_SPEAKER,
    NULL, "write to build-in PC speaker (if present)", (void *) FILTER_SPEAKER
  },
#if 0
  {
    "af", 1, 0, QUH_AF,
    "FILTER[:OPTION]", "same as " OPTION_LONG_S "FILTER[:OPTION]", NULL
  },
  {
    "ao", 1, 0, QUH_AO,
    "FILTER[:OPTION]", "same as " OPTION_LONG_S "af", NULL
  },
#endif
#if 0
  {
    "killefit", 2, 0, QUH_PIPE,
    "PATH", "like " OPTION_LONG_S "pipe", (void *) FILTER_PIPE
  },
#endif
#if 0
// KILLEFIT
  {
    "osci", 0, 0, QUH_OSCI,
    NULL, "enable oscilloscope", (void *) FILTER_OSCI
  },
  {
    "spec", 0, 0, QUH_SPEC,
    NULL, "enable spectrum analyzer", (void *) FILTER_SPEC
  },
#endif
#if 0
  {
    "winamp", 1, 0, QUH_PIPE,
    "PATH", "pipe through an WinAmp plug-in with PATH as filename", (void *) FILTER_WINAMP
  },
  {
    "vst", 1, 0, QUH_PIPE,
    "PATH", "pipe through an QBase/VST plug-in with PATH as filename", (void *) FILTER_VST
  },
#endif
  {NULL, 0, 0, 0, NULL, NULL, NULL}
};



static void
quh_gui_gauge (st_quh_filter_t *file)
{
#if 0
  unsigned long index_pos = file->index_pos[quh_get_index (file)];
  char *p = NULL;
  int units = strtol (quh_get_object_s (quh.filter_chain, QUH_OPTION), NULL, 10);

  printf ("\rI" QUH_INPUT_COUNTER_S ": ", quh.current_file);

  if (file->indices)
    p = (char *) file->index_name[quh_get_index (file)];
  else
    p = (char *) basename2 ((const char *) file->fname);

#if 1
  if (strcmp (p, old_fname) || !(*fname))
    {
      strncpy (old_fname, p, sizeof (old_fname))[sizeof (old_fname) - 1] = 0;
      sprintf (fname, "        %s", p);
      display_pos = 0; // important: restart display at first char
    }
#endif
  printf ("%-8.8s", &fname[display_pos]);
  if (!fname[++display_pos])
    display_pos = 0;

  if (file->indices)
    {
      printf (" " QUH_INDEX_COUNTER_S ":", quh_get_index (file) + 1);
    }

  printf ("%s ", quh_bytes_to_units (file, quh.raw_pos - index_pos, units));

//  quh_gauge_ansi (quh.raw_pos, file->raw_size, quh.start, quh.len, 1, 2);

  printf ("%s  ", quh_bytes_to_units (file, file->raw_size - quh.raw_pos, units));

  fflush (stdout);
#endif
}


static void
quh_filter_output (void)
{
  int x = 0;
  char key_temp[MAXBUFSIZE];
  char buf[MAXBUFSIZE];
  char c;
  const char *p = NULL;
  char *p2 = NULL;

  if (quh.quiet)
    return;

  for (; x < filter_get_total (quh.filter_chain); x ++)
    {
      int id = filter_get_id (quh.filter_chain, x);

      printf ("F" QUH_FILTER_COUNTER_S ": %s\n",
        x,
        filter_get_id_s (quh.filter_chain, x));

      sprintf (key_temp, "%d:%d", id, QUH_OPTION);

      *buf = 0;
      if (!filter_object_read (quh.filter_chain, key_temp, strlen (key_temp) + 1, buf, MAXBUFSIZE))
        printf (QUH_FILTER_COLUMN_S "Option: %s\n", buf);

      sprintf (key_temp, "%d:%d:%d", filter_get_pos_by_id (quh.filter_chain, id), id, QUH_OUTPUT);

      *buf = 0;
      if (!filter_object_read (quh.filter_chain, key_temp, strlen (key_temp) + 1, buf, MAXBUFSIZE))
        {
          p = buf;
    
          for (; (p2 = strchr (p, '\n')); p = p2 + 1)
            {
              c = p2[1];
              p2[1] = 0;
              fputs (QUH_FILTER_COLUMN_S, stdout);
              fputs (p, stdout);
              p2[1] = c;
            }
          fputs (QUH_FILTER_COLUMN_S, stdout);
          fputs (p, stdout);
          fputs ("\n", stdout);
        }
    }

  fflush (stdout);
}


#endif
