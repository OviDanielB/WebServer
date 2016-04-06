//
// Created by laura_trive on 04/04/16.
//

/**
 * Prototypes of functions to send server HTTP responses.
 */


#ifndef WEBSERVER_RESPONSE_H
#define WEBSERVER_RESPONSE_H

/*  Compose header of HTTP message to send to the client */
char *composeHeader(char *result, struct img *image);

/*  Send the HTTP response, composed by header and eventual data,
 * to the client.
 */
void writeResponse(int connfd, char *result, struct img *image);

#endif //WEBSERVER_RESPONSE_H
