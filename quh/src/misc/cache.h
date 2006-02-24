/*
cache.h - Cache framework
           
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
#ifndef MISC_CACHE_H 
#define MISC_CACHE_H 
#ifdef  HAVE_CONFIG_H
#include "config.h"
#endif
//#include <time.h>    // time_t
#ifdef  HAVE_LIMITS_H
#include <limits.h>  // ARG_MAX
#endif  // HAVE_LIMITS_H

#ifndef ARG_MAX
#define ARG_MAX 128
#endif

#if     128 < ARG_MAX
#define CACHE_MAX ARG_MAX
#else
#define CACHE_MAX 128
#endif


#define CACHE_FIFO_SEG 4
#define CACHE_MAX_SEG (CACHE_MAX*8)


typedef struct
{
  int flags;                     // CACHE_MEM, CACHE_FILE, CACHE_LIFO, etc.

  void *key[CACHE_MAX_SEG];                     // malloc'ed key
  unsigned short key_len[CACHE_MAX_SEG];        // key length
      
  void *buffer[CACHE_MAX_SEG];                  // malloc'ed buffer
  unsigned long buffer_len[CACHE_MAX_SEG];      // buffer length
  
  unsigned long size;            // size of a single buffer
  int pause;                     // cache is paused (1==yes or 0==no)
  int full;                      // # of full buffers
  unsigned long len;             // (Bytes) buffered bytes in all buffers

  unsigned int read_buf;         // current read buffer
  unsigned int write_buf;        // current write buffer

  unsigned long read_pos;        // (Bytes) current pos in current read buffer
  unsigned long write_pos;       // (Bytes) current pos in current write buffer
} st_cache_t;


/*
  Cache flags
  CACHE_MEM       use malloc'd memory as cache (default)
  CACHE_FILE      use a tmp file as cache (default if size is larger then mem)
  CACHE_DYN_SIZE  cache changes it's size on demand
                    size argument will be the min. size
  CACHE_LIFO      cache is LIFO (default)
                    depending on size and the other flags
                    this will make use of malloc() or a temp file
  CACHE_FIFO      cache is FIFO
                    depending on size the other flags
                    this will make use of pipe(), fifo(), stream_pipe(), etc.
*/
#define CACHE_MEM      (1<<0)
#define CACHE_FILE     (1<<1)
//#define CACHE_DYN_SIZE (1<<2)
#define CACHE_LIFO     (1<<3)
#define CACHE_FIFO     (1<<4)


/*
  Cache framework

  st_cache_t cache framework struct

  cache_open()    open cache
                      you set the size and flags

  cache_read()    read from cache using a key
  cache_write()   write to cache using a key
  CACHE_READ()    read from cache using no key
  CACHE_WRITE()   write to cache using no key

  cache_close()   close and free cache
                     forces break (stop) and exit() of reading side of cache

  cache_sizeof()  returns current amount of bytes in the cache
  cache_pause()   pause read and write side of cache
  cache_unpause() unpause read and write side of cache
*/
extern st_cache_t * cache_open (unsigned long size, int flags);
extern int cache_close (st_cache_t *c);

extern int cache_write (st_cache_t *c,
                        void *key, unsigned short key_len,
                        void *buffer, unsigned long buffer_len);
extern int cache_read (st_cache_t *c,
                       void *key, unsigned short key_len,
                       void *buffer, unsigned long buffer_len);
#define CACHE_WRITE(c,b,bl) cache_write(c,NULL,0,b,bl)
#define CACHE_READ(c,b,bl) cache_read(c,NULL,0,b,bl)
extern int cache_read_cb (st_cache_t *c,
                          int (*write_func) (void *, unsigned long),
                          unsigned long buffer_len);
extern int cache_sizeof (st_cache_t *c);
//extern void cache_pause (st_cache_t *c);
//extern void cache_unpause (st_cache_t *c);


#endif // MISC_CACHE_H
