/*
net.c - miscellaneous network functions

Copyright (c) 2003 Dirk (d_i_r_k_@gmx.net)
           

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#if     (defined USE_TCP || defined USE_UDP)
#ifdef  _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/file.h>
#include <sys/socket.h>
#endif
#endif
#ifdef  USE_SSL
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif  // USE_SSL
#include "misc.h"
#include "net.h"


#define strnicmp strncasecmp
#define stricmp strcasecmp


#ifdef  MAXBUFSIZE
#undef  MAXBUFSIZE
#endif
#define MAXBUFSIZE 32768

#define MIN(a,b) ((a)<(b)?(a):(b))

#define NET_MAX_CONNECTIONS 1
#ifdef  USE_SSL
#define NET_DEFAULT_HTTPS_PORT 443
#define NET_DEFAULT_CERTFILE "libnetgui.pem"
#endif  // USE_SSL


static int debug = 0;


#if     (defined USE_TCP || defined USE_UDP)
typedef struct
{
#ifdef  USE_SSL
  char *certfile;
  char *cipher;
  SSL_CTX *ssl_ctx;
  SSL *ssl;
  int port;
#endif  // USE_SSL
} st_net_obj_t;


st_net_t *
net_init (int flags)
{
  st_net_t *n = NULL;

  if (!(n = malloc (sizeof (st_net_t))))
    return NULL;

  memset (n, 0, sizeof (st_net_t));

  n->flags = flags;

  if (n->flags & NET_DEBUG)
    debug = 1;

#ifdef  USE_SSL
  if (n->flags & NET_SSL)
    {
      st_net_obj_t *p = NULL;
        
      if (!(n->o = (st_net_obj_t *) malloc (sizeof (st_net_obj_t))))
        {
          free (n);
          return NULL;
        }

      memset (n->o, 0, sizeof (st_net_obj_t));

      p = (st_net_obj_t *) n->o;
      
      p->certfile = NET_DEFAULT_CERTFILE;
      p->port = NET_DEFAULT_HTTPS_PORT;

      SSL_load_error_strings ();
      SSLeay_add_ssl_algorithms ();

      p->ssl_ctx = SSL_CTX_new (SSLv23_server_method ());

      if (p->certfile[0])
        if (!SSL_CTX_use_certificate_file (p->ssl_ctx, p->certfile, SSL_FILETYPE_PEM) ||
            !SSL_CTX_use_PrivateKey_file (p->ssl_ctx, p->certfile, SSL_FILETYPE_PEM) ||
            !SSL_CTX_check_private_key (p->ssl_ctx))
          {
            free (n);
            return NULL;
          }

      if (p->cipher)
        if (!SSL_CTX_set_cipher_list (p->ssl_ctx, p->cipher))
          return NULL;
    }
#endif  // USE_SSL
  
  return n;
}


int
net_quit (st_net_t *n)
{
  if (n->o)
    {
#ifdef  USE_SSL
      if (n->flags & NET_SSL)
        {
          st_net_obj_t *p = (st_net_obj_t *) n->o;
          SSL_free ((SSL *) p->ssl);
        }          
#endif  // USE_SSL
      free (n->o);
    }

  if (n->flags & NET_SERVER)
    if (n->socket)
      {
        shutdown (n->socket, 2);
        wait2 (100);
        close (n->socket);
      }

  free (n);
 
  return 0;
}


int
net_open (st_net_t *n, const char *url_s, int port)
{
  st_strurl_t url;
  struct hostent *host;
  struct sockaddr_in addr;
    
  if (!strurl (&url, url_s)) // parse URL
    return -1;
                    
  if (!(host = gethostbyname (url.host)))
    return -1;
                    
  if (port < 1)
    { 
      if (!stricmp (url.protocol, "http") || url.port == 80)
        port = 80;
      else if (!stricmp (url.protocol, "ftp") || url.port == 21)
        port = 21;
      else
        port = url.port;
    }

  n->socket = socket (AF_INET, SOCK_STREAM, 0);
  if (n->socket == -1)
    return -1;

  memset (&addr, 0, sizeof (struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_addr = *((struct in_addr *) host->h_addr);
  addr.sin_port = htons (net_get_port_by_protocol (url.protocol));

  if (connect (n->socket, (struct sockaddr *) &addr, sizeof (struct sockaddr)) == -1)
    return -1;

  return 0;
}


int
net_bind (st_net_t *n, int port)
{
  struct sockaddr_in addr;

  n->sock0 = socket (AF_INET, SOCK_STREAM, 0);
  if (n->sock0 == -1)
    return -1; 

  memset (&addr, 0, sizeof (struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl (INADDR_ANY);
//#ifdef  USE_SSL
#if 0
  if (n->flags & NET_SSL)
    {
      st_net_obj_t *p = (st_net_obj_t *) n->o;

      addr.sin_port = htons (p->port);
    }
  else
#endif  // USE_SSL
  addr.sin_port = htons (port);

  if (bind (n->sock0, (struct sockaddr *) &addr, sizeof (struct sockaddr)) < 0)
    {
      fprintf (stderr, "ERROR: bind()\n");
      fflush (stderr);
            
      return -1;
    }

  return 0;
}


int
net_listen (st_net_t *n)
{
  // wait for client connections
  if (listen (n->sock0, 5) < 0)
    {
      fprintf  (stderr, "ERROR: listen()\n");
      fflush (stderr);

      return -1;
    }

  // accept waits and "dupes" the socket
  if ((n->socket = accept (n->sock0, 0, 0)) < 0)
    {
      fprintf  (stderr, "ERROR: accept()\n");
      fflush (stderr);

      return -1;
    }

#ifdef  USE_SSL
  if (n->flags & NET_SSL)
    {
      st_net_obj_t *p = (st_net_obj_t *) n->o;

      p->ssl = SSL_new (p->ssl_ctx);
      SSL_set_fd (p->ssl, n->socket);
      if (!SSL_accept (p->ssl))
        {
          fprintf  (stderr, "ERROR: SSL_accept()\n");
          fflush (stderr);

          return -1;
        }
    }
#endif  // USE_SSL

  // TODO: fork()
          
  return 0;
}


int
net_close (st_net_t *n)
{
  return close (n->socket);
}


int
net_read (st_net_t *n, void *buffer, int buffer_len)
{
#ifdef  USE_SSL
  if (n->flags & NET_SSL)
    {
      st_net_obj_t *p = (st_net_obj_t *) n->o;
      return SSL_read (p->ssl, buffer, buffer_len);
    }
  else
#endif  // USE_SSL
#ifdef  _WIN32
  return recv (n->socket, buffer, buffer_len, 0);
#else
  return read (n->socket, buffer, buffer_len);
#endif
}


int
net_write (st_net_t *n, void *buffer, int buffer_len)
{
#ifdef  USE_SSL
  if (n->flags & NET_SSL)
    {
      st_net_obj_t *p = (st_net_obj_t *) n->o;
      return SSL_write (p->ssl, buffer, buffer_len);
    }
  else
#endif  // USE_SSL
#ifdef  _WIN32
  return send (n->socket, buffer, buffer_len);
#else
  return write (n->socket, buffer, buffer_len);
#endif
}


int
net_getc (st_net_t *n)
{
  char buf[2];

#ifdef  USE_SSL
  if (n->flags & NET_SSL)
    {
      st_net_obj_t *p = (st_net_obj_t *) n->o;
      if (SSL_read (p->ssl, (void *) buf, 1) == 1)
        return *buf;
      else
        return -1;
    }
  else
#endif  // USE_SSL
#ifdef  _WIN32
  if (recv (n->socket, (void *) buf, 1) == 1)
#else
  if (read (n->socket, (void *) buf, 1) == 1)
#endif
    return *buf;
  else
    return -1;
}


int
net_putc (st_net_t *n, int c)
{
  unsigned char buf[2];

  *buf = (unsigned char) c & 0xff;

#ifdef  USE_SSL
  if (n->flags & NET_SSL)
    {
      st_net_obj_t *p = (st_net_obj_t *) n->o;
      if (SSL_write (p->ssl, (void *) buf, 1) == 1)
        return *buf;
      else
        return EOF;
    }
  else
#endif  // USE_SSL
#ifdef  _WIN32
  if (send (n->socket, (void *) buf, 1) == 1)
#else
  if (write (n->socket, (void *) buf, 1) == 1)
#endif
    return *buf;
  else
    return EOF;
}


char *
net_gets (st_net_t *n, char *buffer, int buffer_len)
{
  int c = 0, count = 0;
  char *dst = buffer;

  while (count < buffer_len)
    {
      c = net_getc (n);

      if (c < 1)
        return NULL;

      if (c == '\n')
        {
          *dst = 0;

          if (n->flags & NET_DEBUG)
            printf ("%s\n", buffer);

          return buffer;
        }

      if (c == '\r')
        continue;
      else
        {
          *dst++ = c;
          count++;
        }
    }
  *dst = 0;

  if (n->flags & NET_DEBUG)
    printf ("%s\n", buffer);
  
  return buffer;
}


int
net_puts (st_net_t *n, char *buffer)
{
#ifdef  USE_SSL
  if (n->flags & NET_SSL)
    {
      st_net_obj_t *p = (st_net_obj_t *) n->o;
      return SSL_write (p->ssl, buffer, strlen (buffer));
    }
  else
#endif  // USE_SSL
#ifdef  _WIN32
  return send (n->socket, buffer, strlen (buffer));
#else
  return write (n->socket, buffer, strlen (buffer));
#endif
}


int
net_seek (st_net_t *n, int pos)
{
  (void) n;
  (void) pos;

  return 0;
}


int
net_sync (st_net_t *n)
{
  return fsync (n->socket);
}


int net_get_socket (st_net_t *n)
{
  return n->socket;
}


int
net_get_port_by_protocol (const char *protocol)
{
  const struct servent *s = getservbyname (protocol, "tcp");
  return s ? (s->s_port >> 8) : 0; // (-1) ?
}


const char *
net_get_protocol_by_port (int port)
{
  const struct servent *s = getservbyport ((port << 8), "tcp");
  return s ? s->s_name : NULL;
}


#if 0
static int
net_ftp_cmd (st_net_t *n, const char *cmd, int result)
{
  char buf[MAXBUFSIZE];

  sprintf (buf, "%s\r\n", cmd);
  
  net_puts (n, buf);
  net_gets (n, buf, MAXBUFSIZE);
      
  if (buf[3] == '-')
    {
      char res[10];

      strncpy (res, buf, 4);
      *(strchr (res, '-')) = 0;

      if (strtol (res, NULL, 10) == result)
        return 0;
    }
  return -1;
}
#endif


int
net_ftp_get (st_net_t *n, const char *url_s)
{
  (void) n;
  (void) url_s;
#if 0
  char buf[MAXBUFSIZE];
  int x = 0, port = 0, sock1 = 0;
  struct sockaddr_in addr;
  st_strurl_t url;
  
  if (!strurl (&url, url_s))
    return -1;

  // connect
  net_gets (n, buf, MAXBUFSIZE);

  sprintf (buf, "USER %s", url.user);
  if (net_ftp_cmd (n, buf, 331))
    return -1;

  sprintf (buf, "PASS %s@", url.pass);
  if (net_ftp_cmd (n, buf, 230))
    return -1;

  strcpy (buf, "TYPE I");
  if (net_ftp_cmd (n, buf, 200))
    return -1;

#if 0
  strcpy (buf, "LIST");
  if (net_ftp_cmd (n, buf, 0))
    return -1;

  sprintf (buf, "CWD %s", dirname2 (url.request));
  if (net_ftp_cmd (n, buf, 250))
    return -1;
#endif

  // get interface address
  if (getsockname (n->socket, (struct sockaddr *) &addr, &sizeof (addr)) < 0)
    return -1;

  // open data socket
  if ((sock1 = socket (PF_INET, SOCK_STREAM, 0)) < 0)
    return -1;

  addr.sin_family = AF_INET;
  // get the first free port
  for (x = 0; x < 0xC000; x++)
    {
      addr.sin_port = htons (0x4000 + x);
      if (bind (sock1, (struct sockaddr *) &addr, sizeof (struct sockaddr)) >= 0)
        break;
    }
  port = 0x4000 + x;

  if (listen (sock1, 1) < 0)
    return -1;

  x = ntohl (addr.sin_addr.s_addr);
  sprintf (buf, "PORT %d,%d,%d,%d,%d,%d",
           (x >> 24) & 0xff,
           (x >> 16) & 0xff,
           (x >> 8) & 0xff, 
           x & 0xff, 
           (port >> 8) & 0xff,
           port & 0xff);
  if (net_ftp_cmd (n, buf, 200))
    return -1;

  sprintf (buf, "RETR %s", basename2 (url.request));
  if (net_ftp_cmd (n, buf, 150))
    return -1;

  sock1 = accept (sock1, 0, 0);
#endif
  return 0;
}


int
net_ftp_put (st_net_t *n, const char *local_path, const char *url_s)
{
  (void) n;
  (void) local_path;
  (void) url_s;

  return 0;
}


static int
net_pop_cmd (st_net_t *n, const char *cmd)
{
  char buf[MAXBUFSIZE];

  sprintf (buf, "%s\r\n", cmd);

  net_puts (n, buf);
  net_gets (n, buf, MAXBUFSIZE);

  if (!strnicmp (buf, "+OK", 3))  // success
    return 0;
      
  return -1;
}


int
net_pop_get (st_net_t *n, const char *url_s)
{
  char buf[MAXBUFSIZE];
  int mails = 0, x = 0;
  st_strurl_t url;
  FILE *fh = fopen ("Inbox", "wb");
  
  if (!fh)
    return -1;

  if (!strurl (&url, url_s))
    return -1;

  // connect
  net_gets (n, buf, MAXBUFSIZE);
    
  sprintf (buf, "USER %s", url.user);
  if (net_pop_cmd (n, buf))
    return -1;
  
  sprintf (buf, "PASS %s", url.pass);
  if (net_pop_cmd (n, buf))
    return -1;
        
  strcpy (buf, "STAT");
  net_puts (n, buf);
  net_gets (n, buf, MAXBUFSIZE);
  mails = strtol (&buf[4], NULL, 10);

#ifdef  DEBUG
  printf ("%d Mails\n", mails);
  fflush (stdout);
#endif

  fseek (fh, 0, SEEK_END);  // append

  for (x = 0; x < mails; x++)
    {
      sprintf (buf, "RETR %d", x);

      if (!net_pop_cmd (n, buf))
        while ((net_gets (n, buf, MAXBUFSIZE))) 
          {
            if (!strcmp (buf, "."))
              break;

            fprintf (fh, "%s\n", buf); // write to "Inbox"
          }
    }
    
  fclose (fh);

  return 0;
}
#endif  // #if     (defined USE_TCP || defined USE_UDP)


#if 1
char *
net_tag_filter (char *str, const char *tags)
{
  (void) tags;
  int tag = 0;
  char *p = str, *s = str;

  for (; *p; p++)
    switch (*p)
      {
      case '<':
        tag = 1;
        break;

      case '>':
        if (tag)
          {
            tag = 0;
            break;
          }
      default:
        if (!tag)
          {
            *s = *p;
            s++;
          }
        break;
      }
  *s = 0;

  return str;
}
#else 
char *
rmtag (char *str, const char *t)
{
  static char buf[MAXBUFSIZE];
  char *p = str, *s = buf;

  for (; *p; p++)
    switch (*p)
      {
        case '<':
          if (!strncasecmp (p + 1, t, strlen (t)))
            {
              tag = 1;
              *s = '\n';
              s++;
            }
          else
            {
              *s = *p;
              s++;
            }
          break;

        case '>':
          if (tag)
            {
              tag = 0;
              break;
            }

         default:
          if (tag)
            {
              *s = *p;
              s++;
            }
      }

  *s = 0;

  return buf;
}
#endif


char *
net_build_http_request (const char *url_s, const char *user_agent, int keep_alive)
{
  static char buf[MAXBUFSIZE];
  st_strurl_t url;

  if (!strurl (&url, url_s))
    return NULL;
                    
  sprintf (buf, "GET %s HTTP/1.0\r\n"
    "%s\r\n"
    "User-Agent: %s\r\n"
    "Pragma: no-cache\r\n"
    "Host: %s\r\n" 
    "Accept: */*\r\n" // we accept everything
    "\r\n",
    url.request,
    keep_alive ? "Connection: Keep-Alive" : "Connection: close",
    user_agent,
    url.host);

  if (debug)
    printf (buf);
    
  return buf;
}


