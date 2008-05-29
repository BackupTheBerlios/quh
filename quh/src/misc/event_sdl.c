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
#include <SDL.h>                // SDL_Joystick
#endif
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
//#include <sys/wait.h>
#include "defines.h"
#include "event_sdl.h"


#ifdef  USE_SDL
static SDL_Joystick *sdl_joystick[EVENT_MAX_DEVICE];
static SDL_Event sdl_event;
#endif
#define LIRCD "/dev/lircd"
static int lirc_lircd = 0;
static char *lirc_buffer = NULL;


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
//  printf ("id_s: %s ", e->d[e->dev].id_s);
#endif

//  printf ("dev: %d ", e->dev);
  printf ("e: %ld ", e->e);
//  printf ("val: %d ", e->val);
  printf ("\n");

  fflush (stdout);
}
//#endif  // DEBUG


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


#ifdef  USE_SDL 
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
#endif  // USE_SDL


static int
event_open_keyboard (st_event_t *e)
{
#ifdef  USE_SDL
  SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

  e->d[e->devices].id = EVENT_KEYBOARD;
  strcpy (e->d[e->devices].id_s, "Keyboard (SDL)");
  e->d[e->devices].buttons = 256;
  e->devices++;

  SDL_EventState (SDL_KEYUP, SDL_ENABLE);
  SDL_EventState (SDL_KEYDOWN, SDL_ENABLE);
#endif
  return 0;
}


static int
event_open_mouse (st_event_t *e)
{
#ifdef  USE_SDL
//  SDL_WarpMouse (mouse_xpos, mouse_ypos);
#if 0
  if (SDL_ShowCursor (SDL_QUERY) == SDL_DISABLE)
    SDL_ShowCursor (SDL_ENABLE);
#else
  if (SDL_ShowCursor (SDL_QUERY) == SDL_ENABLE)
    SDL_ShowCursor (SDL_DISABLE);
  SDL_WM_GrabInput (SDL_GRAB_ON);
#endif

  e->d[e->devices].id = EVENT_MOUSE;
  strcpy (e->d[e->devices].id_s, "Mouse");
  e->d[e->devices].axes = 2;
  e->d[e->devices].buttons = 3;
  e->devices++;

  SDL_EventState (SDL_MOUSEMOTION, SDL_ENABLE);
  SDL_EventState (SDL_MOUSEBUTTONUP, SDL_ENABLE);
  SDL_EventState (SDL_MOUSEBUTTONDOWN, SDL_ENABLE);
#endif
  return 0;
}


static int
event_open_pad (st_event_t *e, int n)
{
  (void) n;
#ifdef  USE_SDL
  int x = 0;
    
  if (!(SDL_WasInit (SDL_INIT_JOYSTICK) & SDL_INIT_JOYSTICK))
    if (SDL_InitSubSystem (SDL_INIT_JOYSTICK) < 0)
      return -1;

  for (; x < SDL_NumJoysticks (); x++)
    {
      sdl_joystick[x] = SDL_JoystickOpen (x);
      e->d[e->devices].id = EVENT_PAD1;
      strncpy (e->d[e->devices].id_s, SDL_JoystickName (x), EVENT_DEVICE_NAME_MAX)[EVENT_DEVICE_NAME_MAX - 1] = 0;
      e->d[e->devices].axes = SDL_JoystickNumAxes (sdl_joystick[x]);
      e->d[e->devices].buttons = SDL_JoystickNumButtons (sdl_joystick[x]);
//      SDL_JoystickEventState (SDL_ENABLE);
      e->devices++;
    }

  SDL_EventState (SDL_JOYAXISMOTION, SDL_ENABLE);
  SDL_EventState (SDL_JOYBALLMOTION, SDL_ENABLE);
  SDL_EventState (SDL_JOYHATMOTION, SDL_ENABLE);
  SDL_EventState (SDL_JOYBUTTONUP, SDL_ENABLE);
  SDL_EventState (SDL_JOYBUTTONDOWN, SDL_ENABLE);
#endif
  return 0;
}


