//
// Created by laura_trive on 23/03/16.
//

/*
 * Functions implementation to parse lines of client's HTTP request.
 */

#include "requestParser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "io_func.h"

#define MAXLINE             1024


/* This function implement the HTTP request parser,
 * reading information about method requested, client's device,
 * type and quality of file requested.
 *
 * @param sockfd: file descriptor of connection socket
 */
struct req *parseRequest(int sockfd)
{
    struct req *request;
    if ((request=malloc(sizeof(struct req)))==NULL) {
        perror("error in malloc\n");
        exit(1);
    }

    char line[MAXLINE];
    int read;

    while ((read=readline(sockfd,line,MAXLINE))!=0) {

        if (strncmp(line,HTTP_0,strlen(HTTP_0))==0) {
            //not supported protocol
            printf("Not supported protocol\n");
        }

        if (strncmp(line,HTTP_1,strlen(HTTP_1))==0) {
            //read request method GET or HEAD and URL of file requested
            sscanf(line,"%s %s ",request->method,request->url);
            //vedi se come da url nome immagine
        }

        if (strncmp(line,USER_AGENT,strlen(USER_AGENT))==0) {
            // read entity line with device's description
            sscanf(line," %s",request->userAgent);
        }

        if (strncmp(line,ACCEPT,strlen(ACCEPT))==0) {
            //read request method GET or HEAD
            sscanf(line," image/%s; q=%f ",request->type,&request->quality);
        }
    }

    return request;
}