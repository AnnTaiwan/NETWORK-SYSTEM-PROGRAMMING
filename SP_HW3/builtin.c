/*
 * builtin.c : check for shell built-in commands
 * structure of file is
 * 1. definition of builtin functions
 * 2. lookup-table
 * 3. definition of is_builtin and do_builtin
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include "shell.h"

int isPositiveNumber(const char *str) { // test if the string is number or not
    // If the string is empty, it's not a number
    if (str == NULL || *str == '\0') {
        return 0;
    }

    // Check for negative numbers
    if (*str == '-') {
        return -1;
    }

    // Loop through the string and check if every character is a digit
    while (*str) {
        if (!isdigit(*str)) {
            return 0;  // If any character is not a digit, return 0 (false)
        }
        str++;
    }
    return 1;  // If all characters are digits, return 1 (true)
}


/****************************************************************************/
/* builtin function definitions                                             */
/****************************************************************************/

/* "echo" command.  Does not print final <CR> if "-n" encountered. */
static void bi_echo(char **argv) {
  	/* Fill in code. */
  	int exist_n = 0; // if -n exist, exit_n = 1, otherwise, is 0
  	int number = -1; // specified number indicated by -n
  	if(strcmp(argv[1], "-n") == 0) // assume -n flag is the second argument
  	{
  		exist_n = 1;
  		int temp = isPositiveNumber(argv[2]);
  		if(temp == 1) // isPositiveNumber
	  		number = atoi(argv[2]); // assume the number comes after -n
  		// assume negitive number be a normal string when -n flag without prarmeter
  	}
  	else
  		exist_n = 0;
	
	if(exist_n)
	{
		if(number > 0)
		{
			printf("%s", argv[number+2]); // print the indicated string
		}
		else if(number == 0)
		{
			fprintf(stderr, "Error, Please input the number which is greater than 0, like 'echo -n 1 happy now'\n");
		}
		else // when no specified number, just print the whole line without final <CR>
		{
			int j = 2; // echo -n <string1> <string2>, so it starts from 2 
			while(argv[j])// the last string in argv is `NULL`, so it can stop printing
			{
				if(argv[j+1]) // if next string isn't NULL, print the blank space
					printf("%s ", argv[j]);
				else
					printf("%s", argv[j]);
				j++;
			}
			// don't print <CR>
		}
	}
	else // no -n
	{
		int j = 1; // echo  <string1> <string2>, so it starts from 1
		while(argv[j])// the last string in argv is `NULL`, so it can stop printing
		{
			if(argv[j+1]) // if next string isn't NULL, print the blank space
				printf("%s ", argv[j]);
			else
				printf("%s", argv[j]);
			j++;
		}
		printf("\n"); // print <CR>
	}
}
static void bi_quit(char **argv) {
  	/* Fill in code. */
  	free_argv(argv); // free the allocated memory
  	exit(EXIT_SUCCESS);
}
static void bi_exit(char **argv) {
  	/* Fill in code. */
  	free_argv(argv); // free the allocated memory
  	exit(EXIT_SUCCESS);
}
static void bi_logout(char **argv) {
  	/* Fill in code. */
  	free_argv(argv); // free the allocated memory
  	exit(EXIT_SUCCESS);
}
static void bi_bye(char **argv) {
  	/* Fill in code. */
  	free_argv(argv); // free the allocated memory
  	exit(EXIT_SUCCESS);
}
/* Fill in code. */




/****************************************************************************/
/* lookup table                                                             */
/****************************************************************************/

static struct cmd {
	char * keyword;				/* When this field is argv[0] ... */
	void (* do_it)(char **);	/* ... this function is executed. */
} inbuilts[] = {

	/* Fill in code. */

	{ "echo", bi_echo },		/* When "echo" is typed, bi_echo() executes.  */
	{ "quit", bi_quit },
	{ "exit", bi_exit },
	{ "logout", bi_logout },
	{ "bye", bi_bye },
	{ NULL, NULL }				/* NULL terminated. */
};




/****************************************************************************/
/* is_builtin and do_builtin                                                */
/****************************************************************************/

static struct cmd * this; 		/* close coupling between is_builtin & do_builtin */

/* Check to see if command is in the inbuilts table above.
Hold handle to it if it is. */
int is_builtin(char *cmd) {
  	struct cmd *tableCommand;

  	for (tableCommand = inbuilts ; tableCommand->keyword != NULL; tableCommand++)
    	if (strcmp(tableCommand->keyword,cmd) == 0) {
			this = tableCommand;
			return 1;
		}
  	return 0;
}


/* Execute the function corresponding to the builtin cmd found by is_builtin. */
int do_builtin(char **argv) {
  	this->do_it(argv);
  	return 0; // I add it because it is no-void function
}
