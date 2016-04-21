//
// Created by ovi on 4/7/16.
//

#ifndef WEBSERVER_DB_CACHE_H
#define WEBSERVER_DB_CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include "../constants.h"
#include "wand/magick_wand.h"
#include "../Service/adaptImage.h"


/*  Check if server cache is full (CONV_IMG has reached max number of rows) */
int isFull(sqlite3 *db);
/*  Check if searched manipulated image has been just inserted in server cache. */
int isInCache(unsigned long hashcode);

#endif //WEBSERVER_DB_CACHE_H
