//
// Created by laura_trive on 21/03/16.
//
/**
 * Functions prototypes to manage and compare dates
 */

#ifndef WEBSERVER_CALENDAR_H
#define WEBSERVER_CALENDAR_H

/* Today date */
struct date getToday();

/* Convert date from string to struct tm */
struct tm *fromStringToTm(char *date);

/**
 * Compose a string indicating date of today in the format:
 * "Thu, 19 Feb 2009 12:27:04 GMT"
 */
char *getTodayString();

/* Compare dates */
int cmpDates(struct tm *date1, struct tm *date2);

#endif //WEBSERVER_CALENDAR_H