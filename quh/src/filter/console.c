/*
console.c - console filter for Quh

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
#include <ctype.h>
#include <stdlib.h>
#include "misc/defines.h"
#include "misc/itypes.h"
#include "misc/getopt2.h"
#include "misc/misc.h"
#include "misc/file.h"
#include "misc/filter.h"
#include "misc/term.h"
#include "misc/property.h"
#include "quh_defines.h"
#include "quh_filter.h"
#include "quh.h"
#include "quh_misc.h"
#include "console.h"


enum
{
  GAUGE_MODE_METER,
  GAUGE_MODE_TIME,
  GAUGE_MODE_VOL
};


#if     FILENAME_MAX < MAXBUFSIZE
static char fname[MAXBUFSIZE];
static char old_fname[MAXBUFSIZE];
#else
static char fname[FILENAME_MAX]; 
static char old_fname[FILENAME_MAX];
#endif
static unsigned int display_pos = 0;
static int gauge_mode = GAUGE_MODE_METER;  // default
static unsigned long t = 0, t2 = 0;
static int verbose = 0xff;  // the verbosity level shall never exceed 0xff
static int output_once = 0;
static int starting = 0;    // quh is running for the 1st 2 seconds


static void
quh_console_gauge (st_quh_nfo_t *file, int mode)
{
  int width = term_w ();
  unsigned long index_pos = file->index_pos[quh_get_index (file)];
  const char *p = NULL;
  int units = strtol (quh_get_object_s (quh.filter_chain, QUH_OPTION), NULL, 10);

  if (quh.quiet)
    return;

  printf ("\rI%0*d: ", misc_digits (QUH_MAX_FILES), quh.current_file + 1);

  p = file->indices ?
    file->index_name[quh_get_index (file)] : basename2 (file->fname);

  if (strcmp (p, old_fname) || !(*fname))
    {
      strncpy (old_fname, p, sizeof (old_fname))[sizeof (old_fname) - 1] = 0;
      sprintf (fname, "        %s", p);
      display_pos = 0; // important: restart display at first char
    }
  printf ("%-8.8s", &fname[display_pos]);
  if (!fname[++display_pos])
    display_pos = 0;

  if (file->indices)
    printf (" %*d:", misc_digits (QUH_MAX_FILES), quh_get_index (file) + 1);

  fputs (quh_bytes_to_units (file, quh.raw_pos - index_pos, units), stdout);

  switch (mode)
    {
      case GAUGE_MODE_VOL:
        printf (" %3d [", quh.soundcard.vol);
        if (quh.ansi_color)
          gauge (quh.soundcard.vol, width - 51, '=', '-', 0, 4);
        else
          gauge (quh.soundcard.vol, width - 51, '=', '-', -1, -1);
        fputs ("]", stdout);
        break;

      case GAUGE_MODE_TIME:
      default:
        fputs (" [", stdout);
        if (quh.ansi_color)
          gauge (misc_percent (quh.raw_pos, file->raw_size), width - 47, '=', '-', 1, 2);
        else
          gauge (misc_percent (quh.raw_pos, file->raw_size), width - 47, '=', '-', -1, -1);
        fputs ("]", stdout);
        break;
#if 0
      case GAUGE_MODE_METER:
      default:
        {
          int left = rand() % 100;
          int right = left;
          int low, high;

          left = misc_percent (16, left);

          low = misc_percent (12, left);
          high = misc_percent (4, left / 4);

          fputs (" [", stdout);
          if (quh.ansi_color)
            {
              gauge (high, 4, '=', '-', 3, 0);
              gauge (low, 12, '=', '-', 2, 0);
            }
          else
            {
              gauge (high, 4, '=', '-', -1, -1);
              gauge (low, 12, '=', '-', -1, -1);
            }
          fputc ('0', stdout);

          low = MIN (right, MAX (right - 75, 0));
          high = MAX (right, MAX (right - 75, 0)); 

          if (quh.ansi_color)
            {
              gauge (100 - low, 12, '=', '-', 0, 2);
              gauge (100 - high, 4, '=', '-', 0, 3);
            }
          else
            {
              gauge (100 - low, 12, '=', '-', -1, -1);
              gauge (100 - high, 4, '=', '-', -1, -1);
            }
          fputc (']', stdout);
        }
#endif
    }

//  printf ("%s%s%s", term_up(), term_up(), term_up());

  fputs (quh_bytes_to_units (file, file->raw_size - quh.raw_pos, units), stdout);
  fputs ("  ", stdout);

  fflush (stdout);
}


static void
quh_filter_output (void)
{
  int pos = 0;
  char *key = NULL;
  char buf[MAXBUFSIZE];
  char c;
  const char *p = NULL;
  char *p2 = NULL;

  if (!quh.verbose)
    return;

  for (; pos < filter_get_total (quh.filter_chain); pos++)
    {
      int id = filter_get_id (quh.filter_chain, pos);
      int subkey = 0;

      printf ("F%0*d: %s\n",
        misc_digits (FILTER_MAX),
        pos,
        filter_get_id_s (quh.filter_chain, pos));

      subkey = QUH_OPTION;
      key = filter_generate_key (NULL, &id, &subkey);
      if (cache_read (quh.o, key, strlen (key) + 1, buf, MAXBUFSIZE))
        printf ("%*cOption: %s\n", misc_digits (FILTER_MAX) + 3, ' ', buf);

      subkey = QUH_OUTPUT;
      key = filter_generate_key (&pos, &id, &subkey);
      if (cache_read (quh.o, key, strlen (key) + 1, buf, MAXBUFSIZE))
        {
          p = buf;
    
          for (; (p2 = strchr (p, '\n')); p = p2 + 1)
            {
              c = p2[1];
              p2[1] = 0;
              printf ("%*c%s", misc_digits (FILTER_MAX) + 3, ' ', p);
              p2[1] = c;
            }
          printf ("%*c%s\n", misc_digits (FILTER_MAX) + 3, ' ', p);
        }
    }

  fflush (stdout);
}


static int
quh_console_init (st_quh_nfo_t *file)
{
  (void) file;

  if (!quh_get_object_s (quh.filter_chain, QUH_OPTION))
    quh_set_object_s (quh.filter_chain, QUH_OPTION, "0");

  init_conio ();
  term_open ();

  return 0;
}


static int
quh_console_open (st_quh_nfo_t *file)
{
  (void) file;
  char buf[MAXBUFSIZE];

  output_once = 0;

  strcpy (buf, "Keys: crsr, pg up/dn, '>', '<', '9', '0', 'p'ause and 'q'uit");

  quh_set_object_s (quh.filter_chain, QUH_OUTPUT, buf);

  return 0;
}


static int
quh_console_close (st_quh_nfo_t *file)
{
  (void) file;
  quh_console_gauge (file, gauge_mode);

  if (quh.verbose)
    fputc ('\n', stdout);
  
  return 0;
}


static int
quh_console_write (st_quh_nfo_t *file)
{
  char c = 0;
  static int display_delay = 0;

  if (!output_once)
    {
      quh_filter_output ();

#ifdef  USE_TERMCAP
      // scroll 3 rows
//      printf ("\n\n\n%s%s%s", term_up(), term_up(), term_up());
#endif  // USE_TERMCAP
      output_once = 1;
    }

  t2 = time_ms (0);

  if (t2 - t > 100) // only every 10th second
    {
      quh_console_gauge (file, gauge_mode);
      
      if (display_delay)
        display_delay--;
      else
        gauge_mode = GAUGE_MODE_METER; // default

      t = t2;
    }

  if (!kbhit ())
    return 0;

  c = getch();

  display_delay = 10; // 1 second

  switch (c)
    {
      case 'x': // quit
        quh.settings = quh.soundcard.vol;
        quh.quit = 1;
        break;

      case 'q': // quitting
#if 0
        if (quh.quitting)
          quh.quit = 1;
        else
          {
            quh.settings = quh.soundcard.vol;
            quh.quitting = 1;
          }
        else
#endif
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

      case '9': // vol. down
        starting = 2; // stop auto volume
        gauge_mode = GAUGE_MODE_VOL;
        quh.soundcard.vol = MAX (0, quh.soundcard.vol - 3);
        break;

      case '0': // vol. up
        starting = 2; // stop auto volume
        gauge_mode = GAUGE_MODE_VOL;
        quh.soundcard.vol = MIN (100, quh.soundcard.vol + 3);
        break;

      case '>': // one index/file forward
        gauge_mode = GAUGE_MODE_TIME;
        if (file->indices)
          quh.raw_pos = file->index_pos[MIN (quh_get_index (file) + 1, file->indices)];
        else
          quh.raw_pos = file->raw_size;
        filter_seek (quh.filter_chain, file);
        break;

      case '<': // one index/file backward
        gauge_mode = GAUGE_MODE_TIME;
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
                    gauge_mode = GAUGE_MODE_TIME;
                    quh.raw_pos += MIN (quh_ms_to_bytes (file, 600000), file->raw_size - quh.raw_pos);
                    filter_seek (quh.filter_chain, file);
                  }
                break;

              case 0x36: // page down -10 min
                if (getch () == 0x7e)
                  {
                    gauge_mode = GAUGE_MODE_TIME;
                    quh.raw_pos -= MIN (quh_ms_to_bytes (file, 600000), quh.raw_pos);
                    filter_seek (quh.filter_chain, file);
                  }
                break;

              case 0x41: // crsr up +1 min
                gauge_mode = GAUGE_MODE_TIME;
                quh.raw_pos += MIN (quh_ms_to_bytes (file, 60000), file->raw_size - quh.raw_pos);
                filter_seek (quh.filter_chain, file);
                break;

              case 0x42: // crsr down -1 min
                gauge_mode = GAUGE_MODE_TIME;
                quh.raw_pos -= MIN (quh_ms_to_bytes (file, 60000), quh.raw_pos);
                filter_seek (quh.filter_chain, file);
                break;
                
              case 0x43: // crsr right +10 sec
                gauge_mode = GAUGE_MODE_TIME;
                quh.raw_pos += MIN (quh_ms_to_bytes (file, 10000), file->raw_size - quh.raw_pos);
                filter_seek (quh.filter_chain, file);
                break;

              case 0x44: // crsr left -10 sec
                gauge_mode = GAUGE_MODE_TIME;
                quh.raw_pos -= MIN (quh_ms_to_bytes (file, 10000), quh.raw_pos);
                filter_seek (quh.filter_chain, file);
                break;
                
              default:
                fprintf (stderr, "\nUnknown key: 0x%02x\n", c);
//                putc (c, stdout);
                break;
            }
        break;

      default:
        fprintf (stderr, "\nUnknown key: 0x%02x\n", c);
//        putc (c, stdout);
        break;
    }

  return 0;
}

      
static int
quh_console_quit (st_quh_nfo_t *file)
{
  (void) file;
        
  deinit_conio ();

  return 0;
}


QUH_FILTER_FUNC_DUMMY (quh_console_seek)
QUH_FILTER_FUNC_DUMMY (quh_console_ctrl)
QUH_FILTER_OUT(quh_console, QUH_CONSOLE_PASS,"console")


const st_getopt2_t quh_console_usage =
{
    "console", 2, 0, QUH_CONSOLE,
    "UNITS", "enable console (default: enabled)\n"
    "UNITS=0 show time as a clock (default)\n"
    "UNITS=1 count time in milliseconds (1/1000 s)\n"
    "UNITS=2 count time in Bytes\n"
    "UNITS=3 count time in kBits"
};
