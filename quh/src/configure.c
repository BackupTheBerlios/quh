#if 0
#if 0
#if 0 // configure - create config.mak and config.h
#if 0
#if 0 // written by 2005 NoisyB (noisyb@gmx.net)
#if 0
#if 0
cc configure.c -o config.status && ./config.status && exit 0
gcc configure.c -o config.status && ./config.status && exit 0
g++ configure.c -o config.status && ./config.status && exit 0
egcs configure.c -o config.status && ./config.status && exit 0
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#include <stdio.h>      // FILENAME_MAX, FILE
#include <stdlib.h>     // getenv()
#include <string.h>     // strncpy(), strchr()
#include "configure.in" // the "template"


#if     (defined __unix__ && !defined __MSDOS__) || defined __BEOS__ || \
        defined AMIGA || defined __APPLE__      // Mac OS X actually
// GNU/Linux, Solaris, FreeBSD, OpenBSD, Cygwin, BeOS, Amiga, Mac (OS X)
#define FILE_SEPARATOR '/'
#define PATH_SEPARATOR ':'
#else // DJGPP, Win32
#define FILE_SEPARATOR '\\'
#define PATH_SEPARATOR ';'
#endif
#define MAXBUFSIZE 32768
#ifdef  __CYGWIN__
/*
  Weird problem with Cygwin
  When a string with "e (e with diaeresis, one character) is read from an
  environment variable, the character isn't the right character for accessing
  the file system. We fix this.
  TODO: fix the same problem for other non-ASCII characters (> 127).
*/
char *
fix_character_set (char *str)
{
  int n, l = strlen (str);
  unsigned char *ptr = (unsigned char *) str;

  for (n = 0; n < l; n++)
    {
      if (ptr[n] == 0x89)                       // e diaeresis
        ptr[n] = 0xeb;
      else if (ptr[n] == 0x84)                  // a diaeresis
        ptr[n] = 0xe4;
      else if (ptr[n] == 0x8b)                  // i diaeresis
        ptr[n] = 0xef;
      else if (ptr[n] == 0x94)                  // o diaeresis
        ptr[n] = 0xf6;
      else if (ptr[n] == 0x81)                  // u diaeresis
        ptr[n] = 0xfc;
    }

  return str;
}
#endif


static int
find (const char *path)
{
  FILE *fh = NULL;
#if     FILENAME_MAX > MAXBUFSIZE
  char buf[FILENAME_MAX];

  strncpy (buf, path, FILENAME_MAX)[FILENAME_MAX] = 0;
#else
  char buf[MAXBUFSIZE];

  strncpy (buf, path, MAXBUFSIZE)[MAXBUFSIZE] = 0;
#endif

#ifdef  __CYGWIN__
  fix_character_set (buf);
#endif

  printf ("checking for %s... ", buf);

  if ((fh = fopen (buf, "rb")))
    {
      fclose (fh);
      puts ("yes");
      return 0;
    }

  puts ("no");
  return -1;
}


static int
find_lib (const char *libname)
{
  int x = 0;

  while (lib_dirs[x])
    {
      char buf[FILENAME_MAX];

      strcpy (buf, lib_dirs[x]);
      strcpy (strchr (buf, 0), libname);

      if (!find (buf))
        return 0;

      x++;
    }

  return -1;
}


static int
find_header (const char *headername)
{
  int x = 0;

  while (header_dirs[x])
    {
      char buf[FILENAME_MAX];

      strcpy (buf, header_dirs[x]);
      strcpy (strchr (buf, 0), headername);

      if (!find (buf))
        return 0;

      x++;
    }

  return -1;
}


static int
find_file (const char *filename)
{
  const char *p = NULL;
  char *a = NULL, *b = NULL;
#if     FILENAME_MAX > MAXBUFSIZE
  char buf[FILENAME_MAX];
#else
  char buf[MAXBUFSIZE];
#endif
  int x = 0;

  for (; file_dirs[x]; x++)
    {
      strcpy (buf, file_dirs[x]);
      strcpy (strchr (buf, 0), filename);

      if (!find (buf))
        return 0;
    }

  if (!(p = getenv ("PATH")))
    return -1;

#if     FILENAME_MAX > MAXBUFSIZE
  strncpy (buf, p, FILENAME_MAX)[FILENAME_MAX] = 0;
#else
  strncpy (buf, p, MAXBUFSIZE)[MAXBUFSIZE] = 0;
#endif
            
  a = buf;
  while (1)
    {
#if 0
#if     FILENAME_MAX > MAXBUFSIZE
      char path[FILENAME_MAX];

      strncpy (path, a, FILENAME_MAX)[FILENAME_MAX] = 0;
#else
      char path[MAXBUFSIZE];

      strncpy (path, a, MAXBUFSIZE)[MAXBUFSIZE] = 0;
#endif
#else
#if     FILENAME_MAX > MAXBUFSIZE
      char path[FILENAME_MAX];
#else
      char path[MAXBUFSIZE];
#endif
            
      strcpy (path, a);
#endif
      // strip rest including PATH_SEPARATOR
      if ((b = strchr (path, PATH_SEPARATOR)))
        *b = 0;

      // remove FILE_SEPARATOR
      if ((b = strchr (path, 0)))
        if (*(b - 1) == FILE_SEPARATOR)
          *(b - 1) = 0;

      sprintf (strchr (path, 0), "/%s", filename);

      if (!find (path))
        return 0;
 
      // terminate at next PATH_SEPARATOR
      if ((a = strchr (a, PATH_SEPARATOR)))
        a++;
      else
        break;
    }

  return -1;
}


int
main (int argc, char **argv)
{
  int x = 0, y = 0,
    result[MAXBUFSIZE];
  char buf[MAXBUFSIZE];
  FILE *config_mak = NULL;
  FILE *config_h = NULL;
    
  if (!(config_mak = fopen ("config.mak", "wb")))
    return -1;

  if (!(config_h = fopen ("config.h", "wb")))
    {
      fclose (config_mak);
      remove ("config.mak");
      
      return -1;
    }

  for (x = 0; configure_in[x].config_mak ||
              configure_in[x].config_h ||
              configure_in[x].success ||
              configure_in[x].failure; x++)
    {
      st_configure_in_t *c = &configure_in[x];
      result[x] = 1;
      
      if (c->lib[0] || c->header[0] || c->file[0])
        {
          if (c->lib)
            for (y = 0; c->lib[y]; y++)
               if (find_lib (c->lib[y]) == -1)
                 {
                   result[x] = 0;
                   break;
                 }

          if (result[x])
            if (c->header)
              for (y = 0; c->header[y]; y++)
                if (find_header (c->header[y]) == -1)
                  {
                    result[x] = 0;
                    break;
                  }

          if (result[x])
            if (c->file)
              for (y = 0; c->file[y]; y++)
                if (find_file (c->file[y]) == -1)
                  {
                    result[x] = 0;
                    break;
                  }
        }

      if (result[x])
        {
          if (c->config_mak)
            fprintf (config_mak, "%s\n", c->config_mak);
          if (c->config_h)
            fputs (c->config_h, config_h);
        }
    }

  puts ("config.status: creating config.mak\n"
        "config.status: creating config.h\n");

  fclose (config_h);
  fclose (config_mak);
  
  for (x = 0; configure_in[x].config_mak ||
              configure_in[x].config_h ||
              configure_in[x].success ||
              configure_in[x].failure; x++)
    {
      st_configure_in_t *c = &configure_in[x];
            
      if (result[x] && c->success)
        puts (c->success);
      if (!result[x] && c->failure)
        puts (c->failure);
    }

  puts ("");

  return 0;
}