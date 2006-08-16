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
#ifdef USE_SDL
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <SDL.h>                // SDL_Joystick
#include "event_sdl.h"


#define MIN(a,b) ((a)<(b)?(a):(b))


static SDL_Joystick *sdl_joystick[EVENT_MAX_DEVICE];
static int  use_sdl = 0;
static SDL_Event sdl_event;

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
event_sdl_open_keyboard (void)
{
  if (use_sdl)
    {
      SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
      SDL_EventState (SDL_KEYDOWN, SDL_ENABLE);

      return 0;
    }

  return 0;
}


static int
event_sdl_open_mouse (void)
{
  if (use_sdl)
    {
      if (SDL_ShowCursor (SDL_QUERY) == SDL_DISABLE)
        SDL_ShowCursor (SDL_ENABLE);
      SDL_EventState (SDL_MOUSEMOTION, SDL_ENABLE);
      SDL_EventState (SDL_MOUSEBUTTONDOWN, SDL_ENABLE);

      return 0;
    }


  return -1;
}


static int
event_sdl_open_joystick (void)
{
  int x = 0;
    
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

  return 0;
}


st_event_t *
event_sdl_open (int flags, int delay_ms)
{
  int x = 0;

  memset (&event, 0, sizeof (st_event_t));

  event.flags = flags;
  event.delay_ms = delay_ms;

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

  if (flags & EVENT_KEYBOARD)
    if (!event_sdl_open_keyboard ())
      {
        if (use_sdl)
          strcpy (event.d[event.devices].id_s, "Keyboard (SDL)");
        else
          strcpy (event.d[event.devices].id_s, "Keyboard");

        event.d[event.devices].id = EVENT_KEYBOARD;
        event.d[event.devices].buttons = 256;
        event.devices++;
      }

  if (flags & EVENT_MOUSE)
    if (!event_sdl_open_mouse ())
      {
        event.d[event.devices].id = EVENT_MOUSE;
        strcpy (event.d[event.devices].id_s, "Mouse");
        event.d[event.devices].axes = 2;
        event.d[event.devices].buttons = 3;
        event.devices++;
      }

  if (flags & EVENT_JOYSTICK)
    if (!event_sdl_open_joystick ())
      {
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
event_sdl_read_keyboard (st_event_t *event)
{
  int dev = 0;

  for (; dev < event->devices; dev++)
    if (event->d[dev].id == EVENT_KEYBOARD)
      break;

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

  return 0;
}


static int
event_sdl_read_mouse (st_event_t *event)
{
  int dev = 0;

  for (; dev < event->devices; dev++)
    if (event->d[dev].id == EVENT_MOUSE)
      break;

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

  return 0;
}


static int
event_sdl_read_joystick (st_event_t *event)
{
  int dev = 0;

  for (; dev < event->devices; dev++)
    if (event->d[dev].id == EVENT_JOYSTICK)
      break;

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

  return 0;
}



int
event_sdl_read (st_event_t *event)
{
  int result = 0;

  while (event->last_ms > time_ms (0) - event->delay_ms)
    {
      SDL_PollEvent (&sdl_event);
      return 0;
    }
  event->last_ms = time_ms (0);

  if (use_sdl)
    if (!SDL_PollEvent (&sdl_event))
      return 0;

  if (event->flags & EVENT_KEYBOARD)
    result = event_sdl_read_keyboard (event);

  if (!result && event->flags & EVENT_MOUSE)
    result = event_sdl_read_mouse (event); 

  if (!result && event->flags & EVENT_JOYSTICK)
    result = event_sdl_read_joystick (event); 

  return result;
}


static int
event_sdl_close_keyboard (void)
{
  if (use_sdl)
    return 0;

  return 0;
}


static int
event_sdl_close_mouse (void)
{
  if (use_sdl)
    return 0;

  return 0;
}


static int
event_sdl_close_joystick (void)
{
  if (use_sdl)
    {
      if (SDL_WasInit (SDL_INIT_JOYSTICK) & SDL_INIT_JOYSTICK)
        SDL_QuitSubSystem (SDL_INIT_JOYSTICK);

      return 0;
    }

  return 0;
}


int
event_sdl_close (void)
{
  if (event.flags & EVENT_KEYBOARD)
    event_sdl_close_keyboard ();

  if (event.flags & EVENT_MOUSE)
    event_sdl_close_mouse ();

  if (event.flags & EVENT_JOYSTICK)
    event_sdl_close_joystick ();

  return 0;
}


int
event_sdl_flush (void)
{
  if (use_sdl)
    {
      SDL_Event e;
      while (SDL_PollEvent (&e));
    }

  return 0;
}


int
event_sdl_pause (void)
{
  if (use_sdl)
    {
      if (SDL_JoystickOpened (0))
        SDL_JoystickClose (0);
      else
        SDL_JoystickOpen (0);
    }

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
  
  e = event_sdl_open (EVENT_KEYBOARD|EVENT_JOYSTICK);
  if (e)
    while (1)
      {
        int result = event_sdl_read (e);

        if (!result) // no event
          continue;

        if (result == -1) // error
          break;

        // event handling
        if (e->e == EVENT_KEY)
          if (e->val == 'q')
            {
              event_sdl_close ();
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
#endif  // USE_SDL
