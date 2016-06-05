/**
 * Implementation of functions to manage date formats
 */

#include "calendar.h"

/* Today date
 *
 * @return: structure with day, month, year, sec... information
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

    char *date;
    date = (char *) malloc(50*sizeof(char)+1);
    if (date == NULL) {
        perror("error in malloc\n");
        exit(EXIT_FAILURE);
    }

    sprintf(date,"%s, %d %s %d %d:%d:%d GMT",
            weekDay,today->tm_mday,month,today->tm_year+1900,today->tm_hour,today->tm_min,today->tm_sec);
    return date;
}

/**
 * Compose a string indicating date of today in the format of SQL date:
 * "2009-02-19"
 */
char *getTodayToSQL()
{
    struct tm *today = getToday();
    char *date = (char *) malloc(21*sizeof(char));
    if (date == NULL) {
        perror("malloc error\n");
        exit(EXIT_FAILURE);
    }
    sprintf(date, "%d-%d-%d", today->tm_year+1900, today->tm_mon, today->tm_mday);

    return date;
}