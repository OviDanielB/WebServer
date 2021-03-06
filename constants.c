/**
* Definition of all server constants.
*/

#include "constants.h"

/*  path of images' directory    */
const char *PATH =               "/home/laura_trive/Scrivania/ProgettoIIW/ServerFiles/Images/";
/*  path of cached images' directory    */
const char *CACHE_PATH =         "/home/laura_trive/Scrivania/ProgettoIIW/ServerFiles/Cache/";
/*  path of server's database of images */
const char *DB_PATH =            "/home/laura_trive/ClionProjects/WebServer/DataBase/serverContent.db";
/*  path of log file    */
const char *LOG_PATH =           "/home/laura_trive/Scrivania/ProgettoIIW/ServerFiles/Log/log.txt";
/* path of file PHP to execute  */
const char *FIFO_PATH =          "/home/laura_trive/ClionProjects/WebServer/php/fifo.php";
/*  path of PHP program */
const char *PHP_PATH =           "/usr/bin/php";
/* default protocol port number */
const in_port_t DEFAULT_PORT =   5193;
/* size of request queue        */
const int BACKLOG =              10;
/* size of buffer for writing lines on log file  */
const size_t BUFFER_LOG =        50;
/*  line length */
const size_t MAXLINE =           1024;
/* size of database cache table (CACHE) as number of rows   */
const int MAX_CACHE_ROWS_NUM =   50000;
/*  number of days after which an image will be deleted from server cache  */
const int TIMEOUT =              15;
/*  default number of child processes of server to manage requests  */
const int DEFAULT_CHILDREN =     2;
/*  successfully HTTP request   */
const char *HTTP_OK =           "HTTP/1.1 200 OK";
/*  error in serving HTTP request */
const char *HTTP_BAD_REQUEST =  "HTTP/1.1 400 Bad Request";
/*  not found file requested    */
const char *HTTP_NOT_FOUND =    "HTTP/1.1 404 Not Found";
/*  HTML page to show not-found error    */
const char *HTML_404 =          "<html><body><h1>Error 404: Page Not Found </h1></body></html>";
/*  HTML page to show bad-request error    */
const char *HTML_400 =          "<html><body><h1>Error 400: Bad Request </h1></body></html>";
/*  HTML page to show not supported protocol HTTP 1.0 error    */
const char *HTML_NOT_SUPPORTED ="<html><body><h1>Error: Not Supported Protocol HTTP 1.0 </h1></body></html>";
/*  version HTTP protocol 1.1   */
const char *HTTP_1 =            "HTTP/1.1";
/*  version HTTP protocol 1.0   */
const char *HTTP_0 =            "HTTP/1.0";
/*  HTTP request User-Agent line    */
const char *USER_AGENT =        "User-Agent:";
/*  HTTP request Accept line    */
const char *ACCEPT =            "Accept:";
/*  home page of server images to show to the client    */
const char *INDEX =             "index";
