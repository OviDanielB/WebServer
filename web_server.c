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
#include <pthread.h>

#include "logging.h"
#include "DataBase/db_helper.h"
#include "helper/locking.h"
#include "Service/responseWriter.h"
#include "Service/requestParser.h"
#include "php/wurfl.h"

// define sigfunc to simplify signal sys call
typedef void sigfunc(int);
typedef struct sqlite sqlite;

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
void serveRequest(int sockfd, struct img **images, char *serverIp, in_port_t serverPort, struct logline *log)
{
    char result[50];
    char status[4];
    //char final_string[100] = "";

    struct conv_img *adaptedImage = (struct conv_img * ) malloc(sizeof(struct conv_img));
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

        /*log req line*/
        log_requestline(log, request);

        /*   ignoring requests for favicon.ico, generated from browser  */
        if (strcmp(request->resource,"favicon") == 0) {
            return;
            //writeResponse(sockfd, (char *) FAVICON, NULL, NULL, NULL);
        }

        /*  error message for HTTP 1.0 request  */
        if (strcmp(request->resource, HTTP_0) == 0) {
            writeResponse(sockfd, (char *) HTML_NOT_SUPPORTED, NULL, NULL, NULL);
        }

        /*  first client request to get view of server content  */
        if (strcmp(request->resource,INDEX) ==0 ) {
            /*  using field of struct conv_img to pass server info (IP address and port number) */
            sprintf(adaptedImage->last_modified, getServerIp());
            adaptedImage->width = (size_t) serverPort;

            writeResponse(sockfd, (char *)INDEX, NULL, adaptedImage, images);

        } else {
            printf("begin adaptation...\n");
            adaptedImage = adaptImageTo(request);

            printf("end adaptation.\n");

            switch (adaptedImage->name_code) {
                case 400 :
                    sprintf(result, HTTP_BAD_REQUEST);

                    sprintf(status, "400");

                    /*log status*/
                    sprintf(log->status, result);
                    /*log size*/
                    log->size = 0;

                    break;

                case 404 :
                    sprintf(result, HTTP_NOT_FOUND);

                    /*log status*/
                    sprintf(log->status, result);
                    /*log size*/
                    log->size = 0;

                    break;

                default :
                    sprintf(result, HTTP_OK);

                    /*log status*/
                    sprintf(log->status, result);
                    /*log size*/
                    log->size = adaptedImage->length;

                    break;
            }


            printf("begin response...\n");
            writeResponse(sockfd, result, request->method, adaptedImage, NULL);
        }
    }

    pthread_t log_thread;
    /*int sched, prio;

    sched = sched_getscheduler(log_thread);

    if (sched!=0){
        perror("error in getting scheduler");
    }

    prio = sched_get_priority_min(sched);

    if(prio!=0){
        perror("error in getting priority");
    }

    /* set thread priority to minimal value */
    /*if (pthread_setschedprio(log_thread, prio)!=0){
        perror("error in setting priority");
    }

    /* thread which log on file */
    if (pthread_create(&log_thread, NULL, (void *)logonfile, (void *)log)){
        perror("error in creating thread for log");
    }

    if (pthread_join(thread, NULL)){
        perror("error in joining thread for log");
    }
}

void child_main(int index, int listenfd, int addrlen, char *serverIp, in_port_t serverPort, struct img **images);
void child_main(int index, int listenfd, int addrlen, char *serverIp, in_port_t serverPort, struct img **images)
{
    int connfd;
    socklen_t clilen;
    struct sockaddr * cliaddr;
    time_t ticks;
    char buff[MAXLINE];

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

        struct logline *log = (struct logline *) malloc(sizeof(struct logline));
        if (log == NULL) {
            perror("error in malloc\n");
            exit(EXIT_FAILURE);
        }

        /*log date*/
        sprintf(log->date, getTodayToHTTPLine());
        /*log IPAddr*/
        sprintf(log->ip_host, clientIPAddr);

        serveRequest(connfd, images, serverIp, serverPort, log);

        close(connfd);
    }
}

pid_t child_make(int i, int listenfd, int addrlen, char *serverIp, in_port_t serverPort, struct img **images);
pid_t child_make(int i, int listenfd, int addrlen, char *serverIp, in_port_t serverPort, struct img **images)
{
    pid_t pid;

    // if it's father return pid
    if ((pid = fork()) > 0) {

        return pid;
    }

    pid = getpid();

    child_main(i, listenfd, addrlen, serverIp, serverPort, images);

    return pid;
}

int main(int argc, char **argv)
{
    int listensd, i;
    struct sockaddr_in servaddr;
    socklen_t addrlen;
    char *serverIp;
    in_port_t serverPort;

    time_t ticks;

    //pid_t child_make(int, int, int, char *, in_port_t, struct img **);

    // TODO arguments
    if(argc <= 2){
        // if not specified, use default port
        serverPort = DEFAULT_PORT;
    } else if(argc == 3) {
        // use specified port
        serverPort = (in_port_t) atoi(argv[2]);
    } else {
        printf("Usage : ./web_server <ip Address> <port number>");
        exit(EXIT_FAILURE);
    }

    /* load all server images that are in a specified directory */
    struct img **images = dbLoadAllImages((char *) PATH);
    /* load all server images previously manipulated that are in a specified directory */
    //db_load_cache_images((char *) CACHE_PATH);

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
    servaddr.sin_port = htons(serverPort); // returns host_uint16 converted to network byte order

    addrlen = sizeof(servaddr);
    // bind the listening socket to the address; needs casting to sockaddr *
    if(bind(listensd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
        perror("Error in Bind");
        exit(EXIT_FAILURE);
    }

    serverIp = getServerIp();
    printf("IP SERVER: %s - IP PORT: %d\n", serverIp, serverPort);

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
        serverIp = getServerIp();

        pids[i] = child_make(i, listensd, addrlen, serverIp, serverPort, images);
    }

    initializeFifo(pids);

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
}