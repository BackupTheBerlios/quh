<?php
//header ("Content-type: image/png");
/*
draw.php - simple wrapper for drawing
  
Copyright (c) 2007 NoisyB
                            
                            
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
  
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
  
You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


/*
  draw_open()
  draw_close()

  draw_read()           read from display to work surface
  draw_read_from_mem()  read picture from memory to work surface
  draw_read_from_file() read picture from a file to work surface
                              uses cache for speed

  draw_get_w()          return current width of work surface (NOT the selection)
  draw_get_h()          return current height of work surface (NOT the selection)
  draw_get_html_color() return the color of a pixel of the work surface
                          as html string

  draw_ctrl_select()    set selection on work surface 
  draw_ctrl_select_all() select the whole work surface (default)

  draw_ctrl_copy()      copy selection to copy surface with id
  draw_ctrl_paste()     paste copy surface with id to work surface
  draw_ctrl_resize()    resize the selection
  draw_ctrl_canvas()    canvas the work surface
                          work with positive and negative values for x, y, w, h
  draw_ctrl_scale()     scale the selection
  draw_ctrl_flip()      flip selection; Axis: 0 == x, 1 == y, 2 == both
  draw_ctrl_rotate()    rotate selection
  draw_ctrl_blur()      blur selection
  draw_ctrl_invert()    invert colors of selection

  draw_write()          write work surface to display
  draw_write_in_tiles() tiles the picture into 16*16(=256) rectangles
                          the "drawing" is done by the c argument
                          (could be used for fonts)
  draw_write_to_mem()   write work surface to mem
  draw_write_to_file()  write work surface to file (png)
  draw_write_to_SDL()   return work surface as SDL_Surface
  draw_write_pixel()    write pixel to display
  draw_write_rect()     write rectangle to display
  draw_write_box()      write box to display
  draw_write_ellipse()  write filled ellipse to display

  draw_sync()           update screen after one or more draw_write()
*/


class misc_draw
{
var $w = NULL;
var $h = NULL;
var $work = NULL;
var $background = NULL;


function
draw_s_to_color ($html_color)
{
  $col = hexdec (substr ($html_color, 1));

  return imagecolorallocate ($this->work,
                             ($col & 0xff0000) >> 16,
                             ($col & 0xff00) >> 8,
                             ($col & 0xff));
}


function
draw_open ($w, $h)
{
  $this->w = $w;
  $this->h = $h;
//  $this->work = imagecreatetruecolor ($w, $h) or die ("draw_open() failed");
  $this->work = imagecreate ($w, $h) or die ("draw_open() failed");

//  imageantialias ($this->work, 1);
  // black background
  $this->background = $this->draw_s_to_color ("#000000");
}


function
draw_close ()
{
  imagedestroy ($this->work);
}


// read to work surface
/*
function
void draw_read ()
{
}


function
void draw_read_from_mem (const unsigned char *data, int data_len)
{
}
*/


function
draw_read_from_file ($fname)
{
  imagedestroy ($this->work);
  $this->work = imagecreatefrompng ($fname);
}


// work surface stats
function
draw_get_w ()
{
  return imagesx ($this->work);
}


function
draw_get_h ()
{
  return imagesy ($this->work);
}


/*
function
const char *draw_get_html_color (int x, int y)
{
  return imagecolorat ($this->work, $x, $y);
}


// select work area (default: draw_ctrl_select_all())
function
void draw_ctrl_select (int x, int y, int w, int h)
{
}


function
void draw_ctrl_select_all ()
{
}


//function
void draw_ctrl_select_invert ()
{
}


// apply functions to selection
function
void draw_ctrl_copy (int id)
{
}


function
void draw_ctrl_paste (int id, int x, int y)
{
}


function
void draw_ctrl_resize (int w_new, int h_new)
{
}


function
void draw_ctrl_scale (float percent)
{
}


function
void draw_ctrl_blur (int pixels)
{
}


function
void draw_ctrl_flip (int axis)
{
}


function
void draw_ctrl_rotate (float ankle)
{
}


function
void draw_ctrl_invert ()
{
}


function
void draw_ctrl_brightness (float percent_rel)
{
}


function
void draw_ctrl_grayscale ()
{
}


function
void draw_ctrl_canvas (int x_rel, int y_rel, int w_rel, int h_rel)
{
}


function
void draw_ctrl_shift (int pixels_rel)
{
}


function
void draw_ctrl_mode7 (float x, float y, float z)
{
}


function
void draw_ctrl_indexed (int colors)
{
}


function
void draw_ctrl_c64 ()
{
}


// write from work surface to display
function
void draw_write (int x, int y)
{
}


function
void draw_write_in_tiles (int x, int y, const char *tiles)
{
}


function
void draw_write_to_mem (char *data, int *data_len)
{
}


function
void draw_write_to_file (const char *fname)
{
}
*/

// write primitives directly to display
function
draw_write_pixel ($x, $y, $html_color)
{
  imagesetpixel ($this->work, $x, $y, $this->draw_s_to_color ($html_color));
}


function
draw_write_rect ($x, $y, $w, $h, $html_color)
{
  imagerectangle ($this->work, $x, $y, $x + $w, $y + $h, $this->draw_s_to_color ($html_color));
}


function
draw_write_box ($x, $y, $w, $h, $html_color)
{
  imagefilledrectangle ($this->work, $x, $y, $x + $w, $y + $h, $this->draw_s_to_color ($html_color));
}


function
draw_write_ellipse ($x, $y, $w, $h, $html_color)
{
  imageellipse ($this->work, $x, $y, $x + $w, $y + $h, $this->draw_s_to_color ($html_color));
}


// sync display
function
draw_sync ()
{
  imagepng ($this->work);
}


};


?>
