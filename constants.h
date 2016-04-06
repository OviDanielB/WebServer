/**
 * Created by laura_trive on 16/03/16.
 *
 * System constants.
*/

#ifndef WEBSERVER_CONSTANTS_H
#define WEBSERVER_CONSTANTS_H

/*  path of images' directory    */
#define PATH
/*  path of cached images' directory    */
#define CACHE_PATH
/*  path of log file    */
#define LOG_PATH
/* size of a line   */
#define MAXLINE
/* default protocol port number */
#define DEFAULT_PORT
/* size of request queue        */
#define BACKLOG
/*  number of child processes of server to manage requests  */
#define CHILDREN_NUM
/*  successfully HTTP request   */
#define HTTP_OK
/*  error in serving HTTP request */
#define HTTP_BAD_REQUEST
/*  not found file requested    */
#define HTTP_NOT_FOUND


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

#endif //WEBSERVER_CONSTANTS_H
