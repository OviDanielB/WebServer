//
// Created by Sasha on 13/05/16.
//

#ifndef WEBSERVER_LOGGING_H
#define WEBSERVER_LOGGING_H

void free_buffer(char *buffer);

void write_on_file(FILE *log_file, char *log_buffer);

void write_on_buffer(char *line, char *buffer);

void *log_date(struct logline *log);

void *log_requestline(struct logline *log, struct req *request);

void *logonfile(struct logline *log, char *log_buffer);

#endif //WEBSERVER_LOGGING_H

