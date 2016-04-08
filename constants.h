/**
 * Created by laura_trive on 16/03/16.
 *
 * System constants.
*/

#ifndef WEBSERVER_CONSTANTS_H
#define WEBSERVER_CONSTANTS_H

#include <sys/types.h>
#include <netinet/in.h>

/*  path of images' directory    */
const char *PATH;
/*  path of cached images' directory    */
const char *CACHE_PATH;
/*  path of log file    */
const char *LOG_PATH;
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
/*  version HTTP protocol 1.1   */
const char *HTTP_1;
/*  version HTTP protocol 1.0   */
const char *HTTP_0;
/*  HTTP request User-Agent line    */
const char *USER_AGENT;
/*  HTTP request Accept line    */
const char *ACCEPT;


/*  Image struct    */
struct img {
    char    name[50];
    float   quality;
    int     width;
    int     height;
    char    type[4];
    char    last_modified[50];
    long    file_length;
};

/*  Image adaptation    */
typedef struct adapt {
    int width;
    int heigth;
    int colors;
};

/*  HTTP request struct    */
typedef struct req {
    char    method[4];
    char    url[100];
    char    type[4];
    float   quality;
    char    userAgent[50];
};

#endif //WEBSERVER_CONSTANTS_H
