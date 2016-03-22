/*
 Created by laura_trive on 13/03/16.

 Implementations of functions to images adaptation
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "adaptImage.h"
#include "helper.h"
#include "caching.h"
#include "constants.h"
#include "calendar.h"


/**
 *  This function executes a system command line.
 *
 *  @param command: command string to execute on terminal
 */
int execCommand(const char *command)
{
    // execution of command if system is available
    if (system(NULL)==0 || system(command)==-1) {
        perror("error in system\n");
        return 1;
    }
    return 0;
}

/*
 * For the JPEG and MPEG image formats, quality is
 * 1 (lowest image quality and highest compression)
 * to 100 (best quality but least effective compression)
 *
 * @param image = original image with modified field of quality for manipulation
 */
struct img convertQuality(struct img *image)
{
    size_t n = strlen(image->name);
    int i;
    // converted image's path
    char newName[n+4];
    // command line
    char command[2*n+50];
    // percent quality factor
    int factor = (int) (image->quality * 100);

    // converted image's path
    if (sprintf(newName,"%s%d",image->name,factor)<0) {
        perror("error in sprintf");
        return NULL;
    }

    if (sprintf(command, "convert %s.%s -quality %d%% %s%s.%s\n",
                image->name,image->type,factor,CACHE_PATH,newName,image->type)<0) {
        perror("error in sprintf");
        return NULL;
    }
    while (execCommand(command)==1){}
    image->name = newName;
    image->date = getToday();
    return image;
}

/**
 *  This function convert type of the image, creating a copy.
 *
 *  @param image = original image
 *  @param newType = string with new type of image for conversion
 */
struct img *convertType(struct img *image, const char *newType)
{
    size_t n = strlen(image->name);
    char command[2*n+20];

    if (sprintf(command, "convert %s.%s %s%s.%s",image->name,image->type,CACHE_PATH,image->name,newType)<0) {
        perror("error in sprintf");
        return NULL;
    }
    while (execCommand(command)==1){}
    image->type = newType;
    image->date = getToday();
    return image;
}

/**
 * This function create a copy resize of the original image.
 *
 * @param image = original image with width and height fields modified with value fot resizing
 */
struct img *resize(struct img *image)
{
    size_t  n = strlen(image->name);
    //command line
    char command[n+50];
    // copy image's path
    char newName[n+10];
    if (sprintf(newName,"%s%dx%d",image->name,width,height,image->type)<0) {
        perror("error in sprintf");
        return NULL;
    }

    if (sprintf(command, "convert %s.%s -resize %dx%d %s%s.%s\n",
                image->name,image->type,width,height,CACHE_PATH,newName,img->type)<0) {
        perror("error in sprintf");
        return NULL;
    }
    while (execCommand(command)==1){}
    image->name = newName;
    image->width = width;
    image->heigth = height;
    image->date = getToday();
    return image;
}

/**
 * This function creates a copy of the original image with a reduction of
 * the number of colors in.
 * The color reduction also can happen automatically when saving images
 * to color-limited image file formats, such as GIF, and PNG8.
 *
 * @param image = original image
 * @param colors = number of image's colors
 */
struct img *reduceColors(struct img *image, int colors)
{
    //command line
    char command[MAX_LINE];
    // copy image's path
    size_t  n = strlen(image->name);
    char newName[n+10];
    if (sprintf(newName,"%s_no%d",image->name,colors)<0) {
        perror("error in sprintf\n");
        return NULL;
    }

    if (sprintf(command, "convert %s -dither None -colors %d %s%s.%s\n",filename,colors,CACHE_PATH,newName,image->type)<0) {
        perror("error in sprintf\n");
        return NULL;
    }
    while (execCommand(command)==1){}
    image->name = newName;
    image->date = getToday();
    return image;
}