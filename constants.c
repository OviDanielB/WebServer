/**
* Created by laura_trive on 21/03/16.
*
* Definition of system constant.
*/

#include "constants.h"

/*  path of images' directory    */
const char *PATH =               "/home/laura_trive/Scrivania/ServerFiles/";
/*  path of cached images' directory    */
const char *CACHE_PATH =         "/home/laura_trive/Scrivania/ServerFiles/Cache/";
/*  path of server's database of images */
const char *DB_PATH =            "/home/laura_trive/ClionProjects/WebServer/DataBase/serverContent.db";
/*  name of server's database of images */
const char *DB_NAME =            "serverContent";
/*  path of log file    */
const char *LOG_PATH =           "/home/laura_trive/Scrivania/ServerFiles/Log/log.txt";
/* default protocol port number */
const in_port_t DEFAULT_PORT =   5193;
/* size of request queue        */
const int BACKLOG =              10;
/*  line length */
const size_t MAXLINE =           1024;
/* size of database cache table (CONV_IMG) as number of rows   */
const size_t MAX_CACHE_ROWS_NUM =5000000;
/*  number of days after which an image will be deleted from server cache  */
const int TIMEOUT =              15;
/*  number of child processes of server to manage requests  */
const int CHILDREN_NUM =         2;
/*  successfully HTTP request   */
const char *HTTP_OK =           "HTTP/1.1 200 OK";
/*  error in serving HTTP request */
const char *HTTP_BAD_REQUEST =  "HTTP/1.1 400 Bad Request";
/*  not found file requested    */
const char *HTTP_NOT_FOUND =    "HTTP/1.1 404 Not Found";
/*  HTML page to show not-found error    */
const char *HTML_404 =          "<html><body><h1>Error 404: Page Not Found</h1></body></html>";
/*  HTML page to show bad-request error    */
const char *HTML_400 =          "<html><body><h1>Error 400: Bad Request</h1></body></html>";
/*  version HTTP protocol 1.1   */
const char *HTTP_1 =            "HTTP/1.1";
/*  version HTTP protocol 1.0   */
const char *HTTP_0 =            "HTTP/1.0";
/*  HTTP request User-Agent line    */
const char *USER_AGENT =        "User-Agent:";
/*  HTTP request Accept line    */
const char *ACCEPT =            "Accept:";