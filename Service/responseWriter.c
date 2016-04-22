//
// Created by laura_trive on 04/04/16.
//

/**
 * Implementations of functions to send server HTTP responses.
 */

#include "responseWriter.h"

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
    sprintf(path,"%s%ld.%s", CACHE_PATH, image->name_code, image->type);
    if ((cachedImg=fopen(path, "rb"))==NULL) {
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
    if ((imgfd=openCachedImage(image))!=NULL) {
        return imgfd;
    }
    sprintf(path,"%s%s.%s", PATH, image->original_name, image->type);

    if ((imgfd=fopen(path, "rb"))==NULL) {
        perror("error in fopen\n");
        return NULL;
    } else {
        return imgfd;
    }
}

/*  Compose response HTTP message to send to the client */
char *composeHeader(char *result, struct conv_img *image)
{
    char *reply;
    char header[102400];
    long file_length = 0;
    char *date = getTodayToHTTPLine();

    if (strcmp(result,HTTP_OK)==0) {
        if (sprintf(header,
                            "%s\n"
                            "Date: %s\n"
                            "Server: WebServer/1.0.0\n"
                            "Content-Type: image/%s\n"
                            "Content-Length: %ld\n"
                            "Connection: keep-alive\n\n",
                            HTTP_OK, date, image->type, image->length) < 0) {
            perror("error in sprintf\n");
            return "";
        }

        file_length = image->length;
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

    if ((reply=malloc(strlen(header)+file_length))==NULL) {
        perror("error in malloc\n");
        return "";
    }

    if (sprintf(reply,"%s",header)<0){
        perror("error in sprintf\n");
        return "";
    }

    return reply;
}

/* Sending server response, based on result of the elaboration of request.
 *
 * @param: connfd = file descriptor of connection socket
 * @param: result = result of the elaboration of client's request
 * @param: image = image to send as data in the message
 */
void writeResponse(int connfd, char *result, struct conv_img *image)
{
    FILE *imgfd = openImage(image);
    if (imgfd==NULL) {
        strcpy(result,HTTP_BAD_REQUEST);
    }
    char *header = composeHeader(result, image);
    char buff[MAXLINE];
    size_t n;

    write(connfd, header, strlen(header));

    if (strcmp(result, HTTP_OK) == 0) {

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
                }
                if (ferror(imgfd)) {
                    printf("Error Reading \n");
                }
                break;
            }
        }
    } else if (strcmp(result, HTTP_NOT_FOUND) == 0) {
        write(connfd, HTML_404, strlen(HTML_404));
    } else if (strcmp(result, HTTP_BAD_REQUEST) == 0) {
        write(connfd, HTML_400, strlen(HTML_400));
    }
}
