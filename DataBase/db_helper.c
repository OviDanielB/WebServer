//
// Created by ovi on 4/5/16.
//

#include "db_helper.h"

/*  Creating database if not exists and opening it.
 *
 * @param: db =  SQLite database to create/open
 */
void db_open(sqlite3 * db)
{
    int rc;

    rc = sqlite3_open(DB_PATH ,&db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }
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
 * @param: image = TODO
 */
void db_execute_statement(sqlite3 *db, const char *sql, struct img *image)
{
    int rc;
    char * zErrorMsg;

    rc = sqlite3_exec(db, sql, 0, (void *) image, &zErrorMsg);
    if( rc!=SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrorMsg);
        sqlite3_free(zErrorMsg);
    }
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
        // delete by time of insertion (older saved element)
        deleteByAge();
    }
    // delete by timeout if there are expired ones
    deleteByTimeout();

    db_close(db);
}

/*  Insert new image into database in table IMAGES or CONV_IMG (cache)
 *
 * @param: originalImg = image to add to server files (= NULL if it's a cache update)
 * @param: convImg = manipulated image to add to server cache (= NULL if it's a server images' update
 */
void db_insert_img(struct img *originalImg, struct conv_img *convImg) {
    char *statement, *errorMsg = 0;
    int rc;

    sqlite3 *db;
    /*if ((db = malloc(sizeof(sqlite3))) == NULL) {
        perror("error in malloc db\n");
        exit(EXIT_FAILURE);
    }*/
    db_open(db);

    if ((statement = malloc(MAXLINE * sizeof(char)))==NULL) {
        perror("error in malloc \n");
        exit(EXIT_FAILURE);
    }

    if (originalImg != NULL) {
        sprintf(statement, "INSERT INTO IMAGES(Name,Type,Length,Width,Height) " \
            "VALUES('%s','%s',%ld,%ld,%ld);",
                originalImg->name, originalImg->type, originalImg->length, originalImg->width, originalImg->height);
    } else if (convImg != NULL) {
        sprintf(statement, "INSERT INTO CONV_IMG(Original_Name,Name,Type,Last_Modified,Width,Height,Length,Quality) " \
            "VALUES('%s',%ld,'%s','%s',%ld,%ld,%ld,%ld);",
                convImg->original_name, convImg->name_code, convImg->type, convImg->last_modified,
                convImg->width, convImg->height, convImg->length, convImg->quality);

        db_update_cache(db);
    }

    write(STDOUT_FILENO, statement, strlen(statement));

    rc = sqlite3_exec(db, statement, 0, 0, &errorMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL insert error: %s\n", errorMsg);
        sqlite3_free(errorMsg);
        exit(EXIT_FAILURE);
    }

    free(statement);
    db_close(db);
}

/* Callback by db_get_image_by_name to fill the img struct passed as (void *), later casted back */
int fill_img_struct(void *data, int argc, char **argv, char **azColName)
{
    int i;
    struct img *image = (struct img *) data;

    if ((image=malloc(sizeof(struct img)))==NULL) {
        perror("Struct img malloc error.\n");
        return 1;
    }

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
void db_get_image_by_name(char *name,struct img *image)
{
    char *statement, *errorMsg = 0;
    int rc;

    sqlite3 *db;
/*    if ((db=malloc(sizeof(sqlite3)))==NULL) {
        perror("error in malloc db\n");
        exit(EXIT_FAILURE);
    }*/
    db_open(db);

    statement = malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("Malloc error. \n");
        exit(EXIT_FAILURE);
    }

    sprintf(statement,"SELECT * FROM IMAGES WHERE Name='%s';", name);
    printf(statement);

    rc = sqlite3_exec(db,statement,fill_img_struct, image, &errorMsg);
    if(rc != SQLITE_OK){
        fprintf(stderr, "SQLITE SELECT error: %s \n",errorMsg);
        exit(EXIT_FAILURE);
    }

    free(statement);
    db_close(db);
}

/*  Deleting image from database.
 *
 * @param: name = image name to delete from database
 */
void db_delete_image_by_name(char *name)
{
    int rc;
    char *statement, *errorMsg;

    sqlite3 *db;
/*    if ((db=malloc(sizeof(sqlite3)))==NULL) {
        perror("error in malloc db\n");
        exit(EXIT_FAILURE);
    }*/
    db_open(db);

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

    free(statement);
    db_close(db);
}

/* returns a char** with the names of the files in the directory specified by path
 * the char ** in the call     char** files = files_in_dir(path) DOESN'T HAVE TO BE INITIALIZED
 * the names of the files are at maximum 256 as defined in dirent.h
*/
void images_in_dir(char *path,struct img **images){

    DIR *dir;
    struct dirent *ent;

    int fileCount = 0;
    char complete_path[1024];

    MagickWand *m_wand = NULL;
    size_t width,height;

    m_wand = NewMagickWand();

    //
    images = malloc(30 * sizeof(struct img *));
    if ((dir = opendir (path)) != NULL) {
        /* print all the files and directories within directory */

        while ((ent = readdir (dir)) != NULL) {

            /* doesn't put the . and .. directory*/
            if(strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0){
                continue;
            }

            strcat(complete_path,path);
            strcat(complete_path,"/");
            strcat(complete_path,ent->d_name);
            printf("%s\n",complete_path);

            if(MagickReadImage(m_wand,strcat(path,ent->d_name)) == MagickFalse){
                perror("MagickReadImage error");
                exit(EXIT_FAILURE);
            }



            width = MagickGetImageWidth(m_wand);
            height =MagickGetImageHeight(m_wand);

            images[fileCount] = malloc(sizeof(struct img));
            if(images[fileCount] == NULL){
                perror("Malloc error.");
                exit(EXIT_FAILURE);
            }

            strcpy((images[fileCount])->name,ent->d_name);
            images[fileCount]->width =(size_t) 500;  // width;
            images[fileCount]->height =(size_t) 600 ; //height
            printf("name = %s, width = %ld, height = %ld\n",images[fileCount]->name,images[fileCount]->width,images[fileCount]->height);

            if(fileCount>10){
                images = realloc(images,(fileCount + 1) * sizeof(char));
                if(images == NULL){
                    perror("Calloc error.");
                    exit(EXIT_FAILURE);
                }
            }
            printf("%d\n",fileCount);
            fileCount++;

            complete_path[0]=0;



        }

        DestroyMagickWand(m_wand);

        closedir (dir);
    } else {
        /* could not open directory */
        perror ("Could not open directory for images search.");
        return;
    }

    return;

}