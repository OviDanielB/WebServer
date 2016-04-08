//
// Created by laura_trive on 04/04/16.
//

/**
 * Implementations of functions to send server HTTP responses.
 */

#include "responseWriter.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "constants.h"
#include "helper/calendar.h"


/*  Compose response HTTP message to send to the client */
char *composeHeader(char *result, struct img *image)
{
    char *reply;
    char header[102400];
    long file_length = 0;
    char *date = getTodayToString();

    if (strcmp(result,HTTP_OK)==0) {
        if (sprintf(header,
                            "%s\n"
                            "Date: %s\n"
                            "Server: WebServer/1.0.0\n"
                            //"Last-Modified: Wed, 18 Jun 2003 16:05:58 GMT\n"
                            //"ETag: \"56d-9989200-1132c580\"\n"
                            "Content-Type: image/%s\n"
                            "Content-Length: %ld\n"
                            //"Accept-Ranges: bytes\n"
                            "Connection: keep-alive\n\n",
                            HTTP_OK, date, image->type, image->file_length) < 0) {
            perror("error in sprintf\n");
            return "";
        }

        file_length = image->file_length;
    }

    if (strcmp(result,HTTP_NOT_FOUND)==0) {
        if (sprintf(header,
                    "%s\n"
                    "Date: %s\n"
                    "Server: WebServer/1.0.0\n"
                    "Connection: keep-alive\n"
                    "Content-Type: text/html\n"
                    "<html><body><h1>404 Page Not Found.</h1></body></html>\n", HTTP_NOT_FOUND, date) < 0) {
            perror("error in sprintf\n");
            return "";
        }
    }

    if (strcmp(result,HTTP_BAD_REQUEST)==0) {
        if (sprintf(header,
                    "%s\n"
                    "Date: %s\n"
                    "Server: WebServer/1.0.0\n"
                    //"Last-Modified: Wed, 18 Jun 2003 16:05:58 GMT\n"
                    //"ETag: \"56d-9989200-1132c580\"\n"
                    //"Content-Length: %i\n"
                    //"Accept-Ranges: bytes\n"
                    "Connection: keep-alive\n"
                    "Content-Type: text/html\n\n"
                    "<html><body><h1>404 Page Not Found.</h1></body></html>\n",HTTP_BAD_REQUEST, date)<0){
            perror("error in sprintf\n");
            return "";
        }
    }

    if ((reply=malloc(strlen(header)+file_length))==NULL) {
        perror("error in malloc\n");
        return "";
    }

    if (sprintf(reply,"%s",header)<0){
        perror("error in sprintf\n");
        return "";
    }

    return reply;
}

void writeResponse(int connfd, char *result, struct img *image, FILE *imgfd) {

    char *header = composeHeader(result, image);
    char buff[MAXLINE];
    size_t n;

    write(connfd, header, strlen(header));

    if (strcmp(result, HTTP_OK) == 0) {

        while (1) {

            n = fread(buff, 1, MAXLINE, imgfd);
            printf("Bytes read %d \n", (int) n);

            if (n > 0) {
                printf("Sending \n");
                write(connfd, buff, n);

            }

            if (n < MAXLINE) {
                if (feof(imgfd)) {
                    printf("End of file \n ");
                }
                if (ferror(imgfd)) {
                    printf("Error Reading \n");
                }
                break;
            }
        }
    }
}
