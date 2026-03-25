#include<unistd.h>
#include<stdio.h>
int main()
{
    long pageSize = sysconf(_SC_PAGESIZE);
    int pageSize2 = getpagesize();
    printf("sysconf: %ld bytes.\n", pageSize);
    printf("getpagesize: %d bytes.\n", pageSize2);
    return 0;
}
