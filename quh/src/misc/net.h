/*
net.h - miscellaneous network functions

Copyright (c) 2006 Dirk
           

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
#ifndef MISC_NET_H
#define MISC_NET_H
#ifdef  __cplusplus
extern "C" {
#endif


#define NET_MAXBUFSIZE 1024


#if     (defined USE_TCP || defined USE_UDP)
/*
  Network functions

  net_init()     open tcp/udp socket
  net_quit()     close tcp/udp socket

  net_open()     open connection to a server (client)
                   (url_s: [login:pw@]server:/path)

  net_bind()     bind server to a port (server)
  net_listen()   wait for connections from clients (server)

  net_close()    close connection (server/client)

  net_read()
  net_gets()
  net_getc()

  net_write()
  net_putc()
  net_puts()

  net_seek()     for resume


  net_get_port_by_protocol() "http" would return (int) 80
  net_get_protocol_by_port() (int) 80 would return "http"

  net_ftp_get()  ftp protocol get wrapper (url_s: login:pw@ftp:/path)
  net_ftp_put()  ftp protocol put wrapper (url_s: login:pw@ftp:/path)

  net_pop_get()  pop protocol get wrapper (url_s: login:pw@pop)
*/
#define NET_TCP        0
#define NET_CLIENT     0
#define NET_SERVER     (1<<0)
//#define NET_PROXY      (1<<1)
//#define NET_UDP        (1<<2)
//#define NET_KEEP_ALIVE (1<<3)
#define NET_DEBUG      (1<<4)
#ifdef  USE_SSL
#define NET_SSL        (1<<5)
#define NET_TLS        NET_SSL
#endif


typedef struct
{
  int flags;

  int sock0;
  int socket;
  int port;

  int bandwidth_u;
  int bandwidth_u_min;
  int bandwidth_u_max;

  int bandwidth_d;
  int bandwidth_d_min;
  int bandwidth_d_max;

  int bytes_u;
  int bytes_d;

  void *o;                      // st_net_obj_t
} st_net_t;


extern st_net_t *net_init (int flags);
extern int net_quit (st_net_t *n);

extern int net_open (st_net_t *n, const char *url_s, int port);
extern int net_close (st_net_t *n);

extern int net_bind (st_net_t *n, int port);
extern int net_listen (st_net_t *n);

extern int net_read (st_net_t *n, void *buffer, int buffer_len);
extern int net_write (st_net_t *n, void *buffer, int buffer_len);
extern int net_getc (st_net_t *n);
extern int net_putc (st_net_t *n, int c);
extern char *net_gets (st_net_t *n, char *buffer, int buffer_len);
extern int net_puts (st_net_t *n, char *buffer);

extern int net_seek (st_net_t *n, int pos);
extern int net_sync (st_net_t *n);

extern int net_get_socket (st_net_t *n);
extern int net_get_port_by_protocol (const char *protocol);
extern const char *net_get_protocol_by_port (int port);

extern int net_ftp_get (st_net_t *n, const char *url_s);
extern int net_ftp_put (st_net_t *n, const char *local_path, const char *url_s);

extern int net_pop_get (st_net_t *n, const char *url_s);
#endif  // (defined USE_TCP || defined USE_UDP)


/*
  net_tag_filter()          strip (html) tags from a string
                              tags is a list a tags separated by spaces
                              ex.: "a br table tr td"...
                              pass == 1  remove all other tags
                              pass == 0  remove specified tags

  net_build_http_request()  http protocol function
  net_build_http_response() http protocol function

  net_parse_http_request()  http protocol function
  net_parse_http_response() http protocol function
*/
typedef struct
{
  char method[NET_MAXBUFSIZE];        // "GET", "POST", ...
  char request[NET_MAXBUFSIZE];
  char host[NET_MAXBUFSIZE];          // "localhost", ...
  char user_agent[NET_MAXBUFSIZE];
  char connection[NET_MAXBUFSIZE];    // "close", "keep-alive"
  int keep_alive;
  char content_type[NET_MAXBUFSIZE];
  int content_length;
} st_http_header_t;


enum {
  NET_METHOD_GET = 0,
  NET_METHOD_POST
};


extern char *net_tag_filter (char *s, const char *tags, int pass);
extern char *net_build_http_request (const char *url_s, const char *user_agent, int keep_alive, int method);
extern char *net_build_http_response (const char *user_agent, int keep_alive);
#if     (defined USE_TCP || defined USE_UDP)
extern st_http_header_t *net_parse_http_request (st_net_t *n);
extern st_http_header_t *net_parse_http_response (st_net_t *n);
#endif
                                              

/*
  Url parse functions

  stresc()        replace chars with %xx escape sequences
  strunesc()      replace %xx escape sequences with the char
  strurl()        a general routine to parse urls
*/
typedef struct
{
  char url_s[NET_MAXBUFSIZE];     // default: "http://localhost:80/"

  char protocol[NET_MAXBUFSIZE];  // default: "http"
  char user[NET_MAXBUFSIZE];      // default: ""
  char pass[NET_MAXBUFSIZE];      // default: ""
  char host[NET_MAXBUFSIZE];      // default: localhost
  int port;                   // default: 80
  char request[NET_MAXBUFSIZE];   // default: "/"
} st_strurl_t;


extern char *stresc (char *dest, const char *src);
extern char *strunesc (char *dest, const char *src); 
extern st_strurl_t *strurl (st_strurl_t *url, const char *url_s);


#ifdef  __cplusplus
}
#endif
#endif  // MISC_NET_H
