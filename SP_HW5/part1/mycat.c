#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#ifndef BUF_SIZE 
#define BUF_SIZE 1024
#endif


void errExit(char *msg)
{
    if(msg == NULL)
        perror("No error message");
    perror(msg);
    exit(EXIT_FAILURE);
}
int main(int argc, char **argv)
{
    if(argc != 2)
    {
        fprintf(stderr, "Usage: mycat filename\n");
        exit(EXIT_FAILURE);
    }
    
    ssize_t numRead, numWrite; // number of read and write bytes 
    int openFlags; // fd and ouput file's mode
    mode_t filePerms; // new file's usage permission
    char buf[BUF_SIZE]; // store the data
    int inputfd;
    // set the open flag
    openFlags = O_RDONLY;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; // rw-rw-rw-
    // open the target file
    inputfd = open(argv[1], openFlags, filePerms);
    if (inputfd == -1)
       errExit("open file"); // If file cannot be opened, print error and exit
    // start to read the file and write it on the screen
    while((numRead = read(inputfd, buf, BUF_SIZE)) > 0)
    {
	ssize_t totalWritten = 0; // Variable to track the total number of bytes written
	// Write to standard output
	while (totalWritten < numRead) 
	{
	    // Attempt to write the remaining bytes from the buffer to the std_out
	    numWrite = write(STDOUT_FILENO, buf + totalWritten, numRead - totalWritten);
	    if (numWrite == -1) 
	    {
                close(inputfd);
		errExit("write"); // Handle write error
	    }
	    totalWritten += numWrite; // Update the total number of bytes written
	}
    }
    if (numRead == -1) // error
    {
	close(inputfd);
	errExit("read");
    }
	
    // Close opened files
    close(inputfd);
    return 0;
}
