//
// Created by laura_trive on 04/04/16.
//

/**
 * Implementations of functions to send server HTTP responses.
 */

#include "responseWriter.h"
#include "../helper/io_func.h"

char result[50];

/*  This function opens the image file if it was previously adapted and saved in cache;
 *  if that exists, it returns the file descriptor, else 0.
 *
 * @param img: adapted image
 */
FILE *openCachedImage(struct conv_img *image)
{
    char path[MAXLINE];
    FILE *cachedImg;
    sprintf(path,"%s%lu.%s", CACHE_PATH, image->name_code, image->type);
    if ((cachedImg = fopen(path, "rb"))==NULL) {
        return NULL;
    } else {
        sprintf(result, (char *)HTTP_OK);
        return cachedImg;
    }
}

/* Opening original image file and if that exists, it returns the file descriptor.
 *
 * @param img: original image
 */
FILE *openImage(struct conv_img *image)
{
    char path[MAXLINE];
    FILE *imgfd;
    // check if adapted image just manipulated before
    if ((imgfd = openCachedImage(image)) != NULL) {
        return imgfd;
    }
    sprintf(path, "%s%s.%s", PATH, image->original_name, image->type);

    if ((imgfd = fopen(path, "rb")) == NULL) {
        perror("error in fopen\n");
        return NULL;
    } else {
        return imgfd;
    }
}

char *composeHomePage(struct img **images)
{
    char *home;
    if ((home = (char *) malloc(sizeof(char)*MAXLINE*IMAGESNUM))==NULL) {
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
        sprintf(div, "<div><a href = \"http://127.0.0.1:5193/%s.%s\">"
                "%d: %s"
                //"<img src = \"/Images/mare.jpg\" alt = \"mare\" style=\"width:80px;height:80px;border:1;\">"
                " </a></div><br>", images[i]->name, images[i]->type, i+1, images[i]->name);
        strcat(home,div);
    }
    strcat(home,
            "</body>"
            "</html>\n");

    // come src da mettere richiesta immagine al server
    /*sprintf(home,"<!DOCTYPE html>"
            "<html>"
            "<body>"
            "<p>Click on image</p><br>"
            "<div><a href = \"http://127.0.0.1:5193/mare.jpg\">"
            "mare"
            //"<img src = \"/Images/mare.jpg\" alt = \"mare\" style=\"width:80px;height:80px;border:1;\">"
            "</a></div>"
            "</body>"
            "</html>");*/
    return home;
}

/*  Compose header response HTTP message to send to the client */
char *composeHeader(char *result, struct conv_img *image)
{
    //char *reply;
    char *header;
    if ((header = (char *) malloc(MAXLINE))==NULL) {
        perror("error in malloc\n");
        exit(EXIT_FAILURE);
    }
    //long file_length = 0;
    char *date = getTodayToHTTPLine();

    if (strcmp(result,INDEX)==0) {

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
                            "Server: WebServer/1.0.0\n"
                            "Content-Type: image/%s\n"
                            "Content-Length: %ld\n"
                            "Connection: keep-alive\n\n",
                            HTTP_OK, date, image->type, image->length+1) < 0) {
            perror("error in sprintf\n");
            return "";
        }

        //file_length = image->length;
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

    /*if ((reply = (char *) malloc(strlen(header)+file_length+1))==NULL) {
        perror("error in malloc\n");
        return "";
    }*/

    /*if (sprintf(reply,"%s",header)<0){
        perror("error in sprintf\n");
        return "";
    }*/

    //return reply;
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

    //for (;;) {
        if (readline(connfd, buff, (int) MAXLINE) == 0) {
            printf("Client quit connection\n");
            return;
        }

        pid_t p = getpid();
        printf("%d sending response...\n", p);

        /*  requested home page of server content   */
        if (strcmp(result, INDEX) == 0) {

            char *home = composeHomePage(images);

            image->length = strlen(home);
            image->height = (size_t) IMAGESNUM;

            ssize_t n;

            header = composeHeader(result, image);
            write(connfd, header, strlen(header));

            //n = write(connfd, home, strlen(home));
            //printf("Bytes sent %d \n", (int) n);
            while ((n = write(connfd, home, strlen(home))) > 0) {
                printf("Bytes sent %d \n", (int) n);
            }

        } else {

            FILE *imgfd;
            char path[MAXLINE];
            sprintf(path, "%s%lu.%s", CACHE_PATH, image->name_code, image->type);

            if ((imgfd = fopen(path, "rb")) == NULL) {
                perror("error in fopen\n");
                strcpy(result, HTTP_BAD_REQUEST);
            }

            size_t n;
            header = composeHeader(result, image);

            write(connfd, header, strlen(header));

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

                        //if (n < MAXLINE) {
                        if (feof(imgfd)) {
                            printf("End of file \n ");
                            break;
                        }
                        if (ferror(imgfd)) {
                            printf("Error Reading \n");
                            break;
                        }
                            //break;
                        //}
                    }
                }
            } else if (strcmp(result, HTTP_NOT_FOUND) == 0) {
                write(connfd, HTML_404, strlen(HTML_404));
            } else if (strcmp(result, HTTP_BAD_REQUEST) == 0) {
                write(connfd, HTML_400, strlen(HTML_400));
            }
        }
    //}
}
