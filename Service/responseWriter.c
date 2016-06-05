/**
 * Implementations of functions to send server HTTP responses.
 */

#include "responseWriter.h"

/*  Compose index page to offer to client the possibility to choose which resource request
 * in a list of all server images
 *
 * @param images : list of all server images
 * @param info : struct conv_img used to pass some useful information
 */
char *composeHomePage(struct img **images, struct conv_img *info)
{
    /*  initialization of the string to write HTML index page   */
    char *home;
    if ((home = (char *) malloc(sizeof(char)*IMAGESNUM*MAXLINE))==NULL) {
        perror("error in malloc\n");
        exit(EXIT_FAILURE);
    }

    sprintf(home,"<!DOCTYPE html>"
            "<html>"
            "<body bgcolor=\"#99ccff\">"
            "<center><h1 style=\"color:#000066;\"> Choose your image! </h1><br>"
            "<p  style=\"color:#000080\"><i> Click on the name of image you want to download. </i></p></center><br>");

    /*  adding a HTML block for each image in server, associating a link to send the request of the selected resource   */
    int i;
    for (i = 0; i < IMAGESNUM; i++) {
        char div[MAXLINE];
        sprintf(div, "<center><div><a href = \"http://%s:%ld/%s.%s\">"
                " %d: %s "
                " </a></div></center><br>", info->last_modified, info->width, images[i]->name, images[i]->type, i+1, images[i]->name);
        strcat(home,div);
    }
    strcat(home,
            " </body> "
            " </html> \n");
    return home;
}

/*  Compose header of HTTP message to send to the client, based on result of request's elaboration
 * and length of file to send
 *
 * @param result : string with message HTTP for the elaboration result
 * @param image : struct conv_img with information about image to send
 */
char *composeHeader(char *result, struct conv_img *image)
{
    /*  initialization of string where compose message HTTP header  */
    char *header;
    if ((header = (char *) malloc(MAXLINE)) == NULL) {
        perror("error in malloc\n");
        exit(EXIT_FAILURE);
    }
    /*  date of today in HTTP format defined by protocol    */
    char *date = getTodayToHTTPLine();

    /* composed header for message of response for not supported protocol */
    if (strcmp(result, HTTP_0) == 0) {
        if (sprintf(header,
                    "%s\n"
                            "Date: %s\n"
                            "Server: WebServer/1.0.0\n"
                            "Content-Type: text/html; charset=UTF-8 \n"
                            "Content-Length: %ld\n"
                            "Connection: keep-alive\n\n",
                    HTTP_OK, date, strlen(HTML_NOT_SUPPORTED) + 1) < 0) {
            perror("error in sprintf\n");
            return "";
        }
    }

    /* composed header for message of response for request of index page */
    if (strcmp(result, INDEX) == 0) {
        if (sprintf(header,
                    "%s\n"
                            "Date: %s\n"
                            "Server: WebServer/1.0.0\n"
                            "Content-Type: text/html; charset=UTF-8 \n"
                            "Content-Length: %ld\n"
                            "Connection: keep-alive\n\n",
                    HTTP_OK, date, image->length+1) < 0) {
            perror("error in sprintf\n");
            return "";
        }
    }

    /* composed header for message of response for successfully request */
    if (strcmp(result, HTTP_OK) == 0) {
        if (sprintf(header,
                            "%s\n"
                            "Date: %s\n"
                            "Connection: keep-alive\n"
                            "Server: WebServer/1.0.0\n"
                            "Content-Type: image/%s\n"
                            "Content-Length: %ld\n\n",
                            HTTP_OK, date, image->type, image->length+1) < 0) {
            perror("error in sprintf\n");
            return "";
        }
    }

    /* composed header for message of response for Not Found error */
    if (strcmp(result,HTTP_NOT_FOUND) == 0) {
        if (sprintf(header,
                    "%s\n"
                    "Date: %s\n"
                    "Server: WebServer/1.0.0\n"
                    "Content-Type: text/html\n"
                    "Content-Length: %ld\n"
                    "Connection: keep-alive\n\n"
                    "<html><body><h1>404 Page Not Found. </h1></body></html>",
                    HTTP_OK, date, strlen(HTTP_NOT_FOUND)+50) < 0) {
            perror("error in sprintf\n");
            return "";
        }
    }

    /* composed header for message of response for Bad Request error */
    if (strcmp(result, HTTP_BAD_REQUEST) == 0) {
        if (sprintf(header,
                    "%s\n"
                    "Date: %s\n"
                    "Server: WebServer/1.0.0\n"
                    "Content-Type: text/html\n"
                    "Content-Length: %ld\n"
                    "Connection: keep-alive\n\n"
                    "<html><body><h1>400 Bad Request. </h1></body></html>",
                    HTTP_OK, date, strlen(HTTP_BAD_REQUEST)+50)<0){
            perror("error in sprintf\n");
            return "";
        }
    }
    return header;
}