char *
net_build_http_response (const char *user_agent, int keep_alive)
{
  static char buf[MAXBUFSIZE];

  sprintf (buf,
    "HTTP/1.0 302 Found\r\n"
    "Connection: %s\r\n"
//    "Date: %s\r\n" // "Sat, 20 Sep 2003 12:30:58 GMT"
    "Content-Type: %s\r\n"
    "Server: %s\r\n"
//    "Content-length: %d\r\n"
    "\r\n",
    keep_alive ? "Keep-Alive" : "close",
    "text/html",
    user_agent);

  if (debug)
    printf (buf);
      
  return buf;
}


st_http_header_t *
net_parse_http_request (st_net_t *n)
{
  char buf[MAXBUFSIZE];
  static st_http_header_t h;
  int line = 0;

  while (net_gets (n, buf, MAXBUFSIZE))
    {
#if 0
      if (debug)
        {
          printf ("%s\n", buf);
          fflush (stdout);
        }
#endif

      if (!line)
        {
          strncpy (h.request, strchr (buf, ' ') + 1, NET_MAXBUFSIZE)[NET_MAXBUFSIZE - 1] = 0;
          *strchr (h.request, ' ') = 0;
        }
                            
      if (!(*buf))
        return &h;
        
      line++;
    }

  return NULL;
}


st_http_header_t *
net_parse_http_response (st_net_t *n)
{
  (void) n;
  return NULL;
}


char *
strunesc (char *dest, const char *src)
{
  unsigned int c;
  char *p = dest;

  if (!src)
    return NULL;
  if (!src[0])
    return "";

  while ((c = *src++))
    {
      if (c == '%')
        {
          unsigned char buf[4];

          buf[0] = *src++;
          buf[1] = *src++;
          buf[2] = 0;
        
          sscanf ((const char *) buf, "%x", &c);
        }
      else
        if (c == '+')
          c = ' ';

       *p++ = c;
     }
  *p = 0;
  
  return dest;
}


char *
stresc (char *dest, const char *src)
{
//TODO: what if the src was already escaped?
  unsigned char c;
  char *p = dest;
  const char *positiv =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ" 
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
#if 1
    "+" // "/"
#else
    "-_.!~"                     // mark characters
    "*\\()%"                    // do not touch escape character
    ";/?:@"                     // reserved characters
    "&=+$,"                     // see RFC 2396
//  "\x7f ... \xff"    far east languages(Chinese, Korean, Japanese)
#endif
    ;

  if (!src)
    return NULL;
  if (!src[0])
    return "";
            
  while ((c = *src++))
    if (strchr (positiv, c) != NULL || c >= 0x7f)
      *p++ = c;
    else
      {
        sprintf (p, "%%%02X", c);
        p += 3;
      }
  *p = 0;

  return dest;
}


