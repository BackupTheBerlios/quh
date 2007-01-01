/*
quh_filter.h - filter for Quh

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
#ifndef QUH_FILTER_H
#define QUH_FILTER_H
#ifdef  __cplusplus
extern "C" {
#endif


typedef struct
{
  int option;
  const st_filter_t *f;
} st_quh_option_t;


extern const st_quh_option_t quh_option[];
extern const st_filter_t *quh_filter[];
extern const st_getopt2_t *quh_filter_usage[];
#ifdef  __cplusplus
}
#endif
#endif  // QUH_FILTER_H
