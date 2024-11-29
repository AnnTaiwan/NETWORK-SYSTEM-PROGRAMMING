#include "program.h"
int main(int argc, char *argv[]) 
{
    printf("[Producer] AAAA\n");
    if (argc != 5) 
    {
        fprintf(stderr, "[Producer] Usage: %s [M] [R] [B] [consumer group leader pgid]\n", argv[0]);
        return 1;
    }

    int M = atoi(argv[1]);
    int R = atoi(argv[2]);
    int B = atoi(argv[3]);
    int group_pid = atoi(argv[4]);
    
    // open already created shared memory
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if(shm_fd == -1)  
        errExit("shm_open");
    char (*buffer)[80] = mmap(0, B * 80, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(buffer == MAP_FAILED)
        errExit("mmap");
        
    // create mask to catch the signal that main program sends to inform finsh of creation of consumers
    sigset_t blockedMask, emptyMask;
    sigemptyset(&blockedMask);
    sigaddset(&blockedMask, SIGUSR1);
    // in order to block all the previous SIGUSR1 signal, and use sigsuspend to wait for new SIGUSR1 signal 
    if (sigprocmask(SIG_SETMASK, &blockedMask, NULL) == -1) 
        errExit("sigprocmask");
    sigemptyset(&emptyMask);
    printf("[Producer] waits for end of creating consumer processes...\n");
    if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
        errExit("sigsuspend");
    printf("[Producer] catches the signal of completion of creating consumer processes.\n");
    
    for (int i = 0; i < M; i++) {
        usleep(R * 1000);  // usleep accept micro second, so it need to * 1000

        // produce msg
        snprintf(buffer[i % B], 80, "%s%d", HEAD_MSG, i);


        // send signal to consumer
        union sigval value;
        value.sival_int = i; // carry msg idx
        if (sigqueue(-group_pid, SIGUSR1, value) == -1) { // send to the process group by specifing negative pgid
            errExit("Failed to send signal");
        }
    }
    printf("[Producer] finshes sending the signal to consumer group.\n");
    
    
    
    close(shm_fd); // File descriptor is no longer needed after mmap
    if (munmap(buffer, B * 80) == -1)
        errExit("munmap");
    return 0;
}

