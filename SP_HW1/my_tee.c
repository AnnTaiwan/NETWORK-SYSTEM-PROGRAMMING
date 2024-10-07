#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> // for getopt()
#include <string.h>
#include <ctype.h>

#ifndef BUF_SIZE 
#define BUF_SIZE 1024
#endif

#define printable(ch) (isprint((unsigned char) ch) ? ch : '#')

// the global variable `optind` is updated to contain the index of the next unprocessed element of argv
extern int optind, opterr, optopt; 
extern char *optarg;// get the string after flag

static void usageError(char *progName, char *msg, int opt) /* Print "usage" message and exit */
{
	if (msg != NULL && opt != 0)
		fprintf(stderr, "%s (-%c)\n", msg, printable(opt));
	fprintf(stderr, "Usage: %s [-a] <filename>\n", progName);
	exit(EXIT_FAILURE);
}
static void errExit(char *msg)
{
	if (msg != NULL)
		fprintf(stderr, "Error: %s\n", msg);
	exit(EXIT_FAILURE);
}

static void openErrExit(const char *filename)
{
    if (filename != NULL)
        fprintf(stderr, "Error: Fail to open file %s\n", filename);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	int opt = 0; // get the result returned from get_opt
	//char *pstr = NULL; // get the string after flag
	int append = 0; // 0 : don't append, 1 : append
	
	// Iterate over options and handle the `-a` flag, `-a` flag don't expect arguments
    while ((opt = getopt(argc, argv, ":a")) != -1) // suppress the error msg by putting ':' at start of optstring
    {
		switch (opt) 
		{
			case 'a': 
				append = 1; // Set append to 1 when the -a flag is present 
				break;
			case ':': 
				usageError(argv[0], "Missing argument", optopt);
				break;
			case '?': 
				usageError(argv[0], "Unrecognized option", optopt);
				break;
			default: 
				// Replacing `fatal()` with `fprintf()` and `exit()`
				fprintf(stderr, "Unexpected case in switch: %c\n", opt);
				exit(EXIT_FAILURE); // Terminate the program with failure status
		}
		/*
        pstr = optarg;
        printf("optind: %d, opt: %d, optarg: %s\n", optind, opt, pstr);
        printf("argv[optind]: %s\n", argv[optind]);
        */
    }
	if(optind >= argc) // no more files, error
	{
		fprintf(stderr, "Usage: %s [-a] <filename>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
    // Check the value of `append`
    /*
    if (append == 1) {
        printf("-a flag was detected, append mode is ON.\n");
    } else {
        printf("-a flag was NOT detected, append mode is OFF.\n");
    }
    */
	
	ssize_t numRead, numWrite; // number of read and write bytes 
	int openFlags; // fd and ouput file's mode
	mode_t filePerms; // new file's usage permission
	char buf[BUF_SIZE]; // store the data
	
	// set the open flag
	if(append)
		openFlags = O_CREAT | O_WRONLY | O_APPEND;
	else
		openFlags = O_CREAT | O_WRONLY | O_TRUNC;
			
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; // rw-rw-rw-
	
	// Open all files in argv for writing
    int fileCount = argc - optind;
    int outputFds[fileCount]; // record all the outputFds
    for (int i = 0; i < fileCount; i++) {
        outputFds[i] = open(argv[optind + i], openFlags, filePerms);
        if (outputFds[i] == -1) {
            // when fail to open one file, close the previous opened-files
            for (int j = 0; j < i; j++) 
            {
                close(outputFds[j]);
            }
            openErrExit(argv[optind + i]);
        }
    }
    
    while((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0)
	{
		ssize_t totalWritten = 0; // Variable to track the total number of bytes written
		// Write to standard output
		while (totalWritten < numRead) 
		{
			// Attempt to write the remaining bytes from the buffer to the output file descriptor
			numWrite = write(STDOUT_FILENO, buf + totalWritten, numRead - totalWritten);
			if (numWrite == -1) 
			{
				for (int j = 0; j < fileCount; j++) 
				{
                	close(outputFds[j]);
                }
				errExit("write"); // Handle write error
			}
				
			totalWritten += numWrite; // Update the total number of bytes written
		}
			
        // Write to all opened files
        for (int i = 0; i < fileCount; i++) 
        {
			totalWritten = 0; // Variable to track the total number of bytes written

			while (totalWritten < numRead) 
			{
				// Attempt to write the remaining bytes from the buffer to the output file descriptor
				numWrite = write(outputFds[i], buf + totalWritten, numRead - totalWritten);
				if (numWrite == -1) {
					for (int j = 0; j < fileCount; j++) 
					{
		            	close(outputFds[j]);
		            }
					errExit("write"); // Handle write error
				}
					
				totalWritten += numWrite; // Update the total number of bytes written
			}
        }
	}
	if (numRead == -1) // error
	{
	    for (int j = 0; j < fileCount; j++) 
		{
           	close(outputFds[j]);
       	}
	    errExit("read");
	}
	
	// Close all opened files
    for (int i = 0; i < fileCount; i++) {
        if (close(outputFds[i]) == -1) {
            errExit("close");
        }
    }

    exit(EXIT_SUCCESS);
}
