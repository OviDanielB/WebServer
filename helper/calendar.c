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


/**
 * Compose a string indicating date of today in the format:
 * "Thu, 19 Feb 2009 12:27:04 GMT"
 */
char *getTodayString()
{
    struct tm today = getToday();
    char weekDay[3];
    char month[3];

    switch (today.tm_wday) {
        case 0:
            weekDay = "Mon";
        break;
        case 1:
            weekDay = "Tue";
            break;
        case 2:
            weekDay = "Wed";
            break;
        case 3:
            weekDay = "Thu";
            break;
        case 4:
            weekDay = "Fri";
            break;
        case 5:
            weekDay = "Sat";
            break;
        case 6:
            weekDay = "Sun";
            break;
    }

    switch (today.tm_mon) {
        case 0:
            month = "Jan";
            break;
        case 1:
            month = "Feb";
            break;
        case 2:
            month = "Mar";
            break;
        case 3:
            month = "Apr";
            break;
        case 4:
            month = "May";
            break;
        case 5:
            month = "Jun";
            break;
        case 6:
            month = "Jul";
            break;
        case 7:
            month = "Aug";
            break;
        case 8:
            month = "Sep";
            break;
        case 9:
            month = "Oct";
            break;
        case 10:
            month = "Nov";
            break;
        case 11:
            month = "Dec";
            break;
    }

    char *date = weekDay+", "+today.tm_mday+" "+month+" "+today.tm_year+" "
                 +today.tm_hour+":"+today.tm_min+":"+today.tm_sec+" GMT";
    return date;
}

/**
 * Convert date from string to struct tm.
 *
 * @param date: string of HTTP message that describe date
 */
struct tm *fromStringToTm(char *date)
{
    struct tm *date;
    char weekDay = date[0]+date[1]+date[2];
    char mDay = date[5]+date[6];
    char month = date[8]+date[9]+date[10];
    char year = date[12]+date[13]+date[14]+date[15];
    char h = date[17]+date[18];
    char m = date[20]+date[21];
    char s = date[23]+date[24];

    int wDay, mon;

    switch (weekDay) {
        case "Mon":
            wDay = 0;
            break;
        case "Tue":
            wDay = 1;
            break;
        case "Wed":
            wDay = 2;
            break;
        case "Thu":
            wDay = 3;
            break;
        case "Fri":
            wDay = 4;
            break;
        case "Sat":
            wDay = 5;
            break;
        case "Sun":
            wDay = 6;
            break;
    }

    switch (mon) {
        case "Jan":
            mon = 0;
            break;
        case "Feb":
            mon = 1;
            break;
        case "Mar":
            mon = 2;
            break;
        case "Apr":
            mon = 3;
            break;
        case "May":
            mon = 4;
            break;
        case "Jun":
            mon = 5;
            break;
        case "Jul":
            mon = 6;
            break;
        case "Aug":
            mon = 7;
            break;
        case "Sep":
            mon = 8;
            break;
        case "Oct":
            mon = 9;
            break;
        case "Nov":
            mon = 10;
            break;
        case "Dec":
            mon = 11;
            break;
    }

    date->tm_wday = wDay;
    date->tm_mday = mDay;
    date->tm_mon = mon;
    date->tm_year = year;
    date->tm_hour = h;
    date->tm_min = m;
    date->tm_sec = s;
    return date;
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

