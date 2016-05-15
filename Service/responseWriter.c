//
// Created by laura_trive on 04/04/16.
//

/**
 * Implementations of functions to send server HTTP responses.
 */

#include "responseWriter.h"

char *composeHomePage(struct img **images, struct conv_img *info)
{
    char *home;
    if ((home = (char *) malloc(sizeof(char)*IMAGESNUM*MAXLINE))==NULL) {
        perror("error in malloc\n");
        exit(EXIT_FAILURE);
    }

    sprintf(home,"<!DOCTYPE html>"
            "<html>"
            "<body>"
            "<p>Click on image</p><br>");

    int i;
    for (i = 0; i < IMAGESNUM; i++) {
        char div[MAXLINE];
        sprintf(div, "<div><a href = \"http://%s:%ld/%s.%s\">"
                "%d: %s"
                //"<img src = \"/Images/mare.jpg\" alt = \"mare\" style=\"width:80px;height:80px;border:1;\">"
                " </a></div><br>", info->last_modified, info->width, images[i]->name, images[i]->type, i+1, images[i]->name);
        strcat(home,div);
    }
    strcat(home,
            " </body>"
            " </html>\n");
    return home;
}

/*  Compose header response HTTP message to send to the client */
char *composeHeader(char *result, struct conv_img *image)
{
    char *header;
    if ((header = (char *) malloc(MAXLINE))==NULL) {
        perror("error in malloc\n");
        exit(EXIT_FAILURE);
    }
    char *date = getTodayToHTTPLine();

    if (strcmp(result,HTTP_0) == 0) {

        if (sprintf(header,
                    "%s\n"
                            "Date: %s\n"
                            "Server: WebServer/1.0.0\n"
                            "Content-Type: text/html; charset=UTF-8 \n"
                            "Content-Length: %ld\n"
                            "Connection: keep-alive\n\n",
                    HTTP_OK, date, strlen(HTML_NOT_SUPPORTED)+1) < 0) {
            perror("error in sprintf\n");
            return "";
        }
    }

    if (strcmp(result,INDEX) == 0) {

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

    if (strcmp(result,HTTP_OK)==0) {

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

    if (strcmp(result,HTTP_NOT_FOUND)==0) {
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

    if (strcmp(result,HTTP_BAD_REQUEST)==0) {
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
 * @param: connfd = file descriptor of connection socket
 * @param: result = result of the elaboration of client's request
 * @param: method = HTTP request method (GET or HEAD)
 * @param: image = image to send as data in the message
 */
void writeResponse(int connfd, char *result, char *method, struct conv_img *image, struct img **images)
{
    char buff[MAXLINE];
    char *header;

    for (;;) {
        if (readline(connfd, buff, (int) MAXLINE) == 0) {
            printf("Client quit connection\n");
            return;
        }

        pid_t p = getpid();
        printf("%d sending response...\n", p);

        /*  request HTTP 1.0 not supported   */
        if (strcmp(result, HTTP_0) == 0) {

            header = composeHeader(result, image);
            write(connfd, header, strlen(header));

        } else if (strcmp(result, INDEX) == 0) { /*  requested home page of server content   */

            char *home = composeHomePage(images, image);

            image->height = (size_t) IMAGESNUM;
            image->length = strlen(home);

            ssize_t n;

            header = composeHeader(result, image);
            write(connfd, header, strlen(header));

            while ((n = write(connfd, home, strlen(home))) > 0) {
                printf("Bytes sent %d \n", (int) n);
            }

        } else {

            FILE *imgfd;
            char path[MAXLINE];
            /*if (strcmp(result, FAVICON) == 0) {
                sprintf(path, "%s%s.ico", HOME, FAVICON);
            } else {
                sprintf(path, "%s%lu.%s", CACHE_PATH, image->name_code, image->type);
            }*/

            sprintf(path, "%s%lu.%s", CACHE_PATH, image->name_code, image->type);

            printf("PATH FILE TO SEND: %s",path);

            if ((imgfd = fopen(path, "rb")) == NULL) {
                perror("error in fopen\n");
                strcpy(result, HTTP_BAD_REQUEST);
            }
            fseek(imgfd,0,0);

            size_t n;
            header = composeHeader(result, image);

            printf("header to send:\n %s\n",header);
            ssize_t nn = write(connfd, header, strlen(header));
            printf("sent %ld bytes of header(%ld)\n",nn,strlen(header));
            while((nn = write(connfd, header, strlen(header)-nn))>0) {
            }
            printf("header sent");

            if (strcmp(result, HTTP_OK) == 0) {
                /* response to GET method is header + image;
                 * response to HEAD method is only header, composed like that one for GET   */
                if (strcmp(method, "GET") == 0) {
                    while (1) {

                        n = fread(buff, 1, (size_t) MAXLINE, imgfd);
                        printf("Bytes read %d \n", (int) n);

                        if (n > 0) {
                            printf("Sending \n");
                            write(connfd, buff, n);
                        }

                        if (n < MAXLINE) {
                            if (feof(imgfd)) {
                                printf("End of file \n ");
                                fclose(imgfd);
                                break;
                            }
                            if (ferror(imgfd)) {
                                printf("Error Reading \n");
                                fclose(imgfd);
                                break;
                            }
                            break;
                        }
                    }
                }
            } else if (strcmp(result, HTTP_NOT_FOUND) == 0) {
                write(connfd, HTML_404, strlen(HTML_404));
            } else if (strcmp(result, HTTP_BAD_REQUEST) == 0) {
                write(connfd, HTML_400, strlen(HTML_400));
            }
        }
    }
}
