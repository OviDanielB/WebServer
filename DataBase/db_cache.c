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
    int i;
    char *result = (char *) data;
    printf("into the CALLBACKKKKKKKKK e result: %s\n", result);

    for(i = 0; i < argc; i++){
        if (strcmp(azColName[i],"Name") == 0) {
            sscanf("200", "%s", result);
        }
        printf("result = %s\n", argv[i]);
    }

    return 0;
}

/*  Check if searched manipulated image has been just inserted in server cache.
 *
 * @param hashcode: index of adapted image eventually cached, as hash function of original image name and adaptations done.
 */
int isInCache(struct conv_img *im)
{
    /*  if adapted image wasn't just added in cache table, insert it   */
    if (dbInsertImg(NULL, im) == SQLITE_CONSTRAINT) { // if UNIQUE constraints failed
        printf("IN CACHE\n");
        return 1; // true
    }

    printf("NOT IN CACHE\n");
    return 0; // FALSE
}


/*  Check if server cache is full (CONV_IMG has reached max number of rows) */
int isFull()
{
    char *statement;
    int rows = 0;

    statement = (char *) malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("Malloc error. \n");
        return 0; // FALSE
    }

    sprintf(statement,"SELECT COUNT('Name') FROM 'CONV_IMG';");

    dbExecuteStatement(statement, cache_check_status, &rows);
    free(statement);

    printf("rows dopo op:%d\n",rows);

    if (rows <= MAX_CACHE_ROWS_NUM) {
        return 0; // FALSE
    }

    return 1;   // true
}

/*  Delete from CONV_IMG table the older image inserted (with greater lifetime)   */
void deleteByAge()
{
    char *statement;

    statement = (char *) malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("delete image by name malloc error");
        return;
    }

    sprintf(statement, "DELETE FROM CONV_IMG WHERE Last_Modified=( SELECT MIN(Last_Modified) FROM CONV_IMG);");

    dbExecuteStatement(statement, 0, 0);
    free(statement);
}

/*  Delete from CONV_IMG table all the image where number of days between today and last modify date
 *  is greater than TIMEOUT value  */
void deleteByTimeout()
{
    char *statement;

    statement = (char *) malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("malloc error\n");
        return;
    }
    sprintf(statement, "DELETE FROM CONV_IMG WHERE "
            "((strftime('%%d','YYYY-MM-DD') - strftime('%%d','Last_Modified')) >= %d);",TIMEOUT);

    dbExecuteStatement(statement, 0, 0);

    free(statement);
}

/*  Update date of the last access at that image in the server cache.
 *
 * @param adaptImg: struct conv_img * that describes row index of cache table to update (name_code)
 *                  and date of end manipulation
 */
void updateDate(struct conv_img *adaptedImg)
{
    char *statement;

    statement = (char *) malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("malloc error\n");
        return;
    }

    sprintf(statement, "UPDATE CONV_IMG SET Last_Modified='%s' WHERE Name=%lu;",
            adaptedImg->last_modified,adaptedImg->name_code);

    dbExecuteStatement(statement, 0, 0);
    free(statement);
}


/* Update server cache checking for saturation of memory dedicated
 * or after timeout of cached image lifetime.
 *
 * @param db = SQLite database where server cache is
 */
void updateCache()
{
    // check if cache memory is full
    //if (isFull()) {
    if (CACHENUM >= MAX_CACHE_ROWS_NUM) {
        printf("checkes full cache\n");
        // delete by time of insertion (older saved element)
        deleteByAge();
    }
    // delete by timeout if there are expired ones
    deleteByTimeout();
}