/*
 Created by laura_trive on 13/03/16.

 Implementations of functions to images adaptation
*/

#include "adaptImage.h"

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


/*  This function get width of the image
 *
 * @param: filename = image's path to get info from
 */
size_t getWidth(char *filename)
{
    char cmd[MAXLINE];
    size_t width;
    sprintf(cmd,"identify -format ""%%[fx:w]"" %s", filename);
    while (execCommand(cmd)==1){}
    sscanf(stdout,"%ld",&width);
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
    while (execCommand(cmd)==1){}
    sscanf(stdout,"%ld",&height);
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
    while (execCommand(cmd)==1) {}
    sscanf(stdout,"%ld",&col);
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
    // hash sulla stringa formata da: <nomeoriginale><width><height><q><type><c> per indicare univocamente l'immagine modificata
    // caratteristica origianel o  0 se elemento non modificato
    char destination[MAXLINE];

    if (strcmp(adaptImg->type,"")!=0) {
        char conversion[MAXLINE];
        sprintf(conversion,"convert %s%s.%s %s%s.png ; ", PATH,req_image->name,req_image->type,PATH,req_image->name);
        while (execCommand(conversion)==1);
        format = "png";
    }

    sprintf(cmd,"convert %s%s.%s ", PATH,req_image->name,format);

    /* resize of the original image */
    if (adaptImg->width!=-1) {
        char resize[MAXLINE];
        sprintf(resize,"-resize %ldx%ld ", adaptImg->width,adaptImg->height);
        strcat(cmd,resize);
        w = adaptImg->width;
        h = adaptImg->height;
    }

    /* For the JPEG and MPEG image formats, quality is
    * 1 (lowest image quality and highest compression)
    * to 100 (best quality but least effective compression)
    */
    char quality[MAXLINE];
    sprintf(quality,"-quality %f%% ", adaptImg->quality);
    strcat(cmd,quality);

    /* Reduction of the number of colors in the original image.
    * The color reduction also can happen automatically when saving images
    * to color-limited image file formats, such as GIF, and PNG8.*/
    if (adaptImg->colors!=-1) {
        char reduce[MAXLINE];
        sprintf(reduce,"-dither None -colors %ld ", adaptImg->colors);
        strcat(cmd,reduce);
        c = adaptImg->colors;
    }

    // string <nomeoriginale><width><height><q><type><c> to hash
    sprintf((char *)nameToHash, "%s%ld%ld%f%s%ld",req_image->name, w,h,adaptImg->quality,adaptImg->type,c);

    hashcode = getHashCode(nameToHash);
    sprintf(destination,"%s%ld.%s",CACHE_PATH,hashcode,req_image->type);
    strcat(cmd,destination);

    while (execCommand(cmd)==1);

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

    // composes command to do necessary content adaptation
    adaptedImg->name_code = adapt(req_image, adaptedImg);

    sprintf(adaptedImg->last_modified,getTodayToString());

    // aggiunge alla cache db

    return adaptedImg;
}
