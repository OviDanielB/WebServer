//
// Created by laura_trive on 21/03/16.
//

/**
 *  Implementation of helper functions.
 */

#include "helper.h"

/*  Check jpg type    */
/*static int jpg(char *ext)
{
    if ((strcmp(ext,"jpg")==0) || (strcmp(ext,"JPG")==0) || (strcmp(ext,"JPEG")==0) || (strcmp(ext,"jpeg")==0)) {
        return 1; // true
    }
    return 0; // false
}*/

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
