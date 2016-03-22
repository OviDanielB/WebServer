
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define DEFAULT_PORT       5193            /* default protocol port number */
#define BACKLOG           10            /* size of request queue        */
#define MAXLINE             1024


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
 *            the server uses the default given by PROTOPORT.
 *------------------------------------------------------------------------ */

static int num_child;
static pid_t *pids;

static struct flock lock_it,unlock_it;

void my_lock_init(char *pathname);

static int lock_fd = -1;


void child_main(int index, int listenfd, int addrlen) {

    int connfd;
    socklen_t clilen;
    struct sockaddr * cliaddr;
    time_t ticks;
    char buff[MAXLINE];

    cliaddr = malloc((size_t)addrlen);
    printf("child %ld starting \n", (long) getpid());

    for(;;){
        clilen = (socklen_t) addrlen;
        //TODO file lock
        connfd = accept(listenfd,cliaddr,clilen);
        // TODO file unlock


        /* accetta una connessione con un client */
        ticks = time(NULL); /* legge l'orario usando la chiamata di sistema time */
        /* scrive in buff l'orario nel formato ottenuto da ctime;
           snprintf impedisce l'overflow del buffer. */
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks)); /* ctime trasforma la data
        e l�ora da binario in ASCII. \r\n per carriage return e line feed*/

        printf("%d",ticks);
        /* scrive sul socket di connessione il contenuto di buff */
        if (write(connfd, buff, strlen(buff)) != strlen(buff)) {
            perror("errore in write");
            exit(1);
        }
        close(connfd);
    }
}

int main(int argc, char **argv)
{
    int listensd, connsd, i;
    struct sockaddr_in servaddr;
    char buff[MAXLINE];
    socklen_t addrlen = 2;

    in_port_t port;
    time_t ticks;

    pid_t child_make(int, int, int);

    // TODO arguments
    if(argc <= 2){
        // if not specified, use default port
        port = (in_port_t) DEFAULT_PORT;
    } else if(argc == 3) {
        // use specified port
        port = (in_port_t) argv[2];
    } else {
        printf("Usage : ./web_server <ip Address> <port number>");
        exit(EXIT_FAILURE);
    }

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

    // bind the listening socket to the address; needs casting to sockaddr *
    if(bind(listensd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
        perror("Error in Bind");
        exit(EXIT_FAILURE);
    }

    // marks the socket as a passive socket and it is ready to accept connections
    // BACKLOG max number of allowerd connections. if max reached the user will get an error
    if(listen(listensd,BACKLOG) < 0){
        perror("Error in listen");
        exit(EXIT_FAILURE);
    }

    // creates memory for pids
    pids = calloc( (size_t) num_child,sizeof(pid_t));

    // create lock file for child processes
    my_lock_init("/tmp/lock.XXXXXX");

    num_child = 4;
    // create pids array with children pids
    for(i = 0; i < num_child; i++ ){
        pids[i] = child_make(i, listensd, addrlen);
    }

    printf("Finished %d\n", getpid());

    for(;;){

    }
    return 0;


}

void my_lock_init(char *pathname){

    char lock_file[1024];

    strncpy(lock_file,pathname,sizeof(lock_file));

    if((lock_fd = mkstemp(lock_file)) < 0){
        perror("mkstemp error");
        exit(EXIT_FAILURE);
    }

    if(unlink(lock_file) == -1 ){
        perror("unlink error");
        exit(EXIT_FAILURE);
    }

    lock_it.l_type = F_WRLCK;
    lock_it.l_whence = SEEK_SET;
    lock_it.l_start = 0;
    // len = 0 -> all bytes from l_start
    lock_it.l_len = 0;


    unlock_it.l_type = F_UNLCK;
    unlock_it.l_whence = SEEK_SET;
    unlock_it.l_start = 0;
    unlock_it.l_len = 0;

}


pid_t child_make(int i, int listenfd, int addrlen){
    pid_t pid;

    // if it's father return pid
    if((pid = fork()) > 0){
        return pid;
    }

    printf("CIao %d\n",getpid());

    // TODO REMOVE
    //exit(EXIT_SUCCESS);
    child_main(i,listenfd,addrlen);

}





