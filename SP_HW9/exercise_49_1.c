/*
 Exerecise 49-1:
     Write a program, analogous to cp(1), that uses mmap() and memcpy() calls (instead
     of read() or write()) to copy a source file to a destination file. (Use fstat() to obtain
     the size of the input file, which can then be used to size the required memory
     mappings, and use ftruncate() to set the size of the output file.)
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

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
	
    int src_openFlags, dest_openFlags; // fd and ouput file's mode
    mode_t filePerms; // new file's usage permission
    int outputFD, inputFD;
    // setting open flag
    src_openFlags = O_RDONLY;
    dest_openFlags = O_CREAT | O_RDWR | O_TRUNC; 
			
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; // rw-rw-rw-
    
    // open src_file and dest_file
    inputFD = open(argv[1], src_openFlags);
    if (inputFD == -1)
       errExit("open src_file"); // If file cannot be opened, print error and exit
    
    outputFD = open(argv[2], dest_openFlags, filePerms);
    if (outputFD == -1)
    {
     	close(inputFD);
      	errExit("open dest_file"); // If file cannot be opened, print error and exit
    }
    
    int mem_size; // src file size
    struct stat sb;
    if (fstat(inputFD, &sb) == -1) 
    {
        close(inputFD);
        close(outputFD);
        errExit("fstat");
    }
    mem_size = sb.st_size; // file size (bytes)
    char *src_addr, *dest_addr; // point to virtual mapping memory 
    // mapping virtual memory to src file
    src_addr = mmap(NULL, mem_size, PROT_READ, MAP_PRIVATE, inputFD, 0);
    if(src_addr == MAP_FAILED)
        errExit("src mmap");
    close(inputFD); // no longer need fd
    
    // sizing output file
    if(ftruncate(outputFD, mem_size) == -1)
    {
        munmap(src_addr, mem_size);
        close(outputFD);
        errExit("ftruncate");
    }
    // mapping virtual memory to dest file
    dest_addr = mmap(NULL, mem_size, PROT_WRITE, MAP_SHARED, outputFD, 0);
    if(dest_addr == MAP_FAILED)
    {
        munmap(src_addr, mem_size);
        close(outputFD);
        errExit("dest mmap");
    }
    close(outputFD); // no longer need fd


    
    // Copy memory from source to destination
    memcpy(dest_addr, src_addr, mem_size);

    // Clean up
    munmap(src_addr, mem_size);
    munmap(dest_addr, mem_size);
    exit(EXIT_SUCCESS);
}
