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
    char date[12];

    time_t current_time;
    char *current_time_str;

    current_time = time(NULL);

    current_time_str = ctime(&current_time);

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

char *echo_size()
{

}

void *logonfile(struct req *request, struct conv_img *image, char *clientIPAddr)
{
    struct logline *log_line;

    if ((log_line=malloc(sizeof(struct logline)))==NULL){
        perror("error in malloc");
        return NULL;
    }

    log_file = fopen("PATH", "w");

    log_line->ip_host = echo_ip_host(clientIPAddr);
    log_line->date = echo_date();
    log_line->reqline = echo_req_line(request);
    log_line->status = echo_status();
    log_line->size = echo_size();

    if (log_file!=NULL){

        fwrite(log_line, sizeof(struct logline), 1, log_file);
        fclose(log_file);
    }

    /*log_file=fopen("FILE_PATH","w");
    sprintf(log_line->ip_host, echo_ip_host());
    sprintf(log_line->ip_host, echo_user_id());
    sprintf(log_line->ip_host, echo_reqpers_id());
    sprintf(log_line->ip_host, echo_date());
    sprintf(log_line->ip_host, echo_req_line());
    sprintf(log_line->ip_host, echo_status());
    sprintf(log_line->ip_host, echo_size());
    fclose(log_file);*/

}