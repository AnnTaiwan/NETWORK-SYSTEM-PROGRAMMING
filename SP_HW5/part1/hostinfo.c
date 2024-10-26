#include <sys/utsname.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
void errExit(char *msg)
{
    if(msg == NULL)
        perror("No error message");
    perror(msg);
    exit(EXIT_FAILURE);
}


int main()
{
    struct utsname uts; // get the system identification
    int hostid; // return value from gethostid
    if(uname(&uts) == -1)
        errExit("uname");
    
    if((hostid = gethostid()) == -1)
        errExit("gethostid");
    
    printf("hostname: %s\n", uts.nodename);
    printf("%s\n", uts.release);
    printf("hostid: %d\n", hostid);

    return 0;
}
