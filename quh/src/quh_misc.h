/*
quh_misc.h - miscellaneous functions for Quh

written by 2002 Dirk (d_i_r_k_@gmx.net)

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
#ifndef QUH_MISC_H
#define QUH_MISC_H
#ifdef  HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef  __cplusplus
extern "C" {
#endif


/*
  time_ms()  returns milliseconds since midnight
*/
extern unsigned long time_ms (unsigned long *ms);

extern const st_getopt2_t quh_options_usage[],
                          quh_options_usage_bottom[];


/*
  quh_get_index()      get current index (if any)
  quh_random()         (s)rand() wrapper that returns a random number between a and b
  quh_ms_to_bytes()    turn ms into bytes depending on informations in st_quh_filter_t
  quh_bytes_to_ms()    like quh_ms_to_bytes() but the other direction
  quh_bytes_to_units() turn the bytes (time) into different views
  quh_get_id3tag()     get ID3 tag from (any) file
  quh_demux()          open read/input filter by suffix of filename or magic
                         (handles url's and playlist's too)
  quh_wav_write_header() write std. wav header (used by many)
*/
//#ifdef  DEBUG
extern void quh_demuxer_sanity_check (st_quh_filter_t *file);
extern void quh_soundcard_sanity_check (void);
//#endif
extern int quh_get_index (st_quh_filter_t *file);
extern unsigned long quh_parse_optarg (st_quh_filter_t *f, const char *str);
extern unsigned long quh_random (unsigned long a, unsigned long b);
extern unsigned long quh_ms_to_bytes (st_quh_filter_t *file, unsigned long ms);
extern unsigned long quh_bytes_to_ms (st_quh_filter_t *file, unsigned long bytes);
#define QUH_UNITS_MS 1
#define QUH_UNITS_BYTES 2
#define QUH_UNITS_CLOCK 0
extern const char *quh_bytes_to_units (st_quh_filter_t *file, unsigned long bytes, int units);
//extern int quh_demux (const char *fname); // or url or playlist
extern int quh_play (void);
extern int quh_gauge (int pos, int size, int start_pos, int len, int color1, int color2);

extern void quh_set_object_s (st_filter_chain_t *fc, int o_id, const char *o);
extern const char *quh_get_object_s (st_filter_chain_t *fc, int o_id);

extern void quh_set_object (st_filter_chain_t *fc, int o_id, void *o, int o_size);
extern void *quh_get_object (st_filter_chain_t *fc, int o_id, void *o, int o_size);


typedef struct
{
  uint8_t magic[4];       // 'RIFF'
  uint32_t total_length;  // length of file minus the 8 byte riff header

  uint8_t type[4];        // 'WAVE'

  uint8_t fmt[4];         // 'fmt '
  uint32_t header_length; // length of format chunk minus 8 byte header
  uint16_t format;        // identifies WAVE_FORMAT_PCM
  uint16_t channels;
  uint32_t freq;          // samples per second per channel
  uint32_t bytespersecond;       
  uint16_t blockalign;    // basic block size
  uint16_t bitspersample;

  // PCM formats then go straight to the data chunk
  uint8_t data[4];        // 'data'
  uint32_t data_length;   // length of data chunk minus 8 byte header
} st_wav_header_t;

extern int quh_wav_write_header (FILE *fh, int channels, int freq, int size, unsigned long raw_size);


#ifdef  __cplusplus
}
#endif
#endif  // QUH_MISC_H
