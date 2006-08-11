/*
event.h - keyboard/mouse/joystick and infrared event support

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
#ifndef MISC_EVENT_JOYSTICK_H
#define MISC_EVENT_JOYSTICK_H
#include "event.h"


/*
  event_joystick_open()  start event listener

  flags
    see event.h for flags

  event_joystick_close() close event listener

  event_joystick_read()  processes events
                  returns 0 == no event, 1 == event, -1 == error
  event_joystick_flush() flush pending events
  event_joystick_pause() pause events
*/
extern st_event_t * event_joystick_open (int flags, int delay_ms);
extern int event_joystick_read (st_event_t * e);
extern int event_joystick_flush (void);
extern int event_joystick_pause (void);
extern int event_joystick_close (void);

#endif  // MISC_EVENT_JOYSTICK_H
