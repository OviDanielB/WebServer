/**
 * Implementation of functions to parse lines of client's HTTP message request.
 */

#include "requestParser.h"


/* HTTP request parser reading information about method requested, client's device,
 * type and quality of file requested
 *
 * @param sockfd: file descriptor of connection socket
 */
struct req *parseRequest(int sockFd)
{
    /*  initialization of struct request to collect information read from received message  */
    struct req *request;
    if ((request = malloc(sizeof(struct req)))==NULL) {
        perror("error in malloc\n");
        return NULL;
    }
    memset(request->resource,'\0',strlen(request->resource));
    memset(request->type,'\0',strlen(request->type));
    memset(request->userAgent,'\0',strlen(request->userAgent));
    memset(request->method,'\0',strlen(request->method));

    /* reading buffer */
    char line[MAXLINE];
    memset(line,'\0',sizeof(line));

    int n = 0; // counter of necessary rows
    while (n != 3) {
        /*  reading line by line of message */
        if ((readline(sockFd, line, (int) MAXLINE)) == 0) {
            printf("Client quit connection\n");
            return NULL;
        }

        /* request line HTTP 1.0   */
        if (strstr(line,HTTP_0) != NULL) {
            /*  not supported protocol  */
            sprintf(request->resource, HTTP_0);
            return request;
        }

        /* request line HTTP 1.1    */
        if (strstr(line,HTTP_1) != NULL) {
            /*  read request method GET or HEAD of file requested */
            n += 1;

            char *method;
            char *resource;
            char *type;

            /*  request method  */
            method = strtok(line, " ");
            sprintf(request->method, method);

            /* resource name from URI   */
            resource = strtok(NULL, ".");
            resource++;
            sprintf(request->resource, resource);

            /*  if requested index page, stop parsing   */
            if (strcmp(resource, INDEX) == 0) {
                return request;
            }

            /* resource format  */
            type = strtok(NULL, " ");
            sprintf(request->type, type);

            continue;
        }

        /* user-agent line  */
        if (strncmp(line, USER_AGENT, strlen(USER_AGENT)) == 0) {

            n+=1;
            /* read entity line with device's description   */
            sprintf(request->userAgent, strtok(line+strlen(USER_AGENT)+1, "\n"));
            continue;
        }

        /* read resource's type from Accept line as preference  */
        if (strncmp(line, ACCEPT, strlen(ACCEPT)) == 0) {
            n+=1;

            /* check if requested jpeg image format:
             * -if yes, server response will be adapted to factor of quality q
             * defined in this line;
             * -if not, server response will be adapted to client device described by user-agent line    */
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
    }
    return request;
}