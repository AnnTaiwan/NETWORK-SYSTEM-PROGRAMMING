#include "popen_pclose.h"
#include <errno.h>      // For error handling
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define PAT_SIZE 256  // Increase to allow for longer commands
#define PCMD_BUF_SIZE (PAT_SIZE)

void printStatus(int status) {
    if (WIFEXITED(status)) {
        printf("\tChild exited, status=%d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("\tChild killed by signal %d (%s)\n", WTERMSIG(status), strsignal(WTERMSIG(status)));
    } else if (WIFSTOPPED(status)) {
        printf("\tChild stopped by signal %d (%s)\n", WSTOPSIG(status), strsignal(WSTOPSIG(status)));
    } else if (WIFCONTINUED(status)) {
        printf("\tChild continued\n");
    }
}

int main(int argc, char *argv[]) {
    char cmd[PAT_SIZE];                // Buffer for command input
    FILE *fp;                          // File stream returned by my_popen()
    int status;

    while (1) {                        // Loop to accept commands
        printf("myshell -> ");         // Prompt for command input
        fflush(stdout);
        
        if (fgets(cmd, PAT_SIZE, stdin) == NULL) {
            printf("\n");              // Handle EOF gracefully
            break;
        }

        // Remove trailing newline character
        cmd[strcspn(cmd, "\n")] = 0;

        // Handle exit command
        if (strcmp(cmd, "exit") == 0) {
            break;                     // Exit the shell
        }

        // Execute the command
        fp = my_popen(cmd, "r");      // Use custom my_popen()
        if (fp == NULL) {
            printf("my_popen() failed\n");
            continue;                  // Prompt again
        }

        // Read resulting output until EOF
        char pathname[PATH_MAX];
        while (fgets(pathname, PATH_MAX, fp) != NULL) {
            printf("%s", pathname);
        }

        // Close pipe and get the exit status
        status = my_pclose(fp);        // Use custom my_pclose()
        printf("\tmy_pclose() status == %#x\n", (unsigned int) status);

        if (status != -1) {
            printStatus(status);       // Display status information
        }
    }

    return 0;                          // Exit the shell
}

