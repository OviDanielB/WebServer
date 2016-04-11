//
// Created by laura_trive on 21/03/16.
//

/**
 *  Implementation of helper functions.
 */

#include "helper.h"

/*  Get extension of a filename.    */
char *readExtension(const char *filename)
{
    return strrchr(filename,'.')+1;
}

/*  Get filename without extension.     */
char *cutFilename(const char *filename)
{
    int n = strlen(filename)-strlen(readExtension(filename)-1);
    char nameCut[n];
    return strncpy(nameCut,filename,n);
}
