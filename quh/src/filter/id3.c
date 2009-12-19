/*
id3.h - ID3 support for Quh (Linux only)

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
#ifdef  HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef  HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#ifdef  USE_ID3
#include <id3tag.h>
#endif
#include "misc/itypes.h"
#include "misc/getopt2.h"
#include "filter.h"
#include "misc/string.h"
#include "quh_defines.h"
#include "quh_filter.h"
#include "quh.h"
#include "quh_misc.h"
#include "id3.h"


static int
quh_get_id3_tag (st_quh_nfo_t *file, char *buf)
#ifdef  USE_ID3
{
  unsigned int i = 0;
  int first = 1;
  struct id3_file *fh = NULL;
  struct id3_tag *tag = NULL;
  const struct id3_frame *frame = NULL;
  const id3_ucs4_t *ucs4 = NULL;
  id3_latin1_t *latin1 = NULL;
  int title = 0;
  struct
  {
    char const *id;
    char const *name;
  } const info[] = {
    {ID3_FRAME_TITLE,  "Title"},
    {"TIT3",           "Subtitle"},
    {"TCOP",           "Copyright (C)"},
    {"TPRO",           "Produced (P)"},
    {"TCOM",           "Composer"},
    {ID3_FRAME_ARTIST, "Artist"},
    {"TPE2",           "Orchestra"},
    {"TPE3",           "Conductor"},
    {"TEXT",           "Lyricist"},
    {ID3_FRAME_ALBUM,  "Album"},
    {ID3_FRAME_YEAR,   "Year"},
    {ID3_FRAME_TRACK,  "Track"},
    {"TPUB",           "Publisher"},
    {ID3_FRAME_GENRE,  "Genre"},
    {"TRSN",           "Station"},
    {"TENC",           "Encoder"},
    {NULL, NULL}
  };

  if (!(fh = id3_file_open (file->fname, ID3_FILE_MODE_READONLY)))
    return -1;

  tag = id3_file_tag (fh);

  // text information
  *buf = 0;
  for (i = 0; info[i].id; i++)
    {
      const union id3_field *field;
      unsigned int nstrings, j;

      if (!(frame = id3_tag_findframe (tag, info[i].id, 0)))
        continue;

      field = &frame->fields[1];
      nstrings = id3_field_getnstrings (field);

      for (j = 0; j < nstrings; j++)
        {
          if (!(ucs4 = id3_field_getstrings (field, j)))
            continue;

          if (!strcmp (info[i].id, ID3_FRAME_GENRE))
            ucs4 = id3_genre_name (ucs4);

          if (!(latin1 = id3_ucs4_latin1duplicate (ucs4)))
            continue;

          if (!title)
            strcpy (strchr (buf, 0), "Version: 2.x");
          title = 1;

          if (!j)
            {
              if (info[i].name)
                sprintf (strchr (buf, 0), "\n%s: ", info[i].name);

              sprintf (strchr (buf, 0), "%s", latin1);
            }
          else
            sprintf (strchr (buf, 0), ", %s", latin1);

          free (latin1);
        }
    }

  // comments
  i = 0;
  while ((frame = id3_tag_findframe (tag, ID3_FRAME_COMMENT, i++)))
    {
      if (!(ucs4 = id3_field_getstring (&frame->fields[2])))
        continue;

      if (!(ucs4 = id3_field_getfullstring (&frame->fields[3])))
        continue;
              
      if (!(latin1 = id3_ucs4_latin1duplicate (ucs4)))
        continue;

      if (*latin1)
        {
          if (first)
            {
              sprintf (strchr (buf, 0), "\nComment: %s", latin1);
              first = 0;
            }
          else
            sprintf (strchr (buf, 0), " %s", latin1);
        }

      free (latin1);
    }

  id3_file_close (fh);

  quh_set_object_s (quh.filter_chain, QUH_OUTPUT, buf);

  return 0;
}
#else
{
  typedef struct
  {
    char magic[3];
    char title[30];
    char artist[30];
    char album[30];
    char year[4];
    char comment[30];
    unsigned char genre;
  } id3v1_tag_info_t;


  typedef struct
  {
    int id;
    const char *genre;
  } st_genre_t;
  
  
  static const st_genre_t id3v1_genre[] =
  {
    // ID3v1 names (0-79)
    {0,   "Blues"},
    {1,   "Classic Rock"},
    {2,   "Country"},
    {3,   "Dance"},
    {4,   "Disco"},
    {5,   "Funk"},
    {6,   "Grunge"},
    {7,   "Hip-Hop"},
    {8,   "Jazz"},
    {9,   "Metal"},
    {10,  "New Age"},
    {11,  "Oldies"},
    {12,  "Other"},
    {13,  "Pop"},
    {14,  "R&B"},
    {15,  "Rap"},
    {16,  "Reggae"},
    {17,  "Rock"},
    {18,  "Techno"},
    {19,  "Industrial"},
    {20,  "Alternative"},
    {21,  "Ska"},
    {22,  "Death Metal"},
    {23,  "Pranks"},
    {24,  "Soundtrack"},
    {25,  "Euro-Techno"},
    {26,  "Ambient"},
    {27,  "Trip-Hop"},
    {28,  "Vocal"},
    {29,  "Jazz+Funk"},
    {30,  "Fusion"},
    {31,  "Trance"},
    {32,  "Classical"},
    {33,  "Instrumental"},
    {34,  "Acid"},
    {35,  "House"},
    {36,  "Game"},
    {37,  "Sound Clip"},
    {38,  "Gospel"},
    {39,  "Noise"},
    {40,  "Alternative Rock"},
    {41,  "Bass"},
    {43,  "Punk"},
    {44,  "Space"},
    {45,  "Meditative"},
    {46,  "Instrumental Pop"},
    {47,  "Instrumental Rock"},
    {48,  "Ethnic"},
    {49,  "Gothic"},
    {50,  "Darkwave"},
    {51,  "Techno-Industrial"},
    {52,  "Electronic"},
    {53,  "Pop-Folk"},
    {54,  "Eurodance"},
    {55,  "Dream"},
    {56,  "Southern Rock"},
    {57,  "Comedy"},
    {58,  "Cult"},
    {59,  "Gangsta"},
    {60,  "Top 40"},
    {61,  "Christian Rap"},
    {62,  "Pop/Funk"},
    {63,  "Jungle"},
    {64,  "Native US"},
    {65,  "Cabaret"},
    {66,  "New Wave"},
    {67,  "Psychadelic"},
    {68,  "Rave"},
    {69,  "Showtunes"},
    {70,  "Trailer"},
    {71,  "Lo-Fi"},
    {72,  "Tribal"},
    {73,  "Acid Punk"},
    {74,  "Acid Jazz"},
    {75,  "Polka"},
    {76,  "Retro"},
    {77,  "Musical"},
    {78,  "Rock & Roll"},
    {79,  "Hard Rock"},
    // Winamp extensions (80-147)
    {80,  "Folk"},
    {81,  "Folk-Rock"},
    {82,  "National Folk"},
    {83,  "Swing"},
    {84,  "Fast Fusion"},
    {85,  "Bebob"},
    {86,  "Latin"},
    {87,  "Revival"},
    {88,  "Celtic"},
    {89,  "Bluegrass"},
    {90,  "Avantgarde"},
    {91,  "Gothic Rock"},
    {92,  "Progressive Rock"},
    {93,  "Psychedelic Rock"},
    {94,  "Symphonic Rock"},
    {95,  "Slow Rock"},
    {96,  "Big Band"},
    {97,  "Chorus"},
    {98,  "Easy Listening"},
    {99,  "Acoustic"},
    {100, "Humour"},
    {101, "Speech"},
    {102, "Chanson"},
    {103, "Opera"},
    {104, "Chamber Music"},
    {105, "Sonata"},
    {106, "Symphony"},
    {107, "Booty Bass"},
    {108, "Primus"},
    {109, "Porn Groove"},
    {110, "Satire"},
    {111, "Slow Jam"},
    {112, "Club"},
    {113, "Tango"},
    {114, "Samba"},
    {115, "Folklore"},
    {116, "Ballad"},
    {117, "Power Ballad"},
    {118, "Rhytmic Soul"},
    {119, "Freestyle"},
    {120, "Duet"},
    {121, "Punk Rock"},
    {122, "Drum Solo"},
    {123, "Acapella"},
    {124, "Euro-House"},
    {125, "Dance Hall"},
    {126, "Goa"},
    {127, "Drum & Bass"},
    {128, "Club-House"},
    {129, "Hardcore"},
    {130, "Terror"},
    {131, "Indie"},
    {132, "BritPop"},
    {133, "Negerpunk"},
    {134, "Polsk Punk"},
    {135, "Beat"},
    {136, "Christian Gangsta"},
    {137, "Heavy Metal"},
    {138, "Black Metal"},
    {139, "Crossover"},
    {140, "Contemporary C"},
    {141, "Christian Rock"},
    {142, "Merengue"},
    {143, "Salsa"},
    {144, "Thrash Metal"},
    {145, "Anime"},
    {146, "JPop"},
    {147, "SynthPop"},
    {0,   NULL},
  };
  FILE *fh = NULL;
  id3v1_tag_info_t id3v1;
  char *p = NULL;
  int x = 0;
    
  if (!(fh = fopen (file->fname, "rb")))
    return -1;
                         
  fseek (fh, -sizeof (id3v1_tag_info_t), SEEK_END);
  fread (&id3v1, 1, sizeof (id3v1_tag_info_t), fh);
  fclose (fh);

  if (id3v1.magic[0] != 'T' ||
      id3v1.magic[1] != 'A' ||
      id3v1.magic[2] != 'G')
    return -1;

  id3v1.title[30] =
  id3v1.artist[30] =
  id3v1.album[30] =
  id3v1.year[4] =
  id3v1.comment[30] = 0;

  strcpy (buf, "Version: 1.x");
  if (*id3v1.title)
    sprintf (strchr (buf, 0), "\nTitle: %s", id3v1.title);
  if (*id3v1.artist)
    sprintf (strchr (buf, 0), "\nArtist: %s", id3v1.artist);
  if (*id3v1.album)
    sprintf (strchr (buf, 0), "\nAlbum: %s", id3v1.album);
  if (*id3v1.year)
    sprintf (strchr (buf, 0), "\nYear: %s", id3v1.year);
  for (x = 0; id3v1_genre[x].genre; x++)
    if (id3v1_genre[x].id == id3v1.genre)
      {
        sprintf (strchr (buf, 0), "\nGenre: %s", id3v1_genre[x].genre);
        break;
      }

  p = strtriml (strtrimr ((char *) id3v1.comment));
  if (*p)
    {
      for (x = 0; p[x]; x++)
        p[x] = isprint (p[x]) ? p[x] : ' ';
      sprintf (strchr (buf, 0), "\nComment: %-30.30s", p);
    }

  return 0;
}
#endif


static int
quh_id3_in_open (st_quh_nfo_t *file)
{
  char buf[MAXBUFSIZE];
  
  if (file->source != QUH_SOURCE_FILE)
    return 0;

  *buf = 0;
  if (quh_get_id3_tag (file, buf) == -1)
    *buf = 0;

  quh_set_object_s (quh.filter_chain, QUH_OUTPUT, *buf ? buf : "Failed: no ID3 tag found");
      
  return 0;
}


QUH_FILTER_FUNC_STUB (quh_id3_in_write)
QUH_FILTER_FUNC_STUB (quh_id3_in_seek)
QUH_FILTER_FUNC_STUB (quh_id3_in_ctrl)
QUH_FILTER_FUNC_STUB (quh_id3_in_demux)
QUH_FILTER_FUNC_STUB (quh_id3_in_close)



QUH_FILTER_IN(quh_id3_in, QUH_ID3_IN, "id3", NULL)
#if 0
const st_filter_t quh_id3_in =
{
  QUH_ID3_IN,
  "id3",
  NULL,
  0,
  NULL,
  (int (*) (void *)) &quh_id3_in_open,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};
#endif


const st_getopt2_t quh_id3_in_usage =
{
    "id3", 0, 0, QUH_ID3,
    NULL, "read and show ID3 tag"
};
