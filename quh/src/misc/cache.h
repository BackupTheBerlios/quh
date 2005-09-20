/*
cache.h - Cache framework
           
written by 2005 NoisyB (noisyb@gmx.net)


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
#include <time.h>    // time_t
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


/*
  Cache framework

  st_cache_t cache framework struct

  cache_open()    open cache
                      you set the # of buffers and the buffer_size

  cache_read()    read from shared buffer
  cache_read_cb() read from shared buffer to write_func
  cache_write()   write to shared buffer
  cache_close()   close and free cache
                     forces break (stop) and exit() of reading side of cache

  cache_pause()   pause read and write side of cache
  cache_unpause() unpause read and write side of cache
*/


/*
  Cache types
  CACHE_NULL          cache disabled
  CACHE_MALLOC_LIFO   use malloc'd memory as LIFO cache
  CACHE_MALLOC_FIFO   use malloc'd memory as FIFO (ring buffer) cache
  CACHE_FILE_LIFO     use a tmp file as LIFO cache
                               (with a cache of maybe ~100MB you could play mp3 on a i386 ;-)
  CACHE_PIPE          use pipe() as FIFO cache
  CACHE_STREAM_PIPE   use stream_pipe() as FIFO cache
  CACHE_SHM           use shared memory as FIFO cache
*/
enum
{
  CACHE_NULL = 0,
  CACHE_MALLOC_LIFO,
//  CACHE_MALLOC_FIFO,
//  CACHE_FILE_LIFO,
  CACHE_PIPE,
//  CACHE_STREAM_PIPE,
//  CACHE_SHM
};


typedef struct
{
  int type;                      // CACHE_MALLOC_LIFO, CACHE_FILE_LIFO, ...

  int fd[2];                     // for file cache (pipe, streamed pipe)

  int buffers;                   // # of buffers with buffer_size
  unsigned long buffer_size;     // size of a single buffer
  unsigned char **buffer;        // malloc'ed buffer array

  int pause;                     // cache is paused (1==yes or 0==no)
  int full;                      // # of full buffers
  unsigned long len;             // (Bytes) buffered bytes in all buffers

  unsigned int read_buf;         // current read buffer
  unsigned int write_buf;        // current write buffer

  unsigned long read_pos;        // (Bytes) current pos in current read buffer
  unsigned long write_pos;       // (Bytes) current pos in current write buffer
} st_cache_t;


extern st_cache_t * cache_open (int buffers, unsigned long buffer_size, int cache_type);
extern int cache_close (st_cache_t *c);

extern int cache_write (st_cache_t *c,
                          void *buffer,
                          unsigned long buffer_len);
extern int cache_read (st_cache_t *c,
                         void *buffer,
                         unsigned long buffer_len);
extern int cache_read_cb (st_cache_t *c,
                            int (*write_func) (void *, unsigned long),
                            unsigned long buffer_len);
extern int cache_sizeof (st_cache_t *c);
//extern void cache_pause (st_cache_t *c);
//extern void cache_unpause (st_cache_t *c);


#endif // MISC_CACHE_H
