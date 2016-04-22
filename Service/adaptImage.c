/*
 Created by laura_trive on 13/03/16.

 Implementations of functions to images adaptation
*/

#include <wand/MagickWand.h>
#include "adaptImage.h"

/*  Manipulation of original image, based on characteristics specified in input.
 *
 * @param: req_image = requested image;
 * @param: adaptImg = image adapted on device characteristics
 */
unsigned long adapt(struct img *req_image, struct conv_img *adaptImg)
{
    char filename[MAXLINE];
    sprintf(filename, "%s%s.%s", PATH, req_image->name, req_image->type);

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
    // hash sulla stringa formata da: <nomeoriginale><width><height><q><type><c> per indicare univocamente l'immagine modificata
    // caratteristica origianel o  0 se elemento non modificato
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
        if (status == MagickFalse) {
            perror("error in scaling image\n");
            return 400; // ERROR
        }
    }

    /* For the JPEG and MPEG image formats, quality is
    * 1 (lowest image quality and highest compression)
    * to 100 (best quality but least effective compression) */
    status = MagickSetCompressionQuality(magickWand, (size_t) adaptImg->quality);
    if (status == MagickFalse) {
        perror("error in compressing image quality\n");
        return 400; // ERROR
    }

    sprintf(destination,"%s%ld.%s",CACHE_PATH,adaptImg->name_code,req_image->type);

    MagickWriteImage(magickWand,destination);

    DestroyMagickWand(magickWand);

    return 200; // OK
}

/*  This function adapts image's characteristics to client's device,
 *  optimizing width, length, number of colors and quality.
 *
 *  @param image: image to adapt, containing info of requested quality and type
 *  @param request: HTTP request to get device's and requested file's info (size and quality and format) from
 */
struct conv_img *adaptImageTo(struct img *req_image, struct req *request)
{
    struct conv_img *adaptedImg;
    if ((adaptedImg=malloc(sizeof(struct conv_img)))==NULL) {
        perror("error in malloc");
        return NULL;
    }
    unsigned char nameToHash[256];
    unsigned long hashcode;

    /*  Load information about image from database   */
    db_get_image_by_name(request->resource,req_image);
    /*  Check if image is in database else return NOT FOUND */
    if (strcmp(request->resource,req_image->name)!=0) {
        adaptedImg->name_code = 404; // NOT FOUND
        return adaptedImg;
    }

    adaptedImg->quality = (size_t) request->quality*100;
    //adaptedImg->quality = 80;
    //sprintf(adaptedImg->type,"jpg");
    sprintf(adaptedImg->type,request->type);
    // TODO da ottenere con WURFL usando useragent
    adaptedImg->width = 200;
    adaptedImg->height = 200;
    adaptedImg->length = adaptedImg->width*adaptedImg->height;

    // string <nomeoriginale><width><height><q100><type> to hash
    sprintf((char *)nameToHash, "%s%ld%ld%ld%s",
            req_image->name, adaptedImg->width,adaptedImg->height,adaptedImg->quality,adaptedImg->type);

    adaptedImg->name_code = getHashCode(nameToHash);

    char *date = getTodayToSQL();
    sprintf(adaptedImg->last_modified,date);

    if (!isInCache(adaptedImg->name_code)) {
        adapt(req_image, adaptedImg);
        /*  add adapted img to server cache */
        db_insert_img(NULL,adaptedImg);
    } else {
        /*  update last modified date at img in cache   */
        updateDate(adaptedImg);
    }

    printf("end adaptation...\n");

    return adaptedImg;
}