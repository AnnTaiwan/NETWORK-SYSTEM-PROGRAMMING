#include "popen_pclose.h"



void errExit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

FILE *my_popen(const char *command, const char *mode)
{
/*
1. create a pipe
2. fork a child
3. exec a shell
*/
    int pd[2];
    // create a pipe
    if(pipe(pd) == -1)
        errExit("pipe");
    pid_t pid;
    pid = fork();
    switch(pid)
    {
        case -1:
            return NULL;
        case 0: // child
            if(strcmp(mode, "r") == 0) // parent read from child
            {    
                close(pd[0]);
                dup2(pd[1], STDOUT_FILENO); // redirect write end to stdout
                close(pd[1]);
            }
            else if(strcmp(mode, "w") == 0)// parent write to child
            {    
                close(pd[1]);
                dup2(pd[0], STDIN_FILENO); // redirect read end to stdin
                close(pd[0]);
            }
            else
            {
                close(pd[0]);
                close(pd[1]);
                return NULL;
            }
            
            // Prepare to use execvp by tokenizing the command string
            char *args[BUF_SIZE];
            char *cmd = strdup(command);  // Duplicate command to safely modify it
            if (cmd == NULL) // error
                return NULL;
                
            int i = 0;
            args[i] = strtok(cmd, " ");
            while (args[i] != NULL && i < BUF_SIZE - 1) {
                args[++i] = strtok(NULL, " ");
            }
            args[i] = NULL;  // Null-terminate the argument array
            
            // Execute command
            execvp(args[0], args);
            perror("execvp");  // If execvp fails, print error
            free(cmd);
            return NULL;
        default: // parent
            break;
    }
    // Parent process
    FILE *stream;
    if (strcmp(mode, "r") == 0) 
    {
        close(pd[1]);                // Parent reads, so close write end
        stream = fdopen(pd[0], "r"); // Open pipe read end as FILE*
        // Record in popen_table
        if (stream != NULL) 
        {
            // record the opened child process and pipe stream's file descriptor
            popen_table[pd[0]].fp = stream;
            popen_table[pd[0]].pid = pid;
        }
    } 
    else 
    {
        close(pd[0]);                // Parent writes, so close read end
        stream = fdopen(pd[1], "w"); // Open pipe write end as FILE*
        // Record in popen_table
        if (stream != NULL) 
        {
            // record the opened child process and pipe stream's file descriptor
            popen_table[pd[1]].fp = stream;
            popen_table[pd[1]].pid = pid;
        }
    }
    return stream;
}

int my_pclose(FILE *stream)
{
/* 
1, close the pipe
2. wait fro the child shell to terminate
*/
    if (stream == NULL) // Invalid FILE* stream
        return -1;  

    int fd = fileno(stream); // get the file descriptor
    if(fd == -1 || popen_table[fd].fp == NULL) // error
        return -1;
    pid_t pid = popen_table[fd].pid; // record the child process's pid that would be closed
    int status; // receive the exit status
    
    close(fd); //close the pipe
    if (waitpid(pid, &status, 0) == -1) 
    {
        perror("waitpid failed");
        return -1;
    }
    // Clean up the popen table entry
    popen_table[fd].fp = NULL;
    popen_table[fd].pid = 0;
    return status;
}
