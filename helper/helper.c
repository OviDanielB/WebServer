//
// Created by laura_trive on 21/03/16.
//

/**
 *  Implementation of helper functions.
 */

#include "helper.h"


/**
 *  This function executes a system command line.
 *
 *  @param command: command string to execute on terminal
 */
int execCommand(const char *command)
{
    // execution of command if system is available
    if (system(NULL)==0 || system(command)==-1) {
        perror("error in system\n");
        return 1;
    }
    return 0;
}

/**
 * This function read result line from STDOUT after the execution of a command.
 *
 * @param: cmd = command to execute
 */
char *readResultLine(char *cmd)
{
    int stdout_bk = dup(fileno(stdout));
    int pipefd[2];
    char buf[MAXLINE];

    pipe2(pipefd,O_NONBLOCK);

    dup2(pipefd[1], fileno(stdout));

    while (execCommand(cmd)==1) {}

    fflush(stdout);
    close(pipefd[1]);
    dup2(stdout_bk, fileno(stdout));//restore
    read(pipefd[0], buf, 100);

    return buf;
}

/* Hash function to calculate an (almost) unique identifier for every manipulated image
 * from the resource name, where are indicated the adapted values or the original ones.
 *
 * @param: name = string formatted as <originalname><width><height><qualityfactor><type><colorsnum>
 */
unsigned long getHashCode(unsigned char *name)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *name++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

