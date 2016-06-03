//
// Created by laura_trive on 18/05/16.
//

#ifndef WEBSERVER_WURFL_H
#define WEBSERVER_WURFL_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../constants.h"

void getDeviceByUserAgent(char *userAgent, struct device *device);

void initializeFifo(pid_t *pids);

#endif //WEBSERVER_WURFL_H
