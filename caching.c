//
// Created by laura_trive on 21/03/16.
//

/**
 * Implementation of functions to manage cache of previously manipulated images
 */

#include <stdio.h>
#include <time.h>

#include "constants.h"
#include "caching.h"
#include "calendar.h"

/*  Check if the original image was modified after the one saved in cache.
 *
 * @param *img: pointer to origianal img struct
 * @param *cacheImg: pointer to img struct present in cache
 */
boolean isModified(struct img *img, struct img *cacheImg)
{
    if (cmpDates(img->last_modified,cacheImg->last_modified)!=-1) {
        return false;
    }
    return true;
}

/*  Check if the image requested is present in cache
 *
 * @param *img: pointer to struct img to search in cache directory
 */
boolean isInCache(struct img *img)
{
    FILE *s;
    if (s=fopen("%s%s.%s",CACHE_PATH,img->name,img->type,"r")==NULL) {
        return false;
    }
    fclose(s);
    return true;
}