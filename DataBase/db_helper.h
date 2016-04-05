//
// Created by ovi on 4/5/16.
//

#ifndef WEBSERVER_DB_HELPER_H
#define WEBSERVER_DB_HELPER_H

#include "sqlite3.h"
#include <stdio.h>
#include <stdlib.h>

void db_open(sqlite3 * db);
void db_close(sqlite3 * db);

#endif //WEBSERVER_DB_HELPER_H
