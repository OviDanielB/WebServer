//
// Created by laura_trive on 01/04/16.
//

/**
 * Functions prototypes to manage file locking, to avoid more than one connection
 * accepted by the server listening socket.
 */

#ifndef WEBSERVER_LOCKING_H
#define WEBSERVER_LOCKING_H

/* This function initialize lock on file created from template specified.   */
void lock_init(char *pathname);

/*  This function acquires the lock, obtaining exclusive access to file */
void lock_wait();

/*  This function releases the lock on file */
void lock_release();

#endif //WEBSERVER_LOCKING_H