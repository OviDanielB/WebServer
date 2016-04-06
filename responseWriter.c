//
// Created by laura_trive on 04/04/16.
//

/**
 * Implementations of functions to send server HTTP responses.
 */

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <zconf.h>

#include "constants.h"
#include "helper/calendar.h"

char header[102400];
long file_length = 0;
char *reply;

/*  Compose response HTTP message to send to the client */
char *composeHeader(char *result, struct img *image)
{
    char *date = getTodayToString();

    if (strcmp(result,"HTTP_OK")==0) {
        if (sprintf(header,
                    HTTP_OK"\n"
                            "Date: %s\n"
                            "Server: WebServer/1.0.0\n"
                            //"Last-Modified: Wed, 18 Jun 2003 16:05:58 GMT\n"
                            //"ETag: \"56d-9989200-1132c580\"\n"
                            "Content-Type: image/%s\n"
                            "Content-Length: %ld\n"
                            //"Accept-Ranges: bytes\n"
                            "Connection: keep-alive\n", date, image->type, image->file_length) < 0) {
            perror("error in sprintf\n");
            return "";
        }

        file_length = image->file_length;
    }

    if (strcmp(result,"HTTP_NOT_FOUND")==0) {
        if (sprintf(header,
                    HTTP_NOT_FOUND"\n"
                    "Date: %s\n"
                    "Server: WebServer/1.0.0\n"
                    "Connection: keep-alive\n", date) < 0) {
            perror("error in sprintf\n");
            return "";
        }
    }

    if (strcmp(result,"HTTP_BAD_REQUEST")==0) {
        if (sprintf(header,
                    HTTP_BAD_REQUEST"\n"
                    "Date: %s\n"
                    "Server: WebServer/1.0.0\n"
                    //"Last-Modified: Wed, 18 Jun 2003 16:05:58 GMT\n"
                    //"ETag: \"56d-9989200-1132c580\"\n"
                    //"Content-Type: image/"+image->type+"\n"
                    //"Content-Length: %i\n"
                    //"Accept-Ranges: bytes\n"
                    "Connection: keep-alive\n",date)<0){
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

void writeResponse(int connfd, char *result, struct img *image)
{
    char *header = composeHeader(result,image);

    write(connfd, header,strlen(header));

    if (strcmp(result,"HTTP_OK")==0){

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