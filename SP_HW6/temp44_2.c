#include "popen_pclose.h"
#include <errno.h>      // For error handling
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#define BUF_SIZE 1024

int main() {
    printf("Test on sort...\n\n");
    const char *command = "sort"; 
    FILE *stream;

    stream = my_popen(command, "w");
    if (stream == NULL) {
        perror("my_popen failed");
        exit(EXIT_FAILURE);
    }

    fprintf(stream, "orange\n");
    fprintf(stream, "banana\n");
    fprintf(stream, "apple\n");
    fprintf(stream, "grape\n");


    // Read resulting output until EOF
    char pathname[PATH_MAX];
    while (fgets(pathname, PATH_MAX, stream) != NULL) {
            printf("%s", pathname);
        }
        
    if (my_pclose(stream) == -1) {
        perror("my_pclose failed");
        exit(EXIT_FAILURE);
    }

    printf("Test on tr...\n\n");
    command = "tr a-z A-Z"; 

    stream = my_popen(command, "w");
    if (stream == NULL) {
        perror("my_popen failed");
        exit(EXIT_FAILURE);
    }

    fprintf(stream, "orange\n");
    fprintf(stream, "banana\n");
    fprintf(stream, "apple\n");
    fprintf(stream, "grape\n");


    // Read resulting output until EOF
    while (fgets(pathname, PATH_MAX, stream) != NULL) {
            printf("%s", pathname);
        }
        
    if (my_pclose(stream) == -1) {
        perror("my_pclose failed");
        exit(EXIT_FAILURE);
    }
    
    //printf("Data has been sent to the 'sort' command.\n");
    return 0;
}

