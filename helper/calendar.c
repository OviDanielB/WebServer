//
// Created by laura_trive on 21/03/16.
//

/**
 * Implementation of functions to manage and compare dates
 */

#include "calendar.h"

/* Today date
 *
 * @return: structure with day, month, year, sec... informations
 */
struct tm *getToday()
{
    time_t timer = time(NULL);
    return gmtime(&timer);
}


/**
 * Compose a string indicating date of today in the format of date in the HTTP line:
 * "Thu, 19 Feb 2009 12:27:04 GMT"
 */
char *getTodayToHTTPLine()
{
    struct tm *today = getToday();
    char weekDay[3];
    char month[3];

    switch (today->tm_wday) {
        case 0:
            sprintf(weekDay,"Mon");
        break;
        case 1:
            sprintf(weekDay,"Tue");
            break;
        case 2:
            sprintf(weekDay,"Wed");
            break;
        case 3:
            sprintf(weekDay,"Thu");
            break;
        case 4:
            sprintf(weekDay,"Fri");
            break;
        case 5:
            sprintf(weekDay,"Sat");
            break;
        case 6:
            sprintf(weekDay,"Sun");
            break;
        default:break;
    }

    switch (today->tm_mon) {
        case 0:
            sprintf(month,"Jan");
            break;
        case 1:
            sprintf(month,"Feb");
            break;
        case 2:
            sprintf(month,"Mar");
            break;
        case 3:
            sprintf(month,"Apr");
            break;
        case 4:
            sprintf(month,"May");
            break;
        case 5:
            sprintf(month,"Jun");
            break;
        case 6:
            sprintf(month,"Jul");
            break;
        case 7:
            sprintf(month,"Aug");
            break;
        case 8:
            sprintf(month,"Sep");
            break;
        case 9:
            sprintf(month,"Oct");
            break;
        case 10:
            sprintf(month,"Nov");
            break;
        case 11:
            sprintf(month,"Dec");
            break;
        default:break;
    }

    char date[50];
    sprintf(date,"%s, %d %s %d %d:%d:%d GMT",
            weekDay,today->tm_mday,month,today->tm_year,today->tm_hour,today->tm_min,today->tm_sec);
    return date;
}

/**
 * Compose a string indicating date of today in the format of SQL date:
 * "2009-02-19"
 */
char *getTodayToSQL()
{
    struct tm *today = getToday();
    char date[20];
    sprintf(date,"%d-%d-%d",today->tm_year,today->tm_mon,today->tm_mday);

    return date;
}


/**
 * Convert date from string to struct tm.
 *
 * @param date: string of HTTP message that describe date
 */
struct tm *fromStringToTm(char *date)
{
    struct tm *dateTm;
    char weekDay[3];
    char month[3];
    if ((dateTm = malloc(sizeof(struct tm)))==NULL){
        perror("error in malloc\n");
        exit(1);
    }
    int mDay;
    if ((mDay = (int) malloc(sizeof(int)))==NULL) {
        perror("error in malloc\n");
        exit(1);
    }
    int year;
    if ((year = (int) malloc(sizeof(int)))==NULL){
        perror("error in malloc\n");
        exit(1);
    }
    int h; // hour
    if ((h = (int) malloc(sizeof(int)))==NULL){
        perror("error in malloc\n");
        exit(1);
    }
    int m; // minutes
    if ((m = (int) malloc(sizeof(int)))==NULL){
        perror("error in malloc\n");
        exit(1);
    }
    int s; // seconds
    if ((s = (int) malloc(sizeof(int)))==NULL){
        perror("error in malloc\n");
        exit(1);
    }

    sscanf(date,"%s, %d %s %d %d:%d:%d GMT", weekDay,&mDay,month,&year,&h,&m,&s);

    int wDay=-1, mon=-1;

    if (strcmp(weekDay,"Mon")==0) {
        wDay = 0;
    }
    if (strcmp(weekDay,"Tue")==0) {
        wDay = 1;
    }
    if (strcmp(weekDay,"Wed")==0) {
        wDay = 2;
    }
    if (strcmp(weekDay,"Thu")==0) {
        wDay = 3;
    }
    if (strcmp(weekDay,"Fri")==0) {
        wDay = 4;
    }
    if (strcmp(weekDay,"Sat")==0) {
        wDay = 5;
    }
    if (strcmp(weekDay,"Sun")==0) {
        wDay = 6;
    }

    if (strcmp(month,"Jan")==0) {
        mon = 0;
    }
    if (strcmp(month,"Feb")==0) {
        mon = 1;
    }
    if (strcmp(month,"Mar")==0) {
        mon = 2;
    }
    if (strcmp(month,"Apr")==0) {
        mon = 3;
    }
    if (strcmp(month,"May")==0) {
        mon = 4;
    }
    if (strcmp(month,"Jun")==0) {
        mon = 5;
    }
    if (strcmp(month,"Jul")==0) {
        mon = 6;
    }
    if (strcmp(month,"Aug")==0) {
        mon = 7;
    }
    if (strcmp(month,"Sep")==0) {
        mon = 8;
    }
    if (strcmp(month,"Oct")==0) {
        mon = 9;
    }
    if (strcmp(month,"Nov")==0) {
        mon = 10;
    }
    if (strcmp(month,"Dec")==0) {
        mon = 11;
    }

    dateTm->tm_wday = wDay;
    dateTm->tm_mday = mDay;
    dateTm->tm_mon = mon;
    dateTm->tm_year = year;
    dateTm->tm_hour = h;
    dateTm->tm_min = m;
    dateTm->tm_sec = s;

    return dateTm;
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

