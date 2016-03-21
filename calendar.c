//
// Created by laura_trive on 21/03/16.
//

/**
 * Implementation of functions to manage and compare dates
 */

#include <stdio.h>
#include <time.h>


/* Today date
 *
 * @return: structure with day, month, year, sec... informations
 */
struct tm getToday()
{
    time_t timer = time();
    return gmtime(timer);
}

/* Compare two dates.
 *
 * @param date1: first date to compare
 * @param date2: second date to compare
 *
 * @return: 1 if date2 is previous date1
 * @return: -1 if date1 is previous date2
 */
int cmpDates(struct tm *date1, struct tm *date2)
{
    if (date1->tm_year<date2->tm_year) {
        return -1;
    } else if (date1->tm_year==date2->tm_year) {
        if (date1->tm_mon<date2->tm_mon) {
            return -1;
        } else if (date1->tm_mon==date2->tm_mon) {
            if (date1->tm_mday<date2->tm_mday) {
                return -1;
            } else if (date1->tm_mday==date2->tm_mday) {
                if (date1->tm_min<date2->tm_min) {
                    return -1;
                } else if (date1->tm_min==date2->tm_min) {
                    if (date1->tm_sec<date2->tm_sec) {
                        return -1;
                    } else
                        return 1;
                } else
                    return 1;
            } else
                return 1;
        } else
            return 1;
    } else
        return 1;
}

