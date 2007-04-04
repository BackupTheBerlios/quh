/*
misc.c - miscellaneous functions

Copyright (c) 1999 - 2005 NoisyB
Copyright (c) 2001 - 2005 dbjh
Copyright (c) 2002 - 2004 Jan-Erik Karlsson (Amiga code)


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
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef  HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>                             // va_arg()
#include <sys/stat.h>                           // for S_IFLNK

#ifdef  __MSDOS__
#include <dos.h>                                // delay(), milliseconds
#elif   defined __unix__
#include <unistd.h>                             // usleep(), microseconds
#elif   defined __BEOS__
#include <OS.h>                                 // snooze(), microseconds
// Include OS.h before misc.h, because OS.h includes StorageDefs.h which
//  includes param.h which unconditionally defines MIN and MAX.
#elif   defined AMIGA
#include <unistd.h>
#include <fcntl.h>
#include <dos/dos.h>
#include <dos/var.h>
#include <dos/dostags.h>
#include <libraries/lowlevel.h>                 // GetKey()
#include <proto/dos.h>
#include <proto/lowlevel.h>
#elif   defined _WIN32
#include <windows.h>                            // Sleep(), milliseconds
#endif
#ifdef  HAVE_INTTYPES_H
#include "inttypes.h"
#else
#include "misc/itypes.h"
#endif
#include "misc.h"


#ifdef  DJGPP
#ifdef  DLL
#include "dxedll_priv.h"
#endif
#endif


#ifdef  MAXBUFSIZE
#undef  MAXBUFSIZE
#endif  // MAXBUFSIZE
#define MAXBUFSIZE 32768


#ifndef _WIN32
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif


typedef struct st_func_node
{
  void (*func) (void);
  struct st_func_node *next;
} st_func_node_t;

static st_func_node_t func_list = { NULL, NULL };
static int func_list_locked = 0;


int
misc_digits (unsigned long v)
{
  int ret = 1;

  if ( v >= 100000000 ) { ret += 8; v /= 100000000; }
  if ( v >=     10000 ) { ret += 4; v /=     10000; }
  if ( v >=       100 ) { ret += 2; v /=       100; }
  if ( v >=        10 ) { ret += 1;                 }

  return ret;
}


int
bytes_per_second (time_t start_time, int nbytes)
{
  int curr = time (NULL) - start_time;

  if (curr < 1)
    curr = 1;                                   // "round up" to at least 1 sec (no division
                                                //  by zero below)
  return nbytes / curr;                         // # bytes/second (average transfer speed)
}


int
misc_percent (unsigned long pos, unsigned long len)
{
  if (len < 1)
    len = 1;

  return (int) ((((int64_t) 100) * pos) / len);
}


#ifdef  __CYGWIN__
/*
  Weird problem with combination Cygwin uCON64 exe and cmd.exe (Bash is ok):
  When a string with "e (e with diaeresis, one character) is read from an
  environment variable, the character isn't the right character for accessing
  the file system. We fix this.
  TODO: fix the same problem for other non-ASCII characters (> 127).
*/
char *
fix_character_set (char *str)
{
  int n, l = strlen (str);
  unsigned char *ptr = (unsigned char *) str;

  for (n = 0; n < l; n++)
    {
      if (ptr[n] == 0x89)                       // e diaeresis
        ptr[n] = 0xeb;
      else if (ptr[n] == 0x84)                  // a diaeresis
        ptr[n] = 0xe4;
      else if (ptr[n] == 0x8b)                  // i diaeresis
        ptr[n] = 0xef;
      else if (ptr[n] == 0x94)                  // o diaeresis
        ptr[n] = 0xf6;
      else if (ptr[n] == 0x81)                  // u diaeresis
        ptr[n] = 0xfc;
    }

  return str;
}
#endif


void
wait2 (int nmillis)
{
#ifdef  __MSDOS__
  delay (nmillis);
#elif   defined __unix__ || defined __APPLE__   // Mac OS X actually
  usleep (nmillis * 1000);
#elif   defined __BEOS__
  snooze (nmillis * 1000);
#elif   defined AMIGA
  Delay (nmillis * 1000);
#elif   defined _WIN32
  Sleep (nmillis);
#else
#ifdef  __GNUC__
#warning Please provide a wait2() implementation
#else
#pragma message ("Please provide a wait2() implementation")
#endif
  volatile int n;
  for (n = 0; n < nmillis * 65536; n++)
    ;
#endif
}


void
dumper (FILE *output, const void *buffer, size_t bufferlen, int virtual_start,
        unsigned int flags)
#define DUMPER_REPLACER '.'
{
  size_t pos;
  char buf[17];
  const unsigned char *p = (const unsigned char *) buffer;
//  static int found = 0;

  memset (buf, 0, sizeof (buf));
  for (pos = 0; pos < bufferlen; pos++, p++)
    if (flags & DUMPER_TEXT)
      {
        if (isalnum (*p) || *p == ' ')
          {
//            if (found > 1) // at least 2 chars
              fputc (*p, output);
//            found++;
          }
//        else found = 0;
      }
    else if (flags & DUMPER_BIT)
      {
        if (!(pos & 3))
          fprintf (output, (flags & DUMPER_DEC_COUNT ? "%010d  " : "%08x  "),
            (int) (pos + virtual_start));

        fprintf (output, "%02x  %08d  ",
                         *p,
                         ((*p >> 7) & 1) * 10000000 +
                         ((*p >> 6) & 1) * 1000000 +
                         ((*p >> 5) & 1) * 100000 +
                         ((*p >> 4) & 1) * 10000 +
                         ((*p >> 3) & 1) * 1000 +
                         ((*p >> 2) & 1) * 100 +
                         ((*p >> 1) & 1) * 10 +
                         (*p & 1));

        *(buf + (pos & 3)) = isprint (*p) ? *p : DUMPER_REPLACER;
        if (!((pos + 1) & 3))
          fprintf (output, "%s\n", buf);
      }
    else if (flags & DUMPER_CODE)
      {
        fprintf (output, "0x%02x, ", *p);

        if (!((pos + 1) & 7))
          fprintf (output, (flags & DUMPER_DEC_COUNT ? "// (%d) 0x%x\n" : "// 0x%x (%d)\n"),
            (int) (pos + virtual_start + 1),
            (int) (pos + virtual_start + 1));
      }
    else // default
      {
        if (!(pos & 15))
          fprintf (output, (flags & DUMPER_DEC_COUNT ? "%08d  " : "%08x  "),
            (int) (pos + virtual_start));

        fprintf (output, (pos + 1) & 3 ? "%02x " : "%02x  ", *p);

        *(buf + (pos & 15)) = isprint (*p) ? *p : DUMPER_REPLACER;
        if (!((pos + 1) & 15))
          fprintf (output, "%s\n", buf);
      }

  if (flags & DUMPER_TEXT)
    return;
  else if (flags & DUMPER_BIT)
    {
      if (pos & 3)
        {
          *(buf + (pos & 3)) = 0;
          fprintf (output, "%s\n", buf);
        }
    }
  else if (flags & DUMPER_CODE)
    return;
  else // default
    {
      if (pos & 15)
        {
          *(buf + (pos & 15)) = 0;
          fprintf (output, "%s\n", buf);
        }
    }
}


int
change_mem (char *buf, int bufsize, char *searchstr, int strsize,
            char wc, char esc, char *newstr, int newsize, int offset, ...)
// convenience wrapper for change_mem2()
{
  va_list argptr;
  int i, n_esc = 0, retval;
  st_cm_set_t *sets;

  for (i = 0; i < strsize; i++)
    if (searchstr[i] == esc)
      n_esc++;

  sets = (st_cm_set_t *) malloc (n_esc * sizeof (st_cm_set_t));
  va_start (argptr, offset);
  for (i = 0; i < n_esc; i++)
    {
      sets[i].data = va_arg (argptr, char *);   // get next set of characters
      sets[i].size = va_arg (argptr, int);      // get set size
    }
  va_end (argptr);
  retval = change_mem2 (buf, bufsize, searchstr, strsize, wc, esc, newstr,
                        newsize, offset, sets);
  free (sets);
  return retval;
}



#warning MERGE THIS
/* date_parse - parse string dates into internal form
**
** Copyright © 1995 by Jef Poskanzer <jef@mail.acme.com>.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
** OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
*/

/* For a while now I've been somewhat unhappy with the various
** date-parsing routines available - yacc-based, lex-based, ad-hoc.  Large
** code size and not very portable are the main complaints.  So I wrote a
** new one that just does a bunch of sscanf's until one matches.  Slow,
** but small and portable.  To figure out what formats to support I did a
** survey of Date: lines in a bunch of Usenet articles.  The following
** two formats accounted for more than 99% of all articles:
**     DD mth YY HH:MM:SS ampm zone
**     wdy, DD mth YY HH:MM:SS ampm zone
** I added Unix ctime() format and a few others:
**     wdy, mth DD HH:MM:SS ampm zone YY
**     HH:MM:SS ampm zone DD mth YY
**     DD mth YY
**     HH:MM:SS ampm
**     DD/mth/YYYY:HH:MM:SS zone  (httpd common log format date)
** No-zone, no-seconds, and no-am/pm versions of each are also supported.
** Note that dd/mm/yy and mm/dd/yy are NOT supported - those formats are
** dumb.
*/
#ifdef  HAVE_CONFIG_H
#include "config.h"
#endif
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>


