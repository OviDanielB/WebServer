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
sqlite3 *db_open();
/*  Closing database    */
void db_close(sqlite3 * db);
/*  Executing an SQL statement on database  */
void db_execute_statement(sqlite3 * db, const char *sql);
/*  Insert new image into database in table IMAGES or CONV_IMG (cache)  */
void db_insert_img(sqlite3 *db,struct img *originalImg, struct conv_img *convImg);
/*  Select from database an image from table IMAGES */
void db_get_image_by_name(sqlite3 *db, char *name, struct img *image);
/*  Deleting image from database    */
void db_delete_image_by_name(sqlite3 * db, char *name);
/*  Loading into Database all images contained in the server directory of file  */
struct img **db_load_all_images(sqlite3 *db, char *path);


#endif //WEBSERVER_DB_HELPER_H
