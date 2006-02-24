/*
cdda.c - cdda filter for quh

Copyright (c) 2005 NoisyB (noisyb@gmx.net)


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
#ifdef  HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef  HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "misc/bswap.h"
#include "misc/itypes.h"
#include "misc/file.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "cdda.h"
#ifdef  HAVE_CONFIG_H
#include "config.h"
#endif
#ifdef  HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <ctype.h>
#include <sys/ioctl.h>
//#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef  __linux__
#include <linux/cdrom.h>
#include <linux/iso_fs.h>
#else
#include <sundev/srreg.h>
#endif  // __linux__
//#include "misc/itypes.h"
//#include "misc/file.h"
#include "misc/getopt2.h"
#include "misc/filter.h"
#include "quh_defines.h"
#include "quh.h"
#include "quh_misc.h"
#include "cdda.h"


#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))


#ifndef CD_MINS
#define CD_MINS              74 // max. minutes per CD, not really a limit
#define CD_SECS              60 // seconds per minute
#define CD_FRAMES            75 // frames per second
#endif
#define QUH_DEFAULT_PREGAP 150 
#ifndef CD_FRAMESIZE_RAW
#define CD_FRAMESIZE_RAW 2352
#endif


static int cdda_in = 0;
static unsigned long pos = 0;
static FILE *cdda_out = NULL;


static void
dm_lba_to_msf (int32_t lba, int *m, int *s, int *f)
{
  lba += (lba >= -150) ? 150 : 450150; // ?

  *m = lba / CD_SECS / CD_FRAMES;
  *s = (lba - *m * CD_SECS * CD_FRAMES) / CD_FRAMES;
  *f = (lba - *m * CD_SECS * CD_FRAMES) - *s * CD_FRAMES;
}


int32_t
dm_msf_to_lba (int m, int s, int f)
{
  return m * CD_SECS * CD_FRAMES + s * CD_FRAMES + f;
}


static int
quh_cdda_out_init (st_quh_filter_t *file)
{
  (void) file;

  if (!quh_get_object_s (quh.filter_chain, QUH_OPTION))
    quh_set_object_s (quh.filter_chain, QUH_OPTION, "audiodump.wav");
                
  return 0;
}


static int
quh_cdda_out_open (st_quh_filter_t *file)
{
  (void) file;
  st_wav_header_t wav_header;
    
  if (!(cdda_out = fopen (quh_get_object_s (quh.filter_chain, QUH_OPTION), "wb")))
    return -1;

  // reserve space
  memset (&wav_header, 0, sizeof (st_wav_header_t));
  fwrite (&wav_header, 1, sizeof (st_wav_header_t), cdda_out);

  return 0;
}


static int
quh_cdda_out_write_cue_file (st_quh_filter_t *file)
{
  int t = 0;

  if (!file->indices)
    {
// works only for indices
      return 0;
    }

  for (; t < file->indices; t++)
    {
#if     FILENAME_MAX > MAXBUFSIZE
      char buf[FILENAME_MAX];
#else
      char buf[MAXBUFSIZE];
#endif
      int m = 0, s = 0, f = 0;
//      unsigned long len = (t < file->indices ? file->index_pos[t] : file->raw_size) - file->index_pos[t - 1];
      FILE *cdda_out = NULL;

      // new filename
      sprintf (buf, "%s_%d.cue", file->fname, t + 1);

      if (!(cdda_out = fopen (basename2 (buf), "wb")))
        continue;

//      fprintf (cdda_out, "REM This file was generated, manual corrections might be necessary\r\n");

//      fprintf (cdda_out, "CATALOG \"%s\"\r\n", "0123456789abc");

//      fprintf (cdda_out, "CDTEXTFILE \"%s\"\r\n", "");

      fprintf (cdda_out, "FILE \"%s\" ", file->fname);

      fprintf (cdda_out, "WAVE\r\n");

      // PERFORMER, SONGWRITER and TITLE of CD
//      fprintf (cdda_out, "PERFORMER \"%s\"\r\n", ""); // max. 80 chars
//      fprintf (cdda_out, "SONGWRITER \"%s\"\r\n", ""); // max. 80 chars
//      fprintf (cdda_out, "TITLE \"%s\"\r\n", ""); // max. 80 chars


      fprintf (cdda_out, "  TRACK %02d AUDIO\r\n", t + 1);

//      fprintf (cdda_out, "FLAGS %s %s %s %s\r\n", "DCP", "4CH", "PRE", "SCMS"); // DATA

//      fprintf (cdda_out, "ISRC %s\r\n", "0123456789ab");

      // PERFORMER, SONGWRITER and TITLE of TRACK
//      fprintf (cdda_out, "PERFORMER \"%s\"\r\n", ""); // max. 80 chars
//      fprintf (cdda_out, "SONGWRITER \"%s\"\r\n", ""); // max. 80 chars
//      fprintf (cdda_out, "TITLE \"%s\"\r\n", ""); // max. 80 chars

      dm_lba_to_msf (150, &m, &s, &f);
      fprintf (cdda_out, "    PREGAP %02d:%02d:%02d\r\n", m, s, f);

      fprintf (cdda_out, "    INDEX 00 00:00:00\r\n"); // default

//      dm_lba_to_msf (150, &m, &s, &f);
//      fprintf (cdda_out, "    INDEX 01 %02d:%02d:%02d\r\n", m, s, f);

//      dm_lba_to_msf (150, &m, &s, &f);
//      fprintf (cdda_out, "    POSTGAP %02d:%02d:%02d\r\n", m, s, f);

      fclose (cdda_out);
    }

  return 0;
}


static int
quh_cdda_out_close (st_quh_filter_t *file)
{
  (void) file;
  if (cdda_out)
    {
      unsigned long len = 0;

      // re-write header
      fseek (cdda_out, 0, SEEK_END);
      len = ftell (cdda_out) - sizeof (st_wav_header_t);
      fseek (cdda_out, 0, SEEK_SET);

      quh_wav_write_header (cdda_out, 2, 44100, 2, len);
      quh_cdda_out_write_cue_file (file);
      
      fclose (cdda_out);
    }

  return 0;
}


static int
quh_cdda_out_write (st_quh_filter_t *file)
{
  (void) file;

  if (cdda_out)
    fwrite (quh.buffer, 1, quh.buffer_len, cdda_out);

  return 0;
}


static int
quh_cdda_in_open (st_quh_filter_t * file)
{
  int x = 0;
  char buf[MAXBUFSIZE];
  struct cdrom_tochdr toc;
  struct cdrom_tocentry toce;
  struct cdrom_tocentry toce_leadout;
  struct cdrom_mcn mcn;
  struct cdrom_multisession ms;

  if ((cdda_in = open (file->fname, O_RDONLY)) == -1)
    return -1;

  // reset drive 
#if 0
  if (ioctl (cdda_in, CDROMRESET, NULL) == -1)
    {
      close (cdda_in);
      return -1;
    }

  // drive and disc status
  if (ioctl (cdda_in, CDROM_DRIVE_STATUS, CDSL_CURRENT) == -1)
    {
      close (cdda_in);
      return -1;
    }
#endif
                    
  if (ioctl (cdda_in, CDROM_DISC_STATUS) != CDS_AUDIO)
    {
      close (cdda_in);
      return -1; // no cdda
    }
                    
   // get medium catalog number
  if (ioctl (cdda_in, CDROM_GET_MCN, &mcn) == -1)
    {
      close (cdda_in);
      return -1;
    }

  // get multisession
  ms.addr_format = CDROM_LBA;
  if (ioctl (cdda_in, CDROMMULTISESSION, &ms) == -1)
    {
      close (cdda_in);
      return -1;
    }
                                
  // tochdr
  if (ioctl (cdda_in, CDROMREADTOCHDR, &toc) == -1)
    {
      close (cdda_in);
      return -1;
    }

  toce_leadout.cdte_track  = CDROM_LEADOUT;
  toce_leadout.cdte_format = CDROM_MSF;
  if (ioctl (cdda_in, CDROMREADTOCENTRY, &toce_leadout) == -1)
    {
      close (cdda_in);
      return 0;
    }

  file->indices = MIN (toc.cdth_trk1 - toc.cdth_trk0 + 1, QUH_MAXINDEX);
  for (x = 0; x < file->indices; x++)
    {
      toce.cdte_track  = x + 1;
      toce.cdte_format = CDROM_MSF;
      if (ioctl (cdda_in, CDROMREADTOCENTRY, &toce) == -1)
        {
          close (cdda_in);
          return -1;
        }

#ifdef  DEBUG
      printf("%3d: %02d:%02d:%02d (%06d) 0x%x 0x%x %s%s\n",
	       (int)toce.cdte_track,
	       (int)toce.cdte_addr.msf.minute,
	       (int)toce.cdte_addr.msf.second,
	       (int)toce.cdte_addr.msf.frame,
	       (int)toce.cdte_addr.msf.frame +
	       (int)toce.cdte_addr.msf.second*75 + 
	       (int)toce.cdte_addr.msf.minute*75*60,
	       (int)toce.cdte_ctrl,
	       (int)toce.cdte_adr,
	       (toce.cdte_ctrl & CDROM_DATA_TRACK) ? "data ":"audio",
               CDROM_LEADOUT == x ? " (leadout)" : "");
#endif

      if (!(toce.cdte_ctrl & CDROM_DATA_TRACK))
        {
          file->index_pos[x] = dm_msf_to_lba (toce.cdte_addr.msf.minute,
                                               toce.cdte_addr.msf.second,
                                               toce.cdte_addr.msf.frame) * CD_FRAMESIZE_RAW;
        }
    }

  file->raw_size = (dm_msf_to_lba (toce_leadout.cdte_addr.msf.minute,
                                   toce_leadout.cdte_addr.msf.second,
                                   toce_leadout.cdte_addr.msf.frame) + 150) * CD_FRAMESIZE_RAW;
  file->rate = 44100;
  file->channels = 2;
  file->is_signed = 1;
  file->size = 2;
  file->source = QUH_SOURCE_DEVICE;
  file->seekable = QUH_SEEKABLE;

  *buf = 0;
  sprintf (strchr (buf, 0), "MCN: %s\n", mcn.medium_catalog_number);
//  sprintf (strchr (buf, 0), "ISRC: %s\n", "Unknown");
  sprintf (strchr (buf, 0), "Multisession: %s", ms.xa_flag ? "Yes" : "No");

  quh_set_object_s (quh.filter_chain, QUH_OUTPUT, buf);

//  quh_demux_sanity_check (file);

  return 0;
}


static int
quh_cdda_in_close (st_quh_filter_t * file)
{
  (void) file;

  close (cdda_in);
  
  return 0;
}


static int
quh_cdda_in_demux (st_quh_filter_t * file)
{
  int result = 0;

  // not a device?
  if (file->source != QUH_SOURCE_DEVICE)
    return -1;

  result = quh_cdda_in_open (file);
  if (!result)
    quh_cdda_in_close (file);
                      
  return result;
}


static int
quh_cdda_in_write (st_quh_filter_t * file)
#if 1
{
  (void) file;
  struct cdrom_msf *msf = (struct cdrom_msf *) &quh.buffer;
  int m = 0, s = 0, fr = 0;
  unsigned long real_pos = pos;
  
  if (real_pos > 150)
    real_pos -= 150;

  dm_lba_to_msf (real_pos, &m, &s, &fr);
  msf->cdmsf_min0 = m;
  msf->cdmsf_sec0 = s;
  msf->cdmsf_frame0 = fr;
  dm_lba_to_msf (real_pos + 1, &m, &s, &fr);
  msf->cdmsf_min1 = m;
  msf->cdmsf_sec1 = s;
  msf->cdmsf_frame1 = fr;

  if (ioctl (cdda_in, CDROMREADRAW, quh.buffer) >= 0)
    quh.buffer_len = CD_FRAMESIZE_RAW;
  else return -1; // failed

  pos ++;

  return 0;
}
#else
{
  (void) file;
  int m = 0, s = 0, fr = 0;
  int x = 0;

  for (; x < QUH_MAXBUFSIZE / CD_FRAMESIZE_RAW; x++)
    {
      struct cdrom_msf *msf = (struct cdrom_msf *) &quh.buffer[x * CD_FRAMESIZE_RAW];
        
      dm_lba_to_msf (pos++, &m, &s, &fr);
      msf->cdmsf_min0 = m;
      msf->cdmsf_sec0 = s;
      msf->cdmsf_frame0 = fr;
      dm_lba_to_msf (pos + 1, &m, &s, &fr);
      msf->cdmsf_min1 = m;
      msf->cdmsf_sec1 = s;
      msf->cdmsf_frame1 = fr;

      ioctl (cdda_in, CDROMREADRAW, &quh.buffer[x * CD_FRAMESIZE_RAW]);
//        return -1; // failed
   }

  quh.buffer_len = x * CD_FRAMESIZE_RAW;
  pos ++;

  return 0;
}
#endif


static int
quh_cdda_in_seek (st_quh_filter_t * file)
{
  (void) file;

  pos = MAX ((unsigned long) quh.raw_pos / CD_FRAMESIZE_RAW, 0);

  return 0;
}



const st_filter_t quh_cdda_out =
{
  QUH_CDDA_OUT,
  "cdda write (tracks and cue)",
  NULL,
  0,
  NULL,
  (int (*) (void *)) &quh_cdda_out_open,
  (int (*) (void *)) &quh_cdda_out_close,
  NULL,
  (int (*) (void *)) &quh_cdda_out_write,
  NULL,
  NULL,
  (int (*) (void *)) &quh_cdda_out_init,
  NULL
};


const st_getopt2_t quh_cdda_out_usage =
{
  "cdda", 2, 0, QUH_CDDA,
  "MB", "master new CD-DA with MB size and CUE file (default: 700)",
  (void *) QUH_CDDA_OUT
};


const st_filter_t quh_cdda_in = {
  QUH_CDDA_IN,
  "cdda read",
  NULL,
  0,
  (int (*) (void *)) &quh_cdda_in_demux,
  (int (*) (void *)) &quh_cdda_in_open,
  (int (*) (void *)) &quh_cdda_in_close,
  NULL,
  (int (*) (void *)) &quh_cdda_in_write,
  (int (*) (void *)) &quh_cdda_in_seek,
  NULL,
  NULL,
  NULL
};


const st_getopt2_t quh_cdda_in_usage =
{
  "cdda (linux)", 2, 0, QUH_CDDA,
  "DEVICE", "read direct from CD-DA in drive with DEVICE", (void *) QUH_CDDA_IN
};


#if 0
static SDL_CD *fd = NULL;
static unsigned long pos = 0;


static int
cddasdl_open (st_quh_filter_t * file)
{
  int x = 0, t = 0, n = 0;
  int m = 0, s = 0, fr = 0;
  static char buf[MAXBUFSIZE];
  struct stat fstate;
  uint32_t flags = SDL_WasInit (SDL_INIT_EVERYTHING);
  int drive = 0;
  
  // a device?
  if (file->fname[0])
    if (!stat (file->fname, &fstate))
      if (!S_ISBLK (fstate.st_mode))
        return -1;

  if (!(flags & SDL_INIT_CDROM))
#if 1
    if (SDL_Init (SDL_INIT_CDROM) == -1)
#else
    if (SDL_InitSubSystem (SDL_INIT_CDROM) == -1)
#endif
      return -1;

  // no drives
  if (!(drive = SDL_CDNumDrives ()))
    return -1;

  while (drive)
    {
      if (!strcmp (file->fname, SDL_CDName (drive - 1)))
        {
          fd = SDL_CDOpen (drive - 1);
          break;
        }
      drive--;
    }

  if (!fd)
    return -1;

  // is a cd present?
  if (!(CD_INDRIVE (SDL_CDStatus (fd))))
    {
      SDL_CDClose (fd);
      return -1;
    }

  file->indices = fd->numtracks;
  for (x = 0; x < file->indices; x++)
    {
      dm_lba_to_msf (fd->track[x].length, &m, &s, &fr);

      if (x == 0)
        {
          int m2 = 0, s2 = 0, fr2 = 0;
          dm_lba_to_msf (fd->track[fd->numtracks].length, &m2, &s2, &fr2);
          t = m2 * 60 + s2 - m * 60 - s;
        }

      if (fd->track[x].type == SDL_AUDIO_TRACK)
        {
          file->index_pos[x] = fd->track[x].length - (150 * CD_FRAMESIZE_RAW);
          file->index_name[x] = "CD-DA Track";
        }
    }

  quh.raw_pos = 0;

  file->raw_size = fd->track[fd->numtracks].offset + fd->track[fd->numtracks].length - (150 * CD_FRAMESIZE_RAW);
  file->rate = 44100;
  file->channels = 2;
  file->is_signed = TRUE;
  file->size = 2;
  file->source = QUH_SOURCE_DEVICE;
  file->seekable = QUH_SEEKABLE;

  *buf = 0;
  sprintf (strchr (buf, 0), "CDDB Disc Id: 0x%08x", (n % 0xff) << 24 | t << 8 | file->indices);
//  sprintf (strchr (buf, 0), "      MCN: %s\n", mcn.medium_catalog_number);
//  sprintf (strchr (buf, 0), "      ISRC: %s\n", "Unknown");
//  sprintf(strchr (buf, 0), "      Multisession: %s", ms.xa_flag ? "Yes" : "No");
  file->misc = buf;

//  quh_demux_sanity_check (file);

  return 0;
}


static int
cddasdl_close (st_quh_filter_t * file)
{
  (void) file;

  SDL_CDClose (fd);
//  SDL_Quit ();
    
  return 0;
}


static int
cddasdl_read (st_quh_filter_t * file)
{
#if 0
  for (; pos * CD_FRAMESIZE_RAW < quh.len; pos ++)
    {
      struct cdrom_msf msf;
      int m = 0, s = 0, fr = 0;

      dm_lba_to_msf (pos, &m, &s, &fr);
      msf.cdmsf_min0 = m;
      msf.cdmsf_sec0 = s;
      msf.cdmsf_frame0 = fr;
      dm_lba_to_msf (pos + 1, &m, &s, &fr);
      msf.cdmsf_min1 = m;
      msf.cdmsf_sec1 = s;
      msf.cdmsf_frame1 = fr;

      memcpy (quh.buffer, &msf, sizeof (msf));
      if (ioctl (fd, CDROMREADRAW, quh.buffer) >= 0)
        {
          quh.buffer_len = CD_FRAMESIZE_RAW;

          if (write_func (file) == -1)
            return -1;
        }
      else return -1; // failed
    }
#endif
  return 0;
}


static int
cddasdl_seek (st_quh_filter_t * file)
{
#if 0
  pos = MAX (((unsigned long) quh.raw_pos / CD_FRAMESIZE_RAW), 0);
#endif
  return 0;
}
#endif
