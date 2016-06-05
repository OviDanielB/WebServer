/**
 * Prototypes of functions to manage FIFO communication between processes.
 */

#ifndef WEBSERVER_WURFL_H
#define WEBSERVER_WURFL_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../constants.h"

/*  Request of the characteristics supported by the client device
 * describes by the given user agent string  */
void getDeviceByUserAgent(char *userAgent, struct device *device);

/*  Creation of a new PHP process and two FIFO file to communication
 * between the new process and the children ones */
void initializeFifo(pid_t *pids);

#endif //WEBSERVER_WURFL_H
