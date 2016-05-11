/*
 Created by laura_trive on 14/03/16.

Interface to function to images adaption.

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

/*  Function prototypes  */

/*  Composition of the command to execute for manipulating image, based on characteristics in input. */
unsigned long adapt(struct img *req_image, struct conv_img *adaptImg);


/*  This function adapts image's characteristics to client's device,
 *  optimizing width, length, number of colors and quality.
 */
struct conv_img *adaptImageTo(struct req *request);

#endif //WEBSERVER_MANAGE_IMG_H
