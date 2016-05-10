/*------------------------------------------------------------------------
 * Program:   web_server
 * Purpose:   allocate a socket and then repeatedly execute the following:
 *              (1) wait for the next connection from a client
 *              (2) send the response to the client's request
 *              (3) close the connection
 *              (4) go back to step (1)
 * Syntax:    server [ port ]
 *               port  - protocol port number to use
 * Note:      The port argument is optional.  If no port is specified,
 *            the server uses the default given by DEFAULT_PORT.
 *------------------------------------------------------------------------ */


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <ifaddrs.h>

#include "DataBase/db_helper.h"
#include "helper/locking.h"
#include "Service/responseWriter.h"
#include "Service/requestParser.h"

// define sigfunc to simplify signal sys call
typedef void sigfunc(int);
typedef struct sqlite sqlite;

void getServerIp();
void getServerIp()
{
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
        } else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
        }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
}

static pid_t *pids;

/* generic function for signal handling
 * define behavior in case: SIGNALNUMBER for specific signal
 * call signal(sig,sig_handler)
 */
void sig_handler(int sig){
    int i ;

    switch (sig) {

        /* kills all children processes and the father itself; call on terminal ^C (ctrl-C)
         * within the process or by "kill -SIGINT <father pid>"
         */
        case SIGINT:
            for (i = 0; i < sizeof(pids) / sizeof(pid_t); i++) {
                 if (kill(pids[i], SIGKILL) == -1) {
                    perror("error in children kill signal");
                    exit(EXIT_FAILURE);
                }
                printf("pid = %ld killed by SIGINT\n", (long) pids[i]);
            }
            exit(EXIT_SUCCESS);

        default:
            fprintf(stderr,"no action for sig num %d\n", sig);
    }

}

/*  Server work for serving a client's request:
 *  1)Parsing client request
 *  2)Elaborating request (adaptation on client device)
 *  3)Caching
 *  4)Sending response
 *
 *  @param sockfd: file descriptor for connection socket
 */
void serveRequest(sqlite3 *db, int sockfd, struct img **images)
{
    char result[50];
    struct conv_img *adaptedImage;
    adaptedImage = (struct conv_img * ) malloc(sizeof(struct conv_img));
    if (adaptedImage == NULL) {
        perror("error in malloc\n");
        exit(EXIT_FAILURE);
    }

    printf("begin reading request...\n");
    struct req *request = parseRequest(sockfd);
    printf("end reading request...\n");

    if (request == NULL) {

        sprintf(result,HTTP_BAD_REQUEST);
        writeResponse(sockfd, result, NULL, NULL, NULL);

    } else {
        /*  first client request to get view of server content  */
        if (strcmp(request->resource,INDEX)==0) {

            writeResponse(sockfd, (char *)INDEX, NULL, adaptedImage, images);

        } else {
            printf("begin adaptation...\n");
            adaptedImage = adaptImageTo(db, request);

            printf("end adaptation.\n");

            switch (adaptedImage->name_code) {
                case 400 :
                    sprintf(result, HTTP_BAD_REQUEST);
                    break;

                case 404 :
                    sprintf(result, HTTP_NOT_FOUND);
                    break;

                default :
                    sprintf(result, HTTP_OK);
                    break;
            }

            printf("begin response...\n");
            writeResponse(sockfd, result, request->method, adaptedImage, NULL);
        }
    }
}

