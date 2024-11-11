/* exercise22_3.c

   This program times how fast signals are sent and received.

   The program forks to create a parent and a child process that alternately
   send signals to each other (the child starts first). Each process catches
   the signal with a handler, and waits for signals to arrive using
   sigwaitinfo().
   
   sigwaitinfo(): wait for any blocked-signal in the blocked-set
   `int sigwaitinfo(const sigset_t *restrict set, siginfo_t *_Nullable restrict info);`
   * info will receive the detail information, like signal number and process ID of source of signal
   Usage: $ time ./exercise22_3 num-sigs

   The 'num-sigs' argument specifies how many times the parent and
   child send signals to each other.

   Child                                  Parent

   for (s = 0; s < numSigs; s++) {        for (s = 0; s < numSigs; s++) {
       send signal to parent                  wait for signal from child
       wait for a signal from parent          send a signal to child
   }                                      }
*/
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define TESTSIG SIGUSR1
void errExit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
int getInt(const char *arg, int min_value, const char *name) {
    char *endptr;
    long val = strtol(arg, &endptr, 10);
    // strtol is used to convert the argument to a long integer. endptr points to the first invalid character in arg if conversion stops.
    
    // Check if strtol encountered a non-numeric character or overflowed
    if (errno != 0 || *endptr != '\0' || endptr == arg) {
        fprintf(stderr, "Error: Invalid integer value for %s\n", name);
        exit(EXIT_FAILURE);
    }

    // Check if the value is less than the minimum required
    if (val < min_value) {
        fprintf(stderr, "Error: %s must be greater than %d\n", name, min_value - 1);
        exit(EXIT_FAILURE);
    }

    return (int) val;
}


int main(int argc, char *argv[])
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
    {
        fprintf(stderr, "%s num-sigs\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int numSigs = getInt(argv[1], 1, "num-sigs"); 


    siginfo_t si; // get the info

    /* Block the signal before fork(), so that the child doesn't manage
       to send it to the parent before the parent is ready to catch it */

    sigset_t blockedMask;
    sigemptyset(&blockedMask);
    sigaddset(&blockedMask, TESTSIG);
    if (sigprocmask(SIG_SETMASK, &blockedMask, NULL) == -1)
        errExit("sigprocmask");
        
    pid_t childPid = fork();
    switch (childPid) {
    case -1: errExit("fork");

    case 0:     /* child */
        for (int scnt = 0; scnt < numSigs; scnt++) {
            if (kill(getppid(), TESTSIG) == -1)
                errExit("kill");
            if (sigwaitinfo(&blockedMask, &si) == -1)
                    errExit("sigwaitinfo");
        }
        exit(EXIT_SUCCESS);

    default: /* parent */
        for (int scnt = 0; scnt < numSigs; scnt++) {
            if (sigwaitinfo(&blockedMask, &si) == -1)
                    errExit("sigwaitinfo");
            if (kill(childPid, TESTSIG) == -1)
                errExit("kill");
        }
        exit(EXIT_SUCCESS);
    }
}
