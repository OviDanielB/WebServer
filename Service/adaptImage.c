/*
 Created by laura_trive on 13/03/16.

 Implementations of functions to images adaptation
*/

#include <wand/MagickWand.h>
#include "adaptImage.h"
#include "../php/wurfl.h"

/*  Manipulation of original image, based on characteristics specified in input.
 *
 * @param: req_image = requested image;
 * @param: adaptImg = image adapted on device characteristics
 */
unsigned long adapt(struct img *req_image, struct conv_img *adaptImg)
{
    char filename[MAXLINE];
    sprintf(filename, "%s%s.%s", PATH, req_image->name, req_image->type);

//    MagickWandGenesis();
    MagickWand *magickWand = NULL;
    magickWand = NewMagickWand();

    MagickBooleanType status;

    MagickResetIterator(magickWand);
    status = MagickReadImage(magickWand, filename);
    if (status == MagickFalse) {
        perror("error in reading image");
        return 400; // ERROR
    }

    char tmpFilename[MAXLINE];
    char destination[MAXLINE];


    // check if is requested JPG format to convert before manipulating, avoiding data loss due to compression.
    if (jpg(adaptImg->type)){
        sprintf(tmpFilename,"/tmp/%s.gif",req_image->name);

        status = MagickWriteImage(magickWand,tmpFilename);
        if (status == MagickFalse) {
            perror("error in writing image\n");
            return 400; // ERROR
        }

        status = MagickRemoveImage(magickWand);
        if (status == MagickFalse) {
            perror("error in removing image\n");
            return 400; // ERROR
        }

        MagickResetIterator(magickWand);
        status = MagickReadImage(magickWand,tmpFilename);
        if (status == MagickFalse) {
            perror("error in reading image\n");
            return 404; // NOT FOUND
        }
    }

    /*  resizing original image if it's necessary    */
    if (req_image->width!=adaptImg->width || req_image->height!=adaptImg->height) {
        size_t *dim = proportionalSize(req_image->width,req_image->height,adaptImg->width,adaptImg->height);
        status = MagickScaleImage(magickWand,dim[0],dim[1]);
        /*ExceptionInfo exception;
        Image *image= GetImageFromMagickWand(magickWand);
        ThumbnailImage(image,dim[0],dim[1],&exception);*/
        if (status == MagickFalse) {
            perror("error in scaling image\n");
            return 400; // ERROR
        }
    }

    /*  reducing number of original image's number of colors if it's necessary    */
    if (adaptImg->colors != 0) {
        status = MagickQuantizeImage(magickWand, adaptImg->colors, RGBColorspace, 1, MagickTrue, MagickFalse);
        if (status == MagickFalse) {
            perror("error in reducing image's colors\n");
            return 400; // ERROR
        }
    }


    /* using factor of quality just if specified into the request for JPEG images   */
    if (adaptImg->quality != 0) {
        /* For the JPEG and MPEG image formats, quality is
        * 1 (lowest image quality and highest compression)
        * to 100 (best quality but least effective compression) */
        status = MagickSetCompressionQuality(magickWand, (size_t) adaptImg->quality);
        if (status == MagickFalse) {
            perror("error in compressing image quality\n");
            return 400; // ERROR
        }
    }

    sprintf(destination,"%s%lu.%s",CACHE_PATH,adaptImg->name_code,req_image->type);

    printf("destination: %s\n",destination);

    MagickWriteImage(magickWand,destination);

    DestroyMagickWand(magickWand);
    //MagickWandTerminus();

    return 200; // OK
}

/*  This function adapts image's characteristics to client's device,
 *  optimizing width, length, number of colors and quality.
 *
 *  @param image: image to adapt, containing info of requested quality and type
 *  @param request: HTTP request to get device's and requested file's info (size and quality and format) from
 */
struct conv_img *adaptImageTo(struct req *request)
{
    struct conv_img *adaptedImg;
    if ((adaptedImg = (struct conv_img *) malloc(sizeof(struct conv_img)))==NULL) {
        perror("error in malloc");
        return NULL;
    }
    struct img *req_image = (struct img *) malloc(sizeof(struct img));
    if (req_image== NULL) {
        perror("error in malloc\n");
        exit(EXIT_FAILURE);
    }

    unsigned char nameToHash[256];

    /*  Load information about image from database   */
    dbGetImageByName(request->resource, req_image);

    /*  Check if image is in database else return NOT FOUND */
    if (strcmp(request->resource, req_image->name) != 0) {
        adaptedImg->name_code = 404; // NOT FOUND
        return adaptedImg;
    }

    if (request->quality == -1) {
        adaptedImg->quality = 0;
    } else {
        adaptedImg->quality = (size_t) request->quality*100;
        adaptedImg->name_code = 400; // BAD REQUEST: requested format not supported by device
        return adaptedImg;
    }

    sprintf(adaptedImg->original_name, req_image->name);

    /*int write_fd = openChildOwnFifo_w();
    int read_fd = openChildOwnFifo_r();*/

    struct device *dev = malloc(sizeof(struct device));
    if (dev == NULL) {
        perror("Malloc error\n");
        exit(EXIT_FAILURE);
    }

    /*  check if User-Agent just inserted in database   */
    if (isUserAgentKnown(request->userAgent)) {
        /*  if User-Agent is known, get information saved in database   */
        dbGetDeviceByUserAgent(request->userAgent, dev);
    } else {
        /*  query PHP process to get device's information  */
        getDeviceByUserAgent(request->userAgent, dev);
        printf("device info: w %ld h %ld ...\n", dev->width, dev->height);
        /*  update database with new User-Agent */
        dbInsertUserAgent(request->userAgent, dev);
    }

    /*  check if format is available for devices info found */
    if (dev->jpg && jpg(req_image->type) ||
            dev->png && png(req_image->type) ||
            dev->gif && gif(req_image->type)) {
        sprintf(adaptedImg->type, req_image->type);
    } else {

    }

    if (dev->width != 0) {
        adaptedImg->width = dev->width;
    } else {
        adaptedImg->width = req_image->width;
    }
    if (dev->height != 0) {
        adaptedImg->height = dev->height;
    } else {
        adaptedImg->height = req_image->height;
    }
    if (dev->colors != 0) {
        adaptedImg->colors = (size_t) dev->colors;
    } else {
        adaptedImg->colors = 0;
    }
    adaptedImg->length = adaptedImg->width*adaptedImg->height;

    // string <nomeoriginale><width><height><colors><q100><type> to hash
    sprintf((char *)nameToHash, "%s%ld%ld%ld%ld%s",
            adaptedImg->original_name, adaptedImg->width, adaptedImg->height,
            adaptedImg->colors, adaptedImg->quality,adaptedImg->type);

    adaptedImg->name_code = getHashCode(nameToHash);

    char *date = getTodayToSQL();
    sprintf(adaptedImg->last_modified,date);

    printf("before check cache...\n");

    if (!isInCache(adaptedImg)) {

        unsigned long res = adapt(req_image, adaptedImg);
        /*   check result of adaptation */
        if (res != 200) {
            /* if not OK, return error code */
            adaptedImg->name_code = res;
            dbDeleteByImageName(adaptedImg->original_name, adaptedImg->name_code); // if error in adapting, to delete from database
        } /*else {
            /*  add adapted img to server cache /
            dbInsertImg(NULL,adaptedImg);
        }*/
    } else {
        /*  update last modified date at img in cache   */
        updateDate(adaptedImg);
    }

    return adaptedImg;
}