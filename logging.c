//
// Created by root on 4/23/16.
//

#include <stdio.h>
#include <stdlib.h>
#include <constants.h>
#include "constants.h"

FILE *log_file;

struct logline *logonfile()
{
    struct logline *log_line;
    if ((log_line=malloc(sizeof(struct logline)))==NULL){
        perror("error in malloc");
        return NULL;
    }

    log_file=fopen("FILE_PATH","w");
    sprintf(log_line->ip_host, echo_ip_host());
    sprintf(log_line->ip_host, echo_user_id());
    sprintf(log_line->ip_host, echo_reqpers_id());
    sprintf(log_line->ip_host, echo_date());
    sprintf(log_line->ip_host, echo_req_line());
    sprintf(log_line->ip_host, echo_status());
    sprintf(log_line->ip_host, echo_size());
    fclose(log_file);
    return log_line;

}

char *echo_ip_host()
{

}

char *echo_user_id()
{

}

char *echo_reqpers_id()
{

}

char *echo_date()
{

}

char *echo_req_line()
{

}

char *echo_status()
{

}

char *echo_size()
{

}