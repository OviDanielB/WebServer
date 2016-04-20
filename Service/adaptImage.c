/*
 Created by laura_trive on 13/03/16.

 Implementations of functions to images adaptation
*/

#include <wand/MagickWand.h>
#include "adaptImage.h"

size_t dim[2];
/*  Calculation of image's dimensions, maintaining the aspect ratio  */
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
        exit(1);
    }

    size_t h = req_image->height = MagickGetImageHeight(magickWand);
    size_t w = req_image->width = MagickGetImageWidth(magickWand);

    unsigned char nameToHash[256];
    unsigned long hashcode;
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
            return 0;
        }

        status = MagickRemoveImage(magickWand);
        if (status == MagickFalse) {
            perror("error in removing image\n");
            return 0;
        }

        MagickResetIterator(magickWand);
        status = MagickReadImage(magickWand,tmpFilename);
        if (status == MagickFalse) {
            perror("error in reading image\n");
            return 0;
        }
    }

    /*  resizing original image if it's necessary    */
    if (req_image->width!=adaptImg->width || req_image->height!=adaptImg->height) {
        proportionalSize(req_image->width,req_image->height,adaptImg->width,adaptImg->height);
        status = MagickScaleImage(magickWand,dim[0],dim[1]);
        if (status == MagickFalse) {
            perror("error in scaling image\n");
            return 0;
        }
        w = adaptImg->width;
        h = adaptImg->height;
    }

    /* For the JPEG and MPEG image formats, quality is
    * 1 (lowest image quality and highest compression)
    * to 100 (best quality but least effective compression) */
    status = MagickSetCompressionQuality(magickWand, (size_t) adaptImg->quality*100);
    if (status == MagickFalse) {
        perror("error in compressing image quality\n");
        return NULL;
    }

    // string <nomeoriginale><width><height><q100><type> to hash
    sprintf((char *)nameToHash, "%s%ld%ld%ld%s",req_image->name, w,h,adaptImg->quality,adaptImg->type);

    hashcode = getHashCode(nameToHash);
    sprintf(destination,"%s%ld.%s",CACHE_PATH,hashcode,req_image->type);

    MagickWriteImage(magickWand,destination);

    DestroyMagickWand(magickWand);

    return hashcode;
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

    // check if adapted image has been previously modified and it's in cache
    /*if ((name = isInCache(req_image))!=NULL) {
        sprintf(image->name, name);
        return image;
    }*/

    adaptedImg->quality = 80;
    sprintf(adaptedImg->type,"jpg");
    //adaptedImg->quality = (size_t) request->quality*100;
    //sprintf(adaptedImg->type,request->type);
    // TODO da ottenere con WURFL usando useragent
    adaptedImg->width = 200;
    adaptedImg->height = 200;
    adaptedImg->length = adaptedImg->width*adaptedImg->height;

    printf("adapting begins...\n");
    /* necessary content adaptation get an hash function, depending on original name and
       changes done */
    adaptedImg->name_code = adapt(req_image, adaptedImg);
    if (adaptedImg->name_code==0) {
        return NULL;
    }

    printf("end adaptation...\n");

    char *date = getTodayToString();
    sprintf(adaptedImg->last_modified,date);

    return adaptedImg;
}