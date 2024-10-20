#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>  // For getpwnam()
#include <stdlib.h> // For strtol()
#include <dirent.h> // For opendir and readdir
#include <string.h>

static void usageError(char *progName, char *msg) /* Print "usage" message and exit */
{
    if (msg != NULL)
      fprintf(stderr, "%s\n", msg);
    fprintf(stderr, "Usage: %s  <user_name1>\n", progName);
    exit(EXIT_FAILURE);
}
// Error handling function: Prints error message and exits the program
static void errExit(const char *msg) {
    perror(msg);  // Print a system error message corresponding to the last error
    exit(EXIT_FAILURE);  // Exit the program with a failure status
} 
uid_t userIdFromName(const char *name)/* Return UID corresponding to 'name', or -1 on error */
{
    struct passwd *pwd;
    uid_t u;
    char *endptr;

    // Check if the input string is NULL or empty
    if (name == NULL || *name == '\0') 
        return -1;  // Return an error
    
    // Try to convert the string to a numeric value (UID)
    u = strtol(name, &endptr, 10);  // As a convenience to caller
    // If the conversion succeeded (i.e., the whole string is numeric), return the UID
    if (*endptr == '\0') 
        return u;
      
    // Otherwise, treat the string as a username and try to look up the user
    pwd = getpwnam(name);
    if (pwd == NULL)
        return -1;  // Return an error if the user is not found
    
    // Return the UID from the passwd structure
    return pwd->pw_uid;
}
void listUserProcesses(uid_t uid) {
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
            char path[270], line[256];
            // prepare this process's status file path
            snprintf(path, sizeof(path), "/proc/%s/status", entry->d_name);
            
            FILE *status_file = fopen(path, "r");
            if (status_file) // reading this status file
            {
                uid_t process_uid = -1;
                char process_name[256] = {0};
                while (fgets(line, sizeof(line), status_file)) 
                {
                    if (strncmp(line, "Uid:", 4) == 0) // find uid to compare with input uid
                    {
                        sscanf(line, "Uid:\t%u", &process_uid);
                    }
                    if (strncmp(line, "Name:", 5) == 0) // found the command name
                    {
                        sscanf(line, "Name:\t%s", process_name);
                    }
                    if (process_uid != -1 && strlen(process_name) > 0)
                    {
                        break;  // We have what we need
                    }
                }
                fclose(status_file);
                // Check if the process UID matches the user UID
                if (process_uid == uid) 
                {
                    printf("PID: %s, Command: %s\n", entry->d_name, process_name);
                }
            }
            // if status_file == NULL, this file may already disappear, so don't access this file
        }
    }
    closedir(proc_dir);
}
int main(int argc, char **argv)
{
    if(argc < 2)
    {
      usageError(argv[0], "Missing user name.");
    }
    uid_t u;
    u = userIdFromName(argv[1]);  // get the userId by userName
    if (u == (uid_t) -1) {
        fprintf(stderr, "Error: User '%s' not found.\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    // List processes for the given user
    listUserProcesses(u);
    return 0;
}
