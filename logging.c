//
// Created by christy on 4/23/16.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "constants.h"

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


void free_buffer(char buffer[50][300])
{
    printf("flushing the buffer\n");
    int i;
    for (i = 0; i < sizeof(buffer)/ sizeof(*buffer)+1; i++){
        strcpy(buffer[i], "");
    }
    printf("buffer flushed\n");
}

void write_on_buffer(char *line, char buffer[50][300])
{
    printf("writing on the buffer\n");
    int i;
    for(i=0; i < sizeof(buffer)/ sizeof(*buffer)+1; i++) {
        if (strcmp(buffer[i], "")==0) {
            strcpy(buffer[i], line);
            printf("added element is: %s\n", buffer[i]);
        }
    }

}

void write_on_file(char buffer[50][300])
{
    FILE * log_file;
    pthread_mutex_t lock;

    printf("opening the file\n");
    /*  open server logging file in append mode   */
    log_file = fopen("LOG_PATH", "a");
    if (log_file == NULL) {
        perror("error in opening file.");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_lock(&lock);
    /*write on server log and close it*/
    printf("writing on the file\n");
    int i;
    for (i = 0; i < sizeof(buffer)/ sizeof(*buffer)+1; ++i) {
        if (fwrite(buffer[i], strlen(buffer[0]), sizeof(buffer)/ sizeof(*buffer), log_file) == -1) {
            perror("writing log error\n");
            exit(EXIT_FAILURE);
        }
    }
    pthread_mutex_unlock(&lock);
    printf("closing the file\n");
    fclose(log_file);
}

void *logonfile(struct logline *log, char log_buffer[50][300])
{
    char *line;

    line = malloc (sizeof(char)*(strlen(log->reqline)+strlen(log->ip_host)+strlen(log->date)+strlen(log->status)+MAXLINE));
    if (line == NULL) {
        perror("malloc error\n");
        exit(EXIT_FAILURE);
    }

    sprintf(line, "%s - %s  - %s - %s - %ld\n", log->ip_host, log->date, log->reqline, log->status, log->size);

    /* if buffer is not full, write the line in the buffer*/
    if (strcmp(log_buffer[sizeof(log_buffer)/sizeof(*log_buffer)], "")!=0) {
        printf("buffer not full\n");
        write_on_buffer(line, log_buffer);
    }
        /* if buffer is full, write on log file lines in the buffer, free the buffer and write the line in the buffer */
    else {
        printf("buffer full\n");
        write_on_file(log_buffer);
        free_buffer(log_buffer);
        write_on_buffer(line, log_buffer);
    }
}