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
#define READ_USLEEP_TIME 10000
#define FILL_USLEEP_TIME 50000
#define PREFILL_SLEEP_TIME 200
#define STREAM_BUFFER_SIZE 2048


typedef struct
{
  // constats:
  unsigned char *buffer;        // base pointer of the allocated buffer memory
  int buffer_size;              // size of the allocated buffer memory
  int sector_size;              // size of a single sector (2048/2324)
  int back_size;                // we should keep back_size amount of old bytes for backward seek
  int fill_limit;               // we should fill buffer only if space>=fill_limit
  int prefill;                  // we should fill min prefill bytes if cache gets empty

  // filler's pointers:
  int eof;
  unsigned long min_filepos;            // buffer contain only a part of the file, from min-max pos
  unsigned long max_filepos;
  unsigned long offset;                 // filepos <-> bufferpos  offset value (filepos of the buffer's first byte)

  // reader's pointers:
  unsigned long read_filepos;

} cache_vars_t;

//static int min_fill = 0;
//int cache_fill_status = 0;
static int fp = 0;
static unsigned int buf_pos, buf_len;
static unsigned long pos;
static int eof;
static unsigned int cache_pid;

static cache_vars_t *cache_data;
static unsigned char buffer[STREAM_BUFFER_SIZE];
#endif


st_cache_t *
cache_open (int buffers, unsigned long buffer_size, int cache_type)
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


int
cache_close (st_cache_t *c)
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


int
cache_write (st_cache_t *c, void *buffer, unsigned long buffer_len)
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


int
cache_read (st_cache_t *c, void *buffer, unsigned long buffer_len)
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


int
cache_read_cb (st_cache_t *c, int (*write_func) (void *, unsigned long), unsigned long buffer_len)
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


void
cache_pause (st_cache_t *c)
{
  c->pause = c->pause ? 0 : 1;
}




#if 0
  The cache framework can transport data between two filters in
  different ways to avoid latency/buffer-underrun problems

  normal:     linear transfer w/o fork or cache
                (data)-(process)->(data)
  
  callback:   same as normal but the filter writes the output to a callback function
                instead (for example) to (void *) o->data
                (data)-(process)->callback

  forked (a): fork() (every! call)
                parent: read from child() and continue until user-exit()-process
                child: write to OUT cache and exit()
                (data)->fork->child-(data)-(OUT cache)->parent

  forked (b): fork() (every! call)
                parent: write to OUT cache and exit()
                child: read from parent() and continue until user-exit()-process
                (data)->fork->parent-(data)-(OUT cache)->child

  forked (c): fork() (only once)
                parent: write to IN cache and read from OUT cache until
                         user-exit()-process
                child: read from IN cache, process and write to OUT cache until
                         user-exit()-process
                (data)->fork->parent-(data)-(IN cache)->child-(process)-(data)-(OUT cache)->parent


void
cache_stats (cache_vars_t * s)
{
  int newb = s->max_filepos - s->read_filepos;  // new bytes in the buffer

  printf ("0x%06X  [0x%06X]  0x%06X   ",
    (int) s->min_filepos,
    (int) s->read_filepos,
    (int) s->max_filepos);

  printf ("%3d %%  (%3d%%)\n",
    100 * newb / s->buffer_size,
    100 * min_fill / s->buffer_size);
}


int
cache_read (cache_vars_t * s, unsigned char *buf, int size)
{
  int total = 0;

  while (size > 0)
    {
      int pos, newb, len;

      printf("CACHE2_READ: 0x%X <= 0x%X <= 0x%X  \n",s->min_filepos,s->read_filepos,s->max_filepos);

      if (s->read_filepos >= s->max_filepos ||
          s->read_filepos < s->min_filepos)
        {
          // eof?
          if (s->eof)
            break;
          // waiting for buffer fill...
          wait2 (READ_USLEEP_TIME / 1000);
          continue;             // try again...
        }

      newb = s->max_filepos - s->read_filepos;  // new bytes in the buffer
//      if (newb < min_fill)
//        min_fill = newb;        // statistics...

     printf("*** newb: %d bytes ***\n",newb);

      pos = s->read_filepos - s->offset;
      if (pos < 0)
        pos += s->buffer_size;
      else if (pos >= s->buffer_size)
        pos -= s->buffer_size;

      if (newb > s->buffer_size - pos)
        newb = s->buffer_size - pos;    // handle wrap...
      if (newb > size)
        newb = size;

      // check:
      if (s->read_filepos < s->min_filepos)
        fprintf (stderr, "Ehh. s->read_filepos<s->min_filepos !!! Report bug...\n");

      // len=write(mem,newb)
      //printf("Buffer read: %d bytes\n",newb);
      memcpy (buf, &s->buffer[pos], newb);
      buf += newb;
      len = newb;
      // ...

      s->read_filepos += len;
      size -= len;
      total += len;

    }
//  cache_fill_status = 100 * (s->max_filepos - s->read_filepos) / s->buffer_size;

  return total;
}


int
cache_fill (cache_vars_t * s)
{
  int back, back2, newb, space, len, posi;

  if (s->read_filepos < s->min_filepos ||
      s->read_filepos > s->max_filepos)
    {
      // seek...
      fprintf (stderr, "Out of boundaries... seeking to 0x%X  \n", s->read_filepos);

      // streaming: drop cache contents only if seeking backward or too much fwd:
      if (s->read_filepos < s->min_filepos || s->read_filepos >= s->max_filepos + s->buffer_size)
        {
          s->offset = s->min_filepos = s->max_filepos = s->read_filepos;     // drop cache content :(
          if(eof)
            {
              pos=0;
              eof=0;
            }
  
          fprintf (stderr, "Seek to new pos: 0x%X  \n",
             (int) lseek (fp, s->read_filepos, SEEK_SET));
        }
    }

  // calc number of back-bytes:
  back = MAX (s->read_filepos - s->min_filepos, 0); // strange...
  back = MIN (back, s->back_size);

  // calc number of new bytes:
  newb = MAX (s->max_filepos - s->read_filepos, 0); // strange...

  // calc free buffer space:
  space = s->buffer_size - (newb + back);

  // calc bufferpos:
  posi = s->max_filepos - s->offset;
  if (posi >= s->buffer_size)
    posi -= s->buffer_size;      // wrap-around

  if (space < s->fill_limit)
    {
      printf("Buffer is full (%d bytes free, limit: %d)\n",space,s->fill_limit);
      return 0;                 // no fill...
    }

  printf("### read=0x%X  back=%d  newb=%d  space=%d  pos=%d\n",s->read_filepos,back,newb,space,posi);

  // reduce space if needed:
  space = MIN (space, s->buffer_size - posi);
  space = MIN (space, 4 * s->sector_size);

  // back+newb+space <= buffer_size
  back2 = s->buffer_size - (space + newb);      // max back size

  s->min_filepos = MAX (s->min_filepos, (s->read_filepos - back2));

  printf("Buffer fill: %d bytes of %d\n",space,s->buffer_size);

  len = read (fp, &s->buffer[posi], space);

  if (!len)
    s->eof = 1;

  s->max_filepos += len;
  if (posi + len >= s->buffer_size)
    {
      // wrap...
      s->offset += s->buffer_size;
    }

  return len;
}


cache_vars_t *
cache_open (int size, int sector)
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


void
cache_close (void)
{
  if (!cache_pid)
    return;
  kill (cache_pid, SIGKILL);
  waitpid (cache_pid, NULL, 0);

  if (!cache_data)
    return;

  shmem_free (cache_data->buffer, cache_data->buffer_size);
  shmem_free (cache_data, sizeof (cache_vars_t));
}


int
stream_enable_cache (int size, int min, int prefill)
{
  int ss = STREAM_BUFFER_SIZE;

  if (fp < 0)
    {
      // The stream has no 'fd' behind it, so is non-cacheable
      fprintf (stderr, "\rThis stream is non-cacheable\n");
      return 1;
    }

  if (size < 32 * 1024)
    size = 32 * 1024;           // 32kb min
  cache_data = cache_open (size, ss);
  cache_data->prefill = size * prefill;

  if ((cache_pid = fork ()))
    {
      // wait until cache is filled at least prefill_init %
      fprintf (stderr, "CACHE_PRE_INIT: %d [%d] %d  pre:%d  eof:%d  \n",
        cache_data->min_filepos,
        cache_data->read_filepos,
        cache_data->max_filepos,
        min,
        cache_data->eof);

      while (cache_data->read_filepos < cache_data->min_filepos ||
             cache_data->max_filepos - cache_data->read_filepos < min)
        {
          fprintf (stderr, "\rCache fill: %5.2f%% (%d bytes)    ",
            100.0 * (float) (cache_data->max_filepos - cache_data->read_filepos) / (float) (cache_data->buffer_size),
            cache_data->max_filepos - cache_data->read_filepos);

          if (cache_data->eof)
            break;              // file is smaller than prefill size

//          if (mp_input_check_interrupt (PREFILL_SLEEP_TIME))
//            return 0;
        }
      return 1;                 // parent exits
    }

  // child fills cache
  while (TRUE)
    {
      if (!cache_fill (cache_data))
        {
//          usec_sleep (FILL_USLEEP_TIME);        // idle
          wait2 (FILL_USLEEP_TIME / 1000);
        }
//       cache_stats(cache_data);
    }
}


int
cache_stream_fill_buffer (void)
{
  int len;
  if (eof)
    {
      buf_pos = buf_len = 0;
      return 0;
    }

  if (!cache_pid)
    return read (fp, buffer, STREAM_BUFFER_SIZE);
    

//  cache_stats(cache_data);

  if (pos != cache_data->read_filepos)
    fprintf (stderr, "!!! read_filepos differs!!! report this bug...\n");

  len = cache_read (cache_data, buffer, cache_data->sector_size);

  //printf("cache_stream_fill_buffer->read -> %d\n",len);

  if (len <= 0)
    {
      eof = 1;
      buf_pos = buf_len = 0;
      return 0;
    }
  buf_pos = 0;
  buf_len = len;
  pos += len;
//  printf("[%d]",len);fflush(stdout);
  return len;

}


int
cache_stream_seek_long (unsigned long posi)
{
  unsigned long newpos;

  fprintf (stderr,
           "CACHE2_SEEK: 0x%X <= 0x%X (0x%X) <= 0x%X  \n", cache_data->min_filepos,
           (int) posi, cache_data->read_filepos, cache_data->max_filepos);

  newpos = posi / cache_data->sector_size;
  newpos *= cache_data->sector_size;     // align
  pos = cache_data->read_filepos = newpos;
  cache_data->eof = 0;                   // !!!!!!!

  cache_stream_fill_buffer ();

  posi -= newpos;
  if (posi >= 0 && posi <= buf_len)
    {
      buf_pos = posi;    // byte position in sector
      return 1;
    }

//  stream->buf_pos=stream->buf_len=0;
//  return 1;

  fprintf (stderr,
           "cache_stream_seek: WARNING! Can't seek to 0x%llX !\n",
#ifdef _LARGEFILE_SOURCE
           (long long) (posi + newpos)
#else
           (posi + newpos)
#endif
           );
  return 0;
}
#endif

