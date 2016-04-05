//
// Created by ovi on 4/5/16.
//

#include "db_helper.h"

#define DB_PATH "/home/ovi/ClionProjects/WebServer/DataBase/serverContent.db"

static int callback(void *image, int argc, char **argv, char **azColName){


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

    rc = sqlite3_exec(db, sql, callback, (void *) image, &zErrorMsg);
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

    char * sql = "CREATE TABLE IMAGE(Name TEXT);";

    sprintf(statement,"INSERT INTO IMAGES(Name,Type,Length,Width,Height) " \
            "VALUES('%s','%s',%ld,%ld,%ld);", image->name, image->type, image->length, image->width,image->height);

     write(STDOUT_FILENO, statement, strlen(statement));


    rc = sqlite3_exec(datab,sql,callback, 0, &errorMsg);
    if(rc != SQLITE_OK){
        fprintf(stderr,"SQL insert error: %s\n", errorMsg);
        sqlite3_free(errorMsg);
        exit(EXIT_FAILURE);
    }

    db_close(datab);

}