/* Sending server response, based on result of the elaboration of request.
 *
 * @param connfd : file descriptor of connection socket
 * @param result : result of the elaboration of client's request
 * @param method : HTTP request method (GET or HEAD)
 * @param image : image to send as data in the message
 * @param images : list of all server images
 */
void writeResponse(int connfd, char *result, char *method, struct conv_img *image, struct img **images)
{
    /* writing buffer   */
    char buff[MAXLINE];
    /*  response header */
    char *header;

    for (;;) {
        if (readline(connfd, buff, (int) MAXLINE) == 0) {
            printf("Client quit connection\n");
            return;
        }

        printf("sending response...\n");

        /*  request HTTP 1.0 not supported   */
        if (strcmp(result, HTTP_0) == 0) {
            /* compose header and sending response  */
            header = composeHeader(result, image);
            write(connfd, header, strlen(header));

        } else if (strcmp(result, INDEX) == 0) { /*  requested home page of server content   */
            /*   compose index page */
            char *home = composeHomePage(images, image);

            image->height = (size_t) IMAGESNUM;
            image->length = strlen(home);

            ssize_t n;
            /* compose message header   */
            header = composeHeader(result, image);
            /*  send message header */
            write(connfd, header, strlen(header));

            while ((n = write(connfd, home, strlen(home))) > 0) {
                printf("Bytes sent %d \n", (int) n);
            }

        } else if (strcmp(result, HTTP_NOT_FOUND) == 0) {
            /*  sending response for Not Found error    */
            write(connfd, HTML_404, strlen(HTML_404));

        } else if (strcmp(result, HTTP_BAD_REQUEST) == 0) {
            /*  sending response for Bad Request error  */
            write(connfd, HTML_400, strlen(HTML_400));

        } else {
            /* if result is OK  */
            FILE *imgfd;    // file descriptor file to send
            char path[MAXLINE]; // path of file to send

            sprintf(path, "%s%lu.%s", CACHE_PATH, image->name_code, image->type);

            /* opening file to send */
            if ((imgfd = fopen(path, "rb")) == NULL) {
                perror("error in fopen\n");
                write(connfd, HTML_400, strlen(HTML_400));
                return;
            }
            fseek(imgfd,0,0);

            size_t n;
            /*  compose message header  */
            header = composeHeader(result, image);
            /* send message header  */
            ssize_t nn = write(connfd, header, strlen(header));

            while((nn = write(connfd, header, strlen(header)-nn)) > 0) {
            }

            if (strcmp(result, HTTP_OK) == 0) {
                /* response to GET method is header + image;
                 * response to HEAD method is only header, composed like that one for GET   */
                if (strcmp(method, "GET") == 0) {
                    while (1) {
                        /*  reading image file  */
                        n = fread(buff, 1, (size_t) MAXLINE, imgfd);
                        printf("Bytes read %d \n", (int) n);

                        if (n > 0) {
                            /*  scrittura byte letti dal file   */
                            printf("Sending \n");
                            write(connfd, buff, n);
                        }

                        if (n < MAXLINE) {
                            if (feof(imgfd)) {
                                /* check EOF    */
                                printf("End of file \n ");
                                fclose(imgfd);
                                break;
                            }
                            if (ferror(imgfd)) {
                                /*  check error */
                                printf("Error Reading \n");
                                fclose(imgfd);
                                break;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
}
