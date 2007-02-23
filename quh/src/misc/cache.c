/*
cache.c - Cache framework
                      
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cache.h"


#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif


static void
wait2 (int nmillis)
{
#ifdef  __MSDOS__
  delay (nmillis);
#elif   defined __unix__ || defined __APPLE__   // Mac OS X actually
  usleep (nmillis * 1000);
#elif   defined __BEOS__
  snooze (nmillis * 1000);
#elif   defined AMIGA
  Delay (nmillis * 1000);
#elif   defined _WIN32
  Sleep (nmillis);
#else
#ifdef  __GNUC__
#warning Please provide a wait2() implementation
#else
#pragma message ("Please provide a wait2() implementation")
#endif
  volatile int n;
  for (n = 0; n < nmillis * 65536; n++)
    ;
#endif
}


st_cache_t *
cache_open (unsigned long size, int flags)
{
  int x = 0;
  st_cache_t *c = NULL;

  if (!(c = (st_cache_t *) malloc (sizeof (st_cache_t))))
    return NULL;
        
  memset (c, 0, sizeof (st_cache_t));
  c->flags = flags;
  c->size = size;
 
  if (c->flags & CACHE_FIFO)
    {
      if (c->flags & CACHE_FILE) 
        {
          // CACHE_FIFO|CACHE_FILE uses pipe()
        }
      else // CACHE_MEM (default)
        {
          for (x = 0; x < CACHE_FIFO_SEG; x++)
            if (!(c->buffer[x] = (unsigned char *) malloc (size)))
              break;

          if (x == CACHE_FIFO_SEG) // success
            return c;
        }
    }
  else // CACHE_LIFO (default)
    {
      if (c->flags & CACHE_FILE) 
        {
        }
      else // CACHE_MEM (default)
        {
          return c;
        }
    }
    
  cache_close (c);
  return NULL;
}


int
cache_close (st_cache_t *c)
{
  if (c->flags & CACHE_FIFO)
    {
      if (c->flags & CACHE_FILE) 
        {
        }
      else // CACHE_MEM (default)
        {
          int x = 0;

          for (; x < CACHE_FIFO_SEG; x++)
            free (c->buffer[x]);
          free (c);

          return 0;
        }
    }
  else // CACHE_LIFO (default)
    {
      if (c->flags & CACHE_FILE) 
        {
        }
      else // CACHE_MEM (default)
        {
          free (c);

          return 0;
        }
    }

  return -1;
}


static int
cache_write_fifo_mem (st_cache_t *c,
                      void *key,
                      unsigned short key_len,
                      void *buffer,
                      unsigned long buffer_len)
{
  (void) key;
  (void) key_len;
  int buffer_read_pos = 0;
  unsigned long x;

  while (buffer_len > 0)
    {
      if (c->full == CACHE_FIFO_SEG)
        {
          wait2 (50);
          continue; // break?
        }

      x = MIN (c->size - c->write_pos, buffer_len);

      // read
      memcpy (((unsigned char *) c->buffer[c->write_buf]) + c->write_pos, ((unsigned char *) buffer) + buffer_read_pos, x);

      if (!c->write_pos)
        c->full++;

      buffer_len -= x;
      buffer_read_pos += x;
      c->len += x;
      c->write_pos += x;

      // block is full, find next
      if (c->write_pos >= c->size)
        {
          c->write_buf = (c->write_buf + 1) % CACHE_FIFO_SEG;
//              c->full++;
          c->write_pos = 0;
        }
    }

  return buffer_read_pos;
}


static int
cache_read_fifo_mem (st_cache_t *c,
                     void *key,
                     unsigned short key_len,
                     void *buffer,
                     unsigned long buffer_len)
{
  (void) key;
  (void) key_len;
  int buffer_write_pos = 0;
  unsigned long x;

  while (buffer_len > 0)
    {
      if (!c->len) // EOF
        break;

      x = MIN (c->size - c->read_pos, buffer_len);
      x = MIN (x, c->len);

      // write
      memcpy (((unsigned char *) buffer + buffer_write_pos), c->buffer[c->read_buf] + c->read_pos, x);

      buffer_len -= x;
      buffer_write_pos += x;
      c->len -= x;
      c->read_pos += x;

      // block is empty, find next
      if (c->read_pos >= c->size)
        {
          c->read_buf = (c->read_buf + 1) % CACHE_FIFO_SEG;
          c->full--;
          c->read_pos = 0;
        }
    }
  return buffer_write_pos;
}


static int
cache_read_cb_cb (st_cache_t *c, int (*write_func) (void *, unsigned long), unsigned long buffer_len)
{
  int buffer_write_pos = 0;
  unsigned long x;

  while (buffer_len > 0)
    {
      if (!c->len) // EOF
        break;

      x = MIN (c->size - c->read_pos, buffer_len);
      x = MIN (x, c->len);

      write_func (c->buffer[c->read_buf] + c->read_pos, x);

      buffer_len -= x;
      buffer_write_pos += x;
      c->len -= x;
      c->read_pos += x;

      // block is empty, find next
      if (c->read_pos >= c->size)
        {
          c->read_buf = (c->read_buf + 1) % CACHE_FIFO_SEG;
          c->full--;
          c->read_pos = 0;
        }
    }
  return buffer_write_pos;
}


int
cache_read_cb (st_cache_t *c, int (*write_func) (void *, unsigned long), unsigned long buffer_len)
{
#if 1
#warning
  return cache_read_cb_cb (c, write_func, buffer_len);
#else
  int len = 1;
  char buffer[1024*1024];
  while (len > 0)
  {
  len = CACHE_READ (c, &buffer, buffer_len);
  write_func (&buffer, len);
  }
  return len;
#endif
}


static int
cache_write_lifo_mem (st_cache_t *c,
                      void *key,
                      unsigned short key_len,
                      void *buffer,
                      unsigned long buffer_len)
{
  int x = 0;

  for (; x < CACHE_MAX_SEG; x++)
    {
      // overwrite old?
      if ((c->key[x] && !memcmp (c->key[x], key, key_len) && c->key_len[x] == key_len && c->key[x]) ||
          !c->key[x])
        {
          if (!c->key[x])
            if (!(c->key[x] = malloc (key_len)))
              { 
                c->key[x] = c->buffer[x] = NULL;
                c->key_len[x] = c->buffer_len[x] = 0;

                return 0;
              }
        
          memcpy (c->key[x], key, key_len);
          c->key_len[x] = key_len;

          if (c->buffer[x])
            free (c->buffer[x]);

          if (!(c->buffer[x] = malloc (buffer_len)))
            {
              free (c->key[x]);

              c->key[x] = c->buffer[x] = NULL;
              c->key_len[x] = c->buffer_len[x] = 0;                                

              return 0;
            }

          memcpy (c->buffer[x], buffer, buffer_len);
          c->buffer_len[x] = buffer_len;

          return buffer_len;
        }
    }

  return 0;
}


static int
cache_read_lifo_mem (st_cache_t *c,
                     void *key,
                     unsigned short key_len,
                     void *buffer,
                     unsigned long buffer_len)
{
  int x = 0;

  for (; x < CACHE_MAX_SEG && c->key[x]; x++)
    if (c->key_len[x] == key_len)
      if (!memcmp (c->key[x], key, key_len))
        {
          memcpy (buffer, c->buffer[x], MIN (buffer_len, c->buffer_len[x]));

          return MIN (buffer_len, c->buffer_len[x]);
        }

  return 0;
}


int
cache_write (st_cache_t *c,
             void *key,
             unsigned short key_len,
             void *buffer,
             unsigned long buffer_len)
{
  if (c->flags & CACHE_FIFO)
    {
      if (c->flags & CACHE_FILE) 
        {
        }
      else // CACHE_MEM (default)
        {
          return cache_write_fifo_mem (c, key, key_len, buffer, buffer_len);
        }
    }
  else // CACHE_LIFO (default)
    {
      if (c->flags & CACHE_FILE) 
        {
        }
      else // CACHE_MEM (default)
        {
          return cache_write_lifo_mem (c, key, key_len, buffer, buffer_len);
        }
    }

  return 0;
}


int
cache_read (st_cache_t *c,
            void *key,
            unsigned short key_len,
            void *buffer,
            unsigned long buffer_len)
{
  if (c->flags & CACHE_FIFO)
    {
      if (c->flags & CACHE_FILE) 
        {
        }
      else // CACHE_MEM (default)
        {
          return cache_read_fifo_mem (c, key, key_len, buffer, buffer_len);
        }
    }
  else // CACHE_LIFO (default)
    {
      if (c->flags & CACHE_FILE) 
        {
        }
      else // CACHE_MEM (default)
        {
          return cache_read_lifo_mem (c, key, key_len, buffer, buffer_len);
        }
    }

  return 0;
}


int
cache_sizeof (st_cache_t *c)
{
  return c->len;
}


void
cache_pause (st_cache_t *c)
{
  c->pause = c->pause ? 0 : 1;
}
