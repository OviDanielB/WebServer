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
void serveRequest(sqlite3 *db, int sockfd, struct img **images)
{
    char result[50];
    struct conv_img *adaptedImage;
    adaptedImage = malloc(sizeof(struct conv_img));
    if (adaptedImage==NULL) {
        perror("error in malloc\n");
        exit(EXIT_FAILURE);
    }

    struct req *request = parseRequest(sockfd);
    if (request==NULL) {
        sprintf(result,HTTP_BAD_REQUEST);
        writeResponse(sockfd, result, NULL, NULL, NULL);
    } else {
        /*  first client request to get view of server content  */
        if (strcmp(request->resource,INDEX)==0) {

            writeResponse(sockfd, (char *)INDEX, NULL, adaptedImage, images);

        } else {

            adaptedImage = adaptImageTo(db, request);

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

            sprintf(adaptedImage->original_name, request->resource);

            writeResponse(sockfd, result, request->method, adaptedImage, NULL);
        }
    }

}


