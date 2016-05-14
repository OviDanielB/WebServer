//
// Created by ovi on 4/5/16.
//

#ifndef WEBSERVER_DB_HELPER_H
#define WEBSERVER_DB_HELPER_H

#include <sqlite3.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "../constants.h"
#include "db_cache.h"

/*  Creating database if not exists and opening it  */
sqlite3 *dbOpen();
/*  Closing database    */
void dbClose(sqlite3 *db);
/*  Executing an SQL statement on database  */
int dbExecuteStatement(const char *sql, int (*callback)(void *, int, char **, char **), void *arg);
/*  Insert new image into database in table IMAGES or CONV_IMG (cache)  */
int dbInsertImg(struct img *originalImg, struct conv_img *convImg);
/*  Select from database an image from table IMAGES */
void dbGetImageByName(char *name, struct img *image);
/*  Deleting image from database    */
void dbDeleteByImageName(char *name, unsigned long code);
/*  Loading into Database all images contained in the server directory of file  */
struct img **dbLoadAllImages(char *path);
/*  Loading into Database all images contained in the server cache directory of file previously manipulated  */
void db_load_cache_images(char *path);


#endif //WEBSERVER_DB_HELPER_H
