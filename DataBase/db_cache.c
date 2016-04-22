//
// Created by ovi on 4/7/16.
//

#include "db_cache.h"


/*  Callback by isFull to verify TABLE STATUS result saved in size_T *rows passed as void *, later casted back */
int cache_check_status(void *data, int argc, char **argv, char **azColName)
{
    int i;
    size_t rows = (size_t) data;

    /*  SHOW TABLE STATUS output has the following column "Row" to describe actual number of rows of db table */
    for(i = 0; i < argc; i++) {
        if (strcmp(azColName[i], "Rows") == 0) {
            sscanf(argv[i], "%ld", &rows);
        }
    }

    return 0;
}

/*  Callback by isInCache to verify SELECT result saved in char *result passed as void *, later casted back */
int cache_check_result(void *data, int argc, char **argv, char **azColName)
{
    char *result = (char *) data;

    if (argc > 1) {
        sprintf(result,"%d",200); // SELECT returns a tuple, so image is in cache
        printf("Searched image %s = %s\n", azColName[1], argv[0]);
        return 0;
    }

    sprintf(result,"%d",404); // SELECT doesn't return a tuple, so image is not in cache
    printf("Searched image %ld is not in cache \n", atol(argv[1]));

    return 0;
}


/*  Check if searched manipulated image has been just inserted in server cache.
 *
 * @param hashcode: index of adapted image eventually cached, as hash function of original image name and adaptations done.
 */
int isInCache(unsigned long hashcode)
{
    char *statement, *errorMsg = 0;
    char result[3];
    int rc;

    sqlite3 *db;
    /*if ((db=malloc(sizeof(sqlite3)))==NULL) {
        perror("error in malloc db\n");
        exit(EXIT_FAILURE);
    }*/
    db_open(db);

    statement = malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("Malloc error. \n");
        return 0; // false
    }

    sprintf(statement,"SELECT * FROM CONV_IMG WHERE Name=%ld;", hashcode);
    printf(statement);

    rc = sqlite3_exec(db, statement, cache_check_result, result, &errorMsg);
    if(rc != SQLITE_OK){
        fprintf(stderr, "SQLITE SELECT error: %s \n",errorMsg);
        exit(EXIT_FAILURE);
    }

    free(statement);
    db_close(db);

    if (strcmp(result,"404")==0) { // not found in cache table
        return 0; // false
    }

    return 1; // true
}


/*  Check if server cache is full (CONV_IMG has reached max number of rows) */
int isFull(sqlite3 *db)
{
    char *statement, *errorMsg = 0;
    size_t rows = 0;
    int rc;

    statement = malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("Malloc error. \n");
        return 0; // false
    }

    sprintf(statement,"SHOW TABLE STATUS '%s' LIKE CONV_IMG", DB_NAME);
    printf(statement);

    rc = sqlite3_exec(db, statement, cache_check_status, &rows, &errorMsg);
    if(rc != SQLITE_OK){
        fprintf(stderr, "SQLITE SELECT error: %s \n",errorMsg);
        exit(EXIT_FAILURE);
    }

    free(statement);

    if (rows != MAX_CACHE_ROWS_NUM) {
        return 0; // false
    }

    return 1;   // true
}

/*  Delete from CONV_IMG table the older image inserted (with greater lifetime)   */
void deleteByAge()
{
    int rc;
    char *statement, *errorMsg;

    sqlite3 *db;
    /*if ((db=malloc(sizeof(sqlite3)))==NULL) {
        perror("error in malloc db\n");
        exit(EXIT_FAILURE);
    }*/
    db_open(db);

    statement = malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("delete image by name malloc error");
        return;
    }

    sprintf(statement, "DELETE FROM CONV_IMG WHERE DATEDIFF(CURDATE(),Last_Modified)>=%d;",TIMEOUT);

    rc = sqlite3_exec(db,statement,0,0,&errorMsg);
    if(rc != SQLITE_OK){
        fprintf(stderr,"SQLITE DELETE ERROR: %s \n",errorMsg);
        return;
    }

    free(statement);
    db_close(db);
}

/*  Delete from CONV_IMG table all the image where lifetime is greater than LIFETIME value  */
void deleteByTimeout()
{
    int rc;
    char *statement, *errorMsg;

    sqlite3 *db;
    /*if ((db=malloc(sizeof(sqlite3)))==NULL) {
        perror("error in malloc db\n");
        exit(EXIT_FAILURE);
    }*/
    db_open(db);

    statement = malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("delete image by name malloc error");
        return;
    }
// TODO
    sprintf(statement, "DELETE FROM CONV_IMG WHERE Last_Modified='';");

    rc = sqlite3_exec(db,statement,0,0,&errorMsg);
    if(rc != SQLITE_OK){
        fprintf(stderr,"SQLITE DELETE ERROR: %s \n",errorMsg);
        return;
    }

    free(statement);
    db_close(db);
}

/*  Update date of the last access at that image in the server cache.
 *
 * @param adaptImg: struct conv_img * that describes row index of cache table to update (name_code)
 *                  and date of end manipulation
 */
void updateDate(struct conv_img *adaptedImg)
{
    int rc;
    char *statement, *errorMsg;

    sqlite3 *db;
    /*if ((db=malloc(sizeof(sqlite3)))==NULL) {
        perror("error in malloc db\n");
        exit(EXIT_FAILURE);
    }*/
    db_open(db);

    statement = malloc(MAXLINE * sizeof(char));
    if(statement == NULL){
        perror("delete image by name malloc error");
        return;
    }

    sprintf(statement, "UPDATE CONV_IMG SET Last_Modified='%s' WHERE Name=%ld;",
            adaptedImg->last_modified,adaptedImg->name_code);

    rc = sqlite3_exec(db,statement,0,0,&errorMsg);
    if(rc != SQLITE_OK){
        fprintf(stderr,"SQLITE UPDATE ERROR: %s \n",errorMsg);
        return;
    }

    free(statement);
    db_close(db);
}