#ifndef _WIN32
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif


#ifdef OS_SYSV
extern long timezone;
#endif /* OS_SYSV */

struct strlong
{
  char *s;
  long l;
};


static void
pound_case (char *str)
{
  for (; *str != '\0'; ++str)
    {
      if (isupper ((int) *str))
        *str = tolower ((int) *str);
    }
}

static int
strlong_compare (v1, v2)
     char *v1;
     char *v2;
{
  return strcmp (((struct strlong *) v1)->s, ((struct strlong *) v2)->s);
}


static int
strlong_search (char *str, struct strlong *tab, int n, long *lP)
{
  int i, h, l, r;

  l = 0;
  h = n - 1;
  for (;;)
    {
      i = (h + l) / 2;
      r = strcmp (str, tab[i].s);
      if (r < 0)
        h = i - 1;
      else if (r > 0)
        l = i + 1;
      else
        {
          *lP = tab[i].l;
          return 1;
        }
      if (h < l)
        return 0;
    }
}


#define AMPM_NONE 0
#define AMPM_AM 1
#define AMPM_PM 2
static int
ampm_fix (int hour, int ampm)
{
  switch (ampm)
    {
    case AMPM_NONE:
      break;
    case AMPM_AM:
      if (hour == 12)
        hour = 0;
      break;
    case AMPM_PM:
      if (hour != 12)
        hour += 12;
      break;
    }
  return hour;
}


static int
scan_ampm (char *str_ampm, long *ampmP)
{
  static struct strlong ampm_tab[] = {
    {"am", AMPM_AM}, {"pm", AMPM_PM},
  };
  static int sorted = 0;

  if (!sorted)
    {
      (void) qsort (ampm_tab, sizeof (ampm_tab) / sizeof (struct strlong),
                    sizeof (struct strlong), strlong_compare);
      sorted = 1;
    }
  pound_case (str_ampm);
  return strlong_search (str_ampm, ampm_tab,
                         sizeof (ampm_tab) / sizeof (struct strlong), ampmP);
}


static int
scan_wday (char *str_wday, long *tm_wdayP)
{
  static struct strlong wday_tab[] = {
    {"sun", 0}, {"sunday", 0},
    {"mon", 1}, {"monday", 1},
    {"tue", 2}, {"tuesday", 2},
    {"wed", 3}, {"wednesday", 3},
    {"thu", 4}, {"thursday", 4},
    {"fri", 5}, {"friday", 5},
    {"sat", 6}, {"saturday", 6},
  };
  static int sorted = 0;

  if (!sorted)
    {
      (void) qsort (wday_tab, sizeof (wday_tab) / sizeof (struct strlong),
                    sizeof (struct strlong), strlong_compare);
      sorted = 1;
    }
  pound_case (str_wday);
  return strlong_search (str_wday, wday_tab,
                         sizeof (wday_tab) / sizeof (struct strlong),
                         tm_wdayP);
}


static int
scan_mon (char *str_mon, long *tm_monP)
{
  static struct strlong mon_tab[] = {
    {"jan", 0}, {"january", 0},
    {"feb", 1}, {"february", 1},
    {"mar", 2}, {"march", 2},
    {"apr", 3}, {"april", 3},
    {"may", 4},
    {"jun", 5}, {"june", 5},
    {"jul", 6}, {"july", 6},
    {"aug", 7}, {"august", 7},
    {"sep", 8}, {"september", 8},
    {"oct", 9}, {"october", 9},
    {"nov", 10}, {"november", 10},
    {"dec", 11}, {"december", 11},
  };
  static int sorted = 0;

  if (!sorted)
    {
      (void) qsort (mon_tab, sizeof (mon_tab) / sizeof (struct strlong),
                    sizeof (struct strlong), strlong_compare);
      sorted = 1;
    }
  pound_case (str_mon);
  return strlong_search (str_mon, mon_tab,
                         sizeof (mon_tab) / sizeof (struct strlong), tm_monP);
}


static int
scan_gmtoff (char *str_gmtoff, long *gmtoffP)
{
  static struct strlong gmtoff_tab[] = {
    {"gmt", 0L}, {"utc", 0L}, {"ut", 0L},
    {"0000", 0L}, {"+0000", 0L}, {"-0000", 0L},
    {"0100", 3600L}, {"+0100", 3600L}, {"-0100", -3600L},
    {"0200", 7200L}, {"+0200", 7200L}, {"-0200", -7200L},
    {"0300", 10800L}, {"+0300", 10800L}, {"-0300", -10800L},
    {"0400", 14400L}, {"+0400", 14400L}, {"-0400", -14400L},
    {"0500", 18000L}, {"+0500", 18000L}, {"-0500", -18000L},
    {"0600", 21600L}, {"+0600", 21600L}, {"-0600", -21600L},
    {"0700", 25200L}, {"+0700", 25200L}, {"-0700", -25200L},
    {"0800", 28800L}, {"+0800", 28800L}, {"-0800", -28800L},
    {"0900", 32400L}, {"+0900", 32400L}, {"-0900", -32400L},
    {"1000", 36000L}, {"+1000", 36000L}, {"-1000", -36000L},
    {"1100", 39600L}, {"+1100", 39600L}, {"-1100", -39600L},
    {"1200", 43200L}, {"+1200", 43200L}, {"-1200", -43200L},
    {"cet", 3600L}, {"ced", 7200L},     /* Central European time */
    {"mez", 3600L}, {"mesz", 7200L},    /* Mitteleuropdische Zeit */
    {"jst", 32400L}, {"jdt", 36000L},   /* Japan time */
    {"bst", -3600L},
    {"nst", -12600L},
    {"ast", -14400L}, {"adt", -10800L},
    {"est", -18000L}, {"edt", -14400L},
    {"cst", -21600L}, {"cdt", -18000L},
    {"mst", -25200L}, {"mdt", -21600L},
    {"pst", -28800L}, {"pdt", -25200L},
    {"yst", -32400L}, {"ydt", -28800L},
    {"hst", -36000L}, {"hdt", -32400L},
    {"a", -3600L}, {"b", -7200L}, {"c", -10800L}, {"d", -14400L},
    {"e", -18000L}, {"f", -21600L}, {"g", -25200L}, {"h", -28800L},
    {"i", -32400L}, {"k", -36000L}, {"l", -39600L}, {"m", -43200L},
    {"n", 3600L}, {"o", 7200L}, {"p", 10800L}, {"q", 14400L},
    {"r", 18000L}, {"s", 21600L}, {"t", 25200L}, {"u", 28800L},
    {"v", 32400L}, {"w", 36000L}, {"x", 39600L}, {"y", 43200L},
    {"z", 0L},
  };
  static int sorted = 0;

  if (!sorted)
    {
      (void) qsort (gmtoff_tab, sizeof (gmtoff_tab) / sizeof (struct strlong),
                    sizeof (struct strlong), strlong_compare);
      sorted = 1;
    }
  pound_case (str_gmtoff);
  return strlong_search (str_gmtoff, gmtoff_tab,
                         sizeof (gmtoff_tab) / sizeof (struct strlong),
                         gmtoffP);
}


static int
is_leap (int year)
{
  return year % 400 ? (year % 100 ? (year % 4 ? 0 : 1) : 0) : 1;
}


/* Basically the same as mktime(). */
static time_t
tm_to_time (struct tm *tmP)
{
  time_t t;
  static int monthtab[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
  };

  /* Years since epoch, converted to days. */
  t = (tmP->tm_year - 70) * 365;
  /* Leap days for previous years. */
  t += (tmP->tm_year - 69) / 4;
  /* Days for the beginning of this month. */
  t += monthtab[tmP->tm_mon];
  /* Leap day for this year. */
  if (tmP->tm_mon >= 2 && is_leap (tmP->tm_year + 1900))
    ++t;
  /* Days since the beginning of this month. */
  t += tmP->tm_mday - 1;        /* 1-based field */
  /* Hours, minutes, and seconds. */
  t = t * 24 + tmP->tm_hour;
  t = t * 60 + tmP->tm_min;
  t = t * 60 + tmP->tm_sec;

  return t;
}


time_t
date_parse (const char *str)
{
  time_t now;
  struct tm *now_tmP;
  struct tm tm;
  const char *cp;
  char str_mon[500], str_wday[500], str_gmtoff[500], str_ampm[500];
  int tm_sec, tm_min, tm_hour, tm_mday, tm_year;
  long tm_mon, tm_wday, ampm, gmtoff;
  int got_zone;
  time_t t;
  int i;

  /* Initialize tm with relevant parts of current local time. */
  now = time ((time_t *) 0);
  now_tmP = localtime (&now);

  (void) memset ((char *) &tm, 0, sizeof (struct tm));
  tm.tm_sec = now_tmP->tm_sec;
  tm.tm_min = now_tmP->tm_min;
  tm.tm_hour = now_tmP->tm_hour;
  tm.tm_mday = now_tmP->tm_mday;
  tm.tm_mon = now_tmP->tm_mon;
  tm.tm_year = now_tmP->tm_year;
  tm.tm_isdst = now_tmP->tm_isdst;
  ampm = AMPM_NONE;
  got_zone = 0;

  /* Find local zone offset.  This is the only real area of
   ** non-portability, and it's only used for local times that don't
   ** specify a zone - those don't occur in email and netnews.
   */
#ifdef OS_SYSV
  tzset ();
  gmtoff = -timezone;
#else /* OS_SYSV */
#ifdef OS_BSD
  gmtoff = now_tmP->tm_gmtoff;
#else /* OS_BSD */
  /* You have to fill this in yourself. */
//  gmtoff = !!!;
#endif /* OS_BSD */
#endif /* OS_SYSV */

  /* Skip initial whitespace ourselves - sscanf is clumsy at this. */
  for (cp = str; *cp == ' ' || *cp == '\t'; ++cp)
    continue;

  /* And do the sscanfs.  WARNING: you can add more formats here,
   ** but be careful!  You can easily screw up the parsing of existing
   ** formats when you add new ones.  The order is important.
   */

  /* DD/mth/YY:HH:MM:SS zone */
  if (sscanf (cp, "%d/%400[a-zA-Z]/%d:%d:%d:%d %400[^: 	\n]",
              &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min, &tm_sec,
              str_gmtoff) == 7 &&
      scan_mon (str_mon, &tm_mon) && scan_gmtoff (str_gmtoff, &gmtoff))
    {
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = tm_sec;
      got_zone = 1;
    }

  /* DD-mth-YY HH:MM:SS ampm zone */
  else
    if (((sscanf
          (cp, "%d-%400[a-zA-Z]-%d %d:%d:%d %400[apmAPM] %400[^: 	\n]",
           &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min, &tm_sec,
           str_ampm, str_gmtoff) == 8 && scan_ampm (str_ampm, &ampm))
         || sscanf (cp, "%d-%400[a-zA-Z]-%d %d:%d:%d %400[^: 	\n]",
                    &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min,
                    &tm_sec, str_gmtoff) == 7)
        && scan_mon (str_mon, &tm_mon) && scan_gmtoff (str_gmtoff, &gmtoff))
    {
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = tm_sec;
      got_zone = 1;
    }
  /* DD-mth-YY HH:MM ampm zone */
  else
    if (((sscanf
          (cp, "%d-%400[a-zA-Z]-%d %d:%d %400[apmAPM] %400[^: 	\n]",
           &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min, str_ampm,
           str_gmtoff) == 7 && scan_ampm (str_ampm, &ampm))
         || sscanf (cp, "%d-%400[a-zA-Z]-%d %d:%d %400[^: 	\n]",
                    &tm_mday, str_mon, &tm_year, &tm_hour,
                    &tm_min, str_gmtoff) == 6)
        && scan_mon (str_mon, &tm_mon) && scan_gmtoff (str_gmtoff, &gmtoff))
    {
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = 0;
      got_zone = 1;
    }
  /* DD-mth-YY HH:MM:SS ampm */
  else if (((sscanf (cp, "%d-%400[a-zA-Z]-%d %d:%d:%d %400[apmAPM]",
                     &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min, &tm_sec,
                     str_ampm) == 7 &&
             scan_ampm (str_ampm, &ampm)) ||
            sscanf (cp, "%d-%400[a-zA-Z]-%d %d:%d:%d",
                    &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min,
                    &tm_sec) == 6) && scan_mon (str_mon, &tm_mon))
    {
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = tm_sec;
    }
  /* DD-mth-YY HH:MM ampm */
  else if (((sscanf (cp, "%d-%400[a-zA-Z]-%d %d:%d %400[apmAPM]",
                     &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min,
                     str_ampm) == 6 &&
             scan_ampm (str_ampm, &ampm)) ||
            sscanf (cp, "%d-%400[a-zA-Z]-%d %d:%d",
                    &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min) == 5) &&
           scan_mon (str_mon, &tm_mon))
    {
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = 0;
    }

  /* DD mth YY HH:MM:SS ampm zone */
  else
    if (((sscanf
          (cp, "%d %400[a-zA-Z] %d %d:%d:%d %400[apmAPM] %400[^: 	\n]",
           &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min, &tm_sec,
           str_ampm, str_gmtoff) == 8 && scan_ampm (str_ampm, &ampm))
         || sscanf (cp, "%d %400[a-zA-Z] %d %d:%d:%d %400[^: 	\n]",
                    &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min,
                    &tm_sec, str_gmtoff) == 7)
        && scan_mon (str_mon, &tm_mon) && scan_gmtoff (str_gmtoff, &gmtoff))
    {
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = tm_sec;
      got_zone = 1;
    }
  /* DD mth YY HH:MM ampm zone */
  else
    if (((sscanf
          (cp, "%d %400[a-zA-Z] %d %d:%d %400[apmAPM] %400[^: 	\n]",
           &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min, str_ampm,
           str_gmtoff) == 7 && scan_ampm (str_ampm, &ampm))
         || sscanf (cp, "%d %400[a-zA-Z] %d %d:%d %400[^: 	\n]",
                    &tm_mday, str_mon, &tm_year, &tm_hour,
                    &tm_min, str_gmtoff) == 6)
        && scan_mon (str_mon, &tm_mon) && scan_gmtoff (str_gmtoff, &gmtoff))
    {
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = 0;
      got_zone = 1;
    }
  /* DD mth YY HH:MM:SS ampm */
  else if (((sscanf (cp, "%d %400[a-zA-Z] %d %d:%d:%d %400[apmAPM]",
                     &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min, &tm_sec,
                     str_ampm) == 7 &&
             scan_ampm (str_ampm, &ampm)) ||
            sscanf (cp, "%d %400[a-zA-Z] %d %d:%d:%d",
                    &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min,
                    &tm_sec) == 6) && scan_mon (str_mon, &tm_mon))
    {
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = tm_sec;
    }
  /* DD mth YY HH:MM ampm */
  else if (((sscanf (cp, "%d %400[a-zA-Z] %d %d:%d %400[apmAPM]",
                     &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min,
                     str_ampm) == 6 &&
             scan_ampm (str_ampm, &ampm)) ||
            sscanf (cp, "%d %400[a-zA-Z] %d %d:%d",
                    &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min) == 5) &&
           scan_mon (str_mon, &tm_mon))
    {
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = 0;
    }

  /* DD mth HH:MM:SS ampm */
  else if (((sscanf (cp, "%d %400[a-zA-Z] %d:%d:%d %400[apmAPM]",
                     &tm_mday, str_mon, &tm_hour, &tm_min, &tm_sec,
                     str_ampm) == 6 &&
             scan_ampm (str_ampm, &ampm)) ||
            sscanf (cp, "%d %400[a-zA-Z] %d:%d:%d",
                    &tm_mday, str_mon, &tm_hour, &tm_min,
                    &tm_sec) == 5) && scan_mon (str_mon, &tm_mon))
    {
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = tm_sec;
    }
  /* DD mth HH:MM ampm */
  else if (((sscanf (cp, "%d %400[a-zA-Z] %d:%d %400[apmAPM]",
                     &tm_mday, str_mon, &tm_hour, &tm_min,
                     str_ampm) == 5 &&
             scan_ampm (str_ampm, &ampm)) ||
            sscanf (cp, "%d %400[a-zA-Z] %d:%d",
                    &tm_mday, str_mon, &tm_hour, &tm_min) == 4) &&
           scan_mon (str_mon, &tm_mon))
    {
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = 0;
    }

  /* HH:MM:SS ampm zone DD-mth-YY */
  else
    if (((sscanf
          (cp, "%d:%d:%d %400[apmAPM] %400[^: 	\n] %d-%400[a-zA-Z]-%d",
           &tm_hour, &tm_min, &tm_sec, str_ampm, str_gmtoff, &tm_mday,
           str_mon, &tm_year) == 8 && scan_ampm (str_ampm, &ampm))
         || sscanf (cp, "%d:%d:%d %400[^: 	\n] %d-%400[a-zA-Z]-%d",
                    &tm_hour, &tm_min, &tm_sec, str_gmtoff, &tm_mday,
                    str_mon, &tm_year) == 7)
        && scan_gmtoff (str_gmtoff, &gmtoff) && scan_mon (str_mon, &tm_mon))
    {
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = tm_sec;
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
    }
  /* HH:MM ampm zone DD-mth-YY */
  else
    if (((sscanf
          (cp, "%d:%d %400[apmAPM] %400[^: 	\n] %d-%400[a-zA-Z]-%d",
           &tm_hour, &tm_min, str_ampm, str_gmtoff, &tm_mday, str_mon,
           &tm_year) == 7 && scan_ampm (str_ampm, &ampm))
         || sscanf (cp, "%d:%d %400[^: 	\n] %d-%400[a-zA-Z]-%d",
                    &tm_hour, &tm_min, str_gmtoff, &tm_mday, str_mon,
                    &tm_year) == 6)
        && scan_gmtoff (str_gmtoff, &gmtoff) && scan_mon (str_mon, &tm_mon))
    {
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = 0;
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
    }
  /* HH:MM:SS ampm DD-mth-YY */
  else if (((sscanf (cp, "%d:%d:%d %400[apmAPM] %d-%400[a-zA-Z]-%d",
                     &tm_hour, &tm_min, &tm_sec, str_ampm, &tm_mday, str_mon,
                     &tm_year) == 7 &&
             scan_ampm (str_ampm, &ampm)) ||
            sscanf (cp, "%d:%d:%d %d-%400[a-zA-Z]-%d",
                    &tm_hour, &tm_min, &tm_sec, &tm_mday, str_mon,
                    &tm_year) == 6) && scan_mon (str_mon, &tm_mon))
    {
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = tm_sec;
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
    }
  /* HH:MM ampm DD-mth-YY */
  else if (((sscanf (cp, "%d:%d %400[apmAPM] %d-%400[a-zA-Z]-%d",
                     &tm_hour, &tm_min, str_ampm, &tm_mday, str_mon,
                     &tm_year) == 6 &&
             scan_ampm (str_ampm, &ampm)) ||
            sscanf (cp, "%d:%d %d-%400[a-zA-Z]-%d",
                    &tm_hour, &tm_min, &tm_mday, str_mon, &tm_year) == 5) &&
           scan_mon (str_mon, &tm_mon))
    {
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = 0;
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
    }

  /* HH:MM:SS ampm zone DD mth YY */
  else
    if (((sscanf
          (cp, "%d:%d:%d %400[apmAPM] %400[^: 	\n] %d %400[a-zA-Z] %d",
           &tm_hour, &tm_min, &tm_sec, str_ampm, str_gmtoff, &tm_mday,
           str_mon, &tm_year) == 8 && scan_ampm (str_ampm, &ampm))
         || sscanf (cp, "%d:%d:%d %400[^: 	\n] %d %400[a-zA-Z] %d",
                    &tm_hour, &tm_min, &tm_sec, str_gmtoff, &tm_mday,
                    str_mon, &tm_year) == 7)
        && scan_gmtoff (str_gmtoff, &gmtoff) && scan_mon (str_mon, &tm_mon))
    {
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = tm_sec;
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
    }
  /* HH:MM ampm zone DD mth YY */
  else
    if (((sscanf
          (cp, "%d:%d %400[apmAPM] %400[^: 	\n] %d %400[a-zA-Z] %d",
           &tm_hour, &tm_min, str_ampm, str_gmtoff, &tm_mday, str_mon,
           &tm_year) == 7 && scan_ampm (str_ampm, &ampm))
         || sscanf (cp, "%d:%d %400[^: 	\n] %d %400[a-zA-Z] %d",
                    &tm_hour, &tm_min, str_gmtoff, &tm_mday, str_mon,
                    &tm_year) == 6)
        && scan_gmtoff (str_gmtoff, &gmtoff) && scan_mon (str_mon, &tm_mon))
    {
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = 0;
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
    }
  /* HH:MM:SS ampm DD mth YY */
  else if (((sscanf (cp, "%d:%d:%d %400[apmAPM] %d %400[a-zA-Z] %d",
                     &tm_hour, &tm_min, &tm_sec, str_ampm, &tm_mday, str_mon,
                     &tm_year) == 7 &&
             scan_ampm (str_ampm, &ampm)) ||
            sscanf (cp, "%d:%d:%d %d %400[a-zA-Z] %d",
                    &tm_hour, &tm_min, &tm_sec, &tm_mday, str_mon,
                    &tm_year) == 6) && scan_mon (str_mon, &tm_mon))
    {
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = tm_sec;
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
    }
  /* HH:MM ampm DD mth YY */
  else if (((sscanf (cp, "%d:%d %400[apmAPM] %d %400[a-zA-Z] %d",
                     &tm_hour, &tm_min, str_ampm, &tm_mday, str_mon,
                     &tm_year) == 6 &&
             scan_ampm (str_ampm, &ampm)) ||
            sscanf (cp, "%d:%d %d %400[a-zA-Z] %d",
                    &tm_hour, &tm_min, &tm_mday, str_mon, &tm_year) == 5) &&
           scan_mon (str_mon, &tm_mon))
    {
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = 0;
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
    }

  /* wdy, DD-mth-YY HH:MM:SS ampm zone */
  else
    if (((sscanf
          (cp,
           "%400[a-zA-Z,] %d-%400[a-zA-Z]-%d %d:%d:%d %400[apmAPM] %400[^: 	\n]",
           str_wday, &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min,
           &tm_sec, str_ampm, str_gmtoff) == 9
          && scan_ampm (str_ampm, &ampm))
         || sscanf (cp,
                    "%400[a-zA-Z], %d-%400[a-zA-Z]-%d %d:%d:%d %400[^: 	\n]",
                    str_wday, &tm_mday, str_mon, &tm_year, &tm_hour,
                    &tm_min, &tm_sec, str_gmtoff) == 8)
        && scan_wday (str_wday, &tm_wday)
        && scan_mon (str_mon, &tm_mon) && scan_gmtoff (str_gmtoff, &gmtoff))
    {
      tm.tm_wday = tm_wday;
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = tm_sec;
      got_zone = 1;
    }
  /* wdy, DD-mth-YY HH:MM ampm zone */
  else
    if (((sscanf
          (cp,
           "%400[a-zA-Z,] %d-%400[a-zA-Z]-%d %d:%d %400[apmAPM] %400[^: 	\n]",
           str_wday, &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min,
           str_ampm, str_gmtoff) == 8 && scan_ampm (str_ampm, &ampm))
         || sscanf (cp,
                    "%400[a-zA-Z], %d-%400[a-zA-Z]-%d %d:%d %400[^: 	\n]",
                    str_wday, &tm_mday, str_mon, &tm_year, &tm_hour,
                    &tm_min, str_gmtoff) == 7)
        && scan_wday (str_wday, &tm_wday) && scan_mon (str_mon, &tm_mon)
        && scan_gmtoff (str_gmtoff, &gmtoff))
    {
      tm.tm_wday = tm_wday;
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = 0;
      got_zone = 1;
    }
  /* wdy, DD-mth-YY HH:MM:SS ampm */
  else
    if (((sscanf
          (cp, "%400[a-zA-Z,] %d-%400[a-zA-Z]-%d %d:%d:%d %400[apmAPM]",
           str_wday, &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min, &tm_sec,
           str_ampm) == 8 && scan_ampm (str_ampm, &ampm))
         || sscanf (cp, "%400[a-zA-Z], %d-%400[a-zA-Z]-%d %d:%d:%d", str_wday,
                    &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min,
                    &tm_sec) == 7) && scan_wday (str_wday, &tm_wday)
        && scan_mon (str_mon, &tm_mon))
    {
      tm.tm_wday = tm_wday;
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = tm_sec;
    }
  /* wdy, DD-mth-YY HH:MM ampm */
  else
    if (((sscanf
          (cp, "%400[a-zA-Z,] %d-%400[a-zA-Z]-%d %d:%d %400[apmAPM]",
           str_wday, &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min,
           str_ampm) == 7 && scan_ampm (str_ampm, &ampm))
         || sscanf (cp, "%400[a-zA-Z], %d-%400[a-zA-Z]-%d %d:%d", str_wday,
                    &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min) == 6)
        && scan_wday (str_wday, &tm_wday) && scan_mon (str_mon, &tm_mon))
    {
      tm.tm_wday = tm_wday;
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = 0;
    }

  /* wdy, DD mth YY HH:MM:SS ampm zone */
  else
    if (((sscanf
          (cp,
           "%400[a-zA-Z,] %d %400[a-zA-Z] %d %d:%d:%d %400[apmAPM] %400[^: 	\n]",
           str_wday, &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min,
           &tm_sec, str_ampm, str_gmtoff) == 9
          && scan_ampm (str_ampm, &ampm))
         || sscanf (cp,
                    "%400[a-zA-Z], %d %400[a-zA-Z] %d %d:%d:%d %400[^: 	\n]",
                    str_wday, &tm_mday, str_mon, &tm_year, &tm_hour,
                    &tm_min, &tm_sec, str_gmtoff) == 8)
        && scan_wday (str_wday, &tm_wday)
        && scan_mon (str_mon, &tm_mon) && scan_gmtoff (str_gmtoff, &gmtoff))
    {
      tm.tm_wday = tm_wday;
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = tm_sec;
      got_zone = 1;
    }
  /* wdy, DD mth YY HH:MM ampm zone */
  else
    if (((sscanf
          (cp,
           "%400[a-zA-Z,] %d %400[a-zA-Z] %d %d:%d %400[apmAPM] %400[^: 	\n]",
           str_wday, &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min,
           str_ampm, str_gmtoff) == 8 && scan_ampm (str_ampm, &ampm))
         || sscanf (cp,
                    "%400[a-zA-Z], %d %400[a-zA-Z] %d %d:%d %400[^: 	\n]",
                    str_wday, &tm_mday, str_mon, &tm_year, &tm_hour,
                    &tm_min, str_gmtoff) == 7)
        && scan_wday (str_wday, &tm_wday) && scan_mon (str_mon, &tm_mon)
        && scan_gmtoff (str_gmtoff, &gmtoff))
    {
      tm.tm_wday = tm_wday;
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = 0;
      got_zone = 1;
    }
  /* wdy, DD mth YY HH:MM:SS ampm */
  else
    if (((sscanf
          (cp, "%400[a-zA-Z,] %d %400[a-zA-Z] %d %d:%d:%d %400[apmAPM]",
           str_wday, &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min, &tm_sec,
           str_ampm) == 8 && scan_ampm (str_ampm, &ampm))
         || sscanf (cp, "%400[a-zA-Z], %d %400[a-zA-Z] %d %d:%d:%d", str_wday,
                    &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min,
                    &tm_sec) == 7) && scan_wday (str_wday, &tm_wday)
        && scan_mon (str_mon, &tm_mon))
    {
      tm.tm_wday = tm_wday;
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = tm_sec;
    }
  /* wdy, DD mth YY HH:MM ampm */
  else
    if (((sscanf
          (cp, "%400[a-zA-Z,] %d %400[a-zA-Z] %d %d:%d %400[apmAPM]",
           str_wday, &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min,
           str_ampm) == 7 && scan_ampm (str_ampm, &ampm))
         || sscanf (cp, "%400[a-zA-Z], %d %400[a-zA-Z] %d %d:%d", str_wday,
                    &tm_mday, str_mon, &tm_year, &tm_hour, &tm_min) == 6)
        && scan_wday (str_wday, &tm_wday) && scan_mon (str_mon, &tm_mon))
    {
      tm.tm_wday = tm_wday;
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = 0;
    }

  /* wdy, mth DD HH:MM:SS ampm zone YY */
  else
    if (((sscanf
          (cp,
           "%400[a-zA-Z,] %400[a-zA-Z] %d %d:%d:%d %400[apmAPM] %400[^: 	\n] %d",
           str_wday, str_mon, &tm_mday, &tm_hour, &tm_min, &tm_sec, str_ampm,
           str_gmtoff, &tm_year) == 9 && scan_ampm (str_ampm, &ampm))
         || sscanf (cp,
                    "%400[a-zA-Z] %400[a-zA-Z] %d %d:%d:%d %400[^: 	\n] %d",
                    str_wday, str_mon, &tm_mday, &tm_hour, &tm_min,
                    &tm_sec, str_gmtoff, &tm_year) == 8)
        && scan_wday (str_wday, &tm_wday) && scan_mon (str_mon, &tm_mon)
        && scan_gmtoff (str_gmtoff, &gmtoff))
    {
      tm.tm_wday = tm_wday;
      tm.tm_mon = tm_mon;
      tm.tm_mday = tm_mday;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = tm_sec;
      got_zone = 1;
      tm.tm_year = tm_year;
    }
  /* wdy, mth DD HH:MM ampm zone YY */
  else
    if (((sscanf
          (cp,
           "%400[a-zA-Z,] %400[a-zA-Z] %d %d:%d %400[apmAPM] %400[^: 	\n] %d",
           str_wday, str_mon, &tm_mday, &tm_hour, &tm_min, str_ampm,
           str_gmtoff, &tm_year) == 8 && scan_ampm (str_ampm, &ampm))
         || sscanf (cp,
                    "%400[a-zA-Z] %400[a-zA-Z] %d %d:%d %400[^: 	\n] %d",
                    str_wday, str_mon, &tm_mday, &tm_hour, &tm_min,
                    str_gmtoff, &tm_year) == 7)
        && scan_wday (str_wday, &tm_wday)
        && scan_mon (str_mon, &tm_mon) && scan_gmtoff (str_gmtoff, &gmtoff))
    {
      tm.tm_wday = tm_wday;
      tm.tm_mon = tm_mon;
      tm.tm_mday = tm_mday;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = 0;
      got_zone = 1;
      tm.tm_year = tm_year;
    }
  /* wdy, mth DD HH:MM:SS ampm YY */
  else
    if (((sscanf
          (cp, "%400[a-zA-Z,] %400[a-zA-Z] %d %d:%d:%d %400[apmAPM] %d",
           str_wday, str_mon, &tm_mday, &tm_hour, &tm_min, &tm_sec, str_ampm,
           &tm_year) == 8 && scan_ampm (str_ampm, &ampm))
         || sscanf (cp, "%400[a-zA-Z] %400[a-zA-Z] %d %d:%d:%d %d", str_wday,
                    str_mon, &tm_mday, &tm_hour, &tm_min, &tm_sec,
                    &tm_year) == 7) && scan_wday (str_wday, &tm_wday)
        && scan_mon (str_mon, &tm_mon))
    {
      tm.tm_wday = tm_wday;
      tm.tm_mon = tm_mon;
      tm.tm_mday = tm_mday;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = tm_sec;
      tm.tm_year = tm_year;
    }
  /* wdy, mth DD HH:MM ampm YY */
  else
    if (((sscanf
          (cp, "%400[a-zA-Z,] %400[a-zA-Z] %d %d:%d %400[apmAPM] %d",
           str_wday, str_mon, &tm_mday, &tm_hour, &tm_min, str_ampm,
           &tm_year) == 7 && scan_ampm (str_ampm, &ampm))
         || sscanf (cp, "%400[a-zA-Z] %400[a-zA-Z] %d %d:%d %d", str_wday,
                    str_mon, &tm_mday, &tm_hour, &tm_min, &tm_year) == 6)
        && scan_wday (str_wday, &tm_wday) && scan_mon (str_mon, &tm_mon))
    {
      tm.tm_wday = tm_wday;
      tm.tm_mon = tm_mon;
      tm.tm_mday = tm_mday;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = 0;
      tm.tm_year = tm_year;
    }

  /* mth DD HH:MM:SS ampm */
  else if (((sscanf (cp, "%400[a-zA-Z] %d %d:%d:%d %400[apmAPM]",
                     str_mon, &tm_mday, &tm_hour, &tm_min, &tm_sec,
                     str_ampm) == 6 &&
             scan_ampm (str_ampm, &ampm)) ||
            sscanf (cp, "%400[a-zA-Z] %d %d:%d:%d",
                    str_mon, &tm_mday, &tm_hour, &tm_min,
                    &tm_sec) == 5) && scan_mon (str_mon, &tm_mon))
    {
      tm.tm_mon = tm_mon;
      tm.tm_mday = tm_mday;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = tm_sec;
    }
  /* mth DD HH:MM ampm */
  else if (((sscanf (cp, "%400[a-zA-Z] %d %d:%d %400[apmAPM]",
                     str_mon, &tm_mday, &tm_hour, &tm_min,
                     str_ampm) == 5 &&
             scan_ampm (str_ampm, &ampm)) ||
            sscanf (cp, "%400[a-zA-Z] %d %d:%d",
                    str_mon, &tm_mday, &tm_hour, &tm_min) == 4) &&
           scan_mon (str_mon, &tm_mon))
    {
      tm.tm_mon = tm_mon;
      tm.tm_mday = tm_mday;
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = 0;
    }

  /* DD-mth-YY */
  else if (sscanf (cp, "%d-%400[a-zA-Z]-%d",
                   &tm_mday, str_mon, &tm_year) == 3 &&
           scan_mon (str_mon, &tm_mon))
    {
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
      tm.tm_hour = 0;
      tm.tm_min = 0;
      tm.tm_sec = 0;
    }
  /* DD mth YY */
  else if (sscanf (cp, "%d %400[a-zA-Z] %d",
                   &tm_mday, str_mon, &tm_year) == 3 &&
           scan_mon (str_mon, &tm_mon))
    {
      tm.tm_mday = tm_mday;
      tm.tm_mon = tm_mon;
      tm.tm_year = tm_year;
      tm.tm_hour = 0;
      tm.tm_min = 0;
      tm.tm_sec = 0;
    }

  /* HH:MM:SS ampm */
  else if ((sscanf (cp, "%d:%d:%d %400[apmAPM]",
                    &tm_hour, &tm_min, &tm_sec, str_ampm) == 4 &&
            scan_ampm (str_ampm, &ampm)) ||
           sscanf (cp, "%d:%d:%d", &tm_hour, &tm_min, &tm_sec) == 3)
    {
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = tm_sec;
    }
  /* HH:MM ampm */
  else if ((sscanf (cp, "%d:%d %400[apmAPM]", &tm_hour, &tm_min,
                    str_ampm) == 3 &&
            scan_ampm (str_ampm, &ampm)) ||
           sscanf (cp, "%d:%d", &tm_hour, &tm_min) == 2)
    {
      tm.tm_hour = ampm_fix (tm_hour, ampm);
      tm.tm_min = tm_min;
      tm.tm_sec = 0;
    }
  else if (sscanf (cp, "%d", &i) == 1 && i >= 300000000 && i <= 2000000000)
    /*     ~1980             ~2033   */
    {
      t = (time_t) i;
      return t;
    }
  else
    return (time_t) - 1;

  if (tm.tm_year > 1900)
    tm.tm_year -= 1900;
  else if (tm.tm_year < 70)
    tm.tm_year += 100;

  t = tm_to_time (&tm);
  t -= gmtoff;
#ifdef notdef
  /* This doesn't seem to be necessary, at least on SunOS. */
  if (tm.tm_isdst && !got_zone)
    t -= 60 * 60;
#endif

  return t;
}


