/*
event_sdl.c - even more simple SDL wrapper for events

Copyright (c) 2004 Dirk

          
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
#ifdef USE_SDL
#include <SDL.h>                // SDL_Joystick
#endif
#ifdef __linux__
#include <linux/joystick.h>
#endif
#ifdef  USE_LIRC
#include <lirc/lirc_client.h>
#endif
#ifdef  USE_LIBGPM
#include <gpm.h>
#endif
#include "term.h"
#include "event_sdl.h"


#define MIN(a,b) ((a)<(b)?(a):(b))


#ifdef  USE_SDL
static SDL_Joystick *sdl_joystick[EVENT_MAX_DEVICE];
static int  use_sdl = 0;
static SDL_Event sdl_event;
#endif
#ifdef  __linux__
#define JOYSTICK_DEVICE_S "/dev/js%d"
static int joystick[EVENT_MAX_DEVICE] = {0};
#endif  // __linux__
#ifdef  USE_LIRC
static struct lirc_config *lirc_config;
#define LIRC_CONFIGFILE "~/.lircrc"
static int lirc_sock = 0;
#endif
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
event_open_keyboard (void)
{
#ifdef  USE_SDL
  if (use_sdl)
    {
      SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
      SDL_EventState (SDL_KEYDOWN, SDL_ENABLE);

      return 0;
    }
#endif

  init_conio ();

  return 0;
}


static int
event_open_mouse (void)
{
#ifdef  USE_SDL
  if (use_sdl)
    {
      if (SDL_ShowCursor (SDL_QUERY) == SDL_DISABLE)
        SDL_ShowCursor (SDL_ENABLE);
      SDL_EventState (SDL_MOUSEMOTION, SDL_ENABLE);
      SDL_EventState (SDL_MOUSEBUTTONDOWN, SDL_ENABLE);

      return 0;
    }
#endif

#ifdef  USE_LIBGPM
    {
      Gpm_Connect conn;

      conn.eventMask = ~0;          // Want to know about all the events
      conn.defaultMask = 0;         // don't handle anything by default
      conn.minMod = 0;
      conn.maxMod = 0;

      if (Gpm_Open (&conn, 0) == -1)
        return -1;

      return 0;
    }
#endif

  return -1;
}


static int
event_open_joystick (void)
{
#if     (defined USE_SDL || defined __linux__)
  int x = 0;
#endif
    
#ifdef  USE_SDL  
  if (use_sdl)
    {
      if (!(SDL_WasInit (SDL_INIT_JOYSTICK) & SDL_INIT_JOYSTICK))
        if (SDL_InitSubSystem (SDL_INIT_JOYSTICK) < 0)
          return -1;

      for (x = 0; x < SDL_NumJoysticks (); x++)
        {
          sdl_joystick[x] = SDL_JoystickOpen (x);
          event.d[event.devices].id = EVENT_JOYSTICK;
          strncpy (event.d[event.devices].id_s, SDL_JoystickName (x), EVENT_DEVICE_NAME_MAX)[EVENT_DEVICE_NAME_MAX - 1] = 0;
          event.d[event.devices].axes = SDL_JoystickNumAxes (sdl_joystick[x]);
          event.d[event.devices].buttons = SDL_JoystickNumButtons (sdl_joystick[x]);
//          SDL_JoystickEventState (SDL_ENABLE);
          event.devices++;
        }

      SDL_EventState (SDL_JOYAXISMOTION, SDL_ENABLE);
      SDL_EventState (SDL_JOYBUTTONDOWN, SDL_ENABLE);

      return 0;
    }
#endif

#ifdef  __linux__
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
#endif

  return 0;
}


static int
event_open_infrared (void)
{
#ifdef  USE_LIRC
  if ((lirc_sock = lirc_init ("event", 1)) == -1)
    return -1;
            
  if (lirc_readconfig (LIRC_CONFIGFILE, &lirc_config, NULL) != 0)
    {
      lirc_deinit ();
      return -1;
    }
#endif
  return 0;
}


st_event_t *
event_open (int flags, int delay_ms)
{
  int x = 0;

  memset (&event, 0, sizeof (st_event_t));

  event.flags = flags;
  event.delay_ms = delay_ms;

#ifdef  USE_SDL
  if (SDL_WasInit (SDL_INIT_VIDEO) & SDL_INIT_VIDEO)
    use_sdl = 1; // SDL keyboard support works only with SDL_INIT_VIDEO?

  if (use_sdl)
    {
//      SDL_EventState (SDL_ACTIVEEVENT, SDL_IGNORE);
      SDL_EventState (SDL_KEYDOWN, SDL_IGNORE);
      SDL_EventState (SDL_KEYUP, SDL_IGNORE);
      SDL_EventState (SDL_MOUSEMOTION, SDL_IGNORE);
      SDL_EventState (SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
      SDL_EventState (SDL_MOUSEBUTTONUP, SDL_IGNORE);
      SDL_EventState (SDL_JOYAXISMOTION, SDL_IGNORE);
      SDL_EventState (SDL_JOYBALLMOTION, SDL_IGNORE);
      SDL_EventState (SDL_JOYHATMOTION, SDL_IGNORE);
      SDL_EventState (SDL_JOYBUTTONDOWN, SDL_IGNORE);
      SDL_EventState (SDL_JOYBUTTONUP, SDL_IGNORE);
//      SDL_EventState (SDL_QUIT, SDL_IGNORE);
//      SDL_EventState (SDL_SYSWMEVENT, SDL_IGNORE);
//      SDL_EventState (SDL_VIDEORESIZE, SDL_IGNORE);
//      SDL_EventState (SDL_USEREVENT, SDL_IGNORE);
    }
#endif  // USE_SDL

  if (flags & EVENT_KEYBOARD)
    if (!event_open_keyboard ())
      {
#ifdef  USE_SDL
        if (use_sdl)
          strcpy (event.d[event.devices].id_s, "Keyboard (SDL)");
        else
#endif
          strcpy (event.d[event.devices].id_s, "Keyboard");

        event.d[event.devices].id = EVENT_KEYBOARD;
        event.d[event.devices].buttons = 256;
        event.devices++;
      }

  if (flags & EVENT_MOUSE)
    if (!event_open_mouse ())
      {
        event.d[event.devices].id = EVENT_MOUSE;
        strcpy (event.d[event.devices].id_s, "Mouse");
        event.d[event.devices].axes = 2;
        event.d[event.devices].buttons = 3;
        event.devices++;
      }

  if (flags & EVENT_JOYSTICK)
    if (!event_open_joystick ())
      {
      }

  if (flags & EVENT_INFRARED)
    if (!event_open_infrared ())
      {
        event.d[event.devices].id = EVENT_INFRARED;
        strcpy (event.d[event.devices].id_s, "Infrared Control");
        event.devices++;
      }

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
event_read_keyboard (st_event_t *event)
{
  int dev = 0;

  for (; dev < event->devices; dev++)
    if (event->d[dev].id == EVENT_KEYBOARD)
      break;

#ifdef  USE_SDL
  if (use_sdl)
    {
      if (sdl_event.type == SDL_KEYDOWN)
        {
          event->dev = dev;
          event->e = EVENT_KEY;
          event->val = sdl_event.key.keysym.sym;
          return 1;
        }

      return 0;
    }
#endif

#ifdef  __linux__
  if (kbhit ())
    {
      event->dev = dev;
      event->e = EVENT_KEY;
      event->val = getch ();
      return 1;
    }
#endif

  return 0;
}


static int
event_read_mouse (st_event_t *event)
{
  int dev = 0;

  for (; dev < event->devices; dev++)
    if (event->d[dev].id == EVENT_MOUSE)
      break;

#ifdef  USE_SDL
  if (use_sdl)
    {
      // re-enable cursor (if necessary)
      if (SDL_ShowCursor (SDL_QUERY) == SDL_DISABLE)
        SDL_ShowCursor (SDL_ENABLE);
      
      switch (sdl_event.type)
        {
          case SDL_MOUSEMOTION:
            event->dev = dev;
            if (sdl_event.motion.xrel)
              {
                event->e = EVENT_X0;
                event->val = sdl_event.motion.x;
                return 1;
              }
            else if (sdl_event.motion.yrel)
              {
                event->e = EVENT_Y0;
                event->val = sdl_event.motion.y;
                return 1;
              }
            return 0;

          case SDL_MOUSEBUTTONDOWN:
            event->dev = dev;
            event->e = EVENT_BUTTON;
            return 1;
        }

      return 0;
    }
#endif

#ifdef  USE_LIBGPM
    {
      Gpm_Event gpm_event;

      Gpm_GetEvent (&gpm_event);

      if (gpm_event.type & GPM_MOVE)
        {
          event->dev = dev;
          event->e = EVENT_X0;
          event->val = gpm_event.x;
          return 1;
#if 0
          event->dev = dev;
          event->e = EVENT_Y0;
          event->val = gpm_event.y;
          return 1;
#endif
        }

      if (gpm_event.type & GPM_DOWN)
        {
          event->dev = dev;
          event->e = EVENT_BUTTON;
          if (gpm_event.buttons & GPM_B_LEFT)
            event->val = 1;
          else if (gpm_event.buttons & GPM_B_MIDDLE)
            event->val = 2;
          else if (gpm_event.buttons & GPM_B_RIGHT)
            event->val = 3;
          return 1;
        }
    }
#endif

  return 0;
}


static int
event_read_joystick (st_event_t *event)
{
  int dev = 0;

  for (; dev < event->devices; dev++)
    if (event->d[dev].id == EVENT_JOYSTICK)
      break;

#ifdef  USE_SDL
  if (use_sdl)
    switch (sdl_event.type)
      {
        case SDL_JOYAXISMOTION:
          event->dev = dev + sdl_event.jaxis.which;
          event->e = EVENT_X0 + sdl_event.jaxis.axis;
          event->val = sdl_event.jaxis.value;
          return 1;

        case SDL_JOYBUTTONDOWN:
          event->dev = dev + sdl_event.jbutton.which;
          event->e = EVENT_BUTTON;
          event->val = sdl_event.jbutton.button;
          return 1;
      }
#endif

#ifdef  __linux__
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
#endif

  return 0;
}


static int
event_read_infrared (st_event_t *event)
{
  int dev = 0;
  
  for (; dev < event->devices; dev++)
    if (event->d[dev].id == EVENT_INFRARED)
      break;
              
#ifdef  USE_LIRC
  if (lirc_nextcode (&code) != 0)
    {
      return -1;
    }

  if (!code)
    return 0;

  // we put all cmds in a single buffer separated by \n
  while ((r = lirc_code2char (lirc_config, code, &c)) == 0 && c != NULL)
    if (strlen (c) <= 0)
      continue;
#endif

  return 0;
}


int
event_read (st_event_t *event)
{
  int result = 0;

  while (event->last_ms > time_ms (0) - event->delay_ms)
    {
#ifdef  USE_SDL
      SDL_PollEvent (&sdl_event);
#endif
      return 0;
    }
  event->last_ms = time_ms (0);

#ifdef  USE_SDL
  if (use_sdl)
    if (!SDL_PollEvent (&sdl_event))
      return 0;
#endif

  if (event->flags & EVENT_KEYBOARD)
    result = event_read_keyboard (event);

  if (!result && event->flags & EVENT_MOUSE)
    result = event_read_mouse (event); 

  if (!result && event->flags & EVENT_JOYSTICK)
    result = event_read_joystick (event); 

  if (!result && event->flags & EVENT_INFRARED)
    result = event_read_infrared (event); 
                                  
  return result;
}


static int
event_close_keyboard (void)
{
#ifdef  USE_SDL
  if (use_sdl)
    return 0;
#endif

  deinit_conio (); 

  return 0;
}


static int
event_close_mouse (void)
{
#ifdef  USE_SDL
  if (use_sdl)
    return 0;
#endif

#ifdef  USE_LIBGPM
  Gpm_Close ();
#endif  // USE_LIBGPM
  return 0;
}


static int
event_close_joystick (void)
{
#ifdef  USE_SDL
  if (use_sdl)
    {
      if (SDL_WasInit (SDL_INIT_JOYSTICK) & SDL_INIT_JOYSTICK)
        SDL_QuitSubSystem (SDL_INIT_JOYSTICK);

      return 0;
    }
#endif

#ifdef  __linux__
    {
      int x = 0;

      for (; x < EVENT_MAX_DEVICE; x++)
        if (joystick[x])
          close (joystick[x]);
    }
#endif

  return 0;
}


static int
event_close_infrared (void)
{
#ifdef  USE_SDL
  if (use_sdl)
    return 0;
#endif

#ifdef  USE_LIRC
  lirc_freeconfig (lirc_config);
  lirc_deinit ();
#endif

  return 0;
}


int
event_close (void)
{
  if (event.flags & EVENT_KEYBOARD)
    event_close_keyboard ();

  if (event.flags & EVENT_MOUSE)
    event_close_mouse ();

  if (event.flags & EVENT_JOYSTICK)
    event_close_joystick ();

  if (event.flags & EVENT_INFRARED)
    event_close_infrared ();

  return 0;
}


int
event_flush (void)
{
#ifdef  USE_SDL
  if (use_sdl)
    {
      SDL_Event e;
      while (SDL_PollEvent (&e));
    }
#endif

  return 0;
}


int
event_pause (void)
{
#ifdef  USE_SDL
  if (use_sdl)
    {
      if (SDL_JoystickOpened (0))
        SDL_JoystickClose (0);
      else
        SDL_JoystickOpen (0);
    }
#endif

  return 0;
}


#ifdef  TEST
//#if 0
int
main (int argc, char ** argv)
{
  st_event_t * e = NULL;

#if 0
  SDL_Init (SDL_INIT_VIDEO);
  SDL_SetVideoMode (1024, 768, 8, 0);
#endif
  
  e = event_open (EVENT_KEYBOARD|EVENT_JOYSTICK);
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

#if 0
  SDL_Quit ();
#endif

  return 0;
}
#endif  // TEST
