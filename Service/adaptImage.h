/**
 * Prototypes of functions for images adaption.
*/

#ifndef WEBSERVER_MANAGE_IMG_H
#define WEBSERVER_MANAGE_IMG_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../helper/helper.h"
#include "../constants.h"
#include "../helper/calendar.h"
#include "../DataBase/db_helper.h"
#include <wand/MagickWand.h>
#include "../php/wurfl.h"

/*  Adaptation of image's based on:
 * - characteristics to client's device, optimizing width, length, number of colors
 * - requested quality for image/jpeg
 */
unsigned long adapt(struct img *req_image, struct conv_img *adaptImg);

/*  Start of adaptation process based on receives request   */
struct conv_img *adaptImageTo(struct req *request);

#endif //WEBSERVER_MANAGE_IMG_H
