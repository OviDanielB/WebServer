/**
 * Prototypes of functions to send server HTTP responses.
 */


#ifndef WEBSERVER_RESPONSE_H
#define WEBSERVER_RESPONSE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../constants.h"
#include "../helper/calendar.h"
#include "../helper/helper.h"

/*  Compose index page to offer to client the possibility to choose which resource request
 * in a list of all server images   */
char *composeHomePage(struct img **images, struct conv_img *info);

/*  Compose header of HTTP message to send to the client, based on result of request's elaboration
 * and length of file to send   */
char *composeHeader(char *result, struct conv_img *image);

/*  Write server response on the socket, based on result of the request's elaboration */
void writeResponse(int connfd, char *result, char *method, struct conv_img *image, struct img **images);

#endif //WEBSERVER_RESPONSE_H
