//
// Created by root on 4/23/16.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "constants.h"

FILE *log_file;

char *echo_ip_host(char *clientIPAddr)
{
    /*return IP address of the client*/
    return clientIPAddr;
}

/*char *echo_user_id()
{
}
char *echo_reqpers_id()
{
}*/

char *echo_date()
{

    char date[24];
    time_t current_time;
    char *current_time_str;

    current_time = time(NULL);

    /*convert time in string*/
    current_time_str = ctime(&current_time);

    /*return the current date and time in the format "Day Month Date HH:MM:SS Year"*/
    return current_time_str;

}

char *echo_req_line(struct req *request)
{
    char final_string[300] = "";

    strcat(final_string, request->method);
    strcat(final_string, " /");
    strcat(final_string, request->resource);
    strcat(final_string, " HTTP 1.1");

    return final_string;

}

char *echo_status()
{

}

char *echo_size(struct conv_img *image)
{
    char size[1000] = "";

    sprintf(size,"%ld", image->length);

    return size;
}

void *logonfile(char *clientIPAddr, struct req *request, struct conv_img *image)
{
    struct logline *log_line;

    /*malloc of struct*/
    if ((log_line=malloc(sizeof(struct logline)))==NULL){
        perror("error in malloc");
        return NULL;
    }

    /*open server log*/
    log_file = fopen("PATH", "w");
    if (log_file == NULL) {
        perror("error in opening file.");
    }

    /*fill the struct*/
    log_line->ip_host = echo_ip_host(clientIPAddr);
    log_line->date = echo_date();
    log_line->reqline = echo_req_line(request);
    log_line->status = echo_status();
    log_line->size = echo_size(image);

    /*write on server log and close it*/
    if (log_file!=NULL){
        fwrite(log_line, sizeof(struct logline), 1, log_file);
        fclose(log_file);
    }

}