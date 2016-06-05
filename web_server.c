/*------------------------------------------------------------------------
 * Program:   web_server
 * Purpose:   allocate a socket and then repeatedly execute the following:
 *              (1) wait for the next connection from a client
 *              (2) send the response to the client's request
 *              (3) close the connection
 *              (4) go back to step (1)
 * Syntax:    webserver [number of children] [port] [IP address]
 *               port  - protocol port number to use
 * Note:      The three arguments are optional.
 *            If no children number is specified, the server uses
 *            the default given by DEFAULT_CHILDREN.
 *            If no port is specified, the server uses the default
 *            given by DEFAULT_PORT.
 *            If no IP address is specified, the server uses that one
 *            assigned by the LAN which it's connected to.
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

#include "logging.h"
#include "DataBase/db_helper.h"
#include "helper/locking.h"
#include "Service/responseWriter.h"
#include "Service/requestParser.h"


static pid_t *pids;

/** generic function for signal handling
 * define behavior in case: SIGNALNUMBER for specific signal
 * call signal(sig,sig_handler)
 *
 * @param sig : number of signal to manage
 */
void sig_handler(int sig)
{
    int i;

    switch (sig) {
        /* kills all children processes and the father itself; call on terminal ^C (ctrl-C)
         * within the process or by "kill -SIGINT <father pid>" */
        case SIGINT:
            for (i = 0; i < sizeof(pids) / sizeof(pid_t); i++) {
                 if (kill(pids[i], SIGKILL) == -1) {
                    perror("error in children kill signal");
                    exit(EXIT_FAILURE);
                }
                printf("pid = %ld killed by SIGINT\n", (long) pids[i]);
            }
            /*  eliminate all elements into database cache  */
            dbDeleteAll();
            exit(EXIT_SUCCESS);

        default:
            fprintf(stderr,"no action for sig num %d\n", sig);
    }
}

/*  Server service to manage a client's request:
 *  1)Parsing request
 *  2)Elaborating request (adaptation based on client device)
 *  3)Caching of adapted content
 *  4)Sending response
 *
 *  @param sockFd: file descriptor of the connection socket
 *  @param images : list of all server images
 *  @param serverPort : server port number
 *  @param log : struct log to fill
 *  @param logBuffer: list of struct log to write
 */
void serveRequest(int sockFd, struct img **images, char *serverIp, in_port_t serverPort, struct log *log, struct log **logBuffer)
{
    /*  HTTP message of request elaboration */
    char result[50];
    /*  HTTP status of request elaboration  */
    char status[4];

    /*  initialization of struct to info of image to adapt  */
    struct conv_img *adaptedImage = (struct conv_img * ) malloc(sizeof(struct conv_img));
    if (adaptedImage == NULL) {
        perror("error in malloc\n");
        exit(EXIT_FAILURE);
    }

    printf("Reading request...\n");
    struct req *request = parseRequest(sockFd);

    if (request == NULL) {
        /*  error in reading request */
        sprintf(result, "%s", HTTP_BAD_REQUEST);
        /*  sending error message to client */
        writeResponse(sockFd, result, NULL, NULL, NULL);

    } else {

        /* save request line on log */
        sprintf(log->reqLine, "%s /%s.%s %s", request->method, request->resource, request->type, HTTP_1);

        /*   ignoring requests for favicon.ico, auto-generated from browser  */
        if (strcmp(request->resource, "favicon") == 0) {
            return;
        }

        /*  error message for HTTP 1.0 request not supported  */
        if (strcmp(request->resource, HTTP_0) == 0) {
            writeResponse(sockFd, (char *) HTML_NOT_SUPPORTED, NULL, NULL, NULL);
        }

        /*  first client request to get view of server content  */
        if (strcmp(request->resource,INDEX) == 0) {
            /*  using field of struct conv_img to pass server info (IP address and port number) */
            sscanf(serverIp, "%s", adaptedImage->last_modified);
            adaptedImage->width = (size_t) serverPort;

            writeResponse(sockFd, (char *)INDEX, NULL, adaptedImage, images);

        } else {
            /* adaptation of request resource */
            adaptedImage = adaptImageTo(request);
            sprintf(adaptedImage->type, request->type);

            /* saved into name_code adaptation result */
            switch (adaptedImage->name_code) {
                case 400 :
                    /*  generic error   */
                    sprintf(result, HTTP_BAD_REQUEST);
                    sprintf(status, "400");

                    /* save status on log */
                    sprintf(log->status, result);
                    /* save number of bytes sent on log */
                    log->size = 0;

                    break;
                case 404 :
                    /*  resource not found  */
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

            printf("Sending response...\n");
            writeResponse(sockFd, result, request->method, adaptedImage, NULL);
        }
    }

    /* saved into the buffer log for this request */
    logOnFile(log, logBuffer);
}

/** Helper process main.
 *
 * @param listenFd : file descriptor of listening socket
 * @param addrlen : address length
 * @param serverIp: server IP address
 * @param serverPort : server port number
 * @param images : list of all server images
 * */
void child_main(int listenFd, int addrlen, char *serverIp, in_port_t serverPort, struct img **images);
void child_main(int listenFd, int addrlen, char *serverIp, in_port_t serverPort, struct img **images)
{
    struct sockaddr *cliaddr = (struct sockaddr *) malloc((size_t)addrlen);
    if (cliaddr == NULL) {
        perror("malloc error");
        exit(EXIT_FAILURE);
    }
    socklen_t clilen = sizeof(cliaddr);
    struct sockaddr_in * addr;
    /*  IP address of client requesting service */
    char * clientIPAddr;

    /*  file descriptor of connection socket    */
    int connFd;

    /*  initialization of buffer for saving line of log to write    */
    struct log **logBuffer = (struct log **) malloc(BUFFER_LOG*sizeof(struct log *));
    if (logBuffer == NULL) {
        perror("malloc error");
        exit(EXIT_FAILURE);
    }
    int i;
    for(i = 0; i < (int) BUFFER_LOG; i++){
        logBuffer[i] = (struct log *) malloc(BUFFER_LOG*sizeof(struct log));
        if (logBuffer[i] == NULL) {
            perror("malloc error");
            exit(EXIT_FAILURE);
        }
        sprintf(logBuffer[i]->ip_host, "*");
    }

    printf("Child process %ld starting service\n", (long) getpid());

    for(;;){
        /* acquire lock */
        lock_wait();
        connFd = accept(listenFd, cliaddr, &clilen);
        /* unlock  */
        lock_release();

        /* convert sockaddr to sockaddr_in  */
        addr = (struct sockaddr_in *) cliaddr;
        /* use ntoa to convert client address from binary form to readable string   */
        clientIPAddr = inet_ntoa(addr->sin_addr);

        printf("server process %ld accepted request from client %s\n", (long) getpid(), clientIPAddr);

        /*  initialization of log structure to memorize information about request and response  */
        struct log *log = (struct log *) malloc(sizeof(struct log));
        if (log == NULL) {
            perror("error in malloc\n");
            exit(EXIT_FAILURE);
        }

        /* save today date on log */
        sprintf(log->date, "%s", getTodayToHTTPLine());
        /* save client IP address on log    */
        sprintf(log->ip_host, "%s", clientIPAddr);

        /*  starting elaboration of request    */
        serveRequest(connFd, images, serverIp, serverPort, log, logBuffer);

        /*  closing connection at the end of the response   */
        close(connFd);
    }
}

/** Creation of pool of children processes to manage several and concurrent requests.
 *
 * @param i : integer to get count of number of processes created
 * @param listenFd : file descriptor of listening socket
 * @param addrelen : address length
 * @param serverIp : server IP address
 * @param serverPort : server port number
 * @param images : list of all server images loaded into database
 *
 * */
pid_t child_make(int i, int listenFd, int addrlen, char *serverIp, in_port_t serverPort, struct img **images);
pid_t child_make(int i, int listenFd, int addrlen, char *serverIp, in_port_t serverPort, struct img **images)
{
    pid_t pid;

    /*  if it's father, function returns pid   */
    if ((pid = fork()) > 0) {
        return pid;
    }

    pid = getpid();

    /* launch of helper process main */
    child_main(listenFd, addrlen, serverIp, serverPort, images);

    return pid;
}

/*  Web server main  */
int main(int argc, char **argv)
{
    int listenSd, i;
    struct sockaddr_in servaddr;
    socklen_t addrLen;

    /*  string to maintain IP address associated to the server (chosen as argument or assigned by LAN) */
    char *serverIp = (char *) malloc (sizeof(char)*16);
    if (serverIp == NULL) {
        perror("malloc error\n");
        exit(EXIT_FAILURE);
    }
    sprintf(serverIp, "***.***.***.***");

    /*  port number (default one or chosen as argument)  */
    in_port_t serverPort;

    if (argc == 1) {
        // if not specified, use default port
        serverPort = DEFAULT_PORT;
        // if not specified, use default number of helper
        CHILDREN_NUM = DEFAULT_CHILDREN;
    } else if (argc == 2) {
        // use specified number of helper
        CHILDREN_NUM = atoi(argv[1]);
        /* if number is not between 1 and 8 */
        if (CHILDREN_NUM < 1 || CHILDREN_NUM > 8) {
            printf("Insert a number of children between 1 and 8.\n"
                           "Usage : ./web_server <helper number> <port number[xxxx]> <ip Address[xxx.xxx.xxx.xxx]>");
            exit(EXIT_FAILURE);
        }
        // if not specified, use default port
        serverPort = DEFAULT_PORT;
    } else if (argc == 3) {
        /* use specified number of helper   */
        CHILDREN_NUM = atoi(argv[1]);
        /* use specified port   */
        serverPort = (in_port_t) atoi(argv[2]);
    } else if (argc == 4) {
        /* use specified number of helper   */
        CHILDREN_NUM = atoi(argv[1]);
        /* use specified port   */
        serverPort = (in_port_t) atoi(argv[2]);
        /* use specified IP address */
        sscanf(argv[3], "%s", serverIp);
    } else {
        printf("Usage : ./web_server <helper number> <port number[xxxx]> <ip Address[xxx.xxx.xxx.xxx]>");
        exit(EXIT_FAILURE);
    }

    /* load all server images that are in a specified directory into database   */
    struct img **images = dbLoadAllImages((char *) PATH);

    /* creates a listening socket   */
    if ((listenSd=socket(AF_INET,SOCK_STREAM,0)) < 0){
        perror("Error in listening socket");
        exit(EXIT_FAILURE);
    }

    /* set all bytes of the sockaddr_in struct to 0 with memset */
    memset((void *)&servaddr, 0, sizeof(servaddr));
    /* initialization of all struct fields */
    servaddr.sin_family = AF_INET; // AF_INET = IPv4 with 32-bit address and 16-bit port number (in_port_t)
    /* INADDR_ANY allows the program to work without knowing the IP address of the machine it is running on */
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // returns host_uint32 converted to network byte order
    servaddr.sin_port = htons(serverPort); // returns host_uint16 converted to network byte order

    addrLen = sizeof(servaddr);

    /* bind the listening socket to the address; needs casting to sockaddr * */
    if (bind(listenSd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
        perror("Error in Bind");
        exit(EXIT_FAILURE);
    }

    /* marks the socket as a passive socket and it is ready to accept until
     * a maximum of BACKLOG connections.
     * If max reached the user will get an error */
    if (listen(listenSd, BACKLOG) < 0) {
        perror("Error in listen");
        exit(EXIT_FAILURE);
    }

    /*  initialize lock on template filename for child processes    */
    lock_init("/tmp/lock.XXXXXX");

    /* if IP address not specified as argument, know that assigned by server LAN */
    if (strcmp(serverIp, "***.***.***.***") == 0) {
        sprintf(serverIp, getServerIp());
    }

    printf("IP SERVER: %s - IP PORT: %d\n", serverIp, serverPort);

    /* create array with children pids */
    pids = calloc( (size_t) CHILDREN_NUM, sizeof(pid_t));
    if (pids == NULL) {
        perror("calloc error");
        exit(EXIT_FAILURE);
    }

    for(i = 0; i < CHILDREN_NUM; i++ ){
        pids[i] = child_make(i, listenSd, addrLen, serverIp, serverPort, images);
    }

    /*  father creates a new PHP process and two FIFO file to communication
     * between the new process and the children ones   */
    initializeFifo(pids);

    /* when SIGINT arrives (press ctrl-C) the father process and the children terminate */
    if (signal(SIGINT,sig_handler) == SIG_ERR){
        perror("error in signal SIGINT");
        exit(EXIT_FAILURE);
    }

    printf("Father pid = %ld\n", (long) getpid());

    for (;;) {

        pause();
    }

    /*  never reached   */
    close(listenSd);
    return 0;
}
