//
// Created by laura_trive on 21/03/16.
//

/**
 *  Implementation of helper functions.
 */

#include "helper.h"

/* Hash function to calculate an (almost) unique identifier for every manipulated image
 * from the resource name, where are indicated the adapted values or the original ones.
 *
 * @param: name = string formatted as <originalname><width><height><qualityfactor><type><colorsnum>
 */
unsigned long getHashCode(unsigned char *name)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *name++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

size_t dim[2];
/*  Calculation of image's dimensions, maintaining the aspect ratio
 *
 * @param original_w: original image width
 * @param original_h: original image height
 * @param adapted_w: image width requested
 * @param adapted_h: image height requested
 */
size_t *proportionalSize(size_t original_w, size_t original_h, size_t adapted_w, size_t adapted_h)
{
    double orw = (double) original_w;
    double orh = (double) original_h;
    double adw = (double) adapted_w;
    double adh = (double) adapted_h;
    double r = orw/orh;

    if (adw/adh == r) {
        dim[0] = adapted_w , dim[1] = adapted_h;
        return dim;
    } else if (original_w>=original_h){
        dim[0] = adapted_w;
        dim[1] = (size_t) adw/r;
        return dim;
    }
    dim[0] = (size_t) adh*r;
    dim[1] = adapted_h;
    return dim;
}

/*  Read name and extension of a file   */
void readNameAndType(char *s, char *name, char *ext)
{
    if (memset(name,'\0',256)==NULL) {
        perror("memset error\n");
        exit(EXIT_FAILURE);
    }
    if (memset(ext,'\0',4)==NULL) {
        perror("memset error\n");
        exit(EXIT_FAILURE);
    }

    char *p = s;

    int i=0;
    while (*p!='.') {
        name[i] = *p;
        i++;
        p++;
    }
    p++;
    i=0;
    while (*p!='\0') {
        ext[i] = *p;
        i++;
        p++;
    }
}