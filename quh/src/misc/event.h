/*
event.h - simple event wrapper

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
#ifndef MISC_EVENT_H
#define MISC_EVENT_H


#define EVENT_MAX_DEVICE 16
#define EVENT_DEVICE_NAME_MAX 1024


/*
  flags

  EVENT_KEYBOARD  enable keyboard events
  EVENT_MOUSE     enable mouse events
  EVENT_JOYSTICK  enable joystick events
  EVENT_INFRARED  enable infrared events
  EVENT_AUDIO     enable audio (input) events
*/
#define EVENT_KEYBOARD (1<<0)
#define EVENT_MOUSE    (1<<1)
#define EVENT_JOYSTICK (1<<2)
#define EVENT_INFRARED (1<<3)
//#define EVENT_AUDIO    (1<<4)


enum {
  EVENT_BUTTON = 1,
  EVENT_KEY = EVENT_BUTTON,
  EVENT_X0,
  EVENT_Y0,
  EVENT_X1,
  EVENT_Y1,
  EVENT_X2,
  EVENT_Y2,
  EVENT_X3,
  EVENT_Y3,
  EVENT_X4,
  EVENT_Y4,
  EVENT_X5,
  EVENT_Y5,
  EVENT_X6,
  EVENT_Y6,
  EVENT_X7,
  EVENT_Y7,
  EVENT_X8,
  EVENT_Y8,
  EVENT_MAX
};


typedef struct
{
  int id;
  char id_s[EVENT_DEVICE_NAME_MAX];

  int buttons; // keys (if keyboard)
  int axes;    // a mouse has axis too
} st_event_dev_t;


typedef struct
{
  int flags;

  unsigned long last_ms;       // last event (ms since midnight)
  int delay_ms;                // wait since last event before poll

  int devices;                 // total # of devices found
  st_event_dev_t d[EVENT_MAX_DEVICE];

  int dev;  // device of current event
  int e;    // id of current event (EVENT_BUTTON, EVENT_X5, ...)
  int val;  // value of current event ('A', 1, +-100)
} st_event_t;


//#ifdef  DEBUG
extern void event_st_event_t_sanity_check (st_event_t *e);
//#endif  


/*
  event_open()  start event listener

  flags
    see event.h for flags

  event_close() close event listener

  event_read()  processes events
                  returns 0 == no event, 1 == event, -1 == error
  event_flush() flush pending events
  event_pause() pause events
*/
extern st_event_t * event_open (int delay_ms, int flags);
extern int event_read (st_event_t *e);
extern int event_flush (st_event_t *e);
extern int event_pause (st_event_t *e);
extern int event_close (st_event_t *e);


#endif  // MISC_EVENT_H