#ifdef  DEBUG
static void
st_strurl_t_sanity_check (st_strurl_t *url)
{
  printf ("url_s:    %s\n", url->url_s);
  printf ("user:     %s\n", url->user);
  printf ("pass:     %s\n", url->pass);
  printf ("protocol: %s\n", url->protocol);
  printf ("hostname: %s\n", url->host);
  printf ("port:     %d\n", url->port);
  printf ("request:  %s\n", url->request);
        
  fflush (stdout);
}
#endif


st_strurl_t *
strurl (st_strurl_t *url, const char *url_s)
{
  int pos = 0, pos2 = 0;
  char *p = NULL, *p2 = NULL, *p3 = NULL;

#ifdef  DEBUG
  printf ("strurl() url_s: %s\n\n", url_s);
#endif

  if (!url)
    return NULL;
  if (!url_s)
    return NULL;
  if (!url_s[0])
    return NULL;

  memset (url, 0, sizeof (st_strurl_t));
  strcpy (url->url_s, url_s);
  url->port = -1;

  // look for "://"
  if ((p = strstr (url_s, "://")))
    {
      // extract the protocol
      pos = p - url_s;
      strncpy (url->protocol, url_s, MIN (pos, NET_MAXBUFSIZE))[MIN (pos, NET_MAXBUFSIZE - 1)] = 0;

      // jump the "://"
      p += 3;
      pos += 3;
    }
  else
    p = (char *) url_s;

  // check if a user:pass is given
  if ((p2 = strchr (p, '@')))
    {
      int len = p2 - p;
      strncpy (url->user, p, MIN (len, NET_MAXBUFSIZE))[MIN (len, NET_MAXBUFSIZE - 1)] = 0;

      p3 = strchr (p, ':');
      if (p3 != NULL && p3 < p2)
        {
          int len2 = p2 - p3 - 1;

          url->user[p3 - p] = 0;
          strncpy (url->pass, p3 + 1, MIN (len2, NET_MAXBUFSIZE))[MIN (len2, NET_MAXBUFSIZE - 1)] = 0;
        }
      p = p2 + 1;
      pos = p - url_s;
    }

  // look if the port is given
  p2 = strchr (p, ':');                 // If the : is after the first / it isn't the port
  p3 = strchr (p, '/');
  if (p3 && p3 - p2 < 0)
    p2 = NULL;
  if (!p2)
    {
      pos2 =
        (p2 = strchr (p, '/')) ?        // Look if a path is given
        (p2 - url_s) :                  // We have an URL like http://www.hostname.com/file.txt
        (int) strlen (url_s);           // No path/filename
                                        // So we have an URL like http://www.hostname.com
    }
  else
    {
      // We have an URL beginning like http://www.hostname.com:1212
      url->port = atoi (p2 + 1);  // Get the port number
      pos2 = p2 - url_s;
    }

  // copy the hostname into st_strurl_t
  strncpy (url->host, p, MIN (pos2 - pos, NET_MAXBUFSIZE))[MIN (pos2 - pos, NET_MAXBUFSIZE - 1)] = 0;

  // look if a path is given
  if ((p2 = strchr (p, '/')))
    if (strlen (p2) > 1)                // A path/filename is given check if it's not a trailing '/'
      strcpy (url->request, p2);           // copy the path/filename into st_strurl_t

#ifdef  DEBUG
  st_strurl_t_sanity_check (url);
#endif

  return url;
}


#ifdef  TEST
//#if 0
void
test (void)
{
}


int
main (int argc, char ** argv)
{
  char buf[MAXBUFSIZE];
  st_net_t *net = net_init (0);
#if 0
  // client test
  if (!net_open (net, "http://www.google.de", 80))
    {
      char *p = net_build_http_request ("http://www.google.de/index.html", "example", 0);

      net_write  (net, p, strlen (p));

      while (net_gets (net, buf, MAXBUFSIZE))
        printf (buf);
    }
#else
  // server test
  while (1)
    {
      if (!net_bind (net, 80))
        { 
          char *p = net_build_http_response ("example", 0);
          
          net_read (net, buf, MAXBUFSIZE);
          printf (buf);
          
          net_write (net, p, strlen (p));
          net_puts (net, "Hello World!");
          
          break;
        }
      else break;
    }
#endif
  net_close (net);
  net_quit (net);

  return 0;
}
#endif  // TEST
