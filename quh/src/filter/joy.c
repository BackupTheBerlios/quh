/*
joystick.c - joystick filter for Quh

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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#ifdef  USE_SDL
// SDL preferred over native
#include <SDL.h>                // SDL_Joystick
#else
#ifdef  __linux__
#include <linux/joystick.h>
#endif  // __linux__
#endif  // USE_SDL
#include "misc/itypes.h"
#include "misc/getopt2.h"
#include "misc/misc.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "joy.h"


#warning TODO: fix


#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif


static int
quh_joystick_init (st_quh_nfo_t *file)
{
  (void) file;

  if (!quh_get_object_s (quh.filter_chain, QUH_OPTION))
    quh_set_object_s (quh.filter_chain, QUH_OPTION, "0");
                
  return 0;
}


#ifdef  USE_SDL
static SDL_Joystick *joystick = NULL;
static int joy_number = 0;


int
quh_joysdl_open (st_quh_nfo_t *file)
{
  (void) file;
#define NAME_LENGTH 128
  char name[NAME_LENGTH] = "Unknown";
  unsigned char axes = 2, buttons = 2, balls = 2;
  uint32_t flags = SDL_WasInit (SDL_INIT_EVERYTHING);
  joy_number = strtol (quh_get_object_s (quh.filter_chain, QUH_OPTION), NULL, 10);

  if (!(flags & SDL_INIT_JOYSTICK))
#if 1
    if (SDL_Init (SDL_INIT_EVERYTHING) != 0)
#else
    if (!SDL_InitSubSystem (SDL_INIT_JOYSTICK))
#endif
      return -1;
    
  if (SDL_NumJoysticks ())
    if ((joystick = SDL_JoystickOpen (joy_number)))
      {
         strncpy (name, SDL_JoystickName (joy_number), NAME_LENGTH)[NAME_LENGTH - 1] = 0;
         axes = SDL_JoystickNumAxes (joystick);
         buttons = SDL_JoystickNumButtons (joystick);
         balls = SDL_JoystickNumBalls (joystick);

//         SDL_JoystickEventState (SDL_QUERY);

         fprintf (stderr, "Joystick (%s) has %d axes and %d buttons\n", name, axes, buttons);
         fflush (stderr); 

         return 0;
      }

  return -1;
}


int
quh_joysdl_close (st_quh_nfo_t *file)
{
  (void) file;
#if 0
  uint32_t flags = SDL_WasInit (SDL_INIT_EVERYTHING);

  if (!(flags & SDL_INIT_JOYSTICK))
    SDL_QuitSubSystem (SDL_INIT_JOYSTICK);
#else
  SDL_Quit ();
#endif

  return 0;
}


int
quh_joysdl_write (st_quh_nfo_t *file)
{
  SDL_Event event;
  int x = 0, y = 0;

//  while (SDL_PollEvent (&event))
  SDL_PollEvent (&event);
    switch (event.type)
      {
        case SDL_KEYDOWN:
//          printf ("key\n");
          break;

        case SDL_JOYBUTTONDOWN:
//          printf ("button\n");
          break;

        case SDL_JOYAXISMOTION:
//          printf ("axis\n");
          if (!(event.jaxis.axis & 1)) // x axis
            {
              if (event.jaxis.value > 3200)
                x = 1;
              else if (event.jaxis.value < -3200)
                x = -1;
            }
          else
            if (event.jaxis.axis & 1) // y axis
              {
                if (event.jaxis.value > 3200)
                  y = 1;
                else if (event.jaxis.value < -3200)
                  y = -1;
              }
          break;

        default:
          break;
      }

  if (x || y)
    {
      if (x > 0)
        quh.raw_pos = MIN (quh.raw_pos + quh_ms_to_bytes (file, 10000), file->raw_size);
      if (x < 0) 
        quh.raw_pos = MAX (quh.raw_pos - quh_ms_to_bytes (file, 10000), 0);

#if 0
      if (y > 0)
        quh.raw_pos = MAX (quh.raw_pos - quh_ms_to_bytes (file, 60000), 0);
      if (y < 0)
        quh.raw_pos = MIN (quh.raw_pos + quh_ms_to_bytes (file, 60000), file->raw_size);
#endif

      filter_seek (quh.filter_chain, file);
    }

  return 0;
}


#else
#ifdef  __linux__


static int joystick = 0;
static unsigned char axes = 2, buttons = 2; //, balls = 2;

int
quh_joystick_open (st_quh_nfo_t *file)
{
  (void) file;
#define NAME_LENGTH 128
  char name[NAME_LENGTH] = "Unknown";
  char device[255];
  
  sprintf (device, "/dev/js%s", (const char *) map_get (quh.filter_option, (void *) QUH_JOY_PASS));
  int version = 0x000800;

  if ((joystick = open (device, O_RDONLY)) < 0)
    return 0;

  ioctl (joystick, JSIOCGVERSION, &version);
  ioctl (joystick, JSIOCGAXES, &axes);
  ioctl (joystick, JSIOCGBUTTONS, &buttons);
  ioctl (joystick, JSIOCGNAME (NAME_LENGTH), name);

  fprintf (stderr, "Joystick (%s) has %d axes and %d buttons\n", name, axes, buttons);
  fprintf (stderr, "Driver version is %d.%d.%d Linux (%s)\n",
    version >> 16, (version >> 8) & 0xff, version & 0xff, device);
  fflush (stderr); 

  return 0;
}


int
quh_joystick_close (st_quh_nfo_t *file)
{
  (void) file;

  if (joystick)
    close (joystick);

  return 0;
}


int
quh_joystick_write (st_quh_nfo_t *file)
{
  (void) file;
  int x = 0, y = 0;
//  int *button;
  struct js_event js;

//  button = (int *) malloc (buttons * sizeof (char));

//  while (TRUE)
    {
      if (read (joystick, &js, sizeof (struct js_event)) != sizeof (struct js_event))
        return 0; // skip

      switch (js.type & ~JS_EVENT_INIT)
        {
          case JS_EVENT_BUTTON:
//            button[js.number] = js.value;
            break;

          case JS_EVENT_AXIS:
            if (!(js.number & 1)) // x axis
              {
                if (js.value > 3200)
                  x = 1;
                else if (js.value < -3200)
                  x = -1;
              }
            else
              if (js.number & 1) // y axis
                {
                  if (js.value > 3200)
                    y = 1;
                  else if (js.value < -3200)
                    y = -1;
                }
            break;
        }
    }

  if (x || y)
    {
      if (x > 0)
        quh.raw_pos = MIN (quh.raw_pos + quh_ms_to_bytes (file, 10000), file->raw_size);
      if (x < 0) 
        quh.raw_pos = MAX (quh.raw_pos - quh_ms_to_bytes (file, 10000), 0);

#if 0
      if (y > 0)
        quh.raw_pos = MAX (quh.raw_pos - quh_ms_to_bytes (file, 60000), 0);
      if (y < 0)
        quh.raw_pos = MIN (quh.raw_pos + quh_ms_to_bytes (file, 60000), file->raw_size);
#endif

      filter_seek (quh.filter_chain, file);
    }

  return 0;
}
#endif  // __linux__
#endif  // USE_SDL


#ifdef  USE_SDL
const st_filter_t quh_joystick =
{
  QUH_JOY_PASS,
  "joystick (SDL)",
  NULL,
  0,
  NULL,
  (int (*) (void *)) &quh_joysdl_open,
  (int (*) (void *)) &quh_joysdl_close,
  NULL,
  (int (*) (void *)) &quh_joysdl_write,
  NULL,
  NULL,
  (int (*) (void *)) &quh_joystick_init,
  NULL
};
#else
#ifdef  __linux__
const st_filter_t quh_joystick =
{
  QUH_JOY_PASS,
  "joystick",
  NULL,
  0,
  (int (*) (void *)) &quh_joystick_open,
  (int (*) (void *)) &quh_joystick_close,
  NULL,
  (int (*) (void *)) &quh_joystick_write,
  NULL,
  NULL,
  (int (*) (void *)) &quh_joystick_init,
  NULL
};
#endif  // __linux__
#endif  // USE_SDL


const st_getopt2_t quh_joystick_usage =
{
  "joy", 2, 0, QUH_JOY,
  "N", "enable joystick N seek and pause control (default: 0)", (void *) QUH_JOY_PASS
};