void child_main(int index, int listenfd, int addrlen, sqlite3 *db, struct img **images);
void child_main(int index, int listenfd, int addrlen, sqlite3 *db, struct img **images) {

    int connfd, imagefd,pictfd;
    socklen_t clilen;
    struct sockaddr * cliaddr;
    time_t ticks;
    char buff[MAXLINE];
    FILE * clientRequest, *pict;
    ssize_t numRead;

    char * clientIPAddr;
    struct sockaddr_in * addr;

    memset(buff,'0', MAXLINE);
    cliaddr = (struct sockaddr *) malloc((size_t)addrlen);
    clilen = sizeof(cliaddr);

    printf("child process %ld starting \n", (long) getpid());

    for(;;){
        //clilen = (socklen_t) addrlen;
        //clientRequest = fopen("/home/ovi/Desktop/request.txt","a");
        // file lock
        lock_wait();
        connfd = accept(listenfd, cliaddr, &clilen);
        // file unlock
        lock_release();

        // convert sockaddr to sockaddr_in
        addr = (struct sockaddr_in *) cliaddr;
        // use ntoa to convert client address from binary form to readable string
        clientIPAddr = inet_ntoa(addr->sin_addr);

        printf("server process %ld accepted request from client %s\n", (long) getpid(), clientIPAddr);

        serveRequest(db, connfd, images);

        /*log
         * int logging_thread = pthread_create(&log_thread, NULL, &log_on_file());
         * int log_min_priority = sched_get_priority_min(sched_getscheduler(log_thread));
         * if (pthread_setschedprio(log_thread, log_min_priority)!=0){
         *  perror("error in set priority")
         * }
         * */

        close(connfd);
    }
}

pid_t child_make(int i, int listenfd, int addrlen, sqlite3 *db, struct img **images);
pid_t child_make(int i, int listenfd, int addrlen, sqlite3 *db, struct img **images)
{
    pid_t pid;

    // if it's father return pid
    if((pid = fork()) > 0){
        return pid;
    }

    pid = getpid();

    child_main(i,listenfd,addrlen,db,images);

    return pid;
}

int main(int argc, char **argv)
{
    int listensd, connsd, i;
    struct sockaddr_in servaddr;
    char buff[MAXLINE];
    socklen_t addrlen;

    in_port_t port;
    time_t ticks;

    char * sqlStatement;


    pid_t child_make(int, int, int, sqlite3 *, struct img **);

    // TODO arguments
    if(argc <= 2){
        // if not specified, use default port
        port = DEFAULT_PORT;
    } else if(argc == 3) {
        // use specified port
        port = (in_port_t) atoi(argv[2]);
    } else {
        printf("Usage : ./web_server <ip Address> <port number>");
        exit(EXIT_FAILURE);
    }

    /* open server database or create it if doesn't exist */
    sqlite3 *db = db_open();
    /* load all server images that are in a specified directory */
    struct img **images = db_load_all_images(db,(char *)PATH);

    // creates a listening socket
    if((listensd=socket(AF_INET,SOCK_STREAM,0)) < 0){
        perror("Error in listening socket");
        exit(EXIT_FAILURE);
    }

    // set all bytes of the sockaddr_in struct to 0 with memset
    memset((void *)&servaddr,0,sizeof(servaddr));
    // initialize all struct fields
    servaddr.sin_family = AF_INET; // AF_INET = IPv4 with 32-bit address and 16-bit port number (in_port_t)
    // INADDR_ANY allows the program to work without knowing the ip address of the machine it is running on
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // returns host_uint32 converted to network byte order
    servaddr.sin_port = htons(port); // returns host_uint16 converted to network byte order

    addrlen = sizeof(servaddr);
    // bind the listening socket to the address; needs casting to sockaddr *
    if(bind(listensd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
        perror("Error in Bind");
        exit(EXIT_FAILURE);
    }

    getServerIp();

    // marks the socket as a passive socket and it is ready to accept connections
    // BACKLOG max number of allowed connections. if max reached the user will get an error
    if(listen(listensd, BACKLOG) < 0){
        perror("Error in listen");
        exit(EXIT_FAILURE);
    }

    // creates memory for pids
    pids = calloc( (size_t) CHILDREN_NUM,sizeof(pid_t));

    // initialize lock on template filename for child processes
    lock_init("/tmp/lock.XXXXXX");

    // create pids array with children pids
    for(i = 0; i < CHILDREN_NUM; i++ ){
        pids[i] = child_make(i, listensd, addrlen, db, images);
    }

    // when SIGINT arrives (press ctrl-C) the father process and the children terminate
    if(signal(SIGINT,sig_handler) == SIG_ERR){
        perror("error in signal SIGINT");
        exit(EXIT_FAILURE);
    }

    printf("Father pid = %ld\n", (long) getpid());

    for(;;){

        pause();
    }
    return 0;

    close(listensd);

    db_close(db);
}