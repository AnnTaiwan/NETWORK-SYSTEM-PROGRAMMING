#include <stdio.h>
#include <sys/types.h>
#include <dirent.h> // For opendir and readdir
#include <string.h>
#include <unistd.h>  // For readlink
#include <limits.h>  // For PATH_MAX
#include <errno.h>
#include <stdlib.h>
// Error handling function: Prints error message and exits the program
static void errExit(const char *msg) {
    perror(msg);  // Print a system error message corresponding to the last error
    exit(EXIT_FAILURE);  // Exit the program with a failure status
} 
void listAllfd(const char *searchPath) 
{
    printf("Process ID\tFile Descriptor    file pathname\n");
    printf("----------\t---------------    -----------------------------------\n");
    DIR *proc_dir; // needed for opendir, use this directory stream handle pointer to let the readdir can access this dir
    struct dirent *entry; // needed for readdir, which will return a pointer pointing to d_type, d_name,...

    // Open /proc directory
    proc_dir = opendir("/proc");
    if (proc_dir == NULL) {
        errExit("opendir /proc");
    }

    // Read each entry in /proc
    while ((entry = readdir(proc_dir)) != NULL) 
    {
        // Each entry in /proc corresponding to a PID is numeric
        if (entry->d_type == DT_DIR && atoi(entry->d_name) > 0) 
        { // if entry->d_name isn't a numeric string, atoi will return 0
            char fd_path[270];
            // prepare this process's status file path
            snprintf(fd_path, sizeof(fd_path), "/proc/%s/fd", entry->d_name);
            
            DIR *proc_dir_fd; // needed for opendir, use this directory stream handle pointer to let the readdir can access this dir
            // Open /proc/PID/fd directory
            proc_dir_fd = opendir(fd_path);
            if (proc_dir_fd == NULL) 
            {
                perror("opendir");
                fprintf(stderr, "\tDetail : Error: opendir /proc/%d/fd\n", atoi(entry->d_name));
                continue;
            }
            struct dirent *entry_fd; // needed for readdir, which will return a pointer pointing to d_type, d_name,...
            while ((entry_fd = readdir(proc_dir_fd)) != NULL) 
            {
                // Each entry in /proc corresponding to a PID is numeric
                if (entry_fd->d_type == DT_LNK) 
                { 
                    char buffer[PATH_MAX];
                    char link_path[PATH_MAX]; // Full path to the symbolic link
                    
                    snprintf(link_path, sizeof(link_path), "%s/%s", fd_path, entry_fd->d_name); // get the link path
                    
                    ssize_t path_size = readlink(link_path, buffer, PATH_MAX); // get the path
                    if (path_size >= 0) 
                    {
                        // concatenate a NULL-terminatesd symbol in the end of buffer because buffer won't have this in the end
                        buffer[path_size] = '\0'; // Null-terminate the buffer
                        if (strcmp(buffer, searchPath) == 0) 
                        {
                            printf("%d\t\t%d\t\t\t%s\n", atoi(entry->d_name), atoi(entry_fd->d_name), buffer);
                        }
                    } 
                    else 
                    {
                        perror("readlink");
                    }
                }
            }
            if (entry_fd == NULL && errno != 0) 
            {
                perror("readdir on reading fd");
            }
            closedir(proc_dir_fd);
            
        }
    }
    if (entry == NULL && errno != 0) 
    {
        perror("readdir on reading /proc");
    }
    closedir(proc_dir);
}
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // List all open file fds that match the provided file path
    listAllfd(argv[1]);
    return EXIT_SUCCESS;
}
