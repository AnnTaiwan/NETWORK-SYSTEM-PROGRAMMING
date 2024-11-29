#ifndef PROGRAM_H
#define PROGRAM_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#define HEAD_MSG "This is message"
#define SHM_NAME "/shm_buffer"
#define SHM_SIGNAL_NAME "/shm_signal"
#define MAX_CONSUMERS 1000
#define MAX_NUMBER_LEN 20
void errExit(const char*msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
#endif
