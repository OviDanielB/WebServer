//
// Created by ovi on 4/5/16.
//

#include "db_helper.h"

/*  Creating database if not exists and opening it.
 *
 * @param: db =  SQLite database to create/open
 */
sqlite3 *db_open()
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
void db_close(sqlite3 * db)
{
    sqlite3_close(db);
}

/*  Executing a SQLite statement.
 *
 * @param: db = database to be queried
 * @param: sql = statement to execute
 */
void db_execute_statement(sqlite3 *db, const char *sql)
{
    int rc;
    char *zErrorMsg;

    printf("%s\n",sql);

    rc = sqlite3_exec(db, sql, 0, 0, &zErrorMsg);
    printf("\nstatement result:%d\n",rc);
    if( rc!=SQLITE_OK ) {
        fprintf(stderr, "SQL error %d: %s\n", rc, sqlite3_errmsg(db));
        //sqlite3_free(zErrorMsg);
    }

    memset((char *)sql,'\0',MAXLINE);
}

/* Update server cache checking for saturation of memory dedicated
 * or after timeout of cached image lifetime.
 *
 * @param db = SQLite database where server cache is
 */
void db_update_cache(sqlite3 *db)
{
    // check if cache memory is full
    if (isFull(db)) {
        printf("checkes full cache\n");
        // delete by time of insertion (older saved element)
        deleteByAge(db);
    }
    // delete by timeout if there are expired ones
    deleteByTimeout(db);
}

/*  Insert new image into database in table IMAGES or CONV_IMG (cache)
 *
 * @param: originalImg = image to add to server files (= NULL if it's a cache update)
 * @param: convImg = manipulated image to add to server cache (= NULL if it's a server images' update
 */
void db_insert_img(sqlite3 *db, struct img *originalImg, struct conv_img *convImg)
{
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
        sprintf(statement, "INSERT INTO 'CONV_IMG' ('Original_Name','Name','Type','Last_Modified','Width','Height','Length','Quality') " \
            "VALUES ('%s',%lu,'%s','%s',%ld,%ld,%ld,%ld);",
                convImg->original_name, convImg->name_code, convImg->type, convImg->last_modified,
                convImg->width, convImg->height, convImg->length, convImg->quality);

        db_update_cache(db);
    }

   db_execute_statement(db,statement);
}

/* Callback by db_get_conv_image_by_code to fill the conv_img struct passed as (void *), later casted back */
int fill_conv_img_struct(void *data, int argc, char **argv, char **azColName)
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
void db_get_conv_image_by_code(sqlite3 *db, unsigned long code,struct conv_img *image)
{
    char *statement, *errorMsg = 0;
    int rc;

    statement = (char *) malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("Malloc error. \n");
        exit(EXIT_FAILURE);
    }
    memset(statement,'\0',strlen(statement));

    sprintf(statement,"SELECT * FROM CONV_IMG WHERE Name='%lu';", code);
    printf(statement);

    rc = sqlite3_exec(db,statement, fill_conv_img_struct, image, &errorMsg);
    if(rc != SQLITE_OK){
        fprintf(stderr, "SQLITE SELECT error: %s \n",errorMsg);
        exit(EXIT_FAILURE);
    }

    //free(statement);
    //db_close(db);
}

/* Callback by db_get_image_by_name to fill the img struct passed as (void *), later casted back */
int fill_img_struct(void *data, int argc, char **argv, char **azColName)
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

/* Fill an img struct fro database info.
 *
 * @param name: string as the original name of image
 * @param image: struct img * previously allocated with malloc(sizeof(struct img)),
 *               the name and type char * arrays is done by itself
*/
void db_get_image_by_name(sqlite3 *db, char *name,struct img *image)
{
    char *statement, *errorMsg = 0;
    int rc;

    statement = (char *) malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("Malloc error. \n");
        exit(EXIT_FAILURE);
    }
    memset(statement,'\0',strlen(statement));

    sprintf(statement,"SELECT * FROM IMAGES WHERE Name='%s';", name);
    printf(statement);

    rc = sqlite3_exec(db,statement,fill_img_struct, image, &errorMsg);
    if(rc != SQLITE_OK){
        fprintf(stderr, "SQLITE SELECT error: %s \n",errorMsg);
        exit(EXIT_FAILURE);
    }

    //free(statement);
    //db_close(db);
}

/*  Deleting image from database.
 *
 * @param: name = image name to delete from database
 */
void db_delete_image_by_name(sqlite3 *db, char *name)
{
    int rc;
    char *statement, *errorMsg;

    statement = malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("delete image by name malloc error");
        return;
    }

    sprintf(statement, "DELETE FROM IMAGES WHERE Name='%s';",name);

    rc = sqlite3_exec(db,statement,0,0,&errorMsg);
    if(rc != SQLITE_OK){
        fprintf(stderr,"SQLITE DELETE ERROR: %s \n",errorMsg);
        return;
    }

    //free(statement);
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

/* returns a char** with the names of the files in the directory specified by path
 * the char ** in the call     char** files = files_in_dir(path) DOESN'T HAVE TO BE INITIALIZED
 * the names of the files are at maximum 256 as defined in dirent.h
*/
struct img ** db_load_all_images(sqlite3 *db, char *path)
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
            if(strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0){
                continue;
            }

            sprintf(complete_path,"%s%s",path,ent->d_name);
            printf("%d: %s\n",fileCount+1,complete_path);

            images[fileCount] = malloc(sizeof(struct img));
            if(images[fileCount] == NULL){
                perror("Malloc error.");
                exit(EXIT_FAILURE);
            }

            /*  set name, type and size of image    */
            setImgInfo(images[fileCount],complete_path,ent->d_name);
            /*  add image to sever database  */
            db_insert_img(db,images[fileCount],NULL);

            printf("name = %s, type = %s, width = %ld, height = %ld\n",
                   images[fileCount]->name,images[fileCount]->type,images[fileCount]->width,images[fileCount]->height);

            if (fileCount>10) {
                images = realloc(images,(fileCount + 1) * sizeof(struct img*));
                if(images == NULL){
                    perror("Calloc error.");
                    exit(EXIT_FAILURE);
                }
            }
            fileCount++;

            memset(complete_path,0,1024);
	}
        IMAGESNUM = fileCount;
        printf ("%d immagini\n ",fileCount);
        closedir (dir);
    } else {
        /* could not open directory */
        perror ("Could not open directory for images search.");
        exit(EXIT_FAILURE);
    }
    return images;
}