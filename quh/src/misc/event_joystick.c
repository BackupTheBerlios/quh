/*
event_joystick.c - joystick event support

Copyright (c) 2006 Dirk

          
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifdef  HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef __linux__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>
#include "event_joystick.h"
#include "defines.h"


#define JOYSTICK_DEVICE_S "/dev/js%d"
static int joystick[EVENT_MAX_DEVICE] = {0};


static unsigned long
time_ms (unsigned long *ms)
// returns milliseconds since midnight
{
  unsigned long t = 0;
  struct timeval tv;

  if (!gettimeofday (&tv, NULL))
    {
      t = (unsigned long) (tv.tv_usec / 1000);
      t += (unsigned long) ((tv.tv_sec % 86400) * 1000);
    }

  return ms ? *ms = t : t;
}


static int
event_joystick_open2 (st_event_t *e)
{
#if     (defined USE_SDL || defined __linux__)
  int x = 0;
#endif
    
  for (x = 0; x < EVENT_MAX_DEVICE; x++)
    {
      char device[1024];
      int version = 0x000800, value = 0;
      char value_s[EVENT_DEVICE_NAME_MAX];

      sprintf (device, JOYSTICK_DEVICE_S, x);
      if ((joystick[x] = open (device, O_RDONLY)) < 0)
        break;

      ioctl (joystick[x], JSIOCGVERSION, &version);

      e->d[e->devices].id = EVENT_JOYSTICK;
      ioctl (joystick[x], JSIOCGNAME (EVENT_DEVICE_NAME_MAX), value_s);
      strncpy (e->d[e->devices].id_s, value_s, EVENT_DEVICE_NAME_MAX)[EVENT_DEVICE_NAME_MAX - 1] = 0;
      ioctl (joystick[x], JSIOCGAXES, &value);
      e->d[e->devices].axes = value;
      ioctl (joystick[x], JSIOCGBUTTONS, &value);
      e->d[e->devices].buttons = value;
      e->devices++;
    }

  return 0;
}


st_event_t *
event_joystick_open (st_event_t *e)
{
  int x = 0;

  event_joystick_open2 (e);

  if (!e->devices)
    return NULL; // zero input devices

  for (x = 0; x < e->devices; x++)
    fprintf (stderr, "%s has %d axes and %d buttons/keys\n",
      e->d[x].id_s,
      e->d[x].axes,
      e->d[x].buttons);
  fflush (stderr);

  return e;
}


static int
event_joystick_read2 (st_event_t *e)
{
  int dev = 0;

  for (; dev < e->devices; dev++)
    if (e->d[dev].id == EVENT_JOYSTICK)
      break;

    {
      int x = 0;

      for (; x < EVENT_MAX_DEVICE; x++)
        if (joystick[x])
          {
            struct js_event js;

            if (read (joystick[x], &js, sizeof (struct js_event)) != sizeof (struct js_event))
              continue;

            switch (js.type & ~JS_EVENT_INIT)
              {
                case JS_EVENT_AXIS:
                  e->dev = dev + x;
                  e->e = EVENT_X0 + js.number;
                  e->val = js.value;
                  return 1;

                case JS_EVENT_BUTTON:
                  e->dev = dev + x;
                  e->e = EVENT_BUTTON + js.number;
                  e->val = js.value;
                  return 1;
              }
          }
    }

  return 0;
}


int
event_joystick_read (st_event_t *e)
{
  int result = 0;

  while (e->last_ms > time_ms (0) - e->delay_ms)
    {
      return 0;
    }
  e->last_ms = time_ms (0);

  if (!result)
    result = event_joystick_read2 (e); 
                                  
  return result;
}


int
event_joystick_close (st_event_t *e)
{
      int x = 0;

      for (; x < EVENT_MAX_DEVICE; x++)
        if (joystick[x])
          close (joystick[x]);

  return 0;
}


int
event_joystick_flush (st_event_t *e)
{
  return 0;
}


int
event_joystick_pause (st_event_t *e)
{
  return 0;
}

#endif  // #ifdef  __linux__
