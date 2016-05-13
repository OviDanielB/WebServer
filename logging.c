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
    strcat(log->ip_host, clientIPAddr);
}

void *log_date(struct logline *log)
{

    time_t current_time;
    char *current_time_str;

    current_time = time(NULL);

    /*convert time in string*/
    current_time_str = ctime(&current_time);

    /*fill date field in struct logline with the current date and time in the format "Day Month Date HH:MM:SS Year"*/
    strcat(log->date, current_time_str);

}

void *log_requestline(struct logline *log, struct req *request)
{
    /*generate HTTP request line and fill reqline field in struct logline with HTTP request line*/
    strcat(log->reqline, request->method);
    strcat(log->reqline, " /");
    strcat(log->reqline, request->resource);
    strcat(log->reqline, " HTTP 1.1");

}

void *log_status(char status[3], char result[16], struct logline *log)
{
    /*generate HTTP response status*/
    strcat(status, "400 ");
    strcat(status, result);

    /*fill status field in struct logline with HTTP response status*/
    strcat(log->status, status);
}

void *log_size(struct logline *log, struct conv_img *image)
{
    char image_str[]="";

    /*fill size field in struct logline with image lenght*/
    sprintf(image_str, "%zu", image->length);
    strcat(log->size, image_str);

}

void *logonfile(void *logLine)
{

    FILE * log_file;
    struct logline *log = (struct logline *) logLine;

    //TODO: dove definire log_file?

    /*malloc of struct*/
    if ((log=malloc(sizeof(struct logline)))==NULL){
        perror("error in malloc");
        return NULL;
    }

    /*open server log*/
    log_file = fopen("PATH", "w");
    if (log_file == NULL) {
        perror("error in opening file.");
    }

    /*write on server log and close it*/
    if (log_file!=NULL){
        fwrite(log, sizeof(struct logline), 1, log_file);
        fclose(log_file);
    }

}