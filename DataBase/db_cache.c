//
// Created by ovi on 4/7/16.
//

#include "db_cache.h"


/*  Callback by isFull to verify TABLE STATUS result saved in size_T *rows passed as void *, later casted back */
int cache_check_status(void *data, int argc, char **argv, char **azColName)
{
    int *rows = (int *) data;
    *rows = argc;
    printf("rows: %d\n",*rows);
    /*int i;
    size_t rows = (size_t) data;
    printf(argv[1]);
    rows = (size_t ) atoi(argv[1]);
    /*  SHOW TABLE STATUS output has the following column "Row" to describe actual number of rows of db table */
    /*for(i = 0; i < argc; i++) {
        if (strcmp(azColName[i], "Rows") == 0) {
            sscanf(argv[i], "%ld", &rows);
        }
    }*/

    return 0;
}

/*  Callback by isInCache to verify SELECT result saved in char *result passed as void *, later casted back */
int cache_check_result(void *data, int argc, char **argv, char **azColName)
{
    char *result = (char *) data;

    /* if the callback is called, SELECT returns at least a tuple, so image is in cache */
    sscanf("200",result);

    return 0;
}


/*  Check if searched manipulated image has been just inserted in server cache.
 *
 * @param hashcode: index of adapted image eventually cached, as hash function of original image name and adaptations done.
 */
int isInCache(sqlite3 *db, unsigned long hashcode)
{
    char *statement, *errorMsg = 0;
    char *result = "404";
    int rc;

    statement = (char *) malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("Malloc error. \n");
        return 0; // false
    }

    sprintf(statement,"SELECT * FROM CONV_IMG WHERE Name=%lu;", hashcode);
    printf(statement);

    rc = sqlite3_exec(db, statement, cache_check_result, result, &errorMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQLITE SELECT error: %s \n", errorMsg);
        sqlite3_free(errorMsg);
        exit(EXIT_FAILURE);
    }

    free(statement);

    if (strcmp(result, "404") == 0 ) { // not found in cache table
        return 0; // false
    }

    return 1; // true
}


/*  Check if server cache is full (CONV_IMG has reached max number of rows) */
int isFull(sqlite3 *db)
{
    char *statement, *errorMsg = 0;
    int rows = 0;
    int rc;

    statement = (char *) malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("Malloc error. \n");
        return 0; // false
    }

    sprintf(statement,"SELECT COUNT('Name') FROM 'CONV_IMG';");
    printf(statement);

    rc = sqlite3_exec(db, statement, cache_check_status, &rows, &errorMsg);
    if(rc != SQLITE_OK){
        fprintf(stderr, "SQLITE SELECT error: %s \n",errorMsg);
        sqlite3_free(errorMsg);
        exit(EXIT_FAILURE);
    }

    free(statement);

    printf("rows dopo op:%d\n",rows);

    if (rows <= MAX_CACHE_ROWS_NUM) {
        return 0; // false
    }

    return 1;   // true
}

/*  Delete from CONV_IMG table the older image inserted (with greater lifetime)   */
void deleteByAge(sqlite3 *db)
{
    int rc;
    char *statement, *errorMsg;

    statement = (char *) malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("delete image by name malloc error");
        return;
    }

    sprintf(statement, "DELETE FROM CONV_IMG WHERE Last_Modified=( SELECT MIN(Last_Modified) FROM CONV_IMG);");

    rc = sqlite3_exec(db,statement,0,0,&errorMsg);
    if(rc != SQLITE_OK){
        fprintf(stderr,"SQLITE DELETE ERROR: %s \n",errorMsg);
        sqlite3_free(errorMsg);
        return;
    }

    free(statement);
}

/*  Delete from CONV_IMG table all the image where number of days between today and last modify date
 *  is greater than TIMEOUT value  */
void deleteByTimeout(sqlite3 *db)
{
    char *statement;

    statement = (char *) malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("malloc error\n");
        return;
    }
    sprintf(statement, "DELETE FROM CONV_IMG WHERE "
            "((strftime('%%d','YYYY-MM-DD') - strftime('%%d','Last_Modified')) >= %d);",TIMEOUT);

    db_execute_statement(db,statement);

    free(statement);
}

/*  Update date of the last access at that image in the server cache.
 *
 * @param adaptImg: struct conv_img * that describes row index of cache table to update (name_code)
 *                  and date of end manipulation
 */
void updateDate(sqlite3 *db, struct conv_img *adaptedImg)
{
    int rc;
    char *statement, *errorMsg;

    statement = (char *) malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("malloc error\n");
        return;
    }

    sprintf(statement, "UPDATE CONV_IMG SET Last_Modified='%s' WHERE Name=%lu;",
            adaptedImg->last_modified,adaptedImg->name_code);

    rc = sqlite3_exec(db,statement,0,0,&errorMsg);
    if(rc != SQLITE_OK){
        fprintf(stderr,"SQLITE UPDATE ERROR: %s \n",errorMsg);
        sqlite3_free(errorMsg);
        return;
    }

    free(statement);
}