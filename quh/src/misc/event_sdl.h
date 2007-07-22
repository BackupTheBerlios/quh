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


// events
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
  EVENTM_B3,
  EVENTM_B4,
  EVENTM_B5,
  EVENTM_AXIS1,
// generic pad
  EVENTP1_B1, // buttons
  EVENTP1_B2,
  EVENTP1_B3,
  EVENTP1_B4,
  EVENTP1_B5,
  EVENTP1_B6,
  EVENTP1_B7,
  EVENTP1_B8,
  EVENTP1_B9,
  EVENTP1_B10,
  EVENTP1_B11,
  EVENTP1_B12,
  EVENTP1_AXIS0, // axis
  EVENTP1_AXIS1,
  EVENTP1_AXIS2,
  EVENTP1_AXIS3,
  EVENTP1_AXIS4,
  EVENTP1_AXIS5,
  EVENTP1_AXIS6,
  EVENTP1_AXIS7,
  EVENTP1_AXIS8,
#if 0
  EVENTGC_START, // START
  EVENTGC_A,    // A
  EVENTGC_B,    // B
  EVENTGC_L,    // L
  EVENTGC_R,    // R
  EVENTGC_X,    // X
  EVENTGC_Y,    // Y
  EVENTGC_Z,    // Z
  EVENTGC_AXISD,   // digital axis
  EVENTGC_AXISC,   // C axis
  EVENTGC_AXISA,   // analog axis
// pad n64          
  EVENTN64_START,
  EVENTN64_A,
  EVENTN64_B,
  EVENTN64_L,
  EVENTN64_R,
  EVENTN64_Z,
  EVENTN64_AXISD,  // digital axis
  EVENTN64_AXISC,  // C axis
  EVENTN64_AXISA,  // analog axis
// pad playstation dualshock
  EVENTPSDS_START,
  EVENTPSDS_SELECT,
  EVENTPSDS_ANALOG,
  EVENTPSDS_L1,
  EVENTPSDS_L2,
  EVENTPSDS_L3,  // left button
  EVENTPSDS_R1,
  EVENTPSDS_R2,
  EVENTPSDS_R3,  // right button
  EVENTPSDS_TRI, // triangle
  EVENTPSDS_REC, // rectangle
  EVENTPSDS_CIR, // circle
  EVENTPSDS_CRO, // cross
  EVENTPSDS_AXISD,  // digital axis
  EVENTPSDS_AXISL, // left analog axis
  EVENTPSDS_AXISR, // right analog axis
// pad dc
  EVENTDC_START,
  EVENTDC_L,     // left analog button
  EVENTDC_R,     // right analog button
  EVENTDC_A,
  EVENTDC_B,
  EVENTDC_X,
  EVENTDC_Y,
  EVENTDC_AXISD,    // digital axis
  EVENTDC_AXISA,    // analog axis
#endif
};



// keyboard specific event modifiers
#define EVENTK_CTRL (1<<16)
#define EVENTK_ALT (1<<17)
#define EVENTK_SHIFT (1<<18)


// keyboard and button event modifiers
#define EVENT_PUSH (1<<19)
#define EVENT_REL  (1<<20)


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
  int devices;        // total # of devices found
  st_event_dev_t d[EVENT_MAX_DEVICE];

  unsigned long e;    // event (e.g. EVENT_PUSH|EVENTK_w|EVENTK_CTRL)
  int xval;           // x and y axis value (if e == EVENT_AXIS1 e.g.)
  int yval;           //  [-]0...32768 analog or [-]32768 digital
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
#define EVENT_KEYBOARD       1
#define EVENT_MOUSE          (1<<1)
#define EVENT_PAD1           (1<<2)
//#define EVENT_PAD2         (1<<3)
//#define EVENT_PAD1_GC      (1<<4)
//#define EVENT_PAD1_N64     (1<<5)
//#define EVENT_PAD1_PSDS    (1<<6)
//#define EVENT_PAD1_DC      (1<<7)
//#define EVENT_PAD2_GC      (1<<8)
//#define EVENT_PAD2_N64     (1<<9)
//#define EVENT_PAD2_PSDS    (1<<10)
//#define EVENT_PAD2_DC      (1<<11)
#define EVENT_LIRC           (1<<12)
//#define EVENT_AUDIO        (1<<13)

extern st_event_t * event_open (int flags);
//extern int event_read (st_event_t *e);
extern int event_loop (st_event_t *e, int (*callback_func) (st_event_t *e));
extern int event_close (st_event_t *e);


extern int event_flush (st_event_t *e);
extern int event_pause (st_event_t *e);


#endif  // MISC_EVENT_SDL_H
