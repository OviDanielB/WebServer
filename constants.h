/**
 * Created by laura_trive on 16/03/16.
 *
 * System constants.
*/

#ifndef WEBSERVER_CONSTANTS_H
#define WEBSERVER_CONSTANTS_H

#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <wchar.h>
#include <netinet/in.h>

/*  path of images' directory    */
const char *PATH;
/*  path of cached images' directory    */
const char *CACHE_PATH;
/*  path of log file    */
const char *LOG_PATH;
/*  path of server's database of images */
const char *DB_PATH;
/* size of a line   */
const size_t MAXLINE;
/* default protocol port number */
const in_port_t DEFAULT_PORT;
/* size of request queue        */
const int BACKLOG;
/*  number of child processes of server to manage requests  */
const int CHILDREN_NUM;
/*  successfully HTTP request   */
const char *HTTP_OK;
/*  error in serving HTTP request */
const char *HTTP_BAD_REQUEST;
/*  not found file requested    */
const char *HTTP_NOT_FOUND;
/*  HTML page to show not-found error    */
const char *HTML_404;
/*  HTML page to show bad-request error    */
const char *HTML_400;
/*  version HTTP protocol 1.1   */
const char *HTTP_1;
/*  version HTTP protocol 1.0   */
const char *HTTP_0;
/*  HTTP request User-Agent line    */
const char *USER_AGENT;
/*  HTTP request Accept line    */
const char *ACCEPT;

/*  Check png type  */
//static int png(char *ext);

/*  Check mpeg type */
//static int mpeg(char *ext);

/*  Check gif type  */
//static int gif(char *ext);

/*  Image struct    */
typedef struct img {
    char    name[256];
    size_t  width;
    size_t  height;
    char    type[4];
    size_t  length;
};

/*  Image adaptation    */
typedef struct conv_img {
    char             original_name[256];
    // hash sulla stringa formata da: <nomeoriginale><width><height><q><type> per indicare univocamente l'immagine modificata
    unsigned long    name_code;
    size_t           width;
    size_t           height;
    size_t           quality; // quality factor * 100
    char             type[4];
    size_t           length;    // si può anche togliere
    char             last_modified[50];
};

/*  HTTP request struct    */
typedef struct req {
    char    method[4];
    char    uri[100];
    char    resource[256]; //name of resource read from URI
    char    type[4];
    float   quality;
    char    userAgent[50];
};

#endif //WEBSERVER_CONSTANTS_H
