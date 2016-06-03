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
            sprintf(request->resource, HTTP_0);
            return request;
        }

        if (strstr(line,HTTP_1)!=NULL) {
            //read request method GET or HEAD of file requested

            printf("reading request line...\n");
            n += 1;

            char *method;
            char *resource;
            char *type;

            method = strtok(line, " ");
            sprintf(request->method, method);
            printf("METHOD %s ",request->method);

            resource = strtok(NULL, ".");
            resource++;
            sprintf(request->resource, resource);
            printf("NAME %s ",request->resource);

            if (strcmp(resource,INDEX)==0) {
                return request;
            }

            type = strtok(NULL, " ");
            sprintf(request->type, type);
            printf("TYPE %s\n",request->type);

            continue;
        }
        if (strncmp(line,USER_AGENT,strlen(USER_AGENT)) == 0) {

            printf("reading user-agent line...\n");
            n+=1;

            // read entity line with device's description\n
            //strncpy(request->userAgent, line+strlen(USER_AGENT)+1, strlen(line)-strlen(USER_AGENT)-1);
            sprintf(request->userAgent, strtok(line+strlen(USER_AGENT)+1, "\n"));
            printf("device user-agent: %s\n",request->userAgent);
            continue;
        }

        // read resource's type from Accept line
        if (strncmp(line, ACCEPT, strlen(ACCEPT)) == 0) {

            printf("reading accept line...\n");
            n+=1;

            /* check if requested jpeg image format;
             * if yes, server response will be adapted to factor of quality q
             * defined in this line;
             * if not, server response will be adapted to client device described by user-agent line    */
            char *t;
            if ((t = strstr(line,"image/jpeg")) == NULL
                && (t = strstr(line,"image/jpg")) == NULL
                && (t = strstr(line,"image/JPEG")) == NULL) {
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