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


void errExit(char *msg)
{
    if(msg == NULL)
        perror("No error message");
    perror(msg);
    exit(EXIT_FAILURE);
}
int main(int argc, char **argv)
{
    int pd[2];
    char buf[BUF_SIZE];
    int stat;
    if(pipe(pd) == -1) // open a pipe
        errExit("pipe");
    
    pid_t pid = fork(); // then, fork
    switch(pid)
    {
        case -1: // error
            errExit("fork");
            break;
        case 0: // child, do the write ls info into pipe
            close(pd[0]); // close read end
            /* try to redirect pd[1] to STDOUT_FILENO */
            close(STDOUT_FILENO); // in child, don't need to print in the console
            int new_fd = dup(pd[1]); // redirect STD_OUT(1) to a new fd
            if(new_fd == -1)
               errExit("dup");
            if(new_fd != STDOUT_FILENO) // to see if new_fd == 1
                errExit("dup failed to return 1(STDOUT_FILENO)!");
            close(pd[1]); // don't need anymore, already has new_fd
            execlp("ls", "ls", "-l", (char*)NULL); // argv must be NULL-terminated
            errExit("execlp"); // If execlp fails, print error and exit
            break;
        default: // parent
            close(pd[1]); // close write end
            ssize_t numRead, numWrite; // number of read and write bytes 
            while((numRead = read(pd[0], buf, BUF_SIZE)) > 0) // read from pd[0](read end of pipe)
            {
	        ssize_t totalWritten = 0; // Variable to track the total number of bytes written
	        // Write to standard output
	        while (totalWritten < numRead) 
	        {
	            // Attempt to write the remaining bytes from the buffer to the std_out
	            numWrite = write(STDOUT_FILENO, buf + totalWritten, numRead - totalWritten);
	            if (numWrite == -1) 
	            {
                        close(pd[0]);
		        errExit("write"); // Handle write error
	            }
	            totalWritten += numWrite; // Update the total number of bytes written
	        }
            }
            if (numRead == -1) // error
            {
	       close(pd[0]);
	       errExit("read");
            }
            close(pd[0]);
            if (waitpid(pid, &stat, 0) == -1) // `0` indicates the function should block until the child terminates.
		errExit("waitpid");
    }
    return 0;
}
