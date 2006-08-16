/*
event_sdl.h - even more simple SDL wrapper for events

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
#ifndef MISC_EVENT_SDL_H
#define MISC_EVENT_SDL_H
#include "event.h"


/*
  event_sdl_open()  start event listener

  flags

  EVENT_KEYBOARD  enable keyboard events
  EVENT_MOUSE     enable mouse events
  EVENT_JOYSTICK  enable joystick events
  EVENT_INFRARED  enable infrared events
  EVENT_AUDIO     enable audio events

  event_sdl_close() close event listener

  event_sdl_read()  processes events
                  returns 0 == no event, 1 == event, -1 == error
  event_sdl_flush() flush pending events
  event_sdl_pause() pause events
*/
extern st_event_t * event_sdl_open (int flags, int delay_ms);
extern int event_sdl_read (st_event_t * e);
extern int event_sdl_flush (void);
extern int event_sdl_pause (void);
extern int event_sdl_close (void);


#endif  // MISC_EVENT_SDL_H
