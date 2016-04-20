//
// Created by laura_trive on 04/04/16.
//

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

/*  Compose header of HTTP message to send to the client, based on result of request's elaboration */
char *composeHeader(char *result, struct conv_img *image);

/*  This function sends server response, based on result of the request's elaboration */
void writeResponse(int connfd, char *result, struct conv_img *image);

#endif //WEBSERVER_RESPONSE_H
