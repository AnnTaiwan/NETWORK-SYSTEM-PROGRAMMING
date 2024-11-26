#include <errno.h>      // For error handling
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#ifndef BUF_SIZE 
#define BUF_SIZE 1024
#endif

void errExit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
int main(int argc, char *argv[]) {
    if(argc < 4)
    {
        fprintf(stderr, "Usage Error: %s t k N\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    float t, k, N;
    t = atoi(argv[1]);
    k = atoi(argv[2]);
    N = atoi(argv[3]);
    
    pid_t childPid;
    pid_t pipelinePgid;         
    childPid = fork();
    switch (childPid) 
    {
        case -1: /* fork() failed */
     /* Handle error */
        case 0: /* Child */
            if (setpgid(0, pipelinePgid) == -1)
        /* Handle error */
     /* Child carries on to exec the required program */
        default: /* Parent (shell) */
            if (setpgid(childPid, pipelinePgid) == -1 && errno != EACCES)
            {}
        /* Handle error */
     /* Parent carries on to do other things */
      }
    
    return 0;                          
}

