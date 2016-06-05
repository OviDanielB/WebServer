/**
 * Functions prototypes to manage date formats
 */

#ifndef WEBSERVER_CALENDAR_H
#define WEBSERVER_CALENDAR_H

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

/* Today date */
struct tm *getToday();

/* Compose a string indicating date of today in the format:
 * "Thu, 19 Feb 2009 12:27:04 GMT" */
char *getTodayToHTTPLine();

/* Compose a string indicating date of today in the format of SQL date:
 * "2009-02-19" */
char *getTodayToSQL();

#endif //WEBSERVER_CALENDAR_H