static int
event_open_lirc (st_event_t *e)
{
  int lirc_lircd;
  struct sockaddr_un addr;

  addr.sun_family = AF_UNIX;
  strcpy (addr.sun_path, LIRCD);
  lirc_lircd = socket (AF_UNIX, SOCK_STREAM, 0);

  if (lirc_lircd == -1)
    {
      fprintf (stderr, "LIRC: could not open socket\n");
      return -1;
    }

  if (connect (lirc_lircd, (struct sockaddr *) &addr, sizeof (addr)) == -1)
    {
      close (lirc_lircd);
      fprintf (stderr, "LIRC: could not connect to socket\n");
      return -1;
    }

  e->d[e->devices].id = EVENT_LIRC;
  strcpy (e->d[e->devices].id_s, "Lirc");
  e->d[e->devices].axes = 0;
  e->d[e->devices].buttons = 255;
  e->devices++;

  return lirc_lircd;
}


st_event_t *
event_open (int flags)
{
  int x = 0;
  static st_event_t e;

  memset (&e, 0, sizeof (st_event_t));

  e.flags = flags;

  // NO input devices?
  if (!(e.flags & (EVENT_LIRC|EVENT_KEYBOARD|EVENT_MOUSE|EVENT_PAD1)))
    {
      fprintf (stderr, "event_open(): no input devices specified\n");
      return NULL;
    }

#ifdef  USE_SDL
  // Sadly, SDL_Events don't work without SDL_Video
  if (e.flags & (EVENT_KEYBOARD|EVENT_MOUSE|EVENT_PAD1))
    {
      if (!(SDL_WasInit (SDL_INIT_VIDEO) & SDL_INIT_VIDEO))
        { 
          fprintf (stderr, "event_open(): SDL_Event requires running SDL_Video\n");
          return NULL;
        }

      SDL_EventState (SDL_KEYUP, SDL_IGNORE);
      SDL_EventState (SDL_KEYDOWN, SDL_IGNORE);

      SDL_EventState (SDL_MOUSEMOTION, SDL_IGNORE);
      SDL_EventState (SDL_MOUSEBUTTONUP, SDL_IGNORE);
      SDL_EventState (SDL_MOUSEBUTTONDOWN, SDL_IGNORE);

      SDL_EventState (SDL_JOYAXISMOTION, SDL_IGNORE);
      SDL_EventState (SDL_JOYBALLMOTION, SDL_IGNORE);
      SDL_EventState (SDL_JOYHATMOTION, SDL_IGNORE);
      SDL_EventState (SDL_JOYBUTTONUP, SDL_IGNORE);
      SDL_EventState (SDL_JOYBUTTONDOWN, SDL_IGNORE);

//      SDL_EventState (SDL_ACTIVEEVENT, SDL_IGNORE);
//      SDL_EventState (SDL_QUIT, SDL_IGNORE);
//      SDL_EventState (SDL_SYSWMEVENT, SDL_IGNORE);
//      SDL_EventState (SDL_VIDEORESIZE, SDL_IGNORE);
//      SDL_EventState (SDL_USEREVENT, SDL_IGNORE);
    }
#endif

  if (e.flags & EVENT_KEYBOARD)
    event_open_keyboard (&e);

  if (e.flags & EVENT_MOUSE)
    event_open_mouse (&e);

  if (e.flags & (EVENT_PAD1))
    event_open_pad (&e, 0);

  if (e.flags & EVENT_LIRC)
    event_open_lirc (&e);

  if (!e.devices)
    {
      fprintf (stderr, "event_open(): none of the requested input devices was found\n");
      return NULL;
    }

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
event_read_keyboard (st_event_t *e)
{
  return 0;
}


static int
event_read_mouse (st_event_t *e)
{
  return 0;
}


static int
event_read_pad (st_event_t *e)
{
  return 0;
}
#endif


#define PACKET_SIZE 100
int
lirc_nextcode (char **code)
{
  static int packet_size = PACKET_SIZE;
  static int end_len = 0;
  ssize_t len = 0;
  char *end, c;

  *code = NULL;
  if (lirc_buffer == NULL)
    {
      lirc_buffer = (char *) malloc (packet_size + 1);
      if (lirc_buffer == NULL)
        {
          fprintf (stderr, "%s: out of memory\n", "LIRC");
          return (-1);
        }
      lirc_buffer[0] = 0;
    }
  while ((end = strchr (lirc_buffer, '\n')) == NULL)
    {
      if (end_len >= packet_size)
        {
          char *new_buffer;

          packet_size += PACKET_SIZE;
          new_buffer = (char *) realloc (lirc_buffer, packet_size + 1);
          if (new_buffer == NULL)
            {
              return (-1);
            }
          lirc_buffer = new_buffer;
        }
      len = read (lirc_lircd, lirc_buffer + end_len, packet_size - end_len);
      if (len <= 0)
        {
          if (len == -1 && errno == EAGAIN)
            return (0);
          else
            return (-1);
        }
      end_len += len;
      lirc_buffer[end_len] = 0;
      /* return if next code not yet available completely */
      if ((end = strchr (lirc_buffer, '\n')) == NULL)
        {
          return (0);
        }
    }
  /* copy first line to buffer (code) and move remaining chars to
     lirc_buffers start */
  end++;
  end_len = strlen (end);
  c = end[0];
  end[0] = 0;
  *code = strdup (lirc_buffer);
  end[0] = c;
  memmove (lirc_buffer, end, end_len + 1);
  if (*code == NULL)
    return (-1);
  return (0);
}


int
event_lirc_read (st_event_t *e)
{
  (void) e;
  char *code;
#if 0
  while (lirc_nextcode (&code) == 0 && code != NULL)
    {
      printf (code);
      free (code);
    }
#endif
  if (lirc_nextcode (&code) == 0 && code != NULL)
    {
system ("echo >/fhksgdf");
      printf (code);
      free (code);
    }

  return 0;
}

#if 0
int
event_read (st_event_t *e)
{
  int result = 0;

#if 0
    if (!SDL_PollEvent (&sdl_event))
      return 0;
#endif

  if (e->flags & EVENT_KEYBOARD)
    result = event_read_keyboard (event);

  if (!result && e->flags & EVENT_MOUSE)
    result = event_read_mouse (event); 

  if (!result && e->flags & EVENT_PAD1)
    result = event_read_pad (event); 

  if (!result && e->flags & EVENT_LIRC)
    result = event_read_lirc (event); 

  return result;
}
#endif


int
event_loop (st_event_t *e, int (*event_func) (st_event_t *e))
{
  int result = 0;

  if (e->flags & EVENT_LIRC)
    event_lirc_read (e);

  while (!result)
    {
      result = -1;
#ifdef  USE_SDL
      while (SDL_PollEvent (&sdl_event))
        {
          switch (sdl_event.type)
            {
#if 0
              case SDL_ACTIVEEVENT:
                if (!sdl_event.active.gain)
                  game.focus = 0;
                else
                  game.focus = 1;
                break;
    
              case SDL_VIDEORESIZE:
                vo_resize (sdl_event.resize.w, sdl_event.resize.h);
                break;
    
              case SDL_USEREVENT:
                if (e.user.code == SHOW_IMAGE)
                  show_image ();
                break;
#endif
    
              case SDL_KEYDOWN:
                e->e = EVENT_PUSH;
                if (sdl_event.key.keysym.mod & KMOD_CTRL)
                  e->e |= EVENTK_CTRL;
                e->e |= event_translate (sdl_event.key.keysym.sym);
                break;
      
              case SDL_KEYUP:
                e->e = EVENT_REL;
                if (sdl_event.key.keysym.mod & KMOD_CTRL)
                  e->e |= EVENTK_CTRL;
                e->e |= event_translate (sdl_event.key.keysym.sym);
                break;
      
              case SDL_MOUSEMOTION:
                e->e = EVENTM_AXIS1;
                e->xval = sdl_event.motion.xrel;
                e->yval = sdl_event.motion.yrel;
                break;

              case SDL_MOUSEBUTTONDOWN:
//MOUSE_BUTTON_LEFT
//MOUSE_BUTTON_RIGHT
//MOUSE_BUTTON_MIDDLE
printf ("SDL_MOUSEBUTTONDOWN\n");
fflush (stdout);
                e->e = EVENT_PUSH | EVENTM_B1;
                break;
  
              case SDL_MOUSEBUTTONUP:  
printf ("SDL_MOUSEBUTTONUP\n");
fflush (stdout);
                e->e = EVENT_REL | EVENTM_B1;
                break;

              case SDL_JOYAXISMOTION:
                if (sdl_event.jaxis.which == 0)       // we support only the first joystick
                  e->e = EVENTP1_AXIS1;
//                else
//                  e->e = EVENTP2_AXIS1;

                if (!(sdl_event.jaxis.axis & 1))  // x axis
                  e->xval = sdl_event.jaxis.value;
                if (sdl_event.jaxis.axis & 1)     // y axis
                  e->yval = sdl_event.jaxis.value;
                break;

              case SDL_JOYBUTTONUP:
              case SDL_JOYBUTTONDOWN:
                break;
            }
          result = event_func (e);
        }
#endif  // USE_SDL

//if (game->focus) // if there is no focus == no redraw?
      if (result == -1) // no event has occured so just draw
        result = event_func (NULL);
    }

  return 0;
}




static int
event_close_keyboard (st_event_t *e)
{
  (void) e;
  return 0;
}


static int
event_close_mouse (st_event_t *e)
{
  (void) e;
  return 0;
}


static int
event_close_pad (st_event_t *e)
{
  (void) e;
#ifdef  USE_SDL
  if (SDL_WasInit (SDL_INIT_JOYSTICK) & SDL_INIT_JOYSTICK)
    SDL_QuitSubSystem (SDL_INIT_JOYSTICK);
#endif
  return 0;
}


static int
event_close_lirc (st_event_t *e)
{
  (void) e;
  if (lirc_buffer)
    {
      free (lirc_buffer);
      lirc_buffer = NULL;
    }
  close (lirc_lircd);
  return 0;
}


int
event_close (st_event_t *e)
{
  if (e->flags & EVENT_KEYBOARD)
    event_close_keyboard (e);

  if (e->flags & EVENT_MOUSE)
    event_close_mouse (e);

  if (e->flags & EVENT_PAD1)
    event_close_pad (e);

  if (e->flags & EVENT_LIRC)
    event_close_lirc (e);

  return 0;
}


int
event_flush (st_event_t *e)
{
  (void) e;
#ifdef  USE_SDL
  SDL_Event dummy;
  while (SDL_PollEvent (&dummy));
#endif
  return 0;
}


int
event_pause (st_event_t *e)
{
  (void) e;
#ifdef  USE_SDL
  if (SDL_JoystickOpened (0))
    SDL_JoystickClose (0);
  else
    SDL_JoystickOpen (0);
#endif
  return 0;
}


#ifdef  TEST
//#if 0
int
event_cb (st_event_t *e)
{
  event_st_event_t_sanity_check (e);
  return 0;
}


int
main (int argc, char ** argv)
{
  st_event_t *e = NULL;

  e = event_open (EVENT_LIRC);

  if (e)
    event_loop (e, &event_cb);

  return 0;
}
#endif  // TEST
