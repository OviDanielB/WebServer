/**
 * Functions prototypes to parse lines of client's HTTP message request.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../constants.h"
#include "../helper/helper.h"

#ifndef WEBSERVER_REQUESTPARSER_H
#define WEBSERVER_REQUESTPARSER_H

/* HTTP request parser reading information about method requested, client's device,
 * type and quality of file requested */
struct req *parseRequest(int sockFd);

#endif //WEBSERVER_REQUESTPARSER_H
