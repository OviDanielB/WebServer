/**
 * Functions to manage CACHE in server database.
 */

#include "db_cache.h"

/*  Callback by isFull to verify TABLE STATUS result saved in size_T *rows passed as void *, later casted back */
int getRowsNum(void *data, int argc, char **argv, char **azColName)
{
    int *rows = (int *) data;
    sscanf(argv[0], "%d", rows);

    return 0;
}

/*  Check if the searched user agent has been just inserted in database.
 *
 * @param userAgent: string of user agent eventually cached with associated device's characteristics.
 */
int isUserAgentKnown(char *userAgent)
{
    /*  if adapted image wasn't just added in cache table, insert it   */
    if (dbInsertUserAgent(userAgent, NULL) == SQLITE_CONSTRAINT) { // if UNIQUE constraints failed
        printf("IN USER AGENT TABLE\n");
        return TRUE;
    }

    printf("NOT IN USER AGENT TABLE\n");
    return FALSE;
}

/*  Check if searched manipulated image has been just inserted in server cache.
 *
 * @param hashcode: index of adapted image eventually cached, as hash function of original image name and adaptations done.
 */
int isInCache(struct conv_img *im)
{
    /*  if adapted image wasn't just added in cache table, insert it   */
    if (dbInsertImg(NULL, im) == SQLITE_CONSTRAINT) { // if UNIQUE constraints failed
        /*  in cache    */
        return TRUE;
    }
    /* not in cache */
    return FALSE;
}

/*  Verify if CACHE table's size is greater than limit defined, counting number of its rows */
int isFull()
{
    char *statement;
    int *rows = malloc(sizeof(int));
    if (rows == NULL) {
        perror("malloc error");
        exit(EXIT_FAILURE);
    }
    *rows=0;

    statement = (char *) malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("delete image by name malloc error");
        return -1;
    }

    sprintf(statement, "SELECT (COUNT (*)) FROM 'CACHE';");

    dbExecuteStatement(statement, getRowsNum, (void *) rows);
    free(statement);

    printf("Numero di righe nella CACHE: %d\n", *rows);

    if (*rows <= MAX_CACHE_ROWS_NUM) {
        return FALSE;
    }

    return TRUE;
}

/*  Callback by getOlder to get name of older image saved in char *older passed as void *, later casted back */
int fillOlderName(void *data, int argc, char **argv, char **azColName)
{
    char *older = (char *) data;
    sscanf(argv[0], "%s", older);

    return 0;
}


/*  Get from CACHE table the older image inserted (with greater lifetime)   */
char *getOlder()
{
    char *statement;

    statement = (char *) malloc(MAXLINE * sizeof(char));
    if(statement == NULL) {
        perror("malloc error");
        exit(EXIT_FAILURE);
    }

    char *older = (char *) malloc(256*sizeof(char));
    if (older == NULL) {
        perror("malloc error");
        exit(EXIT_FAILURE);
    }
    sprintf(older, "*");

    sprintf(statement, "SELECT Name FROM CACHE WHERE Last_Modified=( SELECT MIN(Last_Modified) FROM CACHE);");

    dbExecuteStatement(statement, fillOlderName, older);

    free(statement);

    return older;
}

/*  Callback by getOlder to get name of older image saved in char *older passed as void *, later casted back */
int fillExpiredName(void *data, int argc, char **argv, char **azColName)
{
    char *exp = (char *) data;

    sscanf(argv[0], "%s", exp);

    return 0;
}

/*  Delete from CACHE table the older image inserted (with greater lifetime)   */
void deleteByAge()
{
    char *statement;

    statement = (char *) malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("delete image by name malloc error");
        return;
    }

    char *older = getOlder();

    if (strcmp(older, "*") != 0) {
        sprintf(statement, "DELETE FROM CACHE WHERE Name = '%s';", older);

        dbExecuteStatement(statement, 0, 0);
        removeFromDisk(older);
    }

    free(statement);
}

/*  Delete from CACHE table all the image where number of days between today and last modify date
 *  is greater than TIMEOUT value  */
char *getExpired()
{
    char *statement;

    statement = (char *) malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("malloc error\n");
        exit(EXIT_FAILURE);
    }

    char *exp = (char *) malloc(256*sizeof(char));
    if (exp == NULL) {
        perror("malloc error");
        exit(EXIT_FAILURE);
    }

    sprintf(exp, "*");

    sprintf(statement, "SELECT Name FROM CACHE WHERE (SELECT julianday('now') - julianday(Last_Modified)) >= %d;",
            TIMEOUT);

    dbExecuteStatement(statement, fillExpiredName, exp);

    free(statement);

    /* check if the file expired exists */
    if (strcmp(exp,"*") != 0) {
        return exp;
    }
    return NULL;
}


/*  Delete from CACHE table all the image where number of days between today and last modify date
 *  is greater than TIMEOUT value  */
void deleteByTimeout()
{
    char *statement;

    statement = (char *) malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("malloc error\n");
        return;
    }

    char *exp = getExpired();

    /* if the file expired exists delete it */
    if (exp != NULL) {
        sprintf(statement, "DELETE FROM CACHE WHERE Name = %s);", exp);

        dbExecuteStatement(statement, 0, 0);

        removeFromDisk(exp);
    }

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

    sprintf(statement, "UPDATE CACHE SET Last_Modified='%s' WHERE Name=%lu;",
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
    /* check if cache memory is full */
    if (isFull()) {
        /* delete by time of insertion (older saved element) */
        deleteByAge();
    }
    /* delete by timeout if there are expired ones  */
    deleteByTimeout();
}