#ifndef MESSAGE_H
#define MESSAGE_H

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

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define HEAD_MSG "This is message"
#define SHM_SIGNAL_NAME "/shm_signal"
#define MAX_NUMBER_CLIENT 3
#define BUF_SIZE 256
#define RESOURCE "TUNNEL_QUIZ3_1" 
typedef struct {
    int src_id;              
    int dest_id;
    char buffer[BUF_SIZE]; 
} Message;

void errExit(const char*msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
#endif
