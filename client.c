//
// Created by ovi on 3/22/16.
//
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "io_func.h"

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#define SERV_PORT   5193
#define MAXLINE     1024
#define IMAGE_PATH "/home/ovi/Desktop/"

void str_cli(FILE* fp, int sockfd) ;

int main(int argc, char **argv)
{
    int sockfd,imagefd;
    ssize_t bytesRec;
    char recvline[MAXLINE] , imagename[MAXLINE];
    char imagePath[sizeof(IMAGE_PATH)] = IMAGE_PATH;
    struct sockaddr_in   servaddr;
    FILE * image, *image2;

    memset(recvline,'0',sizeof(recvline));
    if (argc == 3) { /* controlla numero degli argomenti */

        /* copies the name of the image by input argv[2] to the buffer imagename
         * imagename must be < MAXLINE !!!!
         */
        strcpy(imagename,argv[2]);
    } else {
        fprintf(stderr, "utilizzo: client <indirizzo IP server> <imageName.ext>\n");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { /* crea il socket */
        perror("errore in socket");
        exit(1);
    }

    memset((void *)&servaddr, 0, sizeof(servaddr));   /* azzera servaddr */
    servaddr.sin_family = AF_INET;       /* assegna il tipo di indirizzo */
    servaddr.sin_port   = htons(SERV_PORT);  /* assegna la porta del server */
    /* assegna l'indirizzo del server prendendolo dalla riga di comando.
       L'indirizzo � una stringa e deve essere convertito in intero in
       network byte order. */
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        fprintf(stderr, "errore in inet_pton per %s\n", argv[1]);
        exit(1);
    }

    /* stabilisce la connessione con il server */
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("errore in connect");
        exit(1);
    }

  //  image = fopen(strcat(imagePath,imagename), "ab");

    /* legge dal socket fino a quando non trova EOF */
/*    while ((bytesRec = read(sockfd, recvline, MAXLINE)) > 0) {


        printf("Bytes received %d \n", (int) bytesRec);
        if(fwrite(recvline,1,(size_t) bytesRec,image) < 0){
            perror("error in fwrite");
            exit(EXIT_FAILURE);
        }


    }

*/

    str_cli(stdin,sockfd);

    return 0;
}


void str_cli(FILE* fp, int sockfd) {

    int maxfdp1, stdineof;
    fd_set rset;
    char sendline[MAXLINE], receiveline[MAXLINE];
    ssize_t n;

    stdineof = 0;
    // always initialize the fd_set structure to avoid problems
    FD_ZERO(&rset);

    // WORKING CLIENT WITHOUT SELECT
/*    while (fgets(sendline,MAXLINE,fp) != NULL){

        writen(sockfd,sendline,strlen(sendline));

         if( readline(sockfd,receiveline,MAXLINE) == 0){
             perror("server quit");
             return;
         }

        fputs(receiveline,stdout);

    }
*/

    // WORKING CLIENT WITH SELECT
    for(;;){
        //set the corrisponding bit of the interesed fd in the fd_set structure
        if(stdineof == 0) {
            FD_SET(fileno(fp), &rset);
        }
        FD_SET(sockfd, &rset);

        // calculate the maxfd1 (maximum fd number) to use in SELECT sys call
        maxfdp1 = MAX(fileno(fp),sockfd) + 1; // +1 because fd start at 0 !!

        /* call to select; rset only for reading fd;
         * timeout is NULL so it returns ONLY when a fd is ready for reading;
         * returns -1 on error
         */
        if(select(maxfdp1,&rset,NULL,NULL,NULL) == -1){
            perror("select error");
            exit(EXIT_FAILURE);
        }

        /* after select returns check which fd is ready to be read */
        if(FD_ISSET(sockfd,&rset)){ /* socket is readable */
            if( readline(sockfd,receiveline,MAXLINE) == 0){
                if(stdineof == 1) {
                    return; /* normal termination */
                } else {
                    perror("server quit");
                }
            }

            fputs(receiveline,stdout);
        }

        if(FD_ISSET(fileno(fp),&rset)){  /* input is readable from */

            if(fgets(sendline,MAXLINE,fp) == NULL){
                stdineof = 1;
                shutdown(sockfd,SHUT_WR); /* send FIN */
                FD_CLR(fileno(fp),&rset);
                continue;
            }

            writen(sockfd,sendline,strlen(sendline));
        }
    }
}

