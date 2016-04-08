//
// Created by ovi on 25/03/16.
//

#ifndef WEBSERVER_IO_FUNC_H
#define WEBSERVER_IO_FUNC_H

#include <glob.h>
#include <stdio.h>

ssize_t readn(int fd, void *buf, size_t n);
ssize_t	writen(int fd, const void *buf, size_t n);
int	readline(int fd, void *buf, int maxlen);
ssize_t write_fd(int fd, void *ptr, size_t nbytes, int sendfd);
ssize_t read_fd(int fd, void *ptr, size_t nbytes, int *recvfd);


#endif //WEBSERVER_IO_FUNC_H

