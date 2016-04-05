//
// Created by laura_trive on 01/04/16.
//

/**
 * Implementations of functions to manage file locking, to avoid more than one connection
 * accepted by the server listening socket.
 */

#include "locking.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

static struct flock lock_it, unlock_it;

static int lock_fd = -1;
/* fcntl() will fail if lock_init() not called */

/* This function initialize lock on file created from template specified.
 *
 * @param pathname: template to generate a unique filename
 */
void lock_init(char *pathname)
{
    char lock_file[1024]; /* must copy caller's string, in case it's a constant */

    strncpy(lock_file, pathname, sizeof(lock_file));
    /* generate unique temporary filename */
    if ( (lock_fd = mkstemp(lock_file)) < 0) {
        fprintf(stderr, "error in mkstemp");
        exit(1);
    }
    /* delete a name from filesystem */
    if (unlink(lock_file) == -1) {
        /* but lock_fd remains open */
        fprintf(stderr, "error in unlink for %s", lock_file);
        exit(1);
    }

    /* set struct flock to acquire lock on file at initial position */
    lock_it.l_type = F_WRLCK;
    lock_it.l_whence = SEEK_SET;
    lock_it.l_start = 0;
    lock_it.l_len = 0;

    /* set struct flock to release lock on file at initial position */
    unlock_it.l_type = F_UNLCK;
    unlock_it.l_whence = SEEK_SET;
    unlock_it.l_start = 0;
    unlock_it.l_len = 0;
}

/*  This function acquires the lock, obtaining exclusive access to file */
void lock_wait()
{
    int rc;
    while ( (rc = fcntl(lock_fd, F_SETLKW, &lock_it)) < 0) {
        if (errno == EINTR) continue; // if interrupted by a signal
        else {
            fprintf(stderr, "error fcntl in lock_wait");
            exit(1);
        }
    }
}

/*  This function releases the lock on file */
void lock_release()
{
    if (fcntl(lock_fd, F_SETLKW, &unlock_it) < 0) {
        fprintf(stderr, "error fcntl in lock_release");
        exit(1);
    }
}