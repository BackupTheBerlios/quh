<?php
/*
geoip.php - GeoIP API

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
//phpinfo();

require_once ("misc/geoip/geoip.inc");


function
get_country_by_ip ($ip)
{
  geoip_load_shared_mem ("misc/geoip/GeoIP.dat");

  $gi = geoip_open ("misc/geoip/GeoIP.dat", GEOIP_SHARED_MEMORY);

  $country = geoip_country_name_by_addr ($gi, $ip);

  geoip_close ($gi);

  return $country;
}


function
get_country ()
{
  return get_country_by_ip ($_SERVER['REMOTE_ADDR']);
}


?>
