#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#ifndef BUF_SIZE 
#define BUF_SIZE 1024
#endif
void errExit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
void text_to_uppercase(char * str)
{   
    for(int i = 0; i < strlen(str); i++) // iterate whole text. and turn it into upper-case
    { 
        if(str[i] >= 97 && str[i] <= 122) // a~z
            str[i] -= 32; // become upper-case
    }
}
int main()
{
    int pd_1[2], pd_2[2]; // declare two pipes
    /* 
      pd_1: parent write, child read
      pd_2: parent read, child write
    */
    if(pipe(pd_1) == -1)
        errExit("pipe");
    if(pipe(pd_2) == -1)
        errExit("pipe");
        
    char buf[BUF_SIZE];
    ssize_t numRead, numWrite; // number of read and write bytes 
    int stat;
    pid_t pid;
    pid = fork();
    switch(pid)
    {
        case -1:
            errExit("fork");
            break;
        case 0: // child
            close(pd_1[1]);
            close(pd_2[0]);
            while((numRead = read(pd_1[0], buf, BUF_SIZE)) > 0) // read from the text that parent sends
            {
                buf[numRead] = '\0';  // Null-terminate the string
                text_to_uppercase(buf); // turn it into upper-case
                // write to pd_2, let the parent read
                numWrite = write(pd_2[1], buf, strlen(buf));
                if(numWrite != strlen(buf))
                    errExit("child write to pd_2");
            }      
            close(pd_1[0]);
            close(pd_2[1]);
            exit(EXIT_SUCCESS);  // Child exits after loop
        default: // parent
            break;
    }
    close(pd_1[0]);
    close(pd_2[1]);
    while (1) 
    {
        fputs("Please input text here, press Ctrl+D to quit:\n", stdout);
        fflush(stdout);  // Ensure prompt is displayed immediately

        if (fgets(buf, BUF_SIZE, stdin) == NULL) 
            break;  // Exit loop if no input (EOF)
        
        numWrite = write(pd_1[1], buf, strlen(buf));
        if(numWrite != strlen(buf))
            errExit("parent write to pd_1");
        numRead = read(pd_2[0], buf, numWrite); // read it back with upper-case text
        if (numRead <= 0)
            errExit("parent read from pd_2");
        buf[numRead] = '\0';  // Null-terminate the string
            
        fputs("\nUppercase text:\n", stdout);
        fputs(buf, stdout);  // Print the uppercase text
        fflush(stdout);      // Ensure output is displayed
        // continue with next loop
    }
    close(pd_1[1]); // let the child's read end receive EOF, so child can exit
    close(pd_2[0]);
    waitpid(pid, &stat, 0);
    return 0;
}
