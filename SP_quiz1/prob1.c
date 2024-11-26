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
#ifndef BUF_SIZE 
#define BUF_SIZE 1024
#endif

void errExit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
int main(int argc, char *argv[]) {
    char command[PAT_SIZE];                // Buffer for command input
    char buf[BUF_SIZE];
    ssize_t numRead, numWrite; // number of read and write bytes 
    while (1) {                        // Loop to accept commands
        printf("Commander, please input command: ");         // Prompt for command input
        fflush(stdout);
        
        if (fgets(command, PAT_SIZE, stdin) == NULL) {
            printf("\n");              // Handle EOF gracefully
            break;
        }
        
        // Remove trailing newline character
        command[strcspn(command, "\n")] = 0;
        // Handle exit command
        if (strcmp(command, "exit") == 0) {
            break;                     // Exit the shell
        }
        
        int pd_1[2], pd_2[2]; // declare two pipes
        /* 
          pd_1: parent write, child read
          pd_2: parent read, child write
        */
        if(pipe(pd_1) == -1)
            errExit("pipe");
        if(pipe(pd_2) == -1)
            errExit("pipe");
        int status;
        pid_t pid;
        pid = fork();
        switch(pid)
        {
            case -1:
                errExit("fork");
            case 0: // child
                close(pd_1[1]);
                close(pd_2[0]);
                dup2(pd_2[1], STDOUT_FILENO);
                close(pd_2[1]);
                
                numRead = read(pd_1[0], buf, BUF_SIZE); // read the command name
                if(numRead < 0)
                    errExit("read");
                buf[numRead] = '\0';  // Null-terminate the string
                // Prepare to use execvp by tokenizing the buf string
                char *args[BUF_SIZE];
                char *cmd = strdup(buf);  // Duplicate buf to safely modify it
                
                int i = 0;
                args[i] = strtok(cmd, " ");
                while (args[i] != NULL && i < BUF_SIZE - 1) {
                    args[++i] = strtok(NULL, " ");
                }
                args[i] = NULL;  // Null-terminate the argument array
               
                close(pd_1[0]);
                // Execute command
                execvp(args[0], args);
                perror("execvp");  // If execvp fails, print error
                free(cmd);
                break;
            default: // parent
                close(pd_1[0]);
                close(pd_2[1]);
                
                numWrite = write(pd_1[1], command, strlen(command)); // write to child
                if(numWrite != strlen(command))
                    errExit("parent write to pd_1");
                    
                while((numRead = read(pd_2[0], buf, BUF_SIZE)) > 0) // read it back from child
                {
                    buf[numRead] = '\0';  // Null-terminate the string
                    fputs(buf, stdout);  // Print the uppercase text
                    fflush(stdout);      // Ensure output is displayed
                }
                if (numRead < 0)
                    errExit("parent read from pd_2");
                
                close(pd_1[1]);
                close(pd_2[0]);
                waitpid(pid, &status, 0);
                break;
        }
    }
    return 0;                          // Exit the shell
}

