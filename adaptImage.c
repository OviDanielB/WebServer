/*
 Created by laura_trive on 13/03/16.

 Implementations of functions to images adaptation
*/

#include <stdio.h>
#include <string.h>

#include "adaptImage.h"


/**
 *  This function executes system command.
 *
 *  @param command = command to execute
 */
int execCommand(const char *command) {
    // check terminal availability
    while (system(NULL)==0) {}

    // execution of command
    if (system(command)==-1) {
        perror("error in system");
        return 1;
    }
    return 0;
}

/*
 * For the JPEG and MPEG image formats, quality is
 * 1 (lowest image quality and highest compression)
 * to 100 (best quality but least effective compression)
 *
 * @param q = decimal quality factor
 * @param filename = original image's path */
int convertQuality(const char* filename, float q) {
    size_t n = strlen(filename);
    int i;
    // converted image's path
    char newFilename[n+4];
    // filename without extension type
    char nameCut[n-4];
    strncpy(nameCut,filename,n-4);
    // image's extension type
    char type[3] = {filename[n-3],filename[n-2],filename[n-1]};
    // command line
    char command[2*n+50];

    // percent quality factor
    int factor = (int) (q * 100);

    // converted image's path
    if (sprintf(newFilename,"%s%d.%s",nameCut,factor,type)<0) {
        perror("error in sprintf");
        return 1;
    }

    if (sprintf(command, "convert %s -quality %d%% %s\n",filename, factor, newFilename)<0) {
        perror("error in sprintf");
        return 1;
    }
    return execCommand(command);
}

/**
 *  This function convert type of the image, creating a copy.
 *
 *  @param filename = image path
 *  @param type = extension of the new image to convert
 */
int convertType(const char* filename, char *type) {
    size_t n = strlen(filename);
    char command[150];

    // filename without extension type
    char nameCut[n-4];
    strncpy(nameCut,filename,n-4);

    if (sprintf(command, "convert %s %s.%s",filename, nameCut,type)<0) {
        perror("error in sprintf");
        return 1;
    }
    return execCommand(command);
}

/**
 * This function create a copy resize of the original image.
 *
 * @param filename = image's path
 * @param width = new width of images
 * @param height = new height of images
 */
int resize(const char *filename, int width, int height) {
    size_t  n = strlen(filename);
    //command line
    char command[n+50];
    // filename without extension type
    char nameCut[n-4];
    strncpy(nameCut,filename,n-4);
    // image's extension type
    char type[3] = {filename[n-3],filename[n-2],filename[n-1]};
    // copy image's path
    char newFilename[n+10];
    if (sprintf(newFilename,"%s%dx%d.%s",nameCut,width,height,type)<0) {
        perror("error in sprintf");
        return 1;
    }

    if (sprintf(command, "convert %s -resize %dx%d %s\n",filename,width,height,newFilename)<0) {
        perror("error in sprintf");
        return 1;
    }
    return execCommand(command);
}

/**
 * This function creates a copy of the original image with a reduction of
 * the number of colors in.
 * The color reduction also can happen automatically when saving images
 * to color-limited image file formats, such as GIF, and PNG8.
 *
 * @param filename = image path
 * @param colors = number of image's colors
 */
int reduceColors(const char *filename, int colors) {
    size_t  n = strlen(filename);
    //command line
    char command[n+50];
    // filename without extension type
    char nameCut[n-4];
    strncpy(nameCut,filename,n-4);
    // image's extension type
    char type[3] = {filename[n-3],filename[n-2],filename[n-1]};
    // copy image's path
    char newFilename[n+10];
    if (sprintf(newFilename,"%s_no%d.%s",nameCut,colors,type)<0) {
        perror("error in sprintf");
        return 1;
    }

    if (sprintf(command, "convert %s -dither None -colors %d %s\n",filename,colors,newFilename)<0) {
        perror("error in sprintf");
        return 1;
    }
    return execCommand(command);
}