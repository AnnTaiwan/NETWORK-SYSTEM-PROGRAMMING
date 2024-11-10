#include "system_v.h"



void errExit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

/* sigset() implementation */
void (*my_sigset(int sig, void (*disp)(int)))(int) {
    struct sigaction new_action, old_action;
    new_action.sa_handler = disp;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    /* Set new handler and retrieve old handler */
    if (sigaction(sig, &new_action, &old_action) == -1) {
        errExit("sigaction");
        return SIG_ERR;
    }
    return old_action.sa_handler;
}

/* sighold() implementation */
// adds sig to the calling process's signal mask.
int my_sighold(int sig) {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, sig);

    /* Block the signal */
    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
        errExit("sigprocmask");
        return -1;
    }
    return 0;
}

/* sigrelse() implementation */
// removes sig from the calling process's signal mask.
int my_sigrelse(int sig) {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, sig);

    /* Unblock the signal */
    if (sigprocmask(SIG_UNBLOCK, &set, NULL) == -1) {
        errExit("sigprocmask");
        return -1;
    }
    return 0;
}

/* sigignore() implementation */
// sets the disposition of sig to SIG_IGN.
// ignore that signal
int my_sigignore(int sig) {
    struct sigaction action;
    action.sa_handler = SIG_IGN; // ignore
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    /* Set signal handler to ignore */
    if (sigaction(sig, &action, NULL) == -1) {
        errExit("sigaction");
        return -1;
    }
    return 0;
}

/* sigpause() implementation */
/* The  function  sigpause()  is designed to wait for some signal.  It changes the process's signal mask (set of blocked signals), 
and then waits for a signal to arrive.  
Upon arrival of a signal, the original signal mask is restored.  */
int my_sigpause(int sig) {
    sigset_t mask;
    sigemptyset(&mask);            
    sigaddset(&mask, sig);         

    sigprocmask(SIG_BLOCK, &mask, NULL);  
    sigdelset(&mask, sig); // remove that sig from the mask in order to receive that sig

    return sigsuspend(&mask); // wait for that signal, will restore the original signal mask after it has finished waiting for the signal
}

