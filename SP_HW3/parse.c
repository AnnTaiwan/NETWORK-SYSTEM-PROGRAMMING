/*
 * parse.c : use whitespace to tokenise a line
 * Initialise a vector big enough
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "shell.h"

/* Parse a commandline string into an argv array. */
char ** parse(char *line) {

  	static char delim[] = " \t\n"; /* SPACE or TAB or NL */
  	int count = 0;
  	char * token;
  	char **newArgv;

  	/* Nothing entered. */
  	if (line == NULL) {
    	return NULL;
  	}

  	/* Init strtok with commandline, then get first token.
     * Return NULL if no tokens in line.
	 *
	 * Fill in code.
     */
	token = strtok(line, delim);
	if(token == NULL) // no tokens
	{
		return NULL;
	}
	count++;
  	/* Create array with room for first token.
  	 *
	 * Fill in code.
	 */
	newArgv = (char **)malloc(count * sizeof(char*)); // create one string space
	newArgv[count - 1] = (char *)malloc((strlen(token) + 1) * sizeof(char)); // create a string length space for first token.
	strcpy(newArgv[count - 1], token); // copy token into newArgv
  	/* While there are more tokens...
	 *
	 *  - Get next token.
	 *	- Resize array.
	 *  - Give token its own memory, then install it.
	 * 
  	 * Fill in code.
	 */
	while(token) // keep parsing the line until return NULL
	{
		token = strtok((char *)NULL, delim); // Get next token.
		if(token != NULL)
		{
			count++;
			if ((newArgv = (char **)realloc(newArgv, count * sizeof(char*))) != NULL ) // Resize array
			{
				newArgv[count - 1] = (char *)malloc((strlen(token) + 1) * sizeof(char)); // Give token its own memory
				strcpy(newArgv[count - 1], token); // save token into array
			}
			else 
			{ /* realloc error */
				perror( "Out of storage" );
				exit(EXIT_FAILURE);
			}
		}
	}	

  	/* Null terminate the array and return it.
	 *
  	 * Fill in code.
	 */
	if ((newArgv = (char **)realloc(newArgv, (count+1) * sizeof(char*))) != NULL ) // Resize array
	{
		newArgv[count] = (char*)NULL; // Null terminate the array
		int j = 0;
		while(newArgv[j])// print all the tokens until it reaches the final terminate symbol NULL
		{
			printf("[%d] : %s\n", j, newArgv[j]);
			j++;
		}
	}
	else 
	{ /* realloc error */
		perror( "Out of storage" );
		exit(EXIT_FAILURE);
	}
  	return newArgv;
}


/*
 * Free memory associated with argv array passed in.
 * Argv array is assumed created with parse() above.
 */
void free_argv(char **oldArgv) {

	int i = 0;

	/* Free each string hanging off the array.
	 * Free the oldArgv array itself.
	 *
	 * Fill in code.
	 */
	while(oldArgv[i])// free each char* which points to a string until it reaches the final terminate symbol NULL
	{
		free(oldArgv[i]);
		i++;
	}
	free(oldArgv[i]); // free the last pointer which is NULL
	free(oldArgv); // free whole array(char **)
}
