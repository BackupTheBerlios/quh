/*
spc.h - Super Nintendo/SPC support for Quh

Copyright (c) 2006 NoisyB


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
#ifndef QUH_SPC_H
#define QUH_SPC_H
#ifdef  USE_SPC
extern const st_filter_t quh_spc_in;
extern const st_getopt2_t quh_spc_in_usage;
#endif  // #ifdef  USE_SPC
#endif  // QUH_SPC_H
