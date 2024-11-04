#ifndef POPEN_PCLOSE_H
#define POPEN_PCLOSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#ifndef BUF_SIZE 
#define BUF_SIZE 1024
#endif
// Structure to hold FILE* and PID associations
typedef struct {
    FILE *fp;
    pid_t pid;
} popen_entry;
static popen_entry popen_table[BUF_SIZE];
// Function prototypes
void errExit(const char *msg); 
FILE *my_popen(const char *command, const char *mode);
int my_pclose(FILE *stream);

#endif // POPEN_PCLOSE_H

