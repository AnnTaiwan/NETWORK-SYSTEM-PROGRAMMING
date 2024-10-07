#include <sys/stat.h>  // For file permissions constants (S_IRUSR, S_IWUSR, etc.)
#include <fcntl.h>     // For file control operations (open, O_RDWR, O_CREAT, etc.)
#include <ctype.h>     // For character handling functions (isprint)
#include <stdio.h>     // For standard input/output functions (printf, perror)
#include <stdlib.h>    // For memory allocation and process control (malloc, exit)
#include <string.h>    // For string handling functions (strlen, strcmp)
#include <unistd.h>    // For POSIX API functions (read, write, lseek)
#include <errno.h>     // For error numbers and error handling
#include <stdarg.h>    // For handling variable argument lists in functions

// Error handling function: Prints error message and exits the program
void errExit(const char *msg) {
    perror(msg);  // Print a system error message corresponding to the last error
    exit(EXIT_FAILURE);  // Exit the program with a failure status
}

// Function to print usage error message and exit the program (handles variable arguments)
void usageErr(const char *format, ...) {
    va_list argList;
    va_start(argList, format);  // Initialize the argument list
    vfprintf(stderr, format, argList);  // Print formatted error message to stderr
    va_end(argList);  // Clean up the argument list
    exit(EXIT_FAILURE);  // Exit the program with a failure status
}

// Function to print command-line error and exit the program (handles variable arguments)
void cmdLineErr(const char *format, ...) {
    va_list argList;
    va_start(argList, format);  // Initialize the argument list
    vfprintf(stderr, format, argList);  // Print formatted error message to stderr
    va_end(argList);  // Clean up the argument list
    exit(EXIT_FAILURE);  // Exit the program with a failure status
}

// Converts a string to a long integer with error checking
long getLong(const char *arg, int flags, const char *name) {
    char *endptr;
    long val = strtol(arg, &endptr, flags);  // Convert string to long integer
    if (*endptr != '\0') {  // If the string contains invalid characters
        fprintf(stderr, "Invalid integer: %s\n", name);  // Print error message
        exit(EXIT_FAILURE);  // Exit the program
    }
    return val;  // Return the parsed long integer value
}

int main(int argc, char *argv[]) {
    size_t len;  // Length of data to read or write
    off_t offset;  // File offset for seeking
    int fd, ap, j;  // File descriptor, argument pointer, and loop variable
    char *buf;  // Buffer for reading or writing data
    ssize_t numRead, numWritten;  // Number of bytes read or written

    // Check if the user provided enough arguments, or if they asked for help
    if (argc < 3 || strcmp(argv[1], "--help") == 0)
        usageErr("%s file {r<length>|R<length>|w<string>|s<offset>}...\n", argv[0]);

    // Open the specified file for reading and writing, or create it if it doesn't exist
    fd = open(argv[1], O_RDWR | O_CREAT,
              S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
              S_IROTH | S_IWOTH);  // File permissions: rw-rw-rw-
    if (fd == -1)
        errExit("open");  // If file cannot be opened, print error and exit

    // Loop through command-line arguments starting from the second one
    for (ap = 2; ap < argc; ap++) {
        switch (argv[ap][0]) {
        case 'r':  // Read bytes at the current offset, display as text
        case 'R':  // Read bytes at the current offset, display in hex
            len = getLong(&argv[ap][1], 10, argv[ap]);  // Get the length to read from argument
            buf = malloc(len);  // Allocate buffer for reading data
            if (buf == NULL)
                errExit("malloc");  // If allocation fails, print error and exit
            numRead = read(fd, buf, len);  // Read data from file into buffer
            if (numRead == -1)
                errExit("read");  // If read fails, print error and exit

            // Check if end-of-file is reached
            if (numRead == 0) {
                printf("%s: end-of-file\n", argv[ap]);
            } else {
                printf("%s: ", argv[ap]);
                for (j = 0; j < numRead; j++) {
                    if (argv[ap][0] == 'r')  // If 'r', print as text (printable characters)
                        printf("%c", isprint((unsigned char)buf[j]) ? buf[j] : '?');
                    else  // If 'R', print as hex
                        printf("%02x ", (unsigned int)buf[j]);
                }
                printf("\n");
            }
            free(buf);  // Free the allocated memory after reading
            break;

        case 'w':  // Write string at the current offset
            numWritten = write(fd, &argv[ap][1], strlen(&argv[ap][1]));  // Write string from argument
            if (numWritten == -1)
                errExit("write");  // If write fails, print error and exit
            printf("%s: wrote %ld bytes\n", argv[ap], (long)numWritten);
            break;

        case 's':  // Change the file offset (seek)
            offset = getLong(&argv[ap][1], 10, argv[ap]);  // Get the offset value from argument
            if (lseek(fd, offset, SEEK_SET) == -1)
                errExit("lseek");  // If seek fails, print error and exit
            printf("%s: seek succeeded\n", argv[ap]);
            break;

        default:
            // Handle invalid command-line argument
            cmdLineErr("Argument must start with [rRws]: %s\n", argv[ap]);
        }
    }

    close(fd);  // Close the file after all operations are done
    exit(EXIT_SUCCESS);  // Exit the program successfully
}

