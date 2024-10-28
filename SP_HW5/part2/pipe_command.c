/* 
 * pipe_command.c  :  deal with pipes
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "shell.h"

#define STD_OUTPUT 1
#define STD_INPUT  0

void pipe_and_exec(char **myArgv) {
  	int pipe_argv_index = pipe_present(myArgv);
  	int pipefds[2];
	char **left_argv;
	char **right_argv;

  	switch (pipe_argv_index) {

    	case -1:	/* Pipe at beginning or at end of argv;  See pipe_present(). */
      		fputs ("Missing command next to pipe in commandline.\n", stderr);
      		errno = EINVAL;	/* Note this is NOT shell exit. */
      		break;

    	case 0:	/* No pipe found in argv array or at end of argv array.
			See pipe_present().  Exec with whole given argv array. */
		// Exec with whole given argv array
		// already in child process when do this function
		execvp(myArgv[0], myArgv);
		perror("execvp");
      		return;
    	default:	/* Pipe in the middle of argv array.  See pipe_present(). */

      		/* Split arg vector into two where the pipe symbol was found.
       		 * Terminate first half of vector.
			 *
       		 * Fill in code. */
       		free(myArgv[pipe_argv_index]); // free "|" space
                myArgv[pipe_argv_index] = NULL; // Terminate first half of vector.
                left_argv = myArgv; // left vector
                right_argv = myArgv + pipe_argv_index + 1; // let right pointer point to beginning of right vector
      		/* Create a pipe to bridge the left and right halves of the vector. 
			 *
			 * Fill in code. */
                if(pipe(pipefds) == -1) // open a pipe
                {
                      perror("pipe");
      		      break;
      		}
      		/* Create a new process for the right side of the pipe.
       		 * (The left side is the running "parent".)
       		 *
			 * Fill in code to replace the underline. */
                pid_t pid;
                pid = fork();
      		switch(pid) {

        		case -1 :
	  		    break;

        		/* Talking parent.  Remember this is a child forked from shell. */
        		default :

	  				/* - Redirect output of "parent" through the pipe.
	  				 * - Don't need read side of pipe open.  Write side dup'ed to stdout.
	 	 			 * - Exec the left command.
					 *
					 * Fill in code. */
			    // Redirect output of "parent" through the pipe.
			    close(pipefds[0]); // close read end
			    dup2(pipefds[1], STDOUT_FILENO); // redirect stdout to pipefds[1] (writing end)
			    close(pipefds[1]); // no need anymore
			    // Exec the left command.
			    execvp(left_argv[0], left_argv); // argv must be NULL-terminated
			    // If execvp fails, the code below will be executed
			    perror("parent execvp failed");
	  		    break;

        		/* Listening child. */
        		case 0 :

	  				/* - Redirect input of "child" through pipe.
					  * - Don't need write side of pipe. Read side dup'ed to stdin.
				  	 * - Exec command on right side of pipe and recursively deal with other pipes
					 *
					 * Fill in code. */
			    // Redirect input of "child" through pipe.
			    close(pipefds[1]); // close write end
			    dup2(pipefds[0], STDIN_FILENO); // redirect stdin to pipefds[0] (reading end)
			    close(pipefds[0]); // no need anymore
			    // Exec the right command.
			    execvp(right_argv[0], right_argv); // argv must be NULL-terminated
			    // If execvp fails, the code below will be executed
			    perror("child execvp failed");
			    // recursively deal with other pipes
          		    pipe_and_exec(&myArgv[pipe_argv_index+1]);
			}
	}
	perror("Couldn't fork or exec child process");
  	exit(errno);
}
