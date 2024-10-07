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
	fprintf(stderr, "Usage: %s [-n num] <filename>\n", progName);
	exit(EXIT_FAILURE);
}
// Error handling function: Prints error message and exits the program
static void errExit(const char *msg) {
    perror(msg);  // Print a system error message corresponding to the last error
    exit(EXIT_FAILURE);  // Exit the program with a failure status
}

int main(int argc, char *argv[])
{
	int opt = 0; // get the result returned from get_opt
	char *pstr = NULL; // get the string after flag
	int lines = -1; // number of lines need to print
	
	// Iterate over options and handle the `-a` flag, `-a` flag don't expect arguments
    while ((opt = getopt(argc, argv, ":n:")) != -1) // suppress the error msg by putting ':' at start of optstring, ":" after the flag means need argument
    {
		switch (opt) 
		{
			case 'n':
				pstr = optarg;
				lines = atoi(pstr); // get the integer of input number of lines 
				break;
			case ':': 
				usageError(argv[0], "Missing argument", optopt);
				break;
			case '?': 
				usageError(argv[0], "Unrecognized option", optopt);
				break;
			default: 
				fprintf(stderr, "Unexpected case in switch: %c\n", opt);
				exit(EXIT_FAILURE); // Terminate the program with failure status
		}
		/*
        pstr = optarg;
        printf("optopt: %d, optind: %d, opt: %d, optarg: %s, lines: %d\n", optopt, optind, opt, pstr, lines);
        printf("argv[optind]: %s\n", argv[optind]);
        */
        
    }
    if(lines == -1) // default is 10
    {
    	lines = 10;
    }
    if(argc <= optind) // error: no more files to see 
    {
    	usageError(argv[0], "Missing files", optopt);
    }
    
    
    ssize_t numRead, numWrite; // number of read and write bytes 
	int openFlags; // fd and ouput file's mode
	mode_t filePerms; // new file's usage permission
	char buf[BUF_SIZE]; // store the data
	int inputFD;
	
	// set the open flag
	openFlags = O_RDONLY;		
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; // rw-rw-rw-
    // Open all files in argv for reading
    int fileCount = argc - optind;
    for(int i = optind; i < argc; i++)
    {
    	if(fileCount > 1 && i != optind)
    		printf("\n");
    	if(fileCount > 1) // more than one file, has to print the filename
    		printf("==> %s <==\n", argv[i]);
    		
    	// open src_file
		inputFD = open(argv[i], openFlags, filePerms);
		if (inputFD == -1)
		   errExit("open src_file"); // If file cannot be opened, print error and exit
    	
    	 // Get the file size
		off_t fileSize = lseek(inputFD, 0, SEEK_END);
		if (fileSize == -1)
		{
		 	close(inputFD);
		    errExit("lseek file");
		}	
    	//printf("%ld\n", fileSize);
    	// First, Read calculate the "\n" number from the end of the file
    	int newlineCount = 0; // count the new-line symbol
		off_t currentPos = fileSize; // find the beginning position in the file where start to print out
		int i; // index
		// find the start position where needed to print
		while (currentPos > 0 && newlineCount <= lines) 
		{
		    // Calculate the size to read (either full buffer or remaining part of the file)
		    size_t readSize = (currentPos < BUF_SIZE) ? currentPos : BUF_SIZE;
		    currentPos -= readSize; // point to the head of this reading era
		    //printf("CC%ld\n", currentPos); 
		    if (lseek(inputFD, currentPos, SEEK_SET) == -1) // set the pointer to the current position
		    {
		        close(inputFD);
		        errExit("lseek file");
		    }   
		    // Read the chunk from the file
		    numRead = read(inputFD, buf, readSize);
		    if (numRead == -1)
		    {
		    	close(inputFD);
		        errExit("read file");
		    }

		    // Reverse loop to count newlines
		    for (i = numRead - 1; i >= 0; i--) 
		    {
		        if (buf[i] == '\n') 
		        {
		            newlineCount++;
		            if (newlineCount == lines + 1 && currentPos + i + 1 < fileSize) // have to count until needed newlines plus one
		            {
		                currentPos += i + 1;
		                break;
		            }
		        }
		    }
		}
			
    	// start to write into stdout from the currentPos, so adjust the position
    	if (lseek(inputFD, currentPos, SEEK_SET) == -1) 
        {
			close(inputFD);
			errExit("lseek");  // If seek fails, print error and exit
		}
		while((numRead = read(inputFD, buf, BUF_SIZE)) > 0)
		{
			ssize_t totalWritten = 0; // Variable to track the total number of bytes written
			// write data into dest_file
			while (totalWritten < numRead) 
			{
				// Attempt to write the remaining bytes from the buffer to the std_out
				numWrite = write(STDOUT_FILENO, buf + totalWritten, numRead - totalWritten);
				if (numWrite == -1) 
				{
					close(inputFD);
					errExit("write"); // Handle write error
				}
				totalWritten += numWrite; // Update the total number of bytes written
			}
		}
		if (numRead == -1) // error
		{
			close(inputFD);
			errExit("read");
		}
		close(inputFD); // close this current file
    }
    exit(EXIT_SUCCESS);
}
