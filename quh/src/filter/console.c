/*
console.c - console filter for Quh

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
#include <ctype.h>
#include <stdlib.h>
#include "misc/itypes.h"
#include "misc/getopt2.h"
#include "misc/misc.h"
#include "misc/file.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh_filter.h"
#include "quh.h"
#include "quh_misc.h"
#include "console.h"


#if     QUH_MAX_FILES > 9999
#define QUH_INPUT_COLUMN_S "        "
#define QUH_INPUT_COUNTER_S "%05d"
#elif   QUH_MAX_FILES > 999
#define QUH_INPUT_COLUMN_S "       "
#define QUH_INPUT_COUNTER_S "%04d"
#elif   QUH_MAX_FILES > 99
#define QUH_INPUT_COLUMN_S "      "
#define QUH_INPUT_COUNTER_S "%03d"
#elif   QUH_MAX_FILES > 9
#define QUH_INPUT_COLUMN_S "     "
#define QUH_INPUT_COUNTER_S "%02d"
#else
#define QUH_INPUT_COLUMN_S "    "
#define QUH_INPUT_COUNTER_S "%01d"
#endif

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


#if     FILENAME_MAX < MAXBUFSIZE
static char fname[MAXBUFSIZE];
#else
static char fname[FILENAME_MAX]; 
#endif
static unsigned int display_pos = 0;
static unsigned long t = 0, t2 = 0;
static int verbose = 0xff; // the verbosity level shall never exceed 0xff
static int output_once = 0;


static void
quh_console_gauge (st_quh_filter_t *file)
{
  unsigned long index_pos = file->index_pos[quh_get_index (file)];
  char *p = NULL;
  int units = strtol (quh_get_object_s (quh.filter_chain, QUH_OPTION), NULL, 10);

  printf ("\rI" QUH_INPUT_COUNTER_S ": ", quh.current_file);

  if (file->indices)
    p = (char *) file->index_name[quh_get_index (file)];
  else
    p = (char *) basename2 ((const char *) file->fname);

#if 0
  if (strncmp (p, fname, MIN (strlen (p), strlen (fname))) || !(*fname))
    {
printf ("\n\nSHIT%s\n\n", p);
fflush (stdout);

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

  quh_gauge_ansi (quh.raw_pos, file->raw_size, quh.start, quh.len, 1, 2);

  printf ("%s  ", quh_bytes_to_units (file, file->raw_size - quh.raw_pos, units));

  fflush (stdout);
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


static int
quh_console_init (st_quh_filter_t *file)
{
  (void) file;

  if (!quh_get_object_s (quh.filter_chain, QUH_OPTION))
    quh_set_object_s (quh.filter_chain, QUH_OPTION, "0");

  init_conio ();
  
  return 0;
}


static int
quh_console_ctrl (st_quh_filter_t *file)
{

  return 0;
}


static int
quh_console_open (st_quh_filter_t *file)
{
  (void) file;
  char buf[MAXBUFSIZE];
  output_once = 0;

  *buf = 0;
  strcat (buf, "Keyboard: crsr, page up/down, '>', '<', 'p' and 'q'");

//  map_put (quh.filter_output, (void *) QUH_CONSOLE_PASS, (void *) buf);
  quh_set_object_s (quh.filter_chain, QUH_OUTPUT, buf);

  return 0;
}


static int
quh_console_close (st_quh_filter_t *file)
{
  (void) file;
//  quh_console_gauge (file);

  if (!quh.quiet)
    printf ("\n");
  
  return 0;
}


static int
quh_console_write (st_quh_filter_t *file)
{
  char c = 0;

  if (!output_once)
    quh_filter_output ();
  output_once = 1;

  if ((t2 = time_ms (0)) - t > 100) // only every 10th second
    {
      quh_console_gauge (file);

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

      
static int
quh_console_quit (st_quh_filter_t *file)
{
  (void) file;
        
  deinit_conio ();
  return 0;
}


const st_filter_t quh_console =
{
  QUH_CONSOLE_PASS,
  "console",
  NULL,
  0,
  (int (*) (void *)) &quh_console_open,
  (int (*) (void *)) &quh_console_close,
  NULL,
  (int (*) (void *)) &quh_console_write,
  NULL,
  (int (*) (void *)) &quh_console_ctrl,
  (int (*) (void *)) &quh_console_init,
  (int (*) (void *)) &quh_console_quit
};


const st_getopt2_t quh_console_usage =
{
    "console", 2, 0, QUH_CONSOLE,
    "UNITS", "enable console (default: enabled)\n"
    "UNITS=0 show time as a clock (default)\n"
    "UNITS=1 count time in milliseconds (1/1000 s)\n"
    "UNITS=2 count time in Bytes"
    "UNITS=3 count time in kBits",
    (void *) QUH_CONSOLE_PASS
};


// some mpg123 compatibility
const st_getopt2_t quh_console_usage2 =
{
    "C", 2, 0, QUH_CONSOLE,
    NULL, NULL, (void *) QUH_CONSOLE_PASS
};
