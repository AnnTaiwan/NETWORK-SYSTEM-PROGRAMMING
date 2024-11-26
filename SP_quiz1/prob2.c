#include <errno.h>      // For error handling
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
int status = 0; // used to decide if there exists an anomaly situation for  an anomaly detection

void handler(int signo) // when ctrl+c, change status
{
    status++;
}
void errExit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
int main(int argc, char**argv)
{
   // void (*ohend)(int);
   // ohand = sigset(SIGINT, handler);
    if(argc != 3)
    {
        fprintf(stderr, "Usage Error: %s start_time cycle_time\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    
    int cycle_time = atoi(argv[2]); // period of time to examine the status
    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    
    sigaddset(&sa.sa_mask, SIGQUIT);
    sigaction(SIGINT, &sa, NULL);    
    while(1)
    {
        printf("Detecting...\n");
        int old_status = status;
        sleep(cycle_time);
        if(status != old_status) // anomaly situation happen
        { // send an email
            printf("Sent an email.\n");
            int pd[2];
            if(pipe(pd) == -1)
                errExit("pipe");
            int status;
            pid_t pid = fork();
            switch(pid)
            {
                case -1:
                     errExit("fork");
                case 0: // child
                     close(pd[1]);
                     dup2(pd[0], STDIN_FILENO);
                     close(pd[0]);
                     execlp("mailx","mailx","-s","anomaly situation","ann20021007@gmail.com", (char *)NULL);
                     perror("execlp");
                     exit(1);
                     break;
                default:
                     break;
            }
            close(pd[0]);
            char *str = "Anomaly situation happens!\n";
            write(pd[1], str, strlen(str)); // write email content
            close(pd[1]);
            if(waitpid(pid, &status, 0) == -1)
            {
                errExit("waitpid");
            }
        }
    }
    return 0;
}
