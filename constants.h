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

/*  home server's path  */
const char *HOME;
/* number of images loaded in the server images' table of database   */
int IMAGESNUM;
/* number of images manipulated loaded in the server cache's table of database   */
int CACHENUM;
/*  path of images' directory    */
const char *PATH;
/*  path of cached images' directory    */
const char *CACHE_PATH;
/*  path of log file    */
const char *LOG_PATH;
/*  path of server's database of images */
const char *DB_PATH;
/*  name of server's database of images */
const char *DB_NAME;
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
/*  home page of server images to show to the client    */
const char *INDEX;
/*  HTTP request for favicon.ico    */
const char *FAVICON;

/*  Image struct    */
typedef struct  img {
    char    name[256];
    size_t  width;
    size_t  height;
    char    type[5];
    size_t  length;
};

/*  Image adaptation    */
typedef struct conv_img {
    char             original_name[256];
    unsigned long    name_code; // result hash function
    size_t           width;
    size_t           height;
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

/* Log file line struct */
typedef struct logline {

    //hostIP, userIdent, reqPersonID, date, reqLine, status, size

    char *ip_host;
    char *user_id;
    char *req_id;
    char *date;
    char *reqline;
    char *status;
    char *size;
};

#endif //WEBSERVER_CONSTANTS_H
