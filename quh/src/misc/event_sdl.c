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
#include "defines.h"
#include "event_sdl.h"


static SDL_Joystick *sdl_joystick[EVENT_MAX_DEVICE];
static SDL_Event sdl_event;


//#ifdef  DEBUG
void
event_st_event_t_sanity_check (st_event_t *e)
{
//  printf ("flags: %d\n", e->flags);
//  printf ("devices: %d\n", e->devices);

#if 0
  {
    int x = 0;
    
    for (; x < e->devices; x++)
      {
        printf ("  id: %d\n", e->d[x].id);
        printf ("  id_s: %s\n", e->d[x].id_s);
        printf ("  buttons: %d\n", e->d[x].buttons);
        printf ("  axes: %d\n", e->d[x].axes);
      }
  }
#else
  printf ("id_s: %s ", e->d[e->dev].id_s);
#endif

  printf ("dev: %d ", e->dev);
  printf ("e: %d ", e->e);
  printf ("val: %d ", e->val);
  printf ("\n");

  fflush (stdout);
}


#if 0
static void
event_sanity_check (int buttons, int axes, int *button, int *axis)
{
  int i = 0;
  printf ("\r");

  if (axes)
    {
      printf ("Axes: ");
      for (i = 0; i < axes; i++)
        {
          if (1)
            printf ("%2d:%6d ", i, axis[i]);
          else
            {
              if (!axis[i])
                printf ("   ");
              else
                printf ("%2d ", i);
            }
        }
    }

  if (buttons)
    {
      printf ("Buttons: ");
      for (i = 0; i < buttons; i++)

      if (button[i])
        printf ("%2d ", i);
      else
        printf ("   ");
    }

  fflush (stdout);
}
#endif
//#endif



#define TRANS(p) {SDLK_##p, EVENTK_##p}


static unsigned long trans[][2] =
{
//  TRANS(UNKNOWN),
  TRANS(BACKSPACE),
  TRANS(TAB),
  TRANS(CLEAR),
  TRANS(RETURN),
  TRANS(PAUSE),
  TRANS(ESCAPE),
  TRANS(SPACE),
  TRANS(EXCLAIM),
  TRANS(QUOTEDBL),
  TRANS(HASH),
  TRANS(DOLLAR),
  TRANS(AMPERSAND),
  TRANS(QUOTE),
  TRANS(LEFTPAREN),
  TRANS(RIGHTPAREN),
  TRANS(ASTERISK),
  TRANS(PLUS),
  TRANS(COMMA),
  TRANS(MINUS),
  TRANS(PERIOD),
  TRANS(SLASH),
  TRANS(0),
  TRANS(1),
  TRANS(2),
  TRANS(3),
  TRANS(4),
  TRANS(5),
  TRANS(6),
  TRANS(7),
  TRANS(8),
  TRANS(9),
  TRANS(COLON),
  TRANS(SEMICOLON),
  TRANS(LESS),
  TRANS(EQUALS),
  TRANS(GREATER),
  TRANS(QUESTION),
  TRANS(AT),
  TRANS(LEFTBRACKET),
  TRANS(BACKSLASH),
  TRANS(RIGHTBRACKET),
  TRANS(CARET),
  TRANS(UNDERSCORE),
  TRANS(BACKQUOTE),
  TRANS(a),
  TRANS(b),
  TRANS(c),
  TRANS(d),
  TRANS(e),
  TRANS(f),
  TRANS(g),
  TRANS(h),
  TRANS(i),
  TRANS(j),
  TRANS(k),
  TRANS(l),
  TRANS(m),
  TRANS(n),
  TRANS(o),
  TRANS(p),
  TRANS(q),
  TRANS(r),
  TRANS(s),
  TRANS(t),
  TRANS(u),
  TRANS(v),
  TRANS(w),
  TRANS(x),
  TRANS(y),
  TRANS(z),
  TRANS(DELETE),
// Numeric keypad
  TRANS(KP0),
  TRANS(KP1),
  TRANS(KP2),
  TRANS(KP3),
  TRANS(KP4),
  TRANS(KP5),
  TRANS(KP6),
  TRANS(KP7),
  TRANS(KP8),
  TRANS(KP9),
  TRANS(KP_PERIOD),
  TRANS(KP_DIVIDE),
  TRANS(KP_MULTIPLY),
  TRANS(KP_MINUS),
  TRANS(KP_PLUS),
  TRANS(KP_ENTER),
  TRANS(KP_EQUALS),
// Arrows + Home/End pad
  TRANS(UP),
  TRANS(DOWN),
  TRANS(RIGHT),
  TRANS(LEFT),
  TRANS(INSERT),
  TRANS(HOME),
  TRANS(END),
  TRANS(PAGEUP),
  TRANS(PAGEDOWN),
// Function keys
  TRANS(F1),
  TRANS(F2),
  TRANS(F3),
  TRANS(F4),
  TRANS(F5),
  TRANS(F6),
  TRANS(F7),
  TRANS(F8),
  TRANS(F9),
  TRANS(F10),
  TRANS(F11),
  TRANS(F12),
  TRANS(F13),
  TRANS(F14),
  TRANS(F15),
  TRANS(RSHIFT),
  TRANS(LSHIFT),
  TRANS(RCTRL),
  TRANS(LCTRL),
  TRANS(RALT),
  TRANS(LALT),
  {0, 0}
};


// translate SDL events into wrapper events
static unsigned long
event_translate (unsigned long p)
{
  int i = 0;
  for (; trans[i][0]; i++)
    if (trans[i][0] == p)
      return trans[i][1];
  return 0;
}


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
event_open_keyboard (st_event_t * e)
{
  SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
  SDL_EventState (SDL_KEYDOWN, SDL_ENABLE);

  return 0;
}


static int
event_open_mouse (st_event_t * e)
{
      if (SDL_ShowCursor (SDL_QUERY) == SDL_DISABLE)
        SDL_ShowCursor (SDL_ENABLE);
      SDL_EventState (SDL_MOUSEMOTION, SDL_ENABLE);
      SDL_EventState (SDL_MOUSEBUTTONDOWN, SDL_ENABLE);

      return 0;
}


static int
event_open_joystick (st_event_t * e)
{
  int x = 0;
    
      if (!(SDL_WasInit (SDL_INIT_JOYSTICK) & SDL_INIT_JOYSTICK))
        if (SDL_InitSubSystem (SDL_INIT_JOYSTICK) < 0)
          return -1;

      for (x = 0; x < SDL_NumJoysticks (); x++)
        {
          sdl_joystick[x] = SDL_JoystickOpen (x);
          e->d[e->devices].id = EVENT_PAD1;
          strncpy (e->d[e->devices].id_s, SDL_JoystickName (x), EVENT_DEVICE_NAME_MAX)[EVENT_DEVICE_NAME_MAX - 1] = 0;
          e->d[e->devices].axes = SDL_JoystickNumAxes (sdl_joystick[x]);
          e->d[e->devices].buttons = SDL_JoystickNumButtons (sdl_joystick[x]);
//          SDL_JoystickEventState (SDL_ENABLE);
          e->devices++;
        }

      SDL_EventState (SDL_JOYAXISMOTION, SDL_ENABLE);
      SDL_EventState (SDL_JOYBUTTONDOWN, SDL_ENABLE);

      return 0;
}


st_event_t *
event_open (int delay_ms, int flags)
{
  int x = 0;
  static st_event_t e;

  if (!(flags & EVENT_KEYBOARD) &&
      !(flags & EVENT_MOUSE) &&   
      !(flags & EVENT_PAD1) &&
      !(flags & EVENT_INFRARED))
    return NULL;

  memset (&e, 0, sizeof (st_event_t));

  e.flags = flags;
  e.delay_ms = delay_ms;

  if (!(SDL_WasInit (SDL_INIT_VIDEO) & SDL_INIT_VIDEO))
    return NULL;

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

  if (e.flags & EVENT_KEYBOARD)
    if (!event_open_keyboard (&e))
      {
        strcpy (e.d[e.devices].id_s, "Keyboard (SDL)");

        e.d[e.devices].id = EVENT_KEYBOARD;
        e.d[e.devices].buttons = 256;
        e.devices++;
      }

  if (e.flags & EVENT_MOUSE)
    if (!event_open_mouse (&e))
      {
        e.d[e.devices].id = EVENT_MOUSE;
        strcpy (e.d[e.devices].id_s, "Mouse");
        e.d[e.devices].axes = 2;
        e.d[e.devices].buttons = 3;
        e.devices++;
      }

  if (e.flags & EVENT_PAD1)
    if (!event_open_joystick (&e))
      {
      }

  if (!e.devices)
    return NULL; // zero input devices

  for (x = 0; x < e.devices; x++)
    fprintf (stderr, "%s has %d axes and %d buttons/keys\n",
      e.d[x].id_s,
      e.d[x].axes,
      e.d[x].buttons);
  fflush (stderr);

  return &e;
}


#if 0
static int
event_read_keyboard (st_event_t *event)
{
  int dev = 0;

  for (; dev < event->devices; dev++)
    if (event->d[dev].id == EVENT_KEYBOARD)
      break;

      if (sdl_event.type == SDL_KEYDOWN)
        {
          event->dev = dev;
          event->e = EVENT_KEY;
          event->val = sdl_event.key.keysym.sym;
          return 1;
        }

  return 0;
}


static int
event_read_mouse (st_event_t *event)
{
  int dev = 0;

  for (; dev < event->devices; dev++)
    if (event->d[dev].id == EVENT_MOUSE)
      break;

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


static int
event_read_joystick (st_event_t *event)
{
  int dev = 0;

  for (; dev < event->devices; dev++)
    if (event->d[dev].id == EVENT_PAD1)
      break;

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
event_read (st_event_t *event)
{
  int result = 0;

  while (event->last_ms > time_ms (0) - event->delay_ms)
    {
      SDL_PollEvent (&sdl_event);
      return 0;
    }
  event->last_ms = time_ms (0);

    if (!SDL_PollEvent (&sdl_event))
      return 0;

  if (event->flags & EVENT_KEYBOARD)
    result = event_read_keyboard (event);

  if (!result && event->flags & EVENT_MOUSE)
    result = event_read_mouse (event); 

  if (!result && event->flags & EVENT_PAD1)
    result = event_read_joystick (event); 

  return result;
}
#endif


int
event_loop (int (*event_func) (st_event_t *e))
{
  static st_event_t e;
  int result = 0;

  while (!result)
    {
      SDL_Event event;

      result = -1;

      while (SDL_PollEvent (&event))
        {
          switch (event.type)
            {
              case SDL_ACTIVEEVENT:
/*
                if (!event.active.gain)
                  game.focus = 0;
                else
                  game.focus = 1;
*/
                break;
    
              case SDL_VIDEORESIZE:
//                vo_resize (event.resize.w, event.resize.h);
                break;
    
              case SDL_KEYDOWN:
                e.e = EVENT_PUSH;
                if (event.key.keysym.mod & KMOD_CTRL)
                  e.e |= EVENTK_CTRL;
                e.e |= event_translate (event.key.keysym.sym);
                break;
      
              case SDL_USEREVENT:
#if 0
                if (e.user.code == SHOW_IMAGE)
                  show_image ();
#endif
                break;
    
//#ifdef  USE_MOUSE
              case SDL_MOUSEMOTION:
                break;

              case SDL_MOUSEBUTTONDOWN:
                e.e = EVENT_PUSH | EVENTM_B1;
                break;
  
              case SDL_MOUSEBUTTONUP:  
                e.e = EVENT_REL | EVENTM_B1;
                break;
//#endif

#if 0
//#ifdef  USE_JOYSTICK
              case SDL_JOYAXISMOTION:
                if (e.jaxis.which == 0)       // we support only the first joystick
                  {
                    if (!(e.jaxis.axis & 1))  // x axis
                      {
                        if (e.jaxis.value > 0)
                          mouse_xpos++;
                        if (e.jaxis.value < 0)
                          mouse_xpos--;
      
                        if (mouse_xpos < 0)
                          mouse_xpos = xsize;
                        if (mouse_xpos > xsize)
                          mouse_xpos = 0;
                      }
                    if (e.jaxis.axis & 1)     // y axis
                      {
                        if (e.jaxis.value > 0)
                          mouse_ypos++;
                        if (e.jaxis.value < 0)
                          mouse_ypos--;
      
                        if (mouse_ypos < 0)
                          mouse_ypos = ysize;
                        if (mouse_ypos > ysize)
                          mouse_ypos = 0;
                      }
                    SDL_WarpMouse (mouse_xpos, mouse_ypos);
                  }
                break;

              case SDL_JOYBUTTONDOWN:
              case SDL_JOYBUTTONUP:
                break;
#endif
            }

          result = event_func (&e);
        }

//if (game->focus) // if there is no focus == no redraw?
      if (result == -1) // no event has occured so just draw
        result = event_func (NULL);
    }

  return 0;
}




static int
event_close_keyboard (st_event_t * e)
{
  return 0;
}


static int
event_close_mouse (st_event_t * e)
{
  return 0;
}


static int
event_close_joystick (st_event_t * e)
{
      if (SDL_WasInit (SDL_INIT_JOYSTICK) & SDL_INIT_JOYSTICK)
        SDL_QuitSubSystem (SDL_INIT_JOYSTICK);

  return 0;
}


int
event_close (st_event_t * e)
{
  if (e->flags & EVENT_KEYBOARD)
    event_close_keyboard (e);

  if (e->flags & EVENT_MOUSE)
    event_close_mouse (e);

  if (e->flags & EVENT_PAD1)
    event_close_joystick (e);

  return 0;
}


int
event_flush (st_event_t * e)
{
//      SDL_Event e;
//      while (SDL_PollEvent (&e));
}


int
event_pause (st_event_t * e)
{
      if (SDL_JoystickOpened (0))
        SDL_JoystickClose (0);
      else
        SDL_JoystickOpen (0);

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
  
  e = event_open (EVENT_KEYBOARD|EVENT_PAD1, 100);
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
//event_st_event_t_sanity_check (e);

      }

#if 0
  SDL_Quit ();
#endif

  return 0;
}
#endif  // TEST
#ifdef  TEST
//#if 0
int
main (int argc, char ** argv)
{
  st_event_t * e = NULL;

  e = event_open (100, EVENT_JOYSTICK);
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
              event_close (e);
              break;
            }
event_st_event_t_sanity_check (e);

      }

  return 0;
}
#endif  // TEST



#endif  // USE_SDL
