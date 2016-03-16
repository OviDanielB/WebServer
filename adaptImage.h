/*
 Created by laura_trive on 14/03/16.

Interface to function to images adaption.

*/

#ifndef WEBSERVER_MANAGE_IMG_H
#define WEBSERVER_MANAGE_IMG_H

/*  Function prototypes  */

int convertQuality(const char *filename, float q);

int convertType(const char *filename, char *type);

int resize(const char *filename, int size1, int size2);

int reduceColors(const char *filename, int colors);

#endif //WEBSERVER_MANAGE_IMG_H
