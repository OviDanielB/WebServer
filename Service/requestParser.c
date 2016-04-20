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
        return NULL;
    }

    char line[MAXLINE];
    int read;
    int n=0; // counter of line of information to be read

    //while ((read=readline(sockfd,line,MAXLINE))!=0) {

    while (n<3) {

        if ((read = readline(sockfd, line, (int) MAXLINE)) == 0) {
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
            int i=0, j=0;
            char resource[256];
            char type[4];
            if ((name = strrchr(request->uri,'/'))!=NULL) {
                name++;
                while (*name!='.') {
                    resource[i] = *name;
                    name++;
                    i++;
                }
                sprintf(request->resource,resource);
                // read resource file read from uri
                name++;
                while (*name!=' ') {
                    type[j] = *name;
                    name++;
                    j++;
                }
                sprintf(request->type,type);
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

        // NON PARSA q PER TEST METTO COSTANTE
        request->quality = 0.5;
        n++;
        // fine test

        if (strncmp(line,ACCEPT,strlen(ACCEPT))==0) {
            //read resource type from Accept line
            char *t;
            int i=0;
            char type[4];
            /*if ((t = strstr(line,"image/"))!=NULL) {
                t+=6;
                while (*t!=' ') {
                    type[i] = *t;
                    t++;
                    i++;
                }
                sprintf(request->type,type);
                printf("q: %s\n",type);

            }*/
            //read resource quality from Accept line
            char *q;
            float factor;
            int j=0;
            char quality[3];

            if ((q = strstr(line,"q="))!=NULL) {
                q+=2;
                while (*q!=' ' || *q!=',' || *q!=';') {
                    quality[j] = *q;
                    q++;
                    j++;
                }
                sscanf(quality,"%f",&factor);
                request->quality = (size_t) factor*100;

                printf("q: %f\n",factor);
            }
            n+=1;
        }
    }

    return request;
}