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
#include "../helper/io_func.h"
#include "../helper/helper.h"


char *composeHomePage(struct img **images, struct conv_img *info);

/*  Compose header of HTTP message to send to the client, based on result of request's elaboration */
char *composeHeader(char *result, struct conv_img *image);

/*  This function sends server response, based on result of the request's elaboration */
void writeResponse(int connfd, char *result, char *method, struct conv_img *image, struct img **images);

#endif //WEBSERVER_RESPONSE_H
