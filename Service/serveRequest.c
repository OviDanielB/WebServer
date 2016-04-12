//
// Created by laura_trive on 06/04/16.
//

/**
 * Implementations of functions to do server work for reading client's HTTP request,
 * serve it, adapting the response to client's device, and sends an HTTP response
 * back to the client.
 */

#include "serveRequest.h"


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
    //long hashcode = getHashCode(image->n)
    sprintf(path,"%s%ld.%s", CACHE_PATH, image->name_code, image->type);
    if ((cachedImg=fopen(path, "rb"))==NULL) {
        sprintf(result, (char *)HTTP_NOT_FOUND);
        //writeResponse(sockfd, result, image, cachedImg);
        perror("error in fopen\n");
        return NULL;
        //exit(1);
    } else {
        sprintf(result, (char *)HTTP_OK);
        return cachedImg;
    }
}

/*  this function opens original image file and if that exists, it returns the file descriptor.
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
        //exit(1);
    } else {
        return imgfd;
    }
}


/*  Get image file length   */
long getLength(FILE *image)
{
    long seek, len;
    if ((seek = ftell(image))==-1){
        perror("Error in ftell\n");
        exit(1);
    }
    if (fseek(image,0L,SEEK_END)==-1) {
        perror("Error in fseek\n");
        exit(1);
    }
    if ((len = ftell(image))==-1) {
        perror("Error in ftell\n");
        exit(1);
    }
    if (fseek(image,seek,SEEK_SET)==-1) {
        perror("Error in fseek\n");
        exit(1);
    }
    return len;
}

/*  This function implements server work for serving a client's request:
 *  1)Parsing client request
 *  2)Elaborating request (adaptation on client device)
 *  3)Caching
 *  4)Sending response
 *
 *  @param sockfd: file descriptor for connection socket
 */
void serveRequest(int sockfd)
{

    FILE * image;
    size_t n;
    int read;
    char result[50];

    //char *userAgent;
    struct req *request = parseRequest(sockfd);
    //userAgent = req->user_agent;
    //userAgent = "";
    struct img *reqImage;
    //struct img *reqImage = req->req_image;
    // requested image: {name, quality, width, height, type, last modified, file_length}
    if ((reqImage = malloc(sizeof(struct img)))==NULL) {
            perror("error in malloc\n");
            exit(1);
    }
    sprintf(reqImage->name,request->resource);
    //sprintf(reqImage->name,"mare");
    //reqImage->width = 960; //aggiunto dopo
    //reqImage->height = 600; //aggiunto dopo
    //sprintf(reqImage->type,"jpg");
    sprintf(reqImage->type,request->type);

    char buff[MAXLINE];
/*    char path[MAXLINE];
    sprintf(path,"%s%s.%s", PATH, reqImage->name, reqImage->type);

    //da cercare image su db (cache o server)

    if ((image=fopen(path, "rb"))==NULL) {
        sprintf(result, (char *)HTTP_NOT_FOUND);
        writeResponse(sockfd, result, reqImage, image);
        perror("error in fopen\n");
        return;
        //exit(1);
    }


    sprintf(result, (char *)HTTP_OK);

    reqImage->length = reqImage->width*reqImage->height; // 1 pixel = 1 byte*/
    //reqImage->length = getLength(image);

    //TODO adapting from WURFL info
    struct conv_img *adaptedImage = adaptImageTo(reqImage,request->quality,request->type,request->userAgent);

    if (adaptedImage==NULL) {
        sprintf(result,HTTP_BAD_REQUEST);
    } else {
        sprintf(result,HTTP_OK);
    }
    //add adapted image to db

    for(;;) {
        if ((read = readline(sockfd, buff, MAXLINE)) == 0) {
            printf("Client quit connection\n");
            return;
        }

        /*
        char *httpOK = "HTTP/1.1 200 OK\n";
        char *content = "Content-length: 52917\n";
        char *type = "Content-Type: image/jpeg\n\n";

        write(sockfd, httpOK, strlen(httpOK));
        write(sockfd, content, strlen(content));
        write(sockfd, type, strlen(type));
        */

        FILE *imgfd;
        if ((imgfd = openImage(adaptedImage)) == NULL)
            sprintf(result, (char *) HTTP_NOT_FOUND);
        else
            sprintf(result, (char *) HTTP_OK);

        //!!!!!!!!!!da mettere l'imm adattata come parametro e switch per l'esito
        writeResponse(sockfd, result, adaptedImage, imgfd);
        //writeResponse(sockfd, result, reqImage, image);
    }
}


