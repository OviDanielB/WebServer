
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <signal.h>


#include "io_func.h"
#include "DataBase/db_helper.h"
#include "locking.h"
#include "constants.h"
#include "serveRequest.h"

// define sigfunc to simplify signal sys call
typedef void sigfunc(int);

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

void child_main(int index, int listenfd, int addrlen) {

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
    cliaddr = malloc((size_t)addrlen);
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

        printf("server process %ld accepted request from client %s\n",(long) getpid(), clientIPAddr);

       // pict = fopen("/home/ovi/Desktop/3806.jpg","rb");

/*        while(1){

            size_t nread = fread(buff, 1, MAXLINE, pict);
            printf("Bytes read %d \n", nread);

            if(nread > 0){
                printf("Sending \n");
                write(connfd,buff,nread);

            }

            if(nread < MAXLINE){
                if(feof(pict)){
                    printf("End of file \n ");
                }
                if(ferror(pict)){
                    printf("Error Reading \n");
                }
                break;
            }

        }
*/

        serveRequest(connfd);

        close(connfd);
    }
}

int main(int argc, char **argv)
{
    int listensd, connsd, i;
    struct sockaddr_in servaddr;
    char buff[MAXLINE];
    socklen_t addrlen;

    in_port_t port;
    time_t ticks;

    sqlite3 * db;
    char * sqlStatement;


    pid_t child_make(int, int, int);

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

    // OPEN DATABASE; creates it if doesn't exist
    //db_open(db);


    struct img * image;
    image = malloc(sizeof(struct img));

    /*
    image->name = "Foto";
    image->type = "jpeg";
    image->width = 800;
    image->height = 600;
    image->length = 800*600;

    db_insert_img(image);

     */

   // db_get_image_by_name("Iubita", image);
   // printf("CIAO %s\n", image->name);




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

    // marks the socket as a passive socket and it is ready to accept connections
    // BACKLOG max number of allowerd connections. if max reached the user will get an error
    if(listen(listensd, BACKLOG) < 0){
        perror("Error in listen");
        exit(EXIT_FAILURE);
    }

    // creates memory for pids
    pids = calloc( (size_t) CHILDREN_NUM,sizeof(pid_t));

    // initialize lock on template filename for child processes
    lock_init("/tmp/lock.XXXXXX");

    //num_child = 2;
    // create pids array with children pids
    for(i = 0; i < CHILDREN_NUM; i++ ){
        pids[i] = child_make(i, listensd, addrlen);
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

}

pid_t child_make(int i, int listenfd, int addrlen){
    pid_t pid;

    // if it's father return pid
    if((pid = fork()) > 0){
        return pid;
    }

    child_main(i,listenfd,addrlen);

}

/*
void str_echo(int sockfd){

     WORKING SEND HTML PAGE
    ssize_t n;
    char line[MAXLINE];

    for(;;){
        if(( n = readline(sockfd,line,MAXLINE)) == 0){
            printf("Client quit connection\n");
            return;
        }

        printf("server process %ld received %s\n", (long) getpid() ,line);

        //writen(sockfd,line,(size_t) n);
        write(sockfd,"HTTP/1.1 200 OK\n",16);
        write(sockfd,"Content-length: 58\n",19);
        write(sockfd,"Content-Type: text/html\n\n",25);
        write(sockfd,"<html><body><h1>Ciao Laura e Francesco.</h1></body></html>",58);

    }


    FILE * image;
    size_t n;
    int read;
    char buff[MAXLINE];

    image = fopen("/home/ovi/Desktop/firefox.jpg", "rb");

    for(;;) {
        if ((read = readline(sockfd, buff, MAXLINE)) == 0) {
            printf("Client quit connection\n");
            return;
        }

        char *httpOK = "HTTP/1.1 200 OK\n";
        char *content = "Content-length: 52917\n";
        char *type = "Content-Type: image/jpeg\n\n";


        write(sockfd, httpOK, strlen(httpOK));
        write(sockfd, content, strlen(content));
        write(sockfd, type, strlen(type));

        while (1) {

            n = fread(buff, 1, MAXLINE, image);
            printf("Bytes read %d \n", (int) n);

            if (n > 0) {
                printf("Sending \n");
                write(sockfd, buff, n);

            }

            if (n < MAXLINE) {
                if (feof(image)) {
                    printf("End of file \n ");
                }
                if (ferror(image)) {
                    printf("Error Reading \n");
                }
                break;
            }

        }
    }



}*/