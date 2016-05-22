//
// Created by ovi on 4/5/16.
//

#include "db_helper.h"

/*  Creating database if not exists and opening it.
 *
 * @param: db =  SQLite database to create/open
 */
sqlite3 *dbOpen()
{
    int rc;
    sqlite3 *db;

    rc = sqlite3_open(DB_PATH ,&db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    } else {
        fprintf(stdout, "Opened database successfully\n");
    }

    return db;
}

/*  Closing database.
 *
 * @param: db = SQLite database to close
 */
void dbClose(sqlite3 *db)
{
    sqlite3_close(db);
    fprintf(stdout, "Database closed.\n");
}

/*  Executing a SQLite statement.
 *
 * @param: db = database to be queried
 * @param: sql = statement to execute
 */
int dbExecuteStatement(const char *sql, int (*callback)(void *, int, char **, char **), void *arg)
{
    /* open server database or create it if doesn't exist */
    sqlite3 *db = dbOpen();
    int rc;
    char *zErrorMsg;

    printf("%s\n",sql);

    rc = sqlite3_exec(db, sql, callback, arg, &zErrorMsg);
    printf("\nstatement result:%d\n",rc);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error %d: %s\n", rc, sqlite3_errmsg(db));
        sqlite3_free(zErrorMsg);
    }

    memset((char *)sql,'\0',MAXLINE);
    dbClose(db);

    return rc;
}

/*  Insert new User-Agent line from HTTP request into the database's table USERAGENT
 *
 * @param: userAgent = string of line to add to table
 * @param: dev = struct device * that contains characteristics searched through PHP process for adapting
 *              (= NULL just to check if that line has been just saved in table)
 */
int dbInsertUserAgent(char *userAgent, struct device *dev)
{
    char *statement;

    if ((statement = (char *) malloc(MAXLINE * sizeof(char)))==NULL) {
        perror("error in malloc \n");
        exit(EXIT_FAILURE);
    }

    if (dev != NULL) {
        sprintf(statement, "UPDATE 'USERAGENT' " \
            "SET 'Device'='%s', 'Colors'=%ld, 'Width'=%ld, 'Height'=%ld, 'Jpg'=%d, 'Png'=%d, 'Gif'=%d"
                "WHERE 'Line'='%s';",
                    dev->id, dev->colors, dev->width, dev->height, dev->jpg, dev->png, dev->gif, userAgent);
    } else {
        sprintf(statement, "INSERT INTO 'USERAGENT' ('Line') " \
            "VALUES ('%s');",
                userAgent);
    }

    return dbExecuteStatement(statement, 0, 0);
}

/*  Insert new image into database in table IMAGES or CONV_IMG (cache)
 *
 * @param: originalImg = image to add to server files (= NULL if it's a cache update)
 * @param: convImg = manipulated image to add to server cache (= NULL if it's a server images' update
 */
int dbInsertImg(struct img *originalImg, struct conv_img *convImg)
{
    printf("insert img....\n");

    char *statement;

    if ((statement = (char *) malloc(MAXLINE * sizeof(char)))==NULL) {
        perror("error in malloc \n");
        exit(EXIT_FAILURE);
    }

    if (originalImg != NULL) {
        sprintf(statement, "INSERT INTO 'IMAGES' ('Name','Type','Length','Width','Height') " \
        "VALUES ('%s','%s',%ld,%ld,%ld);",
                originalImg->name, originalImg->type, originalImg->length, originalImg->width, originalImg->height);
    }

    if (convImg != NULL) {
        sprintf(statement, "INSERT INTO 'CONV_IMG' ('Original_Name','Name','Last_Modified') " \
            "VALUES ('%s',%lu,'%s');",
                convImg->original_name, convImg->name_code, convImg->last_modified);

        /* TODO alternative way to count rows   */
        CACHENUM += 1;

        updateCache();
    }

    return dbExecuteStatement(statement, 0, 0);
}

/* Callback by db_get_conv_image_by_code to fill the conv_img struct passed as (void *), later casted back */
int fillConvImgStruct(void *data, int argc, char **argv, char **azColName)
{
    int i;
    struct conv_img *image = (struct conv_img *) data;

    for(i = 0; i < argc; i++){
        if (strcmp(azColName[i],"Original_Name") == 0) {
            sscanf(argv[i], "%s", image->original_name);
        } else if (strcmp(azColName[i],"Name") == 0) {
            sscanf(argv[i], "%lu", &image->name_code);
        } else if(strcmp(azColName[i],"Type") == 0){
            sscanf(argv[i], "%s", image->type);
        } else if(strcmp(azColName[i],"Last_Modified") == 0){
            sscanf(argv[i], "%s", image->last_modified);
        } else if(strcmp(azColName[i],"Width") == 0){
            image->width = (size_t) atoll(argv[i]);
        } else if(strcmp(azColName[i],"Height") == 0){
            image->height = (size_t) atoll(argv[i]);
        } else if(strcmp(azColName[i],"Length") == 0) {
            image->length = (size_t) atoll(argv[i]);
        } else if(strcmp(azColName[i],"Quality") == 0) {
            image->quality = (size_t) atoll(argv[i]);
        }

        printf("%s = %s\n", azColName[i], argv[i]);
    }
    return 0;
}

/* Fill a conv_img struct from database info.
 *
 * @param code: unsigned long as hashcode of the manipulated image to search in server cache
 * @param image: struct conv_img * previously allocated with malloc(sizeof(struct conv_img))
*/
void db_get_conv_image_by_code(unsigned long code,struct conv_img *image)
{
    char *statement;

    statement = (char *) malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("Malloc error. \n");
        exit(EXIT_FAILURE);
    }
    memset(statement,'\0',strlen(statement));

    sprintf(statement,"SELECT * FROM CONV_IMG WHERE Name='%lu';", code);

    dbExecuteStatement(statement, fillConvImgStruct, image);

    //free(statement);
}

/* Callback by dbGetImageByName to fill the img struct passed as (void *), later casted back */
int fillImgStruct(void *data, int argc, char **argv, char **azColName)
{
    int i;
    struct img *image = (struct img *) data;

    for(i = 0; i < argc; i++){
        if (strcmp(azColName[i],"Name") == 0) {
            sscanf(argv[i], "%s",image->name);
        } else if(strcmp(azColName[i],"Type") == 0){
            sscanf(argv[i], "%s",image->type);
        } else if(strcmp(azColName[i],"Width") == 0){
            image->width = (size_t) atoll(argv[i]);
        } else if(strcmp(azColName[i],"Height") == 0){
            image->height = (size_t) atoll(argv[i]);
        } else if(strcmp(azColName[i],"Length") == 0) {
            image->length = (size_t) atoll(argv[i]);
        }

        printf("%s = %s\n", azColName[i], argv[i]);
    }
    return 0;
}

/* Callback by dbGetImageByName to fill the img struct passed as (void *), later casted back */
int fillDeviceStruct(void *data, int argc, char **argv, char **azColName)
{
    int i;
    struct device *device = (struct device *) data;

    for(i = 0; i < argc; i++){
        if(strcmp(azColName[i],"Width") == 0) {
            device->width = (size_t) atoll(argv[i]);
        } else if(strcmp(azColName[i],"Height") == 0) {
            device->height = (size_t) atoll(argv[i]);
        } else if(strcmp(azColName[i],"Colors") == 0) {
            device->colors = (size_t) atoll(argv[i]);
        } else if(strcmp(azColName[i],"Jpg") == 0) {
            sscanf(argv[i], "%d", &device->jpg);
        } else if(strcmp(azColName[i],"Png") == 0) {
            sscanf(argv[i], "%d", &device->png);
        } else if(strcmp(azColName[i],"Gif") == 0) {
            sscanf(argv[i], "%d", &device->gif);
        }

        printf("%s = %s\n", azColName[i], argv[i]);
    }
    return 0;
}

/*  Select from database the characteristics of a device from table USERAGENT
 *
 *  @param userAgent: string as line of User-Agent
 *  @param device: struct device* previously allocated with malloc(sizeof(struct device)),
 *               the name and type char * arrays is done by itself
 */
void dbGetDeviceByUserAgent(char *userAgent, struct device *device)
{
    char *statement;

    statement = (char *) malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("Malloc error. \n");
        exit(EXIT_FAILURE);
    }
    memset(statement,'\0',strlen(statement));

    sprintf(statement,"SELECT * FROM 'USERAGENT' WHERE 'Line'='%s';", userAgent);

    dbExecuteStatement(statement, fillDeviceStruct, device);
}

/* Fill an img struct fro database info.
 *
 * @param name: string as the original name of image
 * @param image: struct img * previously allocated with malloc(sizeof(struct img)),
 *               the name and type char * arrays is done by itself
*/
void dbGetImageByName(char *name, struct img *image)
{
    char *statement;

    statement = (char *) malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("Malloc error. \n");
        exit(EXIT_FAILURE);
    }
    memset(statement,'\0',strlen(statement));

    sprintf(statement,"SELECT * FROM IMAGES WHERE Name='%s';", name);

    dbExecuteStatement(statement, fillImgStruct, image);

    //free(statement);
}

/*  Deleting image from database.
 *
 * @param: name = image name to delete from IMAGES table of database
 * @param: code = image hashcode to delete from CONV_IMG table of database
 */
void dbDeleteByImageName(char *name, unsigned long code)
{
    char *statement;

    statement = malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("delete image by name malloc error");
        return;
    }

    if (code == NULL) {
        sprintf(statement, "DELETE FROM 'IMAGES' WHERE Name='%s';", name);
    } else {
        sprintf(statement, "DELETE FROM 'CONV_IMG' WHERE 'Original_Name'='%s' AND 'Name'=%lu;", name, code);
    }

    dbExecuteStatement(statement, 0, 0);
    //free(statement);
}

/* Set name, type, quality and size of the image
 *
 * @param img: struct conv_img* to fill with info
 * @param complete path: image path
 * @param fullname: image name (type included)
 */
void setConvImgInfo(struct conv_img *img, char *complete_path, char *fullname)
{
    printf("in convimg info\n");

    MagickWand *m_wand = NULL;
    m_wand = NewMagickWand();

    if(MagickReadImage(m_wand,complete_path) == MagickFalse){
        perror("MagickReadImage error");
        exit(EXIT_FAILURE);
    }

    char name[256];
    if (memset(name,'\0',256)==NULL) {
        perror("memset error\n");
        exit(EXIT_FAILURE);
    }
    readNameAndType(fullname, name, img->type);

    printf("in convimg info2\n");

    sscanf(name, "%lu", &img->name_code);
    img->width = MagickGetImageWidth(m_wand);
    img->height = MagickGetImageHeight(m_wand);
    img->length = img->width*img->height;
//    img->quality = MagickGetCompressionQuality(m_wand);

    MagickRemoveImage(m_wand);
    DestroyMagickWand(m_wand);
}


/* Set name type and size of the image
 *
 * @param img: struct img* to fill with info
 * @param complete path: image path
 * @param fullname: image name (type included)
 */
void setImgInfo(struct img *img, char *complete_path, char *fullname)
{
    MagickWand *m_wand = NULL;
    m_wand = NewMagickWand();

    if(MagickReadImage(m_wand,complete_path) == MagickFalse){
        perror("MagickReadImage error");
        exit(EXIT_FAILURE);
    }

    readNameAndType(fullname,img->name,img->type);
    img->width = MagickGetImageWidth(m_wand);
    img->height = MagickGetImageHeight(m_wand);
    img->length = img->width*img->height;

    MagickRemoveImage(m_wand);
    DestroyMagickWand(m_wand);
}

