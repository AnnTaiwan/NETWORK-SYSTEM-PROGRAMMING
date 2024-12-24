#ifndef STACK_H
#define STACK_H
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h> 
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <sys/wait.h>
#include <semaphore.h>
#define SHM_NAME "/shm_buffer"
#define STACK_SIZE 3
#define NUM_CHILD_PROCESSES 5
#define NUM_ACTION 4 // each child process has 4 actions, better to be even number
#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

struct stack {
    char buf[STACK_SIZE];
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t conditionVar;
};

struct action
{
    int pop_or_push; // 0: pop, 1: push
    char value;
};

void errExit(const char*msg);
void push(struct stack *s, char a);
char pop(struct stack *s);
void showstack(struct stack *s);

#endif
