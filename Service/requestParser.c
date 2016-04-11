//
// Created by laura_trive on 23/03/16.
//

/*
 * Functions implementation to parse lines of client's HTTP request.
 */

#include "requestParser.h"


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
    int n=0; // counter of line of information to be read

    //while ((read=readline(sockfd,line,MAXLINE))!=0) {

    while (n<3) {

        if ((read = readline(sockfd, line, MAXLINE)) == 0) {
            printf("Client quit connection\n");
            return NULL;
        }

        if (strstr(line,HTTP_0)!=NULL) {
            //not supported protocol
            printf("Not supported protocol\n");
            return NULL;
        }

        if (strstr(line,HTTP_1)!=NULL) {
            //read request method GET or HEAD and URI of file requested
            sscanf(line,"%s %s ",request->method,request->uri);
            //read resource name from URI in the request-line
            char *name;
            int i=0;
            char resource[256];
            if ((name = strrchr(request->uri,'/'))!=NULL) {
                name++;
                while (*name!='.') {
                    resource[i] = *name;
                    name++;
                    i++;
                }
                sprintf(request->resource,resource);
            }
            n+=1;
            continue;
        }

        if (strncmp(line,USER_AGENT,strlen(USER_AGENT))==0) {
            // read entity line with device's description
            sscanf(line," %s",request->userAgent);
            n+=1;
            continue;
        }

        if (strncmp(line,ACCEPT,strlen(ACCEPT))==0) {
            //read request method GET or HEAD
            sscanf(line," image/%s; q=%f ",request->type,&request->quality);
            n+=1;
        }
    }

    return request;
}