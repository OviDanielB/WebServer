//
// Created by christy on 4/23/16.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "constants.h"
#include <pthread.h>

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


void free_buffer(char *buffer)
{
    printf("flushing the buffer\n");
    for (int i = 0; i < sizeof(buffer)+1; i++){
        buffer[i]=NULL;
    }
    printf("buffer flushed\n");
}

void write_on_buffer(char *line, char *buffer)
{
    printf("writing on the buffer\n");
    for(int i=0; i< sizeof(buffer); i++) {
        if (buffer[i]==NULL) {
            buffer[i] = (char) line;
        }
    }

}

void write_on_file(FILE *log_file, char *log_buffer)
{
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
    for (int i=0; i< sizeof(log_buffer)+1; i++) {
        if (fwrite(log_buffer[i], sizeof(log_buffer[i]), sizeof(log_buffer) + 1, log_file) == -1) {
            perror("writing log error\n");
            exit(EXIT_FAILURE);
        }
    }
    pthread_mutex_unlock(&lock);
    printf("closing the file\n");
    fclose(log_file);
}

void *logonfile(struct logline *log, char *log_buffer)
{
    FILE * log_file;
    char *line;

    line = malloc (sizeof(char)*(strlen(log->reqline)+strlen(log->ip_host)+strlen(log->date)+strlen(log->status)+MAXLINE));
    if (line == NULL) {
        perror("malloc error\n");
        exit(EXIT_FAILURE);
    }

    sprintf(line, "%s - %s  - %s - %s - %ld\n", log->ip_host, log->date, log->reqline, log->status, log->size);

    /* if buffer is full, write on log file lines in the buffer, flush the buffer and write the line in the buffer */
    if (log_buffer[50] == NULL) {
        printf("buffer not full\n");
        write_on_buffer(line, log_buffer);
    }

        /* if buffer is not full, write the line in the buffer*/

    else {
        printf("buffer full\n");
        write_on_file(log_file, log_buffer);
        free_buffer(log_buffer);
        write_on_buffer(line, log_buffer);
    }

    //pthread_mutex_t lock;

    /*set min priority to thread */

    /*struct sched_param param;
    int min_prio;

    min_prio = sched_get_priority_min(SCHED_RR);

    param.__sched_priority = min_prio;

    printf("priority is: %d\n", min_prio);

    if(pthread_setschedparam(pthread_self(),SCHED_RR, &param)!=0) {
        perror("error in setting priority");
    } else {
        printf("thread priority is %d \n", param.__sched_priority);
    }*/

    /*if(pthread_mutex_init(&lock, NULL)!=0){
        perror("error in initialize lock");
    }*/


}