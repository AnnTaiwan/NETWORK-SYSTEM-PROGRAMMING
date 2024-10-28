/*

 * is_background.c :  check for & at end
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "shell.h"

int is_background(char ** myArgv) {

  	if (*myArgv == NULL)
    	return 0;

  	/* Look for "&" in myArgv, and process it.

  	 *
	 *	- Return TRUE if found.
	 *	- Return FALSE if not found.
	 *
	 * Fill in code.
	 */
	int j = 0;
	while(myArgv[j])// find the last token's index, to see if it is "&"
	{
		j++;
	}
	
	// myArgv[j-1] is the last token
	if(strcmp(myArgv[j-1], "&") == 0) // match
	{
		// delete the lasst token which is '&'
		free(myArgv[j]);
		myArgv[j-1] = NULL; // set new NULL-terminated symbol
		return 1; // means it is background command
	}
	else
		return -1; // not background command
}
