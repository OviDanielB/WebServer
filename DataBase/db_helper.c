//
// Created by ovi on 4/5/16.
//

#include "db_helper.h"

#define DB_PATH "./DataBase/serverContent.db"

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}


void db_open(sqlite3 * db){

    int rc;
    char * sqlStatement, * zErrorMsg;


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

void db_execute_statement(sqlite3 *db, const char *sql){

}