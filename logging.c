//
// Created by christy on 4/23/16.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "constants.h"

void freeBuffer(struct log **buffer)
{
    printf("buffer to free\n");

    int i;
    for (i = 0; i < BUFFER_LOG; i++) {
        buffer[i] = realloc(buffer[i], sizeof(struct log));
        sprintf(buffer[i]->ip_host, "*");
    }
}

/*  Saving of line to log into the buffer.
 *  It returns -1 if buffer is full.
 *
 * @param i : index of buffer where save new struct log
 * @param log : struct log which request information are saved in
 * @param buffer : list of struct log to write
 * */
void writeOnBuffer(int i, struct log *log, struct log **buffer)
{
    sprintf(buffer[i]->ip_host, "%s", log->ip_host);
    sprintf(buffer[i]->reqLine, "%s", log->reqLine);
    sprintf(buffer[i]->date, "%s", log->date);
    sprintf(buffer[i]->status, "%s", log->status);
    buffer[i]->size = log->size;
}

void writeOnFile(struct log **buffer)
{
    FILE * log_file;
    pthread_mutex_t lock;

    /*  log line to write on file   */
    char *line;
    line = malloc (sizeof(char)*sizeof(struct log)+MAXLINE);
    if (line == NULL) {
        perror("malloc error\n");
        exit(EXIT_FAILURE);
    }

    printf("opening the file\n");
    /*  open server logging file in append mode   */
    log_file = fopen(LOG_PATH, "a");
    if (log_file == NULL) {
        perror("error in opening file.");
        exit(EXIT_FAILURE);
    }

    int i;
    for (i = 0; i < BUFFER_LOG-1; i++) {
        memset(line, '\0', strlen(line));

        printf("info log to write: %s %s %s %s %ld\n", buffer[i]->ip_host, buffer[i]->reqLine, buffer[i]->date,
        buffer[i]->status, buffer[i]->size);

        sprintf(line, "%s - %s  - %s - %s - %ld\n",
                buffer[i]->ip_host, buffer[i]->date, buffer[i]->reqLine, buffer[i]->status, buffer[i]->size);

        printf("line to write: %s\n",line);

        pthread_mutex_lock(&lock);

        if (fwrite(line, 1, strlen(line), log_file) == -1) {
            perror("writing log error\n");
            exit(EXIT_FAILURE);
        }

        pthread_mutex_unlock(&lock);

    }

    fclose(log_file);
}


void logOnFile(struct log *log, struct log **logBuffer)
{

    /* check if buffer is full finding what position in the buffer hasn't just been used   */


    int i = 0;
    while (strcmp(logBuffer[i]->ip_host, "*") != 0 && i < BUFFER_LOG) {
        /*  loop searching for a buffer position not used, but just initialized at "*" */
        i++;
    }

    if (i < BUFFER_LOG-1) {
        /* buffer not full  */
        printf("buffer not full\n");
        writeOnBuffer(i, log, logBuffer);
    } else {
        printf("buffer full\n");
        /* if buffer is full, write on log file lines in the buffer, free the buffer and write the line in the buffer */
        writeOnFile(logBuffer);
        freeBuffer(logBuffer);
        writeOnBuffer(0, log, logBuffer);
    }
}