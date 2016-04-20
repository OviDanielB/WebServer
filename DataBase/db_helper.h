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

/*  Creating database if not exists and opening it  */
void db_open(sqlite3 * db);
/*  Closing database    */
void db_close(sqlite3 * db);
/*  Insert new image into database in table IMAGES or CONV_IMG (cache)  */
void db_insert_img(struct img *originalImg, struct conv_img *convImg);
/*  Select from database an image from table IMAGES or CONV_IMG (cache) */
void db_get_image_by_name(char *name,struct img * image);
/*  Deleting image from database    */
void db_delete_image_by_name(char *name);

#endif //WEBSERVER_DB_HELPER_H
