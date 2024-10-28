/*
 * builtin.c : check for shell built-in commands
 * structure of file is
 * 1. definition of builtin functions
 * 2. lookup-table
 * 3. definition of is_builtin and do_builtin
*/

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/utsname.h>
#include "shell.h"
#include <string.h>
#include <ctype.h>
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
static void bi_builtin(char ** argv);	/* "builtin" command tells whether a command is builtin or not. */
static void bi_cd(char **argv) ;		/* "cd" command. */
static void bi_echo(char **argv);		/* "echo" command.  Does not print final <CR> if "-n" encountered. */
static void bi_hostname(char ** argv);	/* "hostname" command. */
static void bi_id(char ** argv);		/* "id" command shows user and group of this process. */
static void bi_pwd(char ** argv);		/* "pwd" command. */
static void bi_quit(char **argv);		/* quit/exit/logout/bye command. */




/****************************************************************************/
/* lookup table                                                             */
/****************************************************************************/

static struct cmd {
  	char * keyword;					/* When this field is argv[0] ... */
  	void (* do_it)(char **);		/* ... this function is executed. */
} inbuilts[] = {
  	{ "builtin",    bi_builtin },   /* List of (argv[0], function) pairs. */

    /* Fill in code. */
    { "echo",       bi_echo },
    { "quit",       bi_quit },
    { "exit",       bi_quit },
    { "bye",        bi_quit },
    { "logout",     bi_quit },
    { "cd",         bi_cd },
    { "pwd",        bi_pwd },
    { "id",         bi_id },
    { "hostname",   bi_hostname },
    {  NULL,        NULL }          /* NULL terminated. */
};

static void bi_builtin(char ** argv) {
	/* Fill in code. */
    struct cmd *tableCommand;

    for (tableCommand = inbuilts ; tableCommand->keyword != NULL; tableCommand++)
    {
        if (strcmp(tableCommand->keyword, argv[1]) == 0) 
        {
            printf("%s is a builtin feature.\n", argv[1]);
            return;
        }
    }
    printf("%s is NOT a builtin feature.\n", argv[1]);
}

static void bi_cd(char **argv) {
	/* Fill in code. */
    if (argv[1] == NULL) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
    }

    // Try to change the directory
    if (chdir(argv[1]) != 0) {
        fprintf(stderr, "Error: fail to chdir");
    }
}

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

static void bi_hostname(char ** argv) {
	/* Fill in code. */
    struct utsname uts; // get the system identification
    if(uname(&uts) == -1)
        fprintf(stderr, "Error: uname");
    printf("hostname: %s\n", uts.nodename);
}

static void bi_id(char ** argv) {
 	/* Fill in code. */
    uid_t uid;          // user ID
    gid_t gid;          // group ID
    struct passwd *pw;    // pointer to passwd struct for user info
    struct group *gr;       // pointer to group struct for group info
    // Get user ID and group ID
    uid = getuid();
    gid = getgid();
    // Retrieve username using getpwuid
    pw = getpwuid(uid);
    if (pw == NULL)
        fprintf(stderr, "Error: getpwuid");
        
     // Retrieve group name using getgrgid
    gr = getgrgid(gid);
    if (gr == NULL)
        fprintf(stderr, "Error: getgrgid");
    printf("UserID =  %d(%s), GroupID = %d(%s)\n", uid, pw->pw_name, gid, gr->gr_name);
}

static void bi_pwd(char ** argv) {
	/* Fill in code. */
    char *dir;
    long pathmaxlen = pathconf(".", _PC_PATH_MAX); // record the max path length
    
    dir = getcwd((char *)NULL, pathmaxlen + 1);
    
    if(dir == NULL)
        fprintf(stderr, "Error: getcwd");
        
    printf("%s\n", dir);
    free(dir);
}

static void bi_quit(char **argv) {
    free_argv(argv); // free the allocated memory
    exit(0);
}


/****************************************************************************/
/* is_builtin and do_builtin                                                */
/****************************************************************************/

static struct cmd * this; /* close coupling between is_builtin & do_builtin */

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
	return 0; // i add to prevent warning
}
