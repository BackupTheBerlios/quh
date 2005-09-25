/*
cache.c - Cache framework
                      
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
#ifdef  HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>  // waitpid()
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "misc/string.h"
#include "misc/misc.h"
#include "cache.h"



#ifdef  MAXBUFSIZE
#undef  MAXBUFSIZE
#endif
#define MAXBUFSIZE 32768


#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif


#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE (!FALSE)
#endif


#if 0
static void *
shmem_alloc (int size)
{
  void *p;
  static int devzero = -1;
  struct shmid_ds shmemds;
  int shmemid;

  if ((shmemid = shmget (IPC_PRIVATE, size, IPC_CREAT | 0600)) == -1)
    return NULL;
    
  if ((p = shmat (shmemid, 0, 0)) == (void *) -1)
    {
      shmctl (shmemid, IPC_RMID, &shmemds);
      return NULL;
    }
  
  if (shmctl (shmemid, IPC_RMID, &shmemds) == -1)
    if (shmdt (p) == -1)
      return NULL;

  return p;
}


static int
shmem_free (void *p, int size)
{
  return shmdt (p);
}
#endif


static st_cache_t *
cache_malloc_lifo_open (int buffers, unsigned long buffer_size, int cache_type)
{
  st_cache_t *c = NULL;
  
  if (!(c = (st_cache_t *) malloc (sizeof (st_cache_t))))
    return NULL;
        
  memset (c, 0, sizeof (st_cache_t));
  c->type = cache_type;
          
  if (!(c->buffer = (unsigned char **) malloc (buffers * sizeof (unsigned char *))))
    {
      free (c);
      return NULL;
    }
    
  for (c->buffers = 0; c->buffers < buffers; c->buffers++)
    if (!(c->buffer[c->buffers] = (unsigned char *) malloc (buffer_size)))
      {
        cache_close (c);
        return NULL;
      }
    
  c->buffer_size = buffer_size;
    
  return c;
}


static int
cache_malloc_lifo_close (st_cache_t *c)
{
  while (c->buffers > 0)
    {
      free (c->buffer[c->buffers - 1]);
      c->buffers--;
    }
  free (c->buffer);
  free (c);

  return 0;
}


static int
cache_malloc_lifo_write (st_cache_t *c, void *buffer, unsigned long buffer_len)
{
  int len = 0;
  unsigned long x;

  while (buffer_len > 0)
    {
      if (c->full == c->buffers)
        {
          wait2 (50);
          continue; // break?
        }

      x = MIN (c->buffer_size - c->write_pos, buffer_len);

      // read
      memcpy (((unsigned char *) c->buffer[c->write_buf]) + c->write_pos, ((unsigned char *) buffer) + len, x);

      if (!c->write_pos)
        c->full++;

      len += x;
      buffer_len -= x;
      c->len += x;
      c->write_pos += x;

      // block is full, find next
      if (c->write_pos >= c->buffer_size)
        {
          c->write_buf = (c->write_buf + 1) % c->buffers;
//              c->full++;
          c->write_pos = 0;
        }
    }

  return len;
}


static int
cache_malloc_lifo_read (st_cache_t *c, void *buffer, unsigned long buffer_len)
{
  int len = 0;
  unsigned long x;

  while (buffer_len > 0)
    {
      if (!c->len) // EOF
        break;

      x = MIN (c->buffer_size - c->read_pos, buffer_len);
      x = MIN (x, c->len);

      // write
      memcpy (((unsigned char *) buffer + len), c->buffer[c->read_buf] + c->read_pos, x);

      len += x;
      buffer_len -= x;
      c->len -= x;
      c->read_pos += x;

      // block is empty, find next
      if (c->read_pos >= c->buffer_size)
        {
          c->read_buf = (c->read_buf + 1) % c->buffers;
          c->full--;
          c->read_pos = 0;
        }
    }
  return len;
}


static int
cache_malloc_lifo_read_cb (st_cache_t *c, int (*write_func) (void *, unsigned long), unsigned long buffer_len)
{
  int len = 0;
  unsigned long x;

  while (buffer_len > 0)
    {
      if (!c->len) // EOF
        break;

      x = MIN (c->buffer_size - c->read_pos, buffer_len);
      x = MIN (x, c->len);

      write_func (c->buffer[c->read_buf] + c->read_pos, x);

      len += x;
      buffer_len -= x;
      c->len -= x;
      c->read_pos += x;

      // block is empty, find next
      if (c->read_pos >= c->buffer_size)
        {
          c->read_buf = (c->read_buf + 1) % c->buffers;
          c->full--;
          c->read_pos = 0;
        }
    }
  return len;
}


static st_cache_t *
cache_pipe_open (int buffers, unsigned long buffer_size, int cache_type)
{
  (void) buffers;
  (void) buffer_size;

  st_cache_t *c = NULL;
  
  if (!(c = (st_cache_t *) malloc (sizeof (st_cache_t))))
    return NULL;
        
  memset (c, 0, sizeof (st_cache_t));
  c->type = cache_type;
          
  if (pipe (c->fd) < 0)
    return NULL;

  return c;
}


static int
cache_pipe_close (st_cache_t *c)
{
  close (c->fd[0]);
  close (c->fd[1]);

  return 0; 
}


static int
cache_pipe_write (st_cache_t *c, void *buffer, unsigned long buffer_len)
{
  int result = write (c->fd[1], buffer, buffer_len);
  if (result > 0)
    c->len += result;
  return result;
}


static int
cache_pipe_read (st_cache_t *c, void *buffer, unsigned long buffer_len)
{
  int result = read (c->fd[0], buffer, buffer_len);
  if (result > 0)
    c->len -= result;
  return result;
}


static int
cache_pipe_read_cb (st_cache_t *c, int (*write_func) (void *, unsigned long), unsigned long buffer_len)
{
  (void) c;
  (void) write_func;
  (void) buffer_len;
  
  return 0;
}


#if 0
static st_cache_t *
cache_shm_open (int size, int sector)
{
  int num;
  cache_vars_t *s = shmem_alloc (sizeof (cache_vars_t));

  memset (s, 0, sizeof (cache_vars_t));

  num = size / sector;
  s->buffer_size = num * sector;
  s->sector_size = sector;
  s->buffer = shmem_alloc (s->buffer_size);
  s->fill_limit = 8 * sector;
  s->back_size = size / 2;
  s->prefill = size / 20;       // default: 5%

  return s;
}


static int
cache_shm_close (int size, int sector)
{
}
#endif


st_cache_t *
cache_open (int buffers, unsigned long buffer_size, int cache_type)
{
  st_cache_t *c = NULL;

  switch (cache_type)
    {
      case CACHE_MALLOC_LIFO:
        c = cache_malloc_lifo_open (buffers, buffer_size, cache_type);
        break;
      case CACHE_PIPE:
        c = cache_pipe_open (buffers, buffer_size, cache_type);
    }

  return c;
}


int
cache_close (st_cache_t *c)
{
  int result = -1;

  switch (c->type)
    {
      case CACHE_MALLOC_LIFO:
        result = cache_malloc_lifo_close (c);
        break;
      case CACHE_PIPE:
        result = cache_pipe_close (c);
    }

  return result;
}


int
cache_write (st_cache_t *c, void *buffer, unsigned long buffer_len)
{
  int result = -1;

  switch (c->type)
    {
      case CACHE_MALLOC_LIFO:
        result = cache_malloc_lifo_write (c, buffer, buffer_len);
        break;
      case CACHE_PIPE:
        result = cache_pipe_write (c, buffer, buffer_len);
    }

#ifdef  DEBUG
  printf ("\ncache_write(): %ld\n\n", buffer_len);
  dumper (stdout, buffer, buffer_len, 0, DUMPER_DEFAULT);
  fflush (stdout);
#endif

  return result;
}


int
cache_read (st_cache_t *c, void *buffer, unsigned long buffer_len)
{
  int result = -1;

  switch (c->type)
    {
      case CACHE_MALLOC_LIFO:
        result = cache_malloc_lifo_read (c, buffer, buffer_len);
        break;
      case CACHE_PIPE:
        result = cache_pipe_read (c, buffer, buffer_len);
    }

#ifdef  DEBUG
  printf ("\ncache_read(): %ld\n\n", buffer_len);
  dumper (stdout, buffer, buffer_len, 0, DUMPER_DEFAULT);
  fflush (stdout);
#endif
  
  return result;
}


int
cache_read_cb (st_cache_t *c, int (*write_func) (void *, unsigned long), unsigned long buffer_len)
{
  int result = -1;

  switch (c->type)
    {
      case CACHE_MALLOC_LIFO:
        result = cache_malloc_lifo_read_cb (c, write_func, buffer_len);
        break;
      case CACHE_PIPE:
        result = cache_pipe_read_cb (c, write_func, buffer_len);
    }

  return result;
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
