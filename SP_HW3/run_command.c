/*
 * run_command.c :    do the fork, exec stuff, call other functions
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

#include <unistd.h> // for fork and execvp

#include "shell.h"

void run_command(char **myArgv) {
    pid_t pid;
    int stat;

	/* meaning of number which `is_background` return
		0: no arguments
		1: is background
		-1: not background
	*/
	int background = is_background(myArgv); // see if existing '&' and deal with the '&' symbol
	if(background == 0) // in theory, this won't happen because NULL command will be stopped in shell.c
	{
		fprintf(stderr, "Error, There are no commands. Please input commands.\n");
		return;
	}
    /* Create a new child process.
     * Fill in code.
	 */
	pid = fork(); 
    switch (pid) {

        /* Error. */
        case -1 :
            perror("fork");
            exit(errno);

        /* Parent. */
        default :
            /* Wait for child to terminate.
             * Fill in code.
			 */
			if(background != 1)
			{
				if (waitpid(pid, &stat, 0) == -1) // `0` indicates the function should block until the child terminates.
				{
					perror("waitpid");
					return;
				}

		        /* Optional: display exit status.  (See wstat(5).)
		         * Fill in code.
				 */
				if(WIFSIGNALED(stat))
				{
					printf("%s terminated by signal %d.\n", myArgv[0], WTERMSIG(stat)); 
				}
				else if(WIFEXITED(stat))
				{
					printf("%s exited with status %d.\n", myArgv[0], WEXITSTATUS(stat)); 
				}
				else if(WIFSTOPPED(stat))
				{
					printf("%s stopped by signal %d.\n", myArgv[0], WSTOPSIG(stat)); 
				}
			}
            return;

        /* Child. */
        case 0 :
            /* Run command in child process.
             * Fill in code.
			 */
			// replace process itself with a new process to execute the command
			execvp(myArgv[0], myArgv); // argv must be NULL-terminated
			// If execvp fails, the code below will be executed
			perror("execvp failed");
            /* Handle error return from exec */
			exit(errno);
    }
}
