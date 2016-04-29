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

    while (1) {

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
            char *name;
            char method[4];
            memset(method,'\0',strlen(method));
            char resource[256];
            memset(resource,'\0',strlen(resource));
            char type[4];
            memset(type,'\0',strlen(type));
            int i=0;
            name = &line[0];
            while (*name!=' ') {
                method[i] = *name;
                name++;
                i++;
            }
            sprintf(request->method,method);
            printf("method: %s\n",request->method);
            //read resource name from URI in the request-line
            i=0;
            name++;
            name++;
            while (*name!='.') {
                resource[i] = *name;
                name++;
                i++;
            }
            strcpy(request->resource,resource);
            printf("name of resource: %s\n",resource);
            // read resource file read from uri
            name++;
            i=0;
            while (*name!=' ') {
                type[i] = *name;
                name++;
                i++;
            }
            sprintf(request->type,type);
            printf("type of resource: %s\n",type);
            continue;
        }

        if (strncmp(line,USER_AGENT,strlen(USER_AGENT))==0) {
            // read entity line with device's description
            strcpy(request->userAgent,line+strlen(USER_AGENT)+1);
            printf("device user-agent: %s\n",request->userAgent);
            //n+=1;
            continue;
        }

        // NON PARSA q PER TEST METTO COSTANTE
        //request->quality = 0.5;
        //n++;
        // fine test

        if (strncmp(line,ACCEPT,strlen(ACCEPT))==0) {
            //read resource type from Accept line
            char *t;
            int i=0;
            char type[4];
            if ((t = strstr(line,"image/"))!=NULL) {
                t+=6;
                while (*t!=' ') {
                    type[i] = *t;
                    t++;
                    i++;
                }
                sprintf(request->type,type);
                printf("image format: %s\n",type);

            }
            /*  read resource quality from Accept line    */
            float factor;
            int j=0;
            char quality[3];

            t+=3;
            while (*t!=' ' && j<3) {
                quality[j] = *t;
                t++;
                j++;
            }
            sscanf(quality,"%f",&factor);
            request->quality = factor;

            printf("q: %f\n",factor);

            //n+=1;
        }

        /* end while cycle when "\n\n" read as index of request's end   */
        if (strstr(line,"\n\n")!=NULL) {
            break;
        }
    }

    return request;
}