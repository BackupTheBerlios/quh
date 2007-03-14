<?php
/*
property.php - configfile handling

Copyright (c) 2007 NoisyB


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
//define (PROPERTY_SEPARATOR, '=');
//define (PROPERTY_SEPARATOR_S, "=");
//define (PROPERTY_COMMENT, '#');
//define (PROPERTY_COMMENT_S, "#");


/*
  property_check()       makes sure the configfile with filename exists and
                           has the right version.
                           returns:
                           -1 == failure (property file could not be written)
                           0 == everything is OK
                           1 == property file needs update; set_property() and
                                set_property_array() must be used to update it

  get_property_from_string()
                         parse a property from a string where the property
                         (prop_sep) and the comment separators can (must)
                         be specified. This is useful for parsing a lot of
                         things like http headers or property files with
                         different separators

  get_property()       get value of propname from filename or return value
                         of env with name like propname
                       mode:
                         PROPERTY_MODE_TEXT   return value as normal text
                         PROPERTY_MODE_FILENAME return value as filename
                                                  i.e., it runs realpath2()
                                                  on the filename and fixes
                                                  the characters if necessary
                                                                      (Cygwin)
  get_property_int()   like get_property() but returns an integer which is 0
                         if the value of propname was 0, [Nn] or [Nn][Oo]
                         and an integer or at least 1 for every other case

  set_property()       set propname with value in filename
  set_property_array() set an array of properties (st_property_t) at once

  DELETE_PROPERTY()    like set_property but when value of propname is NULL
                         the whole property will disappear from filename
*/
/*
#ifdef  __MSDOS__
#define PROPERTY_HOME_RC(n) "~" FILE_SEPARATOR_S n ".cfg"
#else
#define PROPERTY_HOME_RC(n) "~" FILE_SEPARATOR_S "." n "rc"
#endif

#if     defined __MSDOS__ || defined __CYGWIN__ || defined _WIN32
#define PROPERTY_MODE_GLOBAL_RC(n)  n ".cfg"
#else
#define PROPERTY_MODE_GLOBAL_RC(n)  FILE_SEPARATOR_S "etc" FILE_SEPARATOR_S n
#endif

#if     defined __MSDOS__ || defined __CYGWIN__ || defined _WIN32
#define PROPERTY_MODE_DIR(n) "~" FILE_SEPARATOR_S 
#elif   defined __unix__ || defined __BEOS__ || defined __APPLE__ // Mac OS X actually
#define PROPERTY_MODE_DIR(n) "~" FILE_SEPARATOR_S "." n FILE_SEPARATOR_S
#else
#define PROPERTY_MODE_DIR(n) ""
#endif
*/


/*
function
property_check ($filename, $version, $verbose)
{
}


function
get_property_from_string ($str, $propname, $prop_sep, $comment_sep)
{
}
*/


//define (PROPERTY_MODE_TEXT, 0);
//define (PROPERTY_MODE_FILENAME, 1);


function
get_property ($filename, $propname, $mode)
{
  return 0;
}


function
get_property_int ($filename, $propname)
{
  return get_property ($filename, $propname, 0);
}


function
set_property ($filename, $propname, $value, $comment)
{
  return 0;
}


function
set_property_int ($filename, $propname, $value, $comment)
{
  return set_property ($filename, $propname, $value, $comment);
}


function
delete_property ($filename, $propname)
{
  return set_property ($filename, $propname, NULL, NULL);
}


?>