#ifdef  TEST
int
main (int argc, char **argv)
{
  char line[5000];
  time_t t;

  while (fgets (line, sizeof (line), stdin) != (char *) 0)
    {
      t = date_parse (line);
      (void) printf ("%d - %s", t, ctime (&t));
    }

  exit (0);
}
#endif // TEST



time_t
parse_date_v2 (const char *s)
{
  int i = 0;
  char y[100], m[100], d[100];
  char h[100], min[100];
//  char sec[100];
  struct tm time_tag;
  time_t t = time (0);
  const char *month_s[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", NULL};

  *y = *m = *d = *h = *min = 0;

  if (s[10] == 'T')                     // YYYY-MM-DDT00:00+00:00
    {
      sscanf (s, " %4s-%2s-%2sT%2s:%2s", y, m, d, h, min);
    }
  else if (s[3] == ',' && s[4] == ' ')  // Mon, 31 Jul 2006 15:05:00 GMT
    {
      sscanf (s + 5, "%2s %s %4s %2s:%2s", d, m, y, h, min);

      for (i = 0; month_s[i]; i++)
        if (!stricmp (m, month_s[i]))
          {
            sprintf (m, "%d", i + 1);
            break;
          }
    }
  else if (s[4] == '-' && s[7] == '-')  // 2006-07-19
    {
      sscanf (s, "%4s-%2s-%2s", y, m, d);
    }
  else                                  // YYYYMMDDTHHMMSS
    {
//      sscanf (s, " %4s%2s%2sT", y, m, d);
    }

  memset (&time_tag, 0, sizeof (struct tm));

  if (*y)
    time_tag.tm_year = strtol (y, NULL, 10) - 1900;
  if (*m)
    time_tag.tm_mon = strtol (m, NULL, 10) - 1;
  if (*d)
    time_tag.tm_mday = strtol (d, NULL, 10);
  if (*h)
    time_tag.tm_hour = strtol (h, NULL, 10);
  if (*min)
    time_tag.tm_min = strtol (min, NULL, 10);

  t = mktime (&time_tag);

  return t;
}


time_t
strptime2 (const char *s)
{
  time_t t = parse_date_v2 (s);

  if (!t)
    return date_parse (s);

  return t;
}


int
change_mem2 (char *buf, int bufsize, char *searchstr, int strsize, char wc,
             char esc, char *newstr, int newsize, int offset, st_cm_set_t *sets)
/*
  Search for all occurrences of string searchstr in buf and replace newsize
  bytes in buf by copying string newstr to the end of the found search string
  in buf plus offset.
  If searchstr contains wildcard characters wc, then n wildcard characters in
  searchstr match any n characters in buf.
  If searchstr contains escape characters esc, sets must point to an array of
  sets. sets must contain as many elements as there are escape characters in
  searchstr. searchstr matches for an escape character if one of the characters
  in sets[i]->data matches.
  Note that searchstr is not necessarily a C string; it may contain one or more
  zero bytes as strsize indicates the length.
  offset is the relative offset from the last character in searchstring and may
  have a negative value.
  The return value is the number of times a match was found.
  This function was written to patch SNES ROM dumps. It does basically the same
  as the old uCON does, with one exception, the line with:
    bufpos -= n_wc;

  As stated in the comment, this causes the search to restart at the first
  wildcard character of the sequence of wildcards that was most recently
  skipped if the current character in buf didn't match the current character
  in searchstr. This makes change_mem() behave a bit more intuitive. For
  example
    char str[] = "f foobar means...";
    change_mem (str, strlen (str), "f**bar", 6, '*', '!', "XXXXXXXX", 8, 2, NULL);
  finds and changes "foobar means..." into "foobar XXXXXXXX", while with uCON's
  algorithm it would not (but does the job good enough for patching SNES ROMs).

  One example of using sets:
    char str[] = "fu-bar     is the same as foobar    ";
    st_cm_set_t sets[] = {{"o-", 2}, {"uo", 2}};
    change_mem (str, strlen (str), "f!!", 3, '*', '!', "fighter", 7, 1, sets);
  This changes str into "fu-fighter is the same as foofighter".
*/
{
  char *set;
  int bufpos, strpos = 0, pos_1st_esc = -1, setsize, i, n_wc, n_matches = 0,
      setindex = 0;

  for (bufpos = 0; bufpos < bufsize; bufpos++)
    {
      if (strpos == 0 && searchstr[0] != esc && searchstr[0] != wc)
        while (bufpos < bufsize && searchstr[0] != buf[bufpos])
          bufpos++;

      // handle escape character in searchstr
      while (searchstr[strpos] == esc && bufpos < bufsize)
        {
          if (strpos == pos_1st_esc)
            setindex = 0;                       // reset argument pointer
          if (pos_1st_esc == -1)
            pos_1st_esc = strpos;

          set = sets[setindex].data;            // get next set of characters
          setsize = sets[setindex].size;        // get set size
          setindex++;
          i = 0;
          // see if buf[bufpos] matches with any character in current set
          while (i < setsize && buf[bufpos] != set[i])
            i++;
          if (i == setsize)
            break;                              // buf[bufpos] didn't match with any char

          if (strpos == strsize - 1)            // check if we are at the end of searchstr
            {
              memcpy (buf + bufpos + offset, newstr, newsize);
              n_matches++;
              break;
            }

          strpos++;
          bufpos++;
        }
      if (searchstr[strpos] == esc)
        {
          strpos = 0;
          continue;
        }

      // skip wildcards in searchstr
      n_wc = 0;
      while (searchstr[strpos] == wc && bufpos < bufsize)
        {
          if (strpos == strsize - 1)            // check if at end of searchstr
            {
              memcpy (buf + bufpos + offset, newstr, newsize);
              n_matches++;
              break;
            }

          strpos++;
          bufpos++;
          n_wc++;
        }
      if (bufpos == bufsize)
        break;
      if (searchstr[strpos] == wc)
        {
          strpos = 0;
          continue;
        }

      if (searchstr[strpos] == esc)
        {
          bufpos--;                             // current char has to be checked, but `for'
          continue;                             //  increments bufpos
        }

      // no escape char, no wildcard -> normal character
      if (searchstr[strpos] == buf[bufpos])
        {
          if (strpos == strsize - 1)            // check if at end of searchstr
            {
              memcpy (buf + bufpos + offset, newstr, newsize);
              n_matches++;
              strpos = 0;
            }
          else
            strpos++;
        }
      else
        {
          bufpos -= n_wc;                       // scan the most recent wildcards too if
          if (strpos > 0)                       //  the character didn't match
            {
              bufpos--;                         // current char has to be checked, but `for'
              strpos = 0;                       //  increments bufpos
            }
        }
    }

  return n_matches;
}


int
build_cm_patterns (st_cm_pattern_t **patterns, const char *filename, int verbose)
/*
  This function goes a bit over the top what memory allocation technique
  concerns, but at least it's stable.
  Note the important difference between (*patterns)[0].n_sets and
  patterns[0]->n_sets (not especially that member). I (dbjh) am too ashamed to
  tell how long it took me to finally realise that...
*/
{
  char src_name[FILENAME_MAX], line[MAXBUFSIZE], buffer[MAXBUFSIZE],
       *token, *last, *ptr;
  int line_num = 0, n_sets, n_codes = 0, n, currentsize1, requiredsize1,
      currentsize2, requiredsize2, currentsize3, requiredsize3;
  FILE *srcfile;

  strcpy (src_name, filename);
  if (access (src_name, F_OK | R_OK))
    return -1;                                  // NOT an error, it's optional

  if ((srcfile = fopen (src_name, "r")) == NULL) // open in text mode
    {
      fprintf (stderr, "ERROR: Can't open \"%s\" for reading\n", src_name);
      return -1;
    }

  *patterns = NULL;
  currentsize1 = requiredsize1 = 0;
  while (fgets (line, sizeof line, srcfile) != NULL)
    {
      line_num++;
      n_sets = 0;

      ptr = line + strspn (line, "\t ");
      if (*ptr == '#' || *ptr == '\n' || *ptr == '\r')
        continue;
      if ((ptr = strpbrk (line, "\n\r#")))      // text after # is comment
        *ptr = 0;

      requiredsize1 += sizeof (st_cm_pattern_t);
      if (requiredsize1 > currentsize1)
        {
          currentsize1 = requiredsize1 + 10 * sizeof (st_cm_pattern_t);
          if (!(*patterns = (st_cm_pattern_t *) realloc (*patterns, currentsize1)))
            {
              fprintf (stderr, "ERROR: Not enough memory for buffer (%d bytes)\n", currentsize1);
              return -1;
            }
        }

      (*patterns)[n_codes].search = NULL;
      currentsize2 = 0;
      requiredsize2 = 1;                        // for string terminator
      n = 0;
      strcpy (buffer, line);
      token = strtok (buffer, ":");
      token = strtok (token, " ");
//      printf ("token: \"%s\"\n", token);
      last = token;
      // token is never NULL here (yes, tested with empty files and such)
      do
        {
          requiredsize2++;
          if (requiredsize2 > currentsize2)
            {
              currentsize2 = requiredsize2 + 10;
              if (!((*patterns)[n_codes].search =
                   (char *) realloc ((*patterns)[n_codes].search, currentsize2)))
                {
                  fprintf (stderr, "ERROR: Not enough memory for buffer (%d bytes)\n", currentsize2);
                  free (*patterns);
                  *patterns = NULL;
                  return -1;
                }
            }
          (*patterns)[n_codes].search[n] = (unsigned char) strtol (token, NULL, 16);
          n++;
        }
      while ((token = strtok (NULL, " ")));
      (*patterns)[n_codes].search_size = n;     // size in bytes

      strcpy (buffer, line);
      token = strtok (last, ":");
      token = strtok (NULL, ":");
      token = strtok (token, " ");
      last = token;
      if (!token)
        {
          printf ("WARNING: Line %d is invalid, no wildcard value is specified\n",
                  line_num);
          continue;
        }
      (*patterns)[n_codes].wildcard = (char) strtol (token, NULL, 16);

      strcpy (buffer, line);
      token = strtok (last, ":");
      token = strtok (NULL, ":");
      token = strtok (token, " ");
      last = token;
      if (!token)
        {
          printf ("WARNING: Line %d is invalid, no escape value is specified\n",
                  line_num);
          continue;
        }
      (*patterns)[n_codes].escape = (char) strtol (token, NULL, 16);

      strcpy (buffer, line);
      token = strtok (last, ":");
      token = strtok (NULL, ":");
      token = strtok (token, " ");
      last = token;
      if (!token)
        {
          printf ("WARNING: Line %d is invalid, no replacement is specified\n", line_num);
          continue;
        }
      (*patterns)[n_codes].replace = NULL;
      currentsize2 = 0;
      requiredsize2 = 1;                        // for string terminator
      n = 0;
      do
        {
          requiredsize2++;
          if (requiredsize2 > currentsize2)
            {
              currentsize2 = requiredsize2 + 10;
              if (!((*patterns)[n_codes].replace =
                   (char *) realloc ((*patterns)[n_codes].replace, currentsize2)))
                {
                  fprintf (stderr, "ERROR: Not enough memory for buffer (%d bytes)\n", currentsize2);
                  free ((*patterns)[n_codes].search);
                  free (*patterns);
                  *patterns = NULL;
                  return -1;
                }
            }
          (*patterns)[n_codes].replace[n] = (unsigned char) strtol (token, NULL, 16);
          n++;
        }
      while ((token = strtok (NULL, " ")));
      (*patterns)[n_codes].replace_size = n;  // size in bytes

      strcpy (buffer, line);
      token = strtok (last, ":");
      token = strtok (NULL, ":");
      token = strtok (token, " ");
      last = token;
      if (!token)
        {
          printf ("WARNING: Line %d is invalid, no offset is specified\n", line_num);
          continue;
        }
      (*patterns)[n_codes].offset = strtol (token, NULL, 10); // yes, offset is decimal

      if (verbose)
        {
          printf ("\n"
                  "line:         %d\n"
                  "searchstring: ",
                  line_num);
          for (n = 0; n < (*patterns)[n_codes].search_size; n++)
            printf ("%02x ", (unsigned char) (*patterns)[n_codes].search[n]);
          printf ("(%d)\n"
                  "wildcard:     %02x\n"
                  "escape:       %02x\n"
                  "replacement:  ",
                  (*patterns)[n_codes].search_size,
                  (unsigned char) (*patterns)[n_codes].wildcard,
                  (unsigned char) (*patterns)[n_codes].escape);
          for (n = 0; n < (*patterns)[n_codes].replace_size; n++)
            printf ("%02x ", (unsigned char) (*patterns)[n_codes].replace[n]);
          printf ("(%d)\n"
                  "offset:       %d\n",
                  (*patterns)[n_codes].replace_size,
                  (*patterns)[n_codes].offset);
        }

      (*patterns)[n_codes].sets = NULL;
      currentsize2 = 0;
      requiredsize2 = 1;                        // for string terminator
      strcpy (buffer, line);
      token = strtok (last, ":");
      token = strtok (NULL, ":");
      last = token;
      while (token)
        {
          requiredsize2 += sizeof (st_cm_set_t);
          if (requiredsize2 > currentsize2)
            {
              currentsize2 = requiredsize2 + 10 * sizeof (st_cm_set_t);
              if (!((*patterns)[n_codes].sets = (st_cm_set_t *)
                    realloc ((*patterns)[n_codes].sets, currentsize2)))
                {
                  fprintf (stderr, "ERROR: Not enough memory for buffer (%d bytes)\n", currentsize2);
                  free ((*patterns)[n_codes].replace);
                  free ((*patterns)[n_codes].search);
                  free (*patterns);
                  *patterns = NULL;
                  return -1;
                }
            }

          (*patterns)[n_codes].sets[n_sets].data = NULL;
          currentsize3 = 0;
          requiredsize3 = 1;                    // for string terminator
          n = 0;
          token = strtok (token, " ");
          do
            {
              requiredsize3++;
              if (requiredsize3 > currentsize3)
                {
                  currentsize3 = requiredsize3 + 10;
                  if (!((*patterns)[n_codes].sets[n_sets].data = (char *)
                        realloc ((*patterns)[n_codes].sets[n_sets].data, currentsize3)))
                    {
                      fprintf (stderr, "ERROR: Not enough memory for buffer (%d bytes)\n", currentsize3);
                      free ((*patterns)[n_codes].sets);
                      free ((*patterns)[n_codes].replace);
                      free ((*patterns)[n_codes].search);
                      free (*patterns);
                      *patterns = NULL;
                      return -1;
                    }
                }
              (*patterns)[n_codes].sets[n_sets].data[n] =
                (unsigned char) strtol (token, NULL, 16);
              n++;
            }
          while ((token = strtok (NULL, " ")));
          (*patterns)[n_codes].sets[n_sets].size = n;

          if (verbose)
            {
              printf ("set:          ");
              for (n = 0; n < (*patterns)[n_codes].sets[n_sets].size; n++)
                printf ("%02x ", (unsigned char) (*patterns)[n_codes].sets[n_sets].data[n]);
              printf ("(%d)\n", (*patterns)[n_codes].sets[n_sets].size);
            }

          strcpy (buffer, line);
          token = strtok (last, ":");
          token = strtok (NULL, ":");
          last = token;

          n_sets++;
        }
      (*patterns)[n_codes].n_sets = n_sets;

      n_codes++;
    }
  fclose (srcfile);
  return n_codes;
}


void
cleanup_cm_patterns (st_cm_pattern_t **patterns, int n_patterns)
{
  int n, m;
  for (n = 0; n < n_patterns; n++)
    {
      free ((*patterns)[n].search);
      (*patterns)[n].search = NULL;
      free ((*patterns)[n].replace);
      (*patterns)[n].replace = NULL;
      for (m = 0; m < (*patterns)[n].n_sets; m++)
        {
          free ((*patterns)[n].sets[m].data);
          (*patterns)[n].sets[m].data = NULL;
        }
      free ((*patterns)[n].sets);
      (*patterns)[n].sets = NULL;
    }
  free (*patterns);
  *patterns = NULL;
}


char *
getenv2 (const char *variable)
/*
  getenv() suitable for enviroments w/o HOME, TMP or TEMP variables.
  The caller should copy the returned string to it's own memory, because this
  function will overwrite that memory on the next call.
  Note that this function never returns NULL.
*/
{
  char *tmp;
  static char value[MAXBUFSIZE];
#if     defined __CYGWIN__ || defined __MSDOS__
/*
  Under DOS and Windows the environment variables are not stored in a case
  sensitive manner. The run-time systems of DJGPP and Cygwin act as if they are
  stored in upper case. Their getenv() however *is* case sensitive. We fix this
  by changing all characters of the search string (variable) to upper case.

  Note that under Cygwin's Bash environment variables *are* stored in a case
  sensitive manner.
*/
  char tmp2[MAXBUFSIZE];

  strcpy (tmp2, variable);
  variable = strupr (tmp2);                     // DON'T copy the string into variable
#endif                                          //  (variable itself is local)

  *value = 0;

  if ((tmp = getenv (variable)) != NULL)
    strcpy (value, tmp);
  else
    {
      if (!strcmp (variable, "HOME"))
        {
          if ((tmp = getenv ("USERPROFILE")) != NULL)
            strcpy (value, tmp);
          else if ((tmp = getenv ("HOMEDRIVE")) != NULL)
            {
              strcpy (value, tmp);
              tmp = getenv ("HOMEPATH");
              strcat (value, tmp ? tmp : FILE_SEPARATOR_S);
            }
          else
            /*
              Don't just use C:\\ under DOS, the user might not have write access
              there (Windows NT DOS-box). Besides, it would make uCON64 behave
              differently on DOS than on the other platforms.
              Returning the current directory when none of the above environment
              variables are set can be seen as a feature. A frontend could execute
              uCON64 with an environment without any of the environment variables
              set, so that the directory from where uCON64 starts will be used.
            */
            {
              char c;
              getcwd (value, FILENAME_MAX);
              c = toupper (*value);
              // if current dir is root dir strip problematic ending slash (DJGPP)
              if (c >= 'A' && c <= 'Z' &&
                  value[1] == ':' && value[2] == '/' && value[3] == 0)
                value[2] = 0;
            }
         }

      if (!strcmp (variable, "TEMP") || !strcmp (variable, "TMP"))
        {
#if     defined __MSDOS__ || defined __CYGWIN__
          /*
            DJGPP and (yet another) Cygwin quirck
            A trailing backslash is used to check for a directory. Normally
            DJGPP's run-time system is able to handle forward slashes in paths,
            but access() won't differentiate between files and dirs if a
            forward slash is used. Cygwin's run-time system seems to handle
            paths with forward slashes quite different from paths with
            backslashes. This trick seems to work only if a backslash is used.
          */
          if (access ("\\tmp\\", R_OK | W_OK) == 0)
#else
          // trailing file separator to force it to be a directory
          if (access (FILE_SEPARATOR_S"tmp"FILE_SEPARATOR_S, R_OK | W_OK) == 0)
#endif
            strcpy (value, FILE_SEPARATOR_S"tmp");
          else
            getcwd (value, FILENAME_MAX);
        }
    }

#ifdef  __CYGWIN__
  /*
    Under certain circumstances Cygwin's run-time system returns "/" as value
    of HOME while that var has not been set. To specify a root dir a path like
    /cygdrive/<drive letter> or simply a drive letter should be used.
  */
  if (!strcmp (variable, "HOME") && !strcmp (value, "/"))
    getcwd (value, FILENAME_MAX);

  return fix_character_set (value);
#else
  return value;
#endif
}


#if     defined __unix__ && !defined __MSDOS__
int
drop_privileges (void)
{
  uid_t uid;
  gid_t gid;

  uid = getuid ();
  if (setuid (uid) == -1)
    {
      fprintf (stderr, "ERROR: Could not set uid\n");
      return 1;
    }
  gid = getgid ();                              // This shouldn't be necessary
  if (setgid (gid) == -1)                       //  if `make install' was
    {                                           //  used, but just in case
      fprintf (stderr, "ERROR: Could not set gid\n"); //  (root did `chmod +s')
      return 1;
    }

  return 0;
}
#endif


int
register_func (void (*func) (void))
{
  st_func_node_t *func_node = &func_list, *new_node;

  while (func_node->next != NULL)
    func_node = func_node->next;

  if ((new_node = (st_func_node_t *) malloc (sizeof (st_func_node_t))) == NULL)
    return -1;

  new_node->func = func;
  new_node->next = NULL;
  func_node->next = new_node;
  return 0;
}


int
unregister_func (void (*func) (void))
{
  st_func_node_t *func_node = &func_list, *prev_node = &func_list;

  while (func_node->next != NULL && func_node->func != func)
    {
      prev_node = func_node;
      func_node = func_node->next;
    }
  if (func_node->func != func)
    return -1;

  if (!func_list_locked)
    {
      prev_node->next = func_node->next;
      free (func_node);
      return 0;
    }
  else
    return -1;
}


void
handle_registered_funcs (void)
{
  st_func_node_t *func_node = &func_list;

  func_list_locked = 1;
  while (func_node->next != NULL)
    {
      func_node = func_node->next;              // first node contains no valid address
      if (func_node->func != NULL)
        func_node->func ();
    }
  func_list_locked = 0;
}


#ifdef  _WIN32
int
truncate (const char *path, off_t size)
{
  int retval;
  HANDLE file = CreateFile (path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (file == INVALID_HANDLE_VALUE)
    return -1;

  SetFilePointer (file, size, 0, FILE_BEGIN);
  retval = SetEndOfFile (file);                 // returns nonzero on success
  CloseHandle (file);

  return retval ? 0 : -1;                       // truncate() returns zero on success
}


char *
mkdtemp (char *template)
{
  char *p = NULL;

  p = _mktemp (template);

  while (_mkdir (p) != 0)
    p = _mktemp (template);

  return template;
}


int
mkstemp (char *template)
{
  char *p = NULL;

  p = _mktemp (template);

  while (access (p, X_OK) != 0)
    p = _mktemp (template);

  fopen (template, "wb");

  return 0;
}


#if     defined __MINGW32__ && defined DLL
// Ugly hack in order to fix something in zlib
FILE *
fdopen (int fd, const char *mode)
{
  return _fdopen (fd, mode);
}
#endif


#elif   defined AMIGA                           // _WIN32
int
truncate (const char *path, off_t size)
{
  BPTR fh;
  ULONG newsize;

  if (!(fh = Open (path, MODE_OLDFILE)))
    return -1;

  newsize = SetFileSize (fh, size, OFFSET_BEGINNING);
  Close (fh);

  return newsize == (ULONG) size ? 0 : -1;      // truncate() returns zero on success
}


int
chmod (const char *path, mode_t mode)
{
  if (!SetProtection ((STRPTR) path,
                      ((mode & S_IRUSR ? 0 : FIBF_READ) |
                       (mode & S_IWUSR ? 0 : FIBF_WRITE | FIBF_DELETE) |
                       (mode & S_IXUSR ? 0 : FIBF_EXECUTE) |
                       (mode & S_IRGRP ? FIBF_GRP_READ : 0) |
                       (mode & S_IWGRP ? FIBF_GRP_WRITE | FIBF_GRP_DELETE : 0) |
                       (mode & S_IXGRP ? FIBF_GRP_EXECUTE : 0) |
                       (mode & S_IROTH ? FIBF_OTR_READ : 0) |
                       (mode & S_IWOTH ? FIBF_OTR_WRITE | FIBF_OTR_DELETE : 0) |
                       (mode & S_IXOTH ? FIBF_OTR_EXECUTE : 0))))
    return -1;
  else
    return 0;
}


int
readlink (const char *path, char *buf, int bufsize)
{
  (void) path;                                  // warning remover
  (void) buf;                                   // idem
  (void) bufsize;                               // idem
  // always return -1 as if anything passed to it isn't a soft link
  return -1;
}


// custom _popen() and _pclose(), because the standard ones (named popen() and
//  pclose()) are buggy
FILE *
_popen (const char *path, const char *mode)
{
  int fd;
  BPTR fh;
  long fhflags;
  char *apipe = malloc (strlen (path) + 7);
  if (!apipe)
    return NULL;

  sprintf (apipe, "PIPE:%08lx.%08lx", (ULONG) FindTask (NULL), (ULONG) time (0));

  if (*mode == 'w')
    fhflags = MODE_NEWFILE;
  else
    fhflags = MODE_OLDFILE;

  if (fh = Open (apipe, fhflags))
    {
      switch (SystemTags(path, SYS_Input, Input(), SYS_Output, fh, SYS_Asynch,
                TRUE, SYS_UserShell, TRUE, NP_CloseInput, FALSE, TAG_END))
        {
        case 0:
          return fopen (apipe, mode);
          break;
        case -1:
          Close (fh);
          return 0;
          break;
        default:
          return 0;
          break;
        }
    }
  return 0;
}


int
_pclose (FILE *stream)
{
  if (stream)
    return fclose (stream);
  else
    return -1;
}
#endif                                          // AMIGA
