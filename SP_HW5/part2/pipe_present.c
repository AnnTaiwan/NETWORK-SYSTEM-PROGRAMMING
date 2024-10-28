/*
 *  pipe_present.c :  check for |
 */

#include <stdio.h>
#include "shell.h"
#include <string.h>
/*
 * Return index offset into argv of where "|" is,
 * -1 if in an illegal position (first or last index in the array),
 * or 0 if not present.
 */
int pipe_present(char ** myCurrentArgv) {
	int index = 0;

  	/* Search through myCurrentArgv for a match on "|". */
        while(myCurrentArgv[index])// print all the tokens until it reaches the final terminate symbol NULL
	{
	    if(strcmp(myCurrentArgv[index], "|") == 0) // found |, and input file is at `index`
	        break;
	    index++;
	}
  	if(index == 0 || (myCurrentArgv[index] != NULL && myCurrentArgv[index+1] == NULL)) /* At the beginning or at the end. */ 
  	{
    	    return -1;
  	} 
  	else if(myCurrentArgv[index] == NULL) /* Off the end. */ 
  	{
    	    return 0;
  	} 
  	else /* In the middle. */ 
  	{
    	    return index;
  	}
}
