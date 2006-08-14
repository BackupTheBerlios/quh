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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#ifdef __linux__
#include <linux/joystick.h>
#endif
#include "event_joystick.h"


#define MIN(a,b) ((a)<(b)?(a):(b))


#ifdef  __linux__
#define JOYSTICK_DEVICE_S "/dev/js%d"
static int joystick[EVENT_MAX_DEVICE] = {0};
#endif  // __linux__

static st_event_t event;
  

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
event_joystick_open2 (void)
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

      event.d[event.devices].id = EVENT_JOYSTICK;
      ioctl (joystick[x], JSIOCGNAME (EVENT_DEVICE_NAME_MAX), value_s);
      strncpy (event.d[event.devices].id_s, value_s, EVENT_DEVICE_NAME_MAX)[EVENT_DEVICE_NAME_MAX - 1] = 0;
      ioctl (joystick[x], JSIOCGAXES, &value);
      event.d[event.devices].axes = value;
      ioctl (joystick[x], JSIOCGBUTTONS, &value);
      event.d[event.devices].buttons = value;
      event.devices++;
    }

  return 0;
}


st_event_t *
event_joystick_open (int flags, int delay_ms)
{
  int x = 0;

  memset (&event, 0, sizeof (st_event_t));

  event.flags = flags;
  event.delay_ms = delay_ms;

  event_joystick_open2 ();

  if (!event.devices)
    return NULL; // zero input devices

  for (x = 0; x < event.devices; x++)
    fprintf (stderr, "%s has %d axes and %d buttons/keys\n",
      event.d[x].id_s,
      event.d[x].axes,
      event.d[x].buttons);
  fflush (stderr);

  return &event;
}


static int
event_joystick_read2 (st_event_t *event)
{
  int dev = 0;

  for (; dev < event->devices; dev++)
    if (event->d[dev].id == EVENT_JOYSTICK)
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
                  event->dev = dev + x;
                  event->e = EVENT_X0 + js.number;
                  event->val = js.value;
                  return 1;

                case JS_EVENT_BUTTON:
                  event->dev = dev + x;
                  event->e = EVENT_BUTTON + js.number;
                  event->val = js.value;
                  return 1;
              }
          }
    }

  return 0;
}


int
event_joystick_read (st_event_t *event)
{
  int result = 0;

  while (event->last_ms > time_ms (0) - event->delay_ms)
    {
      return 0;
    }
  event->last_ms = time_ms (0);

  if (!result)
    result = event_joystick_read2 (event); 
                                  
  return result;
}


int
event_joystick_close (void)
{
      int x = 0;

      for (; x < EVENT_MAX_DEVICE; x++)
        if (joystick[x])
          close (joystick[x]);

  return 0;
}


int
event_joystick_flush (void)
{
  return 0;
}


int
event_joystick_pause (void)
{
  return 0;
}


#ifdef  TEST
//#if 0
int
main (int argc, char ** argv)
{
  st_event_t * e = NULL;

  e = event_open (EVENT_JOYSTICK);
  if (e)
    while (1)
      {
        int result = event_read (e);

        if (!result) // no event
          continue;

        if (result == -1) // error
          break;

        // event handling
        if (e->e == EVENT_KEY)
          if (e->val == 'q')
            {
              event_close ();
              break;
            }
event_st_event_t_sanity_check (e);

      }

  return 0;
}
#endif  // TEST
