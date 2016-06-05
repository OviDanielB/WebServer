/**
 * Functions prototypes to manage file locking.
 */

#ifndef WEBSERVER_LOCKING_H
#define WEBSERVER_LOCKING_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

/* This function initialize lock on file created from template specified.   */
void lock_init(char *pathname);

/*  This function acquires the lock, obtaining exclusive access to file */
void lock_wait();

/*  This function releases the lock on file */
void lock_release();

#endif //WEBSERVER_LOCKING_H