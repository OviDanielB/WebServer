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
#define SERV_PORT   5193
#define MAXLINE     1024

int main(int argc, char **argv)
{
    int              sockfd, bytesRec = 0,imagefd;
    char             recvline[MAXLINE];
    struct sockaddr_in   servaddr;
    FILE * image, *image2;

    memset(recvline,'0',sizeof(recvline));
    if (argc != 2) { /* controlla numero degli argomenti */
        fprintf(stderr, "utilizzo: daytime_clientTCP <indirizzo IP server>\n");
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

    image = fopen("/home/ovi/Desktop/cery.jpg", "ab");

    /* legge dal socket fino a quando non trova EOF */
    while ((bytesRec = read(sockfd, recvline, MAXLINE)) > 0) {
        // recvline[n] = 0;
        /* aggiunge il carattere di terminazione */
        /* stampa il contenuto di recvline sullo standard output */
/*    if (fputs(recvline, stdout) == EOF) {
      fprintf(stderr, "errore in fputs\n");
      exit(1);
*/

        printf("Bytes received %d \n", bytesRec);
        if(fwrite(recvline,1,bytesRec,image) < 0){
            perror("error in fwrite");
            exit(EXIT_FAILURE);
        }

        //  write(imagefd, recvline, sizeof(recvline));


    }


    return 0;
}

