/**
* Created by laura_trive on 21/03/16.
*
* Implementation of system structs and definition of system constant.
*/

#include <wchar.h>
#include <netinet/in.h>

/*  path of images' directory    */
const char *PATH =               "/home/laura_trive/Scrivania/ServerFiles/";
/*  path of cached images' directory    */
const char *CACHE_PATH =         "/home/laura_trive/Scrivania/ServerFiles/Cache/";
/*  path of log file    */
const char *LOG_PATH =           "/home/laura_trive/Scrivania/ServerFiles/Log/log.txt";
/* default protocol port number */
const in_port_t DEFAULT_PORT =          5193;
/* size of request queue        */
const int BACKLOG =               10;
/*  line length */
const size_t MAXLINE =            1024;
/*  number of child processes of server to manage requests  */
const int CHILDREN_NUM =          2;
/*  successfully HTTP request   */
const char *HTTP_OK =          "HTTP/1.1 200 OK";
/*  error in serving HTTP request */
const char *HTTP_BAD_REQUEST = "HTTP/1.1 400 Bad Request";
/*  not found file requested    */
const char *HTTP_NOT_FOUND =   "HTTP/1.1 404 Not Found";
/*  version HTTP protocol 1.1   */
const char *HTTP_1 =           "HTTP/1.1";
/*  version HTTP protocol 1.0   */
const char *HTTP_0 =           "HTTP/1.0";
/*  HTTP request User-Agent line    */
const char *USER_AGENT =       "User-Agent:";
/*  HTTP request Accept line    */
const char *ACCEPT =           "Accept:";

/*  logging file    */
//FILE *log = fopen(LOG_PATH,"a");

/*  Image struct    */
/*typedef struct img {
    char    name[50];
    float   quality;
    int     width;
    int     height;
    char    type[4];
    char    last_modified[50];
    long    file_length;
};

/*  Image adaptation    */
/*typedef struct adapt {
    int width;
    int heigth;
    int colors;
};*/

/*  HTTP request struct    */
/*typedef struct req {
    char    method[4];
    char    url[100];
    char    userAgent[50];
    char    type[4];
    float   quality;
};*/