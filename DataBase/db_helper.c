//
// Created by ovi on 4/5/16.
//

#include "db_helper.h"



/* called by db_get_image_by_name to fill the img struct passed as (void *), later casted back */
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

/*  Creating database if not exists and opening it.
 *
 * @param: db =  SQLite database to create/open
 */
void db_open(sqlite3 * db)
{
    int rc;
    char * sqlStatement;

    rc = sqlite3_open(DB_PATH ,&db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }
}

/*  Closing database.
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

#define lifetime 10 // TODO

/*  TODO
 * Update server cache checking for saturation of memory dedicated
 * or after timeout of cached image lifetime.
 *
 * @param db = SQLite database where server cache is
 */
void db_update_cache(sqlite3 *db)
{
    //check

    // delete by timeout

    // delete by time of insert

    db_close(db);
}

/*  Insert new image into database in table IMAGES or CONV_IMG (cache)
 *
 * @param: originalImg = image to add to server files (= NULL if it's a cache update)
 * @param: convImg = manipulated image to add to server cache (= NULL if it's a server images' update
 */
void db_insert_img(struct img *originalImg, struct conv_img *convImg)
{
    char *statement, *errorMsg = 0;
    int rc;

    sqlite3 *datab;
    rc = sqlite3_open(DB_PATH,&datab);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(datab));
        exit(0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }

    statement = malloc(MAXLINE * sizeof(char));

    if (originalImg!=NULL) {
        sprintf(statement,"INSERT INTO IMAGES(Name,Type,Length,Width,Height) " \
            "VALUES('%s','%s',%ld,%ld,%ld);",
                originalImg->name, originalImg->type, originalImg->length, originalImg->width,originalImg->height);
    } else if (convImg!=NULL) {
        sprintf(statement,"INSERT INTO CONV_IMG(Original_Name,Name,Type,Length,Width,Height) " \
            "VALUES('%s',%ld,'%s','%s',%ld,%ld,%ld,%ld);",
                convImg->original_name,convImg->name_code, convImg->type, convImg->last_modified,
                convImg->width, convImg->height,convImg->length,convImg->quality);

        db_update_cache(datab);
    }

    write(STDOUT_FILENO, statement, strlen(statement));

    rc = sqlite3_exec(datab,statement,0, 0, &errorMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr,"SQL insert error: %s\n", errorMsg);
        sqlite3_free(errorMsg);
        exit(EXIT_FAILURE);
    }

    db_close(datab);

}

/* returns an img struct (as defined in db_helper.h)
 * you only need to pass it a struct img * initializated with malloc(sizeof(struct img)), the name and type char * arrays is done by itself
*/
void db_get_image_by_name(char *name,struct img *image){

    sqlite3 *db;
    char *statement, *errorMsg = 0;
    int rc;

    rc = sqlite3_open(DB_PATH,&db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return;
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }

    statement = malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("Malloc error. \n");
        return;
    }

    sprintf(statement,"SELECT * FROM IMAGES WHERE Name='%s';", name);
    printf(statement);

    rc = sqlite3_exec(db,statement,fill_img_struct, image, &errorMsg);
    if(rc != SQLITE_OK){
        fprintf(stderr, "SQLITE SELECT error: %s \n",errorMsg);
        exit(EXIT_FAILURE);
    }

    db_close(db);
}

/*  Deleting image from database.
 *
 * @param: name = image name to delete from database
 */
void db_delete_image_by_name(char *name){

    sqlite3 *db;
    int rc;
    char *statement, *errorMsg;

    rc = sqlite3_open(DB_PATH,&db);
    if(rc){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return;
    } else {
        fprintf(stderr,"Database opened successfully\n");
    }

    statement = malloc(1024 * sizeof(char));
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

    db_close(db);


}