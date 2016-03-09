/*#include <stdio.h>

int main(void){
    printf("Ciao");
    return 0;
}*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_PORT       5193            /* default protocol port number */
#define BACKLOG           10            /* size of request queue        */


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

int main(int argc, char **argv)
{
    struct  protoent *ptrp;  /* pointer to a protocol table entry   */
    struct  sockaddr_in sad; /* structure to hold server's address  */
    struct  sockaddr_in cad; /* structure to hold client's address  */
    int     listensd, connsd; /* socket descriptors             */
    int     port;        /* protocol port number        */
    unsigned int    alen;        /* length of address           */
    char 	  sendBuf[1024];    /* buffer for data the server sends */

    memset((void *)&sad, 0, sizeof(sad)); /* clear sad */
    sad.sin_family = AF_INET;     /* set family to Internet */
    sad.sin_addr.s_addr = htonl(INADDR_ANY); /* set the local IP address */

    /* Check command-line argument for protocol port and extract
       port number if one is specified.  Otherwise, use the default
       port value given by constant DEFAULT_PORT. */

    if (argc > 1)           /* if argument specified    */
        port = atoi(argv[1]);   /* convert argument to binary   */
    else
        port = DEFAULT_PORT;     /* use default port number      */

    if (port > 0)           /* test for illegal value   */
        sad.sin_port = htons(port);
    else {              /* print error message and exit */
        fprintf(stderr,"bad port number %s\n",argv[1]);
        exit(1);
    }

    /* Map TCP transport protocol name to protocol number */
    if ( (ptrp = getprotobyname("TCP")) == NULL) {
        fprintf(stderr, "cannot map \"TCP\" to protocol number");
        exit(1);
    }

    /* Create a socket */
    if ((listensd = socket(AF_INET, SOCK_STREAM, ptrp->p_proto)) < 0) {
        perror("socket creation failed");
        exit(1);
    }

    /* Bind a local address to the socket */
    if (bind(listensd, (struct sockaddr *)&sad, sizeof(sad)) < 0) {
        perror("bind failed");
        exit(1);
    }

    /* Specify size of request queue */
    if (listen(listensd, BACKLOG) < 0) {
        perror("listen failed");
        exit(1);
    }

    /* Main server loop - accept and handle requests */
    while (1) {
        alen = sizeof(cad);
        if ( (connsd=accept(listensd, (struct sockaddr *)&cad, &alen)) < 0) {
            perror("accept failed");
            exit(1);
        }

        //do server work

        if (write(connsd, sendBuf, strlen(sendBuf)) != strlen(sendBuf)) {
            perror("error in write");
            exit(1);
        }

        close(connsd);
    }
}
