//
// Created by laura_trive on 21/03/16.
//

/**
 * Functions prototypes to manage server cache of previously manipulated images.
 */

#ifndef WEBSERVER_CACHING_H
#define WEBSERVER_CACHING_H

/*  Check if the image was modified  */
boolean isModified(struct img *img);

/*  Check if the image requested is present in cache    */
boolean isInCache(struct img *img);


#endif //WEBSERVER_CACHING_H
