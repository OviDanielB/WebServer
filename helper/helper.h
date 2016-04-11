/**
 * Created by laura_trive on 21/03/16.
 *
 * Helper functions prototypes.
*/

#ifndef WEBSERVER_HELPER_H
#define WEBSERVER_HELPER_H

#include <stdio.h>
#include <string.h>

/*  Get extension of a filename.    */
char *readExtension(const char *filename);

/*  Get filename without extension.     */
char *cutFilename(const char *filename);

#endif //WEBSERVER_HELPER_H
