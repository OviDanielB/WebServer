/*
 Created by laura_trive on 13/03/16.

 Implementations of functions to images adaptation
*/

#include "adaptImage.h"

/*  This function get width of the image
 *
 * @param: filename = image's path to get info from
 */
size_t getWidth(char *filename)
{
    char cmd[MAXLINE];
    size_t width;
    sprintf(cmd,"identify -format ""%%[fx:w]"" %s", filename);

    char buf[MAXLINE];
    buf = readResultLine(cmd);
    sscanf(buf,"%ld",&width);

    return width;
}

/*  This function get height of the image
 *
 * @param: filename = image's path to get info from
 */
size_t getHeight(char *filename)
{
    char cmd[MAXLINE];
    size_t height;
    sprintf(cmd,"identify -format ""%%[fx:h]"" %s", filename);

    char buf[MAXLINE];
    buf = readResultLine(cmd);
    sscanf(buf,"%ld",&height);

    return height;
}

/* Count of number of colors in the image
 *
 * @param: filename = image's path to get colors' number from
 */
size_t getColors(char *filename)
{
    size_t col = 0;
    char cmd[MAXLINE];
    sprintf(cmd, "identify -format '%%k' %s",filename);

    char buf[MAXLINE];
    buf = readResultLine(cmd);
    sscanf(buf,"%ld",&col);

    return col;
}

/*
 * This function composes the command to execute for manipulating image, based on characteristics in input.
 *
 * @param: req_image = requested image;
 * @param: adaptImg = image adapted on device characteristics
 */
unsigned long adapt(struct img *req_image, struct conv_img *adaptImg)
{
    char cmd[MAXLINE];
    char filename[MAXLINE];
    sprintf(filename,"%s%s.%s", PATH, req_image->name, req_image->type);
    size_t c = getColors(filename);
    size_t w = req_image->height = getHeight(filename);
    size_t h = req_image->height = getWidth(filename);
    char *format = req_image->type;
    unsigned char nameToHash[256];
    unsigned long hashcode;
    char conversion[MAXLINE];
    // hash sulla stringa formata da: <nomeoriginale><width><height><q><type><c> per indicare univocamente l'immagine modificata
    // caratteristica origianel o  0 se elemento non modificato
    char destination[MAXLINE];

    if (strcmp(adaptImg->type,"")!=0) {
        sprintf(conversion,"convert %s%s.%s %s%s.png ; ", PATH,req_image->name,req_image->type,PATH,req_image->name);
        while (execCommand(conversion)==1);
        format = "png";
    }

    printf("check type\n");

    sprintf(cmd,"convert %s%s.%s ", PATH,req_image->name,format);

    /* resize of the original image */
    if (adaptImg->width!=-1) {
        char resize[MAXLINE];
        sprintf(resize,"-resize %ldx%ld ", adaptImg->width,adaptImg->height);
        strcat(cmd,resize);
        w = adaptImg->width;
        h = adaptImg->height;
    }

    printf("resized \n");

    /* For the JPEG and MPEG image formats, quality is
    * 1 (lowest image quality and highest compression)
    * to 100 (best quality but least effective compression)
    */
    char quality[MAXLINE];
    sprintf(quality,"-quality %f%% ", adaptImg->quality);
    strcat(cmd,quality);

    printf("quality adapted\n");

    /* Reduction of the number of colors in the original image.
    * The color reduction also can happen automatically when saving images
    * to color-limited image file formats, such as GIF, and PNG8.*/
    if (adaptImg->colors!=-1) {
        char reduce[MAXLINE];
        sprintf(reduce,"-dither None -colors %ld ", adaptImg->colors);
        strcat(cmd,reduce);
        c = adaptImg->colors;
    }

    printf("color adapting\n");

    // string <nomeoriginale><width><height><q><type><c> to hash
    sprintf((char *)nameToHash, "%s%ld%ld%f%s%ld",req_image->name, w,h,adaptImg->quality,adaptImg->type,c);

    hashcode = getHashCode(nameToHash);
    sprintf(destination,"%s%ld.%s",CACHE_PATH,hashcode,req_image->type);
    strcat(cmd,destination);

    // NON SO SE SERVE
    // reset type request which was modified from JPG (if it was) to PNG to avoid data looses
    /*if (strlen(conversion)!=0) {
        sprintf(conversion,"; convert %s %s%ld.%s ; ", destination,CACHE_PATH,hashcode,req_image->type);
        strcat(cmd,conversion);
    }*/

    while (execCommand(cmd)==1);

    printf("comando eseguito\n");

    return hashcode;
}

/*  This function adapts image's characteristics to client's device,
 *  optimizing width, length, number of colors and quality.
 *
 *  @param image: image to adapt, containing info of requested quality and type
 *  @param user_agent: line of HTTP request to get device's and requested file's info from
 */
struct conv_img *adaptImageTo(struct img *req_image, float quality, char *type, char *user_agent)
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

    adaptedImg->quality = quality;
    sprintf(adaptedImg->type,type);
    // TODO da ottenere con WURFL usando useragent
    adaptedImg->width = 200;
    adaptedImg->height = 200;
    adaptedImg->colors = (size_t) -1;

    // check if resize is necessary
    if (req_image->width==adaptedImg->width && req_image->height==adaptedImg->height) {
        adaptedImg->width = (size_t) -1;
        adaptedImg->height = (size_t) -1;
    }

    // check if is requested JPG format to convert before manipulating, avoiding data loss due to compression.
    if (!jpg(adaptedImg->type)){
        sprintf(adaptedImg->type,"");
    }

    // check if reducing number of colors is necessary
    char filename[MAXLINE];
    sprintf(filename,"%s%s.%s", PATH, req_image->name, req_image->type);
    if (getColors(filename)==adaptedImg->colors) {
        adaptedImg->colors = (size_t) -1;
    }

    printf("riempita adaptimg\n");

    // composes command to do necessary content adaptation
    adaptedImg->name_code = adapt(req_image, adaptedImg);

    sprintf(adaptedImg->last_modified,getTodayToString());

    // aggiunge alla cache db

    return adaptedImg;
}