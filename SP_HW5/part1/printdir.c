#include <sys/param.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
void errExit(char *msg)
{
    if(msg == NULL)
        perror("No error message");
    perror(msg);
    exit(EXIT_FAILURE);
}
int main()
{
    char *dir;
    long pathmaxlen = pathconf(".", _PC_PATH_MAX); // record the max path length
    
    dir = getcwd((char *)NULL, pathmaxlen + 1);
    
    if(dir == NULL)
        errExit("getcwd");
        
    printf("%s\n", dir);
    free(dir);
    return 0;
}
