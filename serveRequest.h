//
// Created by laura_trive on 06/04/16.
//

/**
 * Prototypes of functions to do server work for reading client's HTTP request,
 * serve it, adapting the response to client's device, and sends an HTTP response
 * back to the client.
 */

#ifndef WEBSERVER_SERVEREQUEST_H
#define WEBSERVER_SERVEREQUEST_H

/*  Read from connection socket the client request and
 *  write on it the response, adapted on client device. */
void serveRequest(int sockfd);

#endif //WEBSERVER_SERVEREQUEST_H
