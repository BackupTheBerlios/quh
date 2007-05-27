/*
event_sdl.h - even more simple SDL wrapper for events

Copyright (c) 2007 Dirk

          
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


#define EVENT_MAX_DEVICE 16
#define EVENT_DEVICE_NAME_MAX 1024


enum {
// keyboard
//  EVENTK_UNKNOWN,
  EVENTK_BACKSPACE = 1,
  EVENTK_TAB,
  EVENTK_CLEAR,
  EVENTK_RETURN,
  EVENTK_PAUSE,
  EVENTK_ESCAPE,
  EVENTK_SPACE,
  EVENTK_EXCLAIM,
  EVENTK_QUOTEDBL,
  EVENTK_HASH,
  EVENTK_DOLLAR,
  EVENTK_AMPERSAND,
  EVENTK_QUOTE,
  EVENTK_LEFTPAREN,
  EVENTK_RIGHTPAREN,
  EVENTK_ASTERISK,
  EVENTK_PLUS,
  EVENTK_COMMA,
  EVENTK_MINUS,
  EVENTK_PERIOD,
  EVENTK_SLASH,
  EVENTK_0,
  EVENTK_1,
  EVENTK_2,
  EVENTK_3,
  EVENTK_4,
  EVENTK_5,
  EVENTK_6,
  EVENTK_7,
  EVENTK_8,
  EVENTK_9,
  EVENTK_COLON,
  EVENTK_SEMICOLON,
  EVENTK_LESS,
  EVENTK_EQUALS,
  EVENTK_GREATER,
  EVENTK_QUESTION,
  EVENTK_AT,
  EVENTK_LEFTBRACKET,
  EVENTK_BACKSLASH,
  EVENTK_RIGHTBRACKET,
  EVENTK_CARET,
  EVENTK_UNDERSCORE,
  EVENTK_BACKQUOTE,
  EVENTK_a,
  EVENTK_b,
  EVENTK_c,
  EVENTK_d,
  EVENTK_e,
  EVENTK_f,
  EVENTK_g,
  EVENTK_h,
  EVENTK_i,
  EVENTK_j,
  EVENTK_k,
  EVENTK_l,
  EVENTK_m,
  EVENTK_n,
  EVENTK_o,
  EVENTK_p,
  EVENTK_q,
  EVENTK_r,
  EVENTK_s,
  EVENTK_t,
  EVENTK_u,
  EVENTK_v,
  EVENTK_w,
  EVENTK_x,
  EVENTK_y,
  EVENTK_z,
  EVENTK_DELETE,
  EVENTK_KP0,
  EVENTK_KP1,
  EVENTK_KP2,
  EVENTK_KP3,
  EVENTK_KP4,
  EVENTK_KP5,
  EVENTK_KP6,
  EVENTK_KP7,
  EVENTK_KP8,
  EVENTK_KP9,
  EVENTK_KP_PERIOD,
  EVENTK_KP_DIVIDE,
  EVENTK_KP_MULTIPLY,
  EVENTK_KP_MINUS,
  EVENTK_KP_PLUS,
  EVENTK_KP_ENTER,
  EVENTK_KP_EQUALS,
  EVENTK_UP,
  EVENTK_DOWN,
  EVENTK_RIGHT,
  EVENTK_LEFT,
  EVENTK_INSERT,
  EVENTK_HOME,
  EVENTK_END,
  EVENTK_PAGEUP,
  EVENTK_PAGEDOWN,
  EVENTK_F1,
  EVENTK_F2,
  EVENTK_F3,
  EVENTK_F4,
  EVENTK_F5,
  EVENTK_F6,
  EVENTK_F7,
  EVENTK_F8,
  EVENTK_F9,
  EVENTK_F10,
  EVENTK_F11,
  EVENTK_F12,
  EVENTK_F13,
  EVENTK_F14,
  EVENTK_F15,
  EVENTK_RSHIFT,
  EVENTK_LSHIFT,
  EVENTK_RCTRL,
  EVENTK_LCTRL,
  EVENTK_RALT,
  EVENTK_LALT,
// mouse
  EVENTM_B1,
  EVENTM_B2,
// pad
  EVENT_PAD_GC_START, // START (values: 0, 1)
  EVENT_PAD_GC_A,    // A (values: 0, 1)
  EVENT_PAD_GC_B,    // B (values: 0, 1)
  EVENT_PAD_GC_L,    // L (values: 0, 1)
  EVENT_PAD_GC_R,    // R (values: 0, 1)
  EVENT_PAD_GC_X,    // X (values: 0, 1)
  EVENT_PAD_GC_Y,    // Y (values: 0, 1)
  EVENT_PAD_GC_Z,    // Z (values: 0, 1)
  EVENT_PAD_GC_DX,   // digital x axis (values: -1, 0, 1)
  EVENT_PAD_GC_DY,   // digital y axis (values: -1, 0, 1)
  EVENT_PAD_GC_CX,   // C x axis (values: -100, 0, 100)
  EVENT_PAD_GC_CY,   // C y axis (values: -100, 0, 100)
  EVENT_PAD_GC_AX,   // analog x axis (values: -100, 0, 100)
  EVENT_PAD_GC_AY,   // analog y axis (values: -100, 0, 100)
          
  EVENT_PAD_N64_START,
  EVENT_PAD_N64_A,
  EVENT_PAD_N64_B,
  EVENT_PAD_N64_L,
  EVENT_PAD_N64_R,
  EVENT_PAD_N64_Z,
  EVENT_PAD_N64_DX,  // digital x axis (values: -1, 0, 1)
  EVENT_PAD_N64_DY,  // digital y axis (values: -1, 0, 1)
  EVENT_PAD_N64_CX,  // C x axis (values: -100, 0, 100)
  EVENT_PAD_N64_CY,  // C y axis (values: -100, 0, 100)
  EVENT_PAD_N64_AX,  // analog x axis (values: -100, 0, 100)
  EVENT_PAD_N64_AY,  // analog y axis (values: -100, 0, 100)
  
  EVENT_PAD_PSDS_START,
  EVENT_PAD_PSDS_SELECT,
  EVENT_PAD_PSDS_ANALOG,
  EVENT_PAD_PSDS_L1,
  EVENT_PAD_PSDS_L2,
  EVENT_PAD_PSDS_L3,  // left button (values: 0, 1)
  EVENT_PAD_PSDS_R1,
  EVENT_PAD_PSDS_R2,
  EVENT_PAD_PSDS_R3,  // right button (values: 0, 1)
  EVENT_PAD_PSDS_TRI, // triangle (values: 0, 1)
  EVENT_PAD_PSDS_REC, // rectangle (values: 0, 1)
  EVENT_PAD_PSDS_CIR, // circle (values: 0, 1)
  EVENT_PAD_PSDS_CRO, // cross (values: 0, 1)
  EVENT_PAD_PSDS_DX,  // digital x axis (values: -1, 0, 1)
  EVENT_PAD_PSDS_DY,  // digital y axis (values: -1, 0, 1)
  EVENT_PAD_PSDS_LAX, // left analog x axis (values: -100, 0, 100)
  EVENT_PAD_PSDS_LAY, // left analog y axis (values: -100, 0, 100)
  EVENT_PAD_PSDS_RAX, // right analog x axis (values: -100, 0, 100)
  EVENT_PAD_PSDS_RAY, // right analog y axis (values: -100, 0, 100)

  EVENT_PAD_DC_START,
  EVENT_PAD_DC_L,     // left analog button (values: 0, 100)
  EVENT_PAD_DC_R,     // right analog button (values: 0, 100)
  EVENT_PAD_DC_A,
  EVENT_PAD_DC_B,
  EVENT_PAD_DC_X,
  EVENT_PAD_DC_Y,
  EVENT_PAD_DC_DX,    // digital x axis (values: -1, 0, 1)
  EVENT_PAD_DC_DY,    // digital y axis (values: -1, 0, 1)
  EVENT_PAD_DC_AX,    // analog x axis (values: -100, 0, 100)
  EVENT_PAD_DC_AY,    // analog y axis (values: -100, 0, 100)

#if 0
  EVENT_PAD_OTHER_B1,
  EVENT_PAD_OTHER_B2,
  EVENT_PAD_OTHER_B3,
  EVENT_PAD_OTHER_B4,
  EVENT_PAD_OTHER_B5,
  EVENT_PAD_OTHER_B6,
  EVENT_PAD_OTHER_B7,
  EVENT_PAD_OTHER_B8,
  EVENT_PAD_OTHER_B9,
  EVENT_PAD_OTHER_B10,
  EVENT_PAD_OTHER_B11,
  EVENT_PAD_OTHER_B12,
  EVENT_PAD_OTHER_LAX, // left analog x axis (values: -100, 0, 100)
  EVENT_PAD_OTHER_LAY, // left analog y axis (values: -100, 0, 100)
  EVENT_PAD_OTHER_RAX, // right analog x axis (values: -100, 0, 100)
  EVENT_PAD_OTHER_RAY, // right analog y axis (values: -100, 0, 100)

  EVENT_BUTTON,
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
#endif
};


// keyboard specific modifiers
#define EVENTK_CTRL (1<<18)
#define EVENTK_ALT (1<<17)
#define EVENTK_SHIFT (1<<16)

// modifiers
#define EVENT_PUSH 0
#define EVENT_REL  (1<<31)
//#define EVENT_MOVE (1<<30)

// types
#define EVENT_KEYBOARD 0
#define EVENT_MOUSE (1<<29)
#define EVENT_PAD1  (1<<28)
#define EVENT_PAD2  (1<<27)
#define EVENT_PAD_GC       (1<<26)
#define EVENT_PAD_N64      (1<<25)
#define EVENT_PAD_PSDS     (1<<24)
#define EVENT_PAD_DC       (1<<23)
//#define EVENT_PAD_OTHER    (1<<22)
//#define EVENT_PAD_KEYBOARD (1<<21)
#define EVENT_INFRARED (1<<20)
//#define EVENT_AUDIO    (1<<19)


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
  unsigned long e;    // id of current event (EVENT_BUTTON, EVENT_X5, ...)
  int val;  // value of current event ('A', 1, +-100)
} st_event_t;


#ifdef  DEBUG
extern void event_st_event_t_sanity_check (st_event_t *e);
#endif  


/*
  event_open()  start event listener
  event_close() close event listener
  event_loop()

  event_flush() flush pending events
  event_pause() pause events
*/
extern st_event_t * event_open (int delay_ms, int flags);
//extern int event_read (st_event_t *e);
extern int event_loop (int (*callback_func) (st_event_t *e));
extern int event_close (st_event_t *e);


extern int event_flush (st_event_t *e);
extern int event_pause (st_event_t *e);


#endif  // MISC_EVENT_SDL_H
