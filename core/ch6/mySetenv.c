/*
Exercises 6-3 - setenv()
Implement setenv() and unsetenv() using getenv(), putenv(), and, where necessary,
code that directly modifies environ. Your version of unsetenv() should check to see
whether there are multiple definitions of an environment variable, and remove
them all (which is what the glibc version of unsetenv() does).
*
*/
#include<stdio.h>
#include "tlpi_hdr.h"
extern char **environ;
int mysetenv(const char *name, const char *value, int overwrite)
{// overwrite = 1: do, 0: not do
    if(overwrite == 0)
    {
        char *envStr = getenv(name);
        if(envStr != NULL) // the name already exists, and don't overwrite
            return 0;
    }
    // prepare the name=value string
    // In order to give this ptr to putenv to put in environ list, it shouldn't be static
    // so, make it stay in heap and don't free
    int len = strlen(name) + strlen(value) + 2;
    char *obj = (char*)malloc(len * sizeof(char));
    if(!obj)
        return -1;
    snprintf(obj, len, "%s=%s", name, value);
    int ret = putenv(obj);
    // modify the environ string
    if(ret != 0) // error
    {
        free(obj);
        return -1;
    }

    return 0;
}
int main(int argc, char*argv[])
{
    /* 
     * The command line should be ./a name value integer(0/nonzero)
    */
    if(argc != 4)
    {
        usageErr("%s name value integer(0/non-zero)\n", argv[0]);
    }
    int ret = mysetenv(argv[1], argv[2], atoi(argv[3]));
    if(ret == -1)
        errExit("mysetenv");
    

    char **ep;
    for(ep = environ; *ep != NULL; ep++)
    {
        puts(*ep);
    }
    exit(EXIT_SUCCESS);
}
