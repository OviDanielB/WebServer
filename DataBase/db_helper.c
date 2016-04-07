//
// Created by ovi on 4/5/16.
//

#include "db_helper.h"

#define DB_PATH "/home/ovi/ClionProjects/WebServer/DataBase/serverContent.db"


/* called by db_get_image_by_name to fill the img struct passed as (void *), later casted back */
int fill_img_struct(void *data, int argc, char **argv, char **azColName){

    int i;
    struct img *image;

    image = (struct img *) data;

    for(i = 0; i < argc; i++){
        if(strcmp(azColName[i],"Name") == 0){
            image->name = malloc(50);
            if(image->name == NULL){
                perror("Struct img malloc error.\n");
                return 1;
            }
            sscanf(argv[i], "%s",image->name);
        } else if(strcmp(azColName[i],"Type") == 0){
            image->type = malloc(5);
            if(image->type == NULL){
                perror("Struct img malloc error");
                return 1;
            }
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


void db_open(sqlite3 * db){

    int rc;
    char * sqlStatement;

    rc = sqlite3_open(DB_PATH ,&db);

    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }

}

void db_close(sqlite3 * db){
    sqlite3_close(db);
}

void db_execute_statement(sqlite3 *db, const char *sql, struct img *image){

    int rc;
    char * zErrorMsg;

    rc = sqlite3_exec(db, sql, 0, (void *) image, &zErrorMsg);
    if( rc!=SQLITE_OK ) {
        fprintf(stderr, "SQL error: %s\n", zErrorMsg);
        sqlite3_free(zErrorMsg);
    }

}


void db_insert_img(struct img *image){

    char *statement, *errorMsg = 0;
    int rc;

    sqlite3 *datab;
    rc = sqlite3_open(DB_PATH,&datab);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(datab));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }

    statement = malloc(2048 * sizeof(char));

    sprintf(statement,"INSERT INTO IMAGES(Name,Type,Length,Width,Height) " \
            "VALUES('%s','%s',%ld,%ld,%ld);", image->name, image->type, image->length, image->width,image->height);

     write(STDOUT_FILENO, statement, strlen(statement));


    rc = sqlite3_exec(datab,statement,0, 0, &errorMsg);
    if(rc != SQLITE_OK){
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

    statement = malloc(2048 * sizeof(char));
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