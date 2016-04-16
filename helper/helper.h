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


/*  Check jpg type    */
static int jpg(char *ext)
{
    if ((strcmp(ext,"jpg")==0) || (strcmp(ext,"JPG")==0) || (strcmp(ext,"JPEG")==0) || (strcmp(ext,"jpeg")==0)) {
        return 1; // true
    }
    return 0; // false
}

/*  Executes a system command line  */
int execCommand(const char *command);

/*  Read result line from STDOUT after the execution of a command. */
char *readResultLine(char *cmd);

/* Hash function to calculate an (almost) unique identifier for every manipulated image
 * from the resource name, where are indicated the adapted values or the original ones.  */
unsigned long getHashCode(unsigned char *name);

#endif //WEBSERVER_HELPER_H
