/**
 * Created by laura_trive on 21/03/16.
 *
 * Helper functions prototypes.
*/

#ifndef WEBSERVER_HELPER_H
#define WEBSERVER_HELPER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <zconf.h>

#include "../constants.h"

/*  Check gif type    */
static int gif(char *ext)
{
    if (strcmp(ext,"gif") == 0 || strcmp(ext,"GIF") == 0) {
        return TRUE;
    }
    return FALSE;
}

/*  Check png type    */
static int png(char *ext)
{
    if ( strcmp(ext,"png") == 0 || strcmp(ext,"PNG") == 0) {
        return TRUE;
    }
    return FALSE;
}

/*  Check jpg type    */
static int jpg(char *ext)
{
    if (strcmp(ext,"jpg") == 0 || strcmp(ext,"JPG") == 0 || strcmp(ext,"JPEG") == 0 || strcmp(ext,"jpeg") == 0) {
        return TRUE;
    }
    return FALSE;
}

/* Hash function to calculate an (almost) unique identifier for every manipulated image
 * from the resource name, where are indicated the adapted values or the original ones.  */
unsigned long getHashCode(unsigned char *name);

char *getServerIp();

/*  Calculation of image's dimensions, maintaining the aspect ratio  */
size_t *proportionalSize(size_t original_w, size_t original_h, size_t adapted_w, size_t adapted_h);

/*  Read name and extension of a file   */
void readNameAndType(char *s, char *name, char *ext);

#endif //WEBSERVER_HELPER_H