/* Load all server images previously manipulated into CONV_IMG table of Database.
 *
 * @param: path = string of path to the server cache's directory
 */
void db_load_cache_images(char *path)
{
    printf("in load cache\n");

    DIR *dir;
    struct dirent *ent;
    int fileCount = 0;
    char complete_path[strlen(CACHE_PATH)+280];
    complete_path[0] = 0;

    struct conv_img **images = malloc(30 * sizeof(struct conv_img *));
    if (images == NULL) {
        perror("malloc error\n");
        exit(EXIT_FAILURE);
    }

    if ((dir = opendir (path)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            /* doesn't put the . and .. directory*/
            printf("in loading3\n");
            if (strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0){
                continue;
            }

            sprintf(complete_path,"%s%s", path, ent->d_name);
            printf("%d: %s\n", fileCount+1, complete_path);

            images[fileCount] = malloc(sizeof(struct conv_img));
            if (images[fileCount] == NULL){
                perror("Malloc error.");
                exit(EXIT_FAILURE);
            }

            /*  set name, type, size and quality of image    */
            setConvImgInfo(images[fileCount], complete_path, ent->d_name);
            /*  add manipulated image to sever database  */
            dbInsertImg(NULL, images[fileCount]);

            printf("name_code = %lu, type = %s, width = %ld, height = %ld, quality = %ld\n",
                   images[fileCount]->name_code,images[fileCount]->type,images[fileCount]->width,
                   images[fileCount]->height,images[fileCount]->quality);

            if (fileCount > 10) {
                images = realloc(images, (fileCount + 1) * sizeof(struct conv_img *));
                if(images == NULL){
                    perror("Calloc error.");
                    exit(EXIT_FAILURE);
                }
            }
            fileCount++;

            memset(complete_path,0,1024);
        }
        /* number of manipulated images loaded in the database    */
        CACHENUM = fileCount;

        closedir (dir);

    } else {
        /* could not open directory */
        perror ("Could not open directory for images search.");
        exit(EXIT_FAILURE);
    }
}

/* Load all server images into the IMAGES table of Database.
 *
 * @param: path = string of path where directory of images is located
 */
struct img ** dbLoadAllImages(char *path)
{
    DIR *dir;
    struct dirent *ent;
    int fileCount = 0;
    char complete_path[1024];
    complete_path[0] = 0;

    struct img **images = malloc(30 *sizeof(struct img *));
    if (images==NULL) {
        perror("malloc error\n");
        exit(EXIT_FAILURE);
    }

    if ((dir = opendir (path)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            /* doesn't put the . and .. directory*/
            if (strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0){
                continue;
            }

            sprintf(complete_path, "%s%s", path, ent->d_name);
            printf("%d: %s\n",fileCount+1,complete_path);

            images[fileCount] = malloc(sizeof(struct img));
            if (images[fileCount] == NULL){
                perror("Malloc error.");
                exit(EXIT_FAILURE);
            }

            /*  set name, type and size of image    */
            setImgInfo(images[fileCount], complete_path, ent->d_name);
            /*  add image to sever database  */
            dbInsertImg(images[fileCount], NULL);

            printf("name = %s, type = %s, width = %ld, height = %ld\n",
                   images[fileCount]->name,images[fileCount]->type,images[fileCount]->width,images[fileCount]->height);

            if (fileCount > 10) {
                images = realloc(images, (fileCount + 1) * sizeof(struct img *));
                if(images == NULL){
                    perror("Calloc error.");
                    exit(EXIT_FAILURE);
                }
            }
            fileCount++;

            memset(complete_path, 0, 1024);
        }
        /* total number of images loaded in the database    */
        IMAGESNUM = fileCount;

        closedir (dir);

    } else {
        /* could not open directory */
        perror ("Could not open directory for images search.");
        exit(EXIT_FAILURE);
    }
    return images;
}