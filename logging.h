//
// Created by Sasha on 13/05/16.
//

#ifndef WEBSERVER_LOGGING_H
#define WEBSERVER_LOGGING_H

void *log_clientIP(char *clientIPAddr, struct logline *log);

void *log_date(struct logline *log);

void *log_requestline(struct logline *log, struct req *request);

void *log_status(char *result, struct logline *log);

void *log_size(struct logline *log, size_t length);

void *logonfile(struct logline *log);

#endif //WEBSERVER_LOGGING_H

