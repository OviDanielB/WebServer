//
// Created by Sasha on 4/23/16.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "constants.h"

void *log_clientIP(char *clientIPAddr, struct logline *log)
{
    /*fill ip_host field in struct logline with IP address of client*/
    sprintf(log->ip_host, clientIPAddr);
}

void *log_date(struct logline *log)
{

    time_t current_time;
    char *current_time_str;

    current_time = time(NULL);

    /*convert time in string*/
    current_time_str = ctime(&current_time);

    /*fill date field in struct logline with the current date and time in the format "Day Month Date HH:MM:SS Year"*/
    sprintf(log->date, current_time_str);
}

void *log_requestline(struct logline *log, struct req *request)
{
    /*generate HTTP request line and fill reqline field in struct logline with HTTP request line*/
    sprintf(log->reqline, "%s /%s.%s %s", request->method, request->resource, request->type, HTTP_1);
}

void *log_status(char *result, struct logline *log)
{
    /*fill status field in struct logline with HTTP response status*/
    sprintf(log->status, result);
}

void *log_size(struct logline *log, size_t length)
{
    /*  fill size field in struct logline with image lenght (= 0 if not successful request)    */
    log->size = length;
}

void *logonfile(void *logLine)
{
    struct logline *log = (struct logline *) logLine;
    FILE * log_file;
    char *line;

    line = malloc (sizeof(char)*(strlen(log->reqline)+strlen(log->ip_host)+strlen(log->date)+strlen(log->status)+MAXLINE));
    if (line == NULL) {
        perror("malloc error\n");
        exit(EXIT_FAILURE);
    }

    sprintf(line, "%s - %s  - %s - %s - %ld\n", log->ip_host, log->date, log->reqline, log->status, log->size);

    /*  open server logging file in append mode   */
    log_file = fopen(LOG_PATH, "a");
    if (log_file == NULL) {
        perror("error in opening file.");
        exit(EXIT_FAILURE);
    }

    /*write on server log and close it*/
    if (fwrite(line, strlen(line), 1, log_file) == -1) {
        perror("writing log error\n");
        exit(EXIT_FAILURE);
    }
    fclose(log_file);
}