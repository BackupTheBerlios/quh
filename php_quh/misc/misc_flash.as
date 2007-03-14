/*
misc_flash.as - miscellaneous actionscript functions

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


class misc_flash
{

static function
main ()
{
// TODO: cache
  var s = new Sound ();
//  s.onLoad = function (success)
  s.onLoadComplete = function ()
    {
//      if (success)
        s.start (parseInt (_root.start), 1);

//      s.getBytesLoaded ()
//      s.getBytesTotal ()
    };

//  s.onSoundComplete = function ()
//    {
//    }

//  if (_root.volume)
//    s.setVolume (_root.volume);

  s.loadSound (_root.url, parseInt (_root.stream) == 1 ? true : false);

  if (_root.next)
    getURL (_root.next, "_top", "GET");
}


/*


function
test ()
{
  getURL ('javascript:test("test")');
  getURL (url, target, method);
  LoadVars.send (url, target, method);
}


function
test2 ()
{
  var doc = new XML ();
  doc.onload = function ()
    {
      _root.createTextField("test", 1, 0, 0, 200, 200);
      _root.test.text = this.firstChild.firstChild.nodevalue;
    }
  doc.load ("http://rss.slashdot.org/Slashdot/slashdot");
}


*/
}
