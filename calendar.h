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

/* Compare dates */
int cmpDates(struct tm *date1, struct tm *date2);

#endif //WEBSERVER_CALENDAR_H
