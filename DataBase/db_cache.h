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
#include "db_helper.h"


/* Update server cache checking for saturation of memory dedicated or after timeout
 * of cached image lifetime and update CACHE table in Database    */
void updateCache();
/*  Check if server cache is full (CONV_IMG has reached max number of rows) */
int isFull();
/*  Check if searched manipulated image has been just inserted in server cache. */
int isInCache(struct conv_img *img);
/*  Check if the searched user agent has been just inserted in database. */
int isUserAgentKnown(char *userAgent);
/*  Delete from CONV_IMG table the older image inserted (with greater lifetime)   */
void deleteByAge();
/*  Delete from CONV_IMG table all the image where lifetime is greater than LIFETIME value  */
void deleteByTimeout();
/*  Update date of the last access at that image in the server cache.   */
void updateDate(struct conv_img *adaptedImg);

#endif //WEBSERVER_DB_CACHE_H
