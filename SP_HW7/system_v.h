#ifndef SYSTEM_V_H
#define SYSTEM_V_H
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// Function prototypes
void (*my_sigset(int sig, void (*disp)(int)))(int);
int my_sighold(int sig);
int my_sigrelse(int sig); 
int my_sigignore(int sig);
int my_sigpause(int sig);

#endif // SYSTEM_V_H

