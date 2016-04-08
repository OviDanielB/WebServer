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
#include "helper/calendar.h"

/*  Check if the original image was modified after the one saved in cache.
 *
 * @param *img: pointer to original img struct
 * @param *cacheImg: pointer to img struct present in cache
 */
int isModified(struct img *img, struct img *cacheImg)
{
    if (cmpDates(img->last_modified,cacheImg->last_modified)!=-1) {
        return 0; //false
    }
    return 1; //true
}

/*  Checks if the image requested is present in cache.
 *  If yes, the return value is the name of
 *
 * @param *img: pointer to struct img to search in cache directory
 */
char *isInCache(struct img *img) {

    // select su db_cache per vedere se presente img->name con le caratteristiche
    if (sqlite3_) {

        return img->name;
    } else
        return NULL;
}

/*  Check if the image requested is present in cache
 *
 * @param *img: pointer to struct img to search in cache directory
 */
/*FILE *isInCache(struct img *img)
{
    return fopen("%s%s.%s",CACHE_PATH,img->name,img->type,"r");
}*/


