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

