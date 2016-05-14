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
    if ((request = malloc(sizeof(struct req)))==NULL) {
        perror("error in malloc\n");
        return NULL;
    }

    memset(request->resource,'\0',strlen(request->resource));
    memset(request->type,'\0',strlen(request->type));
    memset(request->userAgent,'\0',strlen(request->userAgent));
    memset(request->method,'\0',strlen(request->method));

    char line[MAXLINE];
    memset(line,'\0',sizeof(line));

    int n=0;
    //while (1) {
    while (n!=3) {
        if ((readline(sockfd, line, (int) MAXLINE)) == 0) {
            printf("Client quit connection\n");
            return NULL;
        }

        printf("%s\n",line);

        if (strstr(line,HTTP_0)!=NULL) {
            //not supported protocol
            printf("Not supported protocol\n");
            return NULL;
        }

        if (strstr(line,HTTP_1)!=NULL) {
            //read request method GET or HEAD of file requested

            printf("reading request line...\n");
            n += 1;

            char *name;
            char method[5];
            memset(method,'\0',strlen(method));
            char resource[256];
            memset(resource,'\0',strlen(resource));
            char type[5];
            memset(type,'\0',strlen(type));
            int i=0;
            name = &line[0];
            sscanf(" %c", name);
            while (*name!=' ') {
                method[i] = *name;
                name++;
                sscanf(" %c", name);
                i++;
            }
            sprintf(request->method,method);
            printf("method: %s\n",request->method);
            //read resource name from URI in the request-line
            i=0;
            name = strchr(line,'/');
            name++;
            sscanf(" %c", name);
            /*if ((name=strstr(line,"Images/"))==NULL) {
                name = strchr(line,'/');
                name++;
            } else {
                name+=7;
            }*/
            while (*name != '.') {
                resource[i] = *name;
                name++;
                sscanf(" %c", name);
                i++;
            }
            sprintf(request->resource,resource);
            printf("name of resource: %s\n",resource);

            if (strcmp(resource,INDEX)==0) {
                return request;
            }

            // read resource file read from uri
            name++;
            sscanf(" %c", name);
            i=0;
            while (*name!=' ') {
                type[i] = *name;
                name++;
                sscanf(" %c", name);
                i++;
            }
            sprintf(request->type,type);
            printf("type of resource: %s\n",type);
            continue;
        }

        if (strncmp(line,USER_AGENT,strlen(USER_AGENT))==0) {

            printf("reading user-agent line...\n");
            n+=1;

            // read entity line with device's description
            strcpy(request->userAgent,line+strlen(USER_AGENT)+1);
            printf("device user-agent: %s\n",request->userAgent);
            continue;
        }

        // read resource's type from Accept line
        if (strncmp(line,ACCEPT,strlen(ACCEPT))==0) {

            printf("reading accept line...\n");
            n+=1;

            /* check if requested jpeg image format;
             * if yes, server response will be adapted to factor of quality q
             * defined in this line;
             * if not, server response will be adapted to client device described by user-agent line    */
            char *t;
            if ((t = strstr(line,"image/jpeg")) == NULL || (t = strstr(line,"image/jpg")) == NULL) {
                request->quality = -1;
            } else {
                /*  read quality of JPEG resource's format from Accept line    */
                float factor;
                int j = 0;
                char quality[4];
                sscanf(" %c", t);
                while (*t != 'q') {
                    t++;
                    sscanf(" %c", t);
                }
                t += 2;
                while (j < 3) {
                    quality[j] = *t;
                    t++;
                    sscanf(" %c", t);
                    j++;
                }
                sscanf(quality, "%f", &factor);
                request->quality = factor;

                printf("factor of quality q: %f\n", factor);
            }
        }

        /* end while cycle when "\n\n" read as index of request's end   */
        if (strstr(line,"\n\n") != NULL) {

            printf("end request...\n");

            break;
        }
    }
    return request;
}