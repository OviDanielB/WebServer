/**
 * Definition of all server constants and structures.
*/

#ifndef WEBSERVER_CONSTANTS_H
#define WEBSERVER_CONSTANTS_H

#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <wchar.h>
#include <netinet/in.h>


/* size of buffer for log line  */
const size_t BUFFER_LOG;
/* number of images loaded in the server database   */
int IMAGESNUM;
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
/* size of database cache table (CONV_IMG) as number of rows   */
const int MAX_CACHE_ROWS_NUM;
/* default protocol port number */
const in_port_t DEFAULT_PORT;
/* size of request queue        */
const int BACKLOG;
/*  number of days after which an image will be deleted from server cache  */
const int TIMEOUT;
/* path of file PHP to execute  */
const char *FIFO_PATH;
/*  path of PHP program */
const char *PHP_PATH;
/*  default number of child processes of server to manage requests  */
const int DEFAULT_CHILDREN;
/*  number of child processes of server to manage requests  */
int CHILDREN_NUM;
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
/*  HTML page to show not supported protocol HTTP 1.0 error    */
const char *HTML_NOT_SUPPORTED;
/*  version HTTP protocol 1.1   */
const char *HTTP_1;
/*  version HTTP protocol 1.0   */
const char *HTTP_0;
/*  HTTP request User-Agent line    */
const char *USER_AGENT;
/*  HTTP request Accept line    */
const char *ACCEPT;
/*  home page of server images to show to the client    */
const char *INDEX;

/*  Image struct    */
typedef struct  img {
    char    name[256];
    size_t  width;
    size_t  height;
    char    type[5];
    size_t  length;
};

/*  Image adapted struct   */
typedef struct conv_img {
    char             original_name[256];
    unsigned long    name_code; // result hash function
    size_t           width;
    size_t           height;
    size_t           colors;
    size_t           quality; // quality factor * 100
    char             type[5];
    size_t           length;
    char             last_modified[21]; // date in SQL format
};

/*  HTTP request struct    */
typedef struct req {
    char    method[5];  // GET or HEAD
    char    resource[256]; // name of resource read from URI
    char    type[5];
    float   quality;
    char    userAgent[1024];
};

/* boolean definition */
typedef int bool;
#define TRUE 1
#define FALSE 0

/*  Device characteristics and capabilities struct */
typedef struct device {
    char id[100];
    size_t width;
    size_t height;
    long colors;
    bool jpg;
    bool png;
    bool gif;
} device ;

/* Log file line struct */
typedef struct log
{
    char ip_host[16];   // IP address of the client
    char date[30];      // current date and time
    char reqLine[300];  // HTTP request line
    char status[50];    // HTTP response status
    size_t size;        // number of bytes sent
};

#endif //WEBSERVER_CONSTANTS_H
