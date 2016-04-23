//
// Created by laura_trive on 06/04/16.
//

/**
 * Implementations of functions to do server work for reading client's HTTP request,
 * serve it, adapting the response to client's device, and sends an HTTP response
 * back to the client.
 */

#include "serveRequest.h"


/*  This function implements server work for serving a client's request:
 *  1)Parsing client request
 *  2)Elaborating request (adaptation on client device)
 *  3)Caching
 *  4)Sending response
 *
 *  @param sockfd: file descriptor for connection socket
 */
void serveRequest(sqlite3 *db, int sockfd)
{
    FILE * image;
    size_t n;
    int read;
    char buff[MAXLINE];
    char result[50];
    struct conv_img *adaptedImage;

    //char *userAgent;
    struct req *request = parseRequest(sockfd);
    if (request==NULL) {
        sprintf(result,HTTP_BAD_REQUEST);
    } else {
        //userAgent = req->user_agent;
        //userAgent = "";
        struct img *reqImage;
        //struct img *reqImage = req->req_image;
        // requested image: {name, quality, width, height, type, last modified, file_length}
        if ((reqImage = malloc(sizeof(struct img))) == NULL) {
            perror("error in malloc\n");
            exit(1);
        }

        adaptedImage = adaptImageTo(db, reqImage, request);

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

        sprintf(adaptedImage->original_name, reqImage->name);
    }

    for(;;) {
        if ((read = readline(sockfd, buff, (int) MAXLINE)) == 0) {
            printf("Client quit connection\n");
            return;
        }

        printf("sending response...\n");

        writeResponse(sockfd, result, request->method, adaptedImage);
    }
}


