//
// Created by laura_trive on 06/04/16.
//

/**
 * Implementations of functions to do server work for reading client's HTTP request,
 * serve it, adapting the response to client's device, and sends an HTTP response
 * back to the client.
 */

#include "serveRequest.h"

#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <memory.h>

#include "constants.h"
#include "requestParser.h"
#include "responseWriter.h"
#include "adaptImage.h"
#include "io_func.h"

#define MAXLINE             1024


/*  Get image file length   */
long getLength(FILE *image)
{
    long seek, len;
    if ((seek = ftell(image))==-1){
        perror("Error in ftell\n");
        exit(1);
    }
    if (fseek(image,0L,SEEK_END)==-1) {
        perror("Error in fseek\n");
        exit(1);
    }
    if ((len = ftell(image))==-1) {
        perror("Error in ftell\n");
        exit(1);
    }
    if (fseek(image,seek,SEEK_SET)==-1) {
        perror("Error in fseek\n");
        exit(1);
    }
    return len;
}

/*  This function implements server work for serving a client's request:
 *  1)Parsing client request
 *  2)Elaborating request (adaptation on client device)
 *  3)Caching
 *  4)Sending response
 *
 *  @param sockfd: file descriptor for connection socket
 */
void serveRequest(int sockfd)
{

    FILE * image;
    size_t n;
    int read;

    //char *userAgent;
    struct req *req = parseRequest(sockfd);
    //userAgent = req->user_agent;
    //userAgent = "";
    struct img *reqImage;
    //struct img *reqImage = req->req_image;
    // requested image: {name, quality, width, height, type, last modified, file_length}
    if ((reqImage = malloc(sizeof(struct img)))==NULL) {
            perror("error in malloc\n");
            exit(1);
    }
    sprintf(reqImage->name,"mare");
    reqImage->width = 960;
    reqImage->height = 600;
    sprintf(reqImage->type,req->type);


    char buff[MAXLINE];
    char path[100];
    sprintf(path,"%s%s.%s", PATH, reqImage->name, reqImage->type);

    //da cercare image su db (cache o server)

    if ((image=fopen("/home/laura_trive/Scrivania/ServerFiles/mare.jpg", "rb"))==NULL) {
        perror("error in fopen\n");
        exit(1);
    }

    reqImage->file_length = getLength(image);

    struct img *adaptedImage = adaptImageTo(reqImage,req->userAgent);

    for(;;) {
        if ((read = readline(sockfd, buff, MAXLINE)) == 0) {
            printf("Client quit connection\n");
            return;
        }

        /*
        char *httpOK = "HTTP/1.1 200 OK\n";
        char *content = "Content-length: 52917\n";
        char *type = "Content-Type: image/jpeg\n\n";

        write(sockfd, httpOK, strlen(httpOK));
        write(sockfd, content, strlen(content));
        write(sockfd, type, strlen(type));
        */

        //struct img *adaptedImg = adaptTo(userAgent);
        //da mettere l'imm adattata come parametro e switch per l'esito
        writeResponse(sockfd, HTTP_OK, reqImage, image);

    }
}


