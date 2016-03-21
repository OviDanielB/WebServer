/**
* Created by laura_trive on 21/03/16.
*
* Implementation of system structs and definition of system constant.
*/

/*  path of images' directory    */
#define PATH           "/home/laura_trive/Scrivania/ServerFiles/"
/*  path of cached images' directory    */
#define CACHE_PATH     "/home/laura_trive/Scrivania/ServerFiles/Cache/"
/*  path of log file    */
#define LOG_PATH       "/home/laura_trive/Scrivania/ServerFiles/Log/log.txt"
/* size of a line   */
#define MAX_LINE            1024
/* default protocol port number */
#define DEFAULT_PORT        5193
/* size of request queue        */
#define BACKLOG             10
/*  number of child processes of server to manage requests  */
#define CHILDREN_NUM        4


/*  logging file    */
FILE *log = fopen(LOG_PATH,"a");

/*  Image struct    */
typedef struct img {
    char    *name;
    float   quality;
    int     width;
    int     height;
    char    *type;
    struct tm last_modified;
};