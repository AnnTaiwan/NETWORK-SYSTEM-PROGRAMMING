#include <stdio.h>
#include <time.h>

int main() {
    time_t timer;
    struct tm *Now;
    char buffer[80];  // save teh output string

    // get local time
    time(&timer);
    Now = localtime(&timer);

    // Use strftime to customize the format
    strftime(buffer, sizeof(buffer), "%b %d(%a), %Y %I:%M PM", Now); // Oct 7(Tue), 2019 1:22 PM
    printf("%s\n", buffer);
    return 0;
}

