//
// Created by laura_trive on 23/03/16.
//

/*
 * Functions prototypes to parse lines of client's HTTP request.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../constants.h"
#include "../helper/io_func.h"

#ifndef WEBSERVER_REQUESTPARSER_H
#define WEBSERVER_REQUESTPARSER_H

/* This function implement the HTTP request parser,
 * reading information about method requested, client's device,
 * type and quality of file requested.
 */
struct req *parseRequest(int sockfd);

#endif //WEBSERVER_REQUESTPARSER_H
