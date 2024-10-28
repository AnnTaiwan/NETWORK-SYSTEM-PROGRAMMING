/*
 * redirect_in.c  :  check for <
 */

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "shell.h"
#define STD_OUTPUT 1
#define STD_INPUT  0

/*
 * Look for "<" in myArgv, then redirect input to the file.
 * Returns 0 on success, sets errno and returns -1 on error.
 */
int redirect_in(char ** myArgv) {
  	int i = 0;
  	int fd;

  	/* search forward for <
  	 *
	 * Fill in code. */
	while(myArgv[i])// print all the tokens until it reaches the final terminate symbol NULL
	{
	    if(strcmp(myArgv[i], "<") == 0) // found <, and input file is at `i`
	    {
	        i++;
	        break;
	    }
	    i++;
	}
  	if (myArgv[i]) {	/* found "<" in vector. */

    	/* 1) Open file.
     	 * 2) Redirect stdin to use file for input.
   		 * 3) Cleanup / close unneeded file descriptors.
   		 * 4) Remove the "<" and the filename from myArgv.
		 *
   		 * Fill in code. */
   	      int openFlags; // allowed action toward this open file
   	      mode_t filePerms; // file permission when creating a new file, not needed here
   	      // set the open flag
   	      openFlags = O_RDONLY;
   	      filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; // rw-rw-rw-
   	      fd = open(myArgv[i], openFlags, filePerms); // open this file
   	      if (fd == -1)
   	      {
   	          perror("open");  // Print the error message based on `errno`
   	          return -1;
   	      }
   	      // Redirect stdin to the file
              if (dup2(fd, STDIN_FILENO) == -1) {
                  perror("dup2");
                  close(fd);
              }
              // Close the original file descriptor since stdin now refers to the file
              close(fd);
              
              i++; // let i point to the string after file
              while(myArgv[i]) // swap the string after < file forward two spaces
              {
                  char *temp = myArgv[i-2];
                  myArgv[i-2] = myArgv[i];
                  myArgv[i] = temp;
                  i++;
              }
              // free string space for < or file name
              free(myArgv[i-2]); 
              free(myArgv[i-1]);
              myArgv[i-2] = NULL; // replace NULL
  	}
  	return 0;
}
