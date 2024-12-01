#ifndef PROGRAM_H
#define PROGRAM_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#define HEAD_MSG "This is message"
#define SHM_NAME "/shm_buffer"
#define SHM_SIGNAL_NAME "/shm_signal"
#define SHM_LOSS_NAME "/shm_loss"
#define MAX_CONSUMERS 1100
#define MAX_NUMBER_LEN 20
typedef struct {
    int shared_var;              // The variable to protect
    pthread_mutex_t mutex;       // Mutex to protect the variable
} SharedData;

void errExit(const char*msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
#endif
