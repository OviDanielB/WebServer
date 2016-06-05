//
// Created by Sasha on 13/05/16.
//

#ifndef WEBSERVER_LOGGING_H
#define WEBSERVER_LOGGING_H

void free_buffer(char *buffer);

void write_on_file(FILE *log_file, char *log_buffer);

void write_on_buffer(char *line, char *buffer);

void *logOnFile(struct log *log, struct log **logBuffer);

#endif //WEBSERVER_LOGGING_H

