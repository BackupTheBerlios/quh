<?php
/*
sql.php - simplified wrappers for SQL access

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


class misc_sql
{


var $conn = NULL;
var $res = NULL;
//var $row_pos = -1;


function
sql_stresc ($s)
{
//  return mysql_escape_string ($s); // deprecated
  return mysql_real_escape_string ($s, $this->conn);
}


function
sql_open ($host, $user, $pass, $db_name)
{
  if (!is_null ($this->conn))
    mysql_close ($this->conn);

  $this->conn = mysql_connect ($host, $user, $pass) or die (mysql_error ());
  mysql_select_db ($db_name, $this->conn);
}


function
sql_read ($debug)
{
  $a = Array ();

  if (!isset ($this->res))
    return NULL;

  if (is_null ($this->res))
    return NULL;

  if ($this->res == FALSE)
    return NULL;

//  while ($row = mysql_fetch_array ($this->res, MYSQL_BOTH))
  while ($row = mysql_fetch_array ($this->res))
    $a[] = $row;

  if ($debug == 1)
    {
      $p = "";
      $i_max = sizeof ($a);
      for ($i = 0; $i < $i_max; $i++)
        {
          $j_max = sizeof ($a[$i]);
          for ($j = 0; $j < $j_max; $j++)
            $p .= $a[$i][$j]
                 ." ";

          $p .= "<br>";
        }

      echo $p;
    }

  return $a;
}


function
sql_getrow ($row, $debug)
{
  if (is_null ($this->res))
    return NULL;

  if ($this->res == FALSE)
    return NULL;

  if ($row >= mysql_num_rows ($this->res))
    return NULL;

  if (mysql_data_seek ($this->res, $row) == false)
    return NULL;

//  $this->row_pos = $row;

  $a = mysql_fetch_row ($this->res);

  if ($debug == 1)
    {
      $p = "";
      $i_max = sizeof ($a);
      for ($i = 0; $i < $i_max; $i++)
        $p .= $a[$i]
           ." ";

      $p .= "<br>";

      echo $p;
    }

  return $a;
}


function
sql_write ($sql_statement, $debug)
{
  if ($debug == 1)
    echo "<br><br>"
        .$sql_statement
        ."<br><br>";

//  if ($this->res)
//    mysql_free_result ($this->res);

  $this->res = mysql_query ($sql_statement);

  if ($this->res != FALSE)
    return 1;
  return 0;
}


function
sql_close ()
{
  if (!is_null ($this->res))
    { 
      mysql_free_result ($this->res);
      $this->res = NULL;
    }

  if (!is_null ($this->conn))
    {
      mysql_close ($this->conn);
      $this->conn = NULL;
    }

//  $this->row_pos = -1;
}


function
sql_seek ($row)
{
  return mysql_data_seek ($this->res, $row);
}


function
sql_get_result ()
{
  return $this->res;
}

}


?>
