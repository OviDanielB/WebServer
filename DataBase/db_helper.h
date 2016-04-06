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


/* initialized in db_get_image_by_name with char *name[50] and char *type[5] */
typedef struct img {
    char   *name;
    float  quality;
    size_t width;
    size_t height;
    size_t length;
    char   *type;
    //TODO image Date
    //struct tm last_modified;
} img;

void db_open(sqlite3 * db);
void db_close(sqlite3 * db);
void db_insert_img(struct img *image);
void db_get_image_by_name(char *name,struct img * image);
void db_delete_image_by_name(char *name);

#endif //WEBSERVER_DB_HELPER_H
