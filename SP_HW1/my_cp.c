#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> // for getopt()
#include <string.h>
#include <ctype.h>
#include <errno.h>

#ifndef BUF_SIZE 
#define BUF_SIZE 1024
#endif

#ifndef SEEK_DATA
    #define SEEK_DATA 3   // if don't support SEEK_DATA, set it to 3 by linux standard
#endif



static void cmdUsageError(char *progName) /* Print "usage" message and exit */
{
	if (progName != NULL)
		fprintf(stderr, "Command Usage Error: %s <source_file> <dest_file>\n", progName);
	exit(EXIT_FAILURE);
}
// Error handling function: Prints error message and exits the program
static void errExit(const char *msg) {
    perror(msg);  // Print a system error message corresponding to the last error
    exit(EXIT_FAILURE);  // Exit the program with a failure status
}

int main(int argc, char *argv[])
{
	// Check if the user provided enough arguments
	if(argc < 3)
	    cmdUsageError(argv[0]);
	
	ssize_t numRead, numWrite; // number of read and write bytes 
	int src_openFlags, dest_openFlags; // fd and ouput file's mode
	mode_t filePerms; // new file's usage permission
	char buf[BUF_SIZE]; // store the data
	int outputFD, inputFD;
	// setting open flag
	src_openFlags = O_RDONLY;
	dest_openFlags = O_CREAT | O_WRONLY | O_TRUNC; 
			
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; // rw-rw-rw-
    
    // open src_file and dest_file
	inputFD = open(argv[1], src_openFlags, filePerms);
    if (inputFD == -1)
       errExit("open src_file"); // If file cannot be opened, print error and exit
    
    outputFD = open(argv[2], dest_openFlags, filePerms);
	if (outputFD == -1)
	{
     	close(inputFD);
      	errExit("open dest_file"); // If file cannot be opened, print error and exit
    }
    
	//off_t offset = 0;
    off_t data_offset = 0;//, hole_offset = 0
	while((data_offset = lseek(inputFD, data_offset, SEEK_DATA)) != -1) // data_offset is absolute which starts from the beginning of file
	{ // keep read ths src_file until no more data era, and then return -1
		if (lseek(outputFD, data_offset, SEEK_SET) == -1) // adjust the offset in the dest_file, let the dest_file has the same holes
        {
			close(inputFD);
			close(outputFD);
			errExit("lseek");  // If seek fails, print error and exit
		}	
       // printf("Next data starts at offset: %lld\n", (long long) data_offset);
        // read data from src_file
		while((numRead = read(inputFD, buf, BUF_SIZE)) > 0)
		{
			ssize_t totalWritten = 0; // Variable to track the total number of bytes written
			// write data into dest_file
			while (totalWritten < numRead) 
			{
				// Attempt to write the remaining bytes from the buffer to the output file descriptor
				numWrite = write(outputFD, buf + totalWritten, numRead - totalWritten);
				if (numWrite == -1) 
				{
					close(inputFD);
					close(outputFD);
					errExit("write"); // Handle write error
				}
				totalWritten += numWrite; // Update the total number of bytes written
			}
		}
		if (numRead == -1) // error
		{
			close(inputFD);
			close(outputFD);
			errExit("read");
		}

        data_offset += 1; // plus one byte  in order to search for next data era
        
	}
	
    if (data_offset == -1) 
    {
       if (errno == ENXIO) 
       {
       		//printf("No more data, reached end of file.\n");
       } 
       else 
       {
       		close(inputFD);
			close(outputFD);
            errExit("lseek");
       }
   	}
        
    close(inputFD);
	close(outputFD);
    exit(EXIT_SUCCESS);
}
