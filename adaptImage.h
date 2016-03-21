/*
 Created by laura_trive on 14/03/16.

Interface to function to images adaption.

*/

#ifndef WEBSERVER_MANAGE_IMG_H
#define WEBSERVER_MANAGE_IMG_H

#include "constants.h"

/*  Function prototypes  */

int execCommand(const char *command);

struct img convertQuality(struct img *image);

struct img convertType(struct img *image, const char *newType);

struct img resize(struct img *image);

struct img reduceColors(struct img *image, int colors);

#endif //WEBSERVER_MANAGE_IMG_H
