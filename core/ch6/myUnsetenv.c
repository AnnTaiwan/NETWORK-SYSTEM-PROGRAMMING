/*
Exercises 6-3 - unsetenv()
Implement setenv() and unsetenv() using getenv(), putenv(), and, where necessary,
code that directly modifies environ. Your version of unsetenv() should check to see
whether there are multiple definitions of an environment variable, and remove
them all (which is what the glibc version of unsetenv() does).
*
*/
#include<stdio.h>
#include "tlpi_hdr.h"
extern char **environ;
int myunsetenv(const char *name)
{// overwrite = 1: do, 0: not do
    if(strlen(name) == 0 || name == NULL)
        return -1;
    for(int i = 0; i < strlen(name); i++)
    {
        if(name[i] == '=') // name can't include '='
            return -1;
    }
    if(!getenv(name)) 
    {
        // No this name exists in environ list
        return 0;
    }
    size_t len = strlen(name);
    char **ep = environ;

    // Use while loop so we can control the increment
    while (*ep != NULL) {
        // 2. Check for match with "name="
        if (strncmp(*ep, name, len) == 0 && (*ep)[len] == '=') {
            printf("Removing %s\n", *ep);
            
            // 3. Shift all following pointers forward
            char **sp = ep;
            while (*sp != NULL) {
                *sp = *(sp + 1);
                sp++;
            }
            // CRITICAL: Do not increment ep here! 
            // The next element has moved into the current 'ep' position.
        } else {
            ep++; // Only move to next if no deletion occurred
        }
    }
    return 0;
}
int main(int argc, char*argv[])
{
    /* 
     * The command line should be ./a name
    */
    if(argc != 2)
    {
        usageErr("%s name\n", argv[0]);
    }
    int ret = myunsetenv(argv[1]);
    if(ret == -1)
        errExit("myunsetenv");
    

    char **ep;
    for(ep = environ; *ep != NULL; ep++)
    {
        puts(*ep);
    }
    exit(EXIT_SUCCESS);
}
