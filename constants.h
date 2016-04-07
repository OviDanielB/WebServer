/**
 * Created by laura_trive on 16/03/16.
 *
 * System constants.
*/

#ifndef WEBSERVER_CONSTANTS_H
#define WEBSERVER_CONSTANTS_H

/*  path of images' directory    */
char *PATH;
/*  path of cached images' directory    */
char *CACHE_PATH;
/*  path of log file    */
char *LOG_PATH;
/* size of a line   */
#define MAXLINE
/* default protocol port number */
#define DEFAULT_PORT
/* size of request queue        */
#define BACKLOG
/*  number of child processes of server to manage requests  */
int CHILDREN_NUM;
/*  successfully HTTP request   */
char *HTTP_OK;
/*  error in serving HTTP request */
char *HTTP_BAD_REQUEST;
/*  not found file requested    */
char *HTTP_NOT_FOUND;
/*  version HTTP protocol 1.1   */
char *HTTP_1;
/*  version HTTP protocol 1.0   */
char *HTTP_0;
/*  HTTP request User-Agent line    */
char *USER_AGENT;
/*  HTTP request Accept line    */
char *ACCEPT;


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
