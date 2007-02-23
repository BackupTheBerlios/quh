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

  Flags
    NET_TCP      use TCP/IP (default)
    NET_CLIENT   use for client connections (default)
    NET_SERVER   use as server
//    NET_PROXY    use as proxy
    NET_UDP      use UDP
    NET_DEBUG    print DEBUG output
    NET_SSL      use SSL for connections (if available)
    NET_TLS      same as NET_SSL

  net_open()     open connection to a server (client)
                   (url_s: [login:pw@]server:/path)

  net_bind()     bind server to a port (server)
  net_listen()   wait for connections from clients (server)
  net_accept()   accept an incoming connection

  net_inetd()    use inetd for TCP/IP commuication
                   use this _OR_ net_bind(), net_listen(), and 
                   net_accept()
  Flags
    NET_INETD_EXT  use external inetd (default)
    NET_INETD_INT  use internal inetd

  net_read()
  net_getc()
  net_gets()

  net_write()
  net_putc()
  net_puts()
  net_print()

  net_seek()     for resume

  net_close()    close connection (server/client)

  net_quit()     quit

  net_get_socket()
*/
#define NET_TCP        0
#define NET_CLIENT     0
#define NET_SERVER     (1<<0)
//#define NET_PROXY      (1<<1)
//#define NET_UDP        (1<<2)
//#define NET_DEBUG      (1<<4)
#ifdef  USE_SSL
//#define NET_SSL        (1<<5)
//#define NET_TLS        NET_SSL
#endif


typedef struct
{
  int flags;

#if      (defined USE_THREAD && !defined _WIN32)
  pthread_t tid;
#endif
  int inetd;
  int inetd_flags;

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

extern int net_bind (st_net_t *n, int port);
extern int net_listen (st_net_t *n);
extern st_net_t *net_accept (st_net_t *n);


enum {
  NET_INETD_EXT = 0,
//  NET_INETD_INT
};


extern int net_inetd (st_net_t *n, int flags);

extern int net_read (st_net_t *n, void *buffer, int buffer_len);
extern int net_write (st_net_t *n, void *buffer, int buffer_len);
extern int net_getc (st_net_t *n);
extern int net_putc (st_net_t *n, int c);
extern char *net_gets (st_net_t *n, char *buffer, int buffer_len);
extern int net_puts (st_net_t *n, char *buffer);
//extern int net_print (st_net_t *n, const char *format, ...);

extern int net_seek (st_net_t *n, int pos);
extern int net_sync (st_net_t *n);

extern int net_close (st_net_t *n);

// TODO: remove this?
//extern int net_get_socket (st_net_t *n);


/*
  Miscellaneous

  net_get_port_by_protocol() "http" would return (int) 80
  net_get_protocol_by_port() (int) 80 would return "http"
*/
extern int net_get_port_by_protocol (const char *protocol);
extern const char *net_get_protocol_by_port (int port);
#endif  // (defined USE_TCP || defined USE_UDP)


/*
  HTTP header build/parse functions

  net_build_http_request()  http protocol function
  net_build_http_response() http protocol function

  net_parse_http_request()  http protocol function
  net_parse_http_response() http protocol function

  net_http_get_to_temp()    decide if url_or_fname is a url or fname
                              it will eventually download the file and
                              return the name of a temporary file
                              OR the fname when it was a local file
*/
enum {
  NET_METHOD_GET = 0,
  NET_METHOD_POST
};


typedef struct
{
  char header[NET_MAXBUFSIZE * 16];   // the whole header

  int method;                         // the method
  char method_s[NET_MAXBUFSIZE];      // the method as string "GET", "POST", ...

  char host[NET_MAXBUFSIZE];          // "localhost", ...
  char request[NET_MAXBUFSIZE];

  char user_agent[NET_MAXBUFSIZE];    // or "server:"

  char connection[NET_MAXBUFSIZE];    // "close", "keep-alive"
  int keep_alive;

  int gzip;                           // compression enabled

  char content_type[NET_MAXBUFSIZE];
  int content_length;
} st_http_header_t;


extern char *net_build_http_request (const char *url_s, const char *user_agent, int keep_alive, int method, int gzip);
extern char *net_build_http_response (const char *user_agent, int keep_alive, unsigned int content_len, int gzip);
#if     (defined USE_TCP || defined USE_UDP)
extern st_http_header_t *net_parse_http_request (st_net_t *n);
extern st_http_header_t *net_parse_http_response (st_net_t *n);
extern const char *net_http_get_to_temp (const char *url_s, const char *user_agent, int gzip);
#endif
                                              

/*
  Tag parse functions

  net_tag_filter()          filter (html)tags in a string
                              continuous_flag must be set to 0 when starting at the beginning of
                                a new file with (html)tags; this makes sure that (html)tags which
                                start and end in different lines get parsed correctly when the
                                file is parsed line-wise; set this to 0 if str is the whole
                                file with (html) tags

                                Example (continuous_flag):
                                  st_tag_filter_t f[] = {{"a", a_remove},{NULL,NULL}};
                                  strcpy (buf, "<w><a");
                                  cf = net_tag_filter (buf, f, 1, 0);
                                  printf ("%s", buf);
                                  strcpy (buf, "><w>");
                                  cf = net_tag_filter (buf, f, 1, cf);
                                  printf ("%s", buf);
                                will output "<w><w>"

                            net_tag_filter() returns -1 on ERROR (malloc failed) or the
                              continuous_flag to be used for the next call (see above)

  st_tag_filter_t
    start_tag                name of the tag to filter
                               if the name is "" then this filter will apply for all
                               tags - all following filters in the st_tag_filter_t
                               array will be ignored
    filter                   the actual filter
                               takes the string including '<' and '>'
                               and returns the replacement
                               can be used to pass, remove, and replace
                               (custom) tags

  net_tag_arg()              splits a tag into an argv-like array and returns argc
*/
typedef struct
{
  const char *start_tag;
//  const char *end_tag;
  const char *(* filter) (const char *);
} st_tag_filter_t;
extern unsigned long net_tag_filter (char *str, st_tag_filter_t *f, unsigned long continuous_flag);
extern const char *net_tag_get_name (const char *tag);
extern const char *net_tag_get_value (const char *tag, const char *value_name);
//extern int net_tag_arg (char **argv, char *tag);


/*
  Url parse functions

  stresc()        replace chars with %xx escape sequences
  strunesc()      replace %xx escape sequences with the char
  strurl()        a general routine to parse urls
*/
typedef struct
{
  char url_s[NET_MAXBUFSIZE];

  char protocol[NET_MAXBUFSIZE];
  char user[NET_MAXBUFSIZE];
  char pass[NET_MAXBUFSIZE];
  char host[NET_MAXBUFSIZE];
  int port;
  char request[NET_MAXBUFSIZE];

//  int argc;
//  char *argv[NET_MAXBUFSIZE];

  char priv[NET_MAXBUFSIZE];
} st_strurl_t;


extern char *stresc (char *dest, const char *src);
extern char *strunesc (char *dest, const char *src); 
extern st_strurl_t *strurl (st_strurl_t *url, const char *url_s);


#ifdef  __cplusplus
}
#endif
#endif  // MISC_NET_H
