/**
* Created by laura_trive on 21/03/16.
*
* Implementation of system structs and definition of system constant.
*/

/*  path of images' directory    */
#define PATH                "/home/laura_trive/Scrivania/ServerFiles/"
/*  path of cached images' directory    */
#define CACHE_PATH          "/home/laura_trive/Scrivania/ServerFiles/Cache/"
/*  path of log file    */
#define LOG_PATH            "/home/laura_trive/Scrivania/ServerFiles/Log/log.txt"
/*  line length */
#define MAXLINE             1024
/*  successfully HTTP request   */
#define HTTP_OK             "HTTP/1.1 200 OK"
/*  error in serving HTTP request */
#define HTTP_BAD_REQUEST    "HTTP/1.1 400 Bad Request"
/*  not found file requested    */
#define HTTP_NOT_FOUND      "HTTP/1.1 404 Not Found"


/*  logging file    */
FILE *log = fopen(LOG_PATH,"a");

/*  Image struct    */
typedef struct img {
    char    name[50];
    float   quality;
    int     width;
    int     height;
    char    type[4];
    char    last_modified[50];
    long    file_length;
};