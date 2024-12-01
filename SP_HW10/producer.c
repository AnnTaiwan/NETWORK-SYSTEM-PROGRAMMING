#include "program.h"
void sigusr1_handler(int signo) {
    printf("[Producer] Received SIGUSR1 signal.\n");
}
int main(int argc, char *argv[]) 
{
    if (argc != 5) 
    {
        fprintf(stderr, "[Producer] Usage: %s [M] [R] [B] [consumer group leader pgid]\n", argv[0]);
        return 1;
    }
    printf("[Producer]'s gpid is %d\n", (int)getpgid(getpid()));
    int M = atoi(argv[1]);
    int R = atoi(argv[2]);
    int B = atoi(argv[3]);
    int group_pid = atoi(argv[4]);
    
    // set handler for SIGUSR1, in order not to let SIGUSR1 terminate this process
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = sigusr1_handler; 
    sigemptyset(&sa.sa_mask);         
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        errExit("sigaction failed");
    }
    // open already created shared memory
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if(shm_fd == -1)  
        errExit("shm_open");
    char (*buffer)[80] = mmap(0, B * 80, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(buffer == MAP_FAILED)
        errExit("mmap");
        
    // open signal data shm
    int shm_signal_fd = shm_open(SHM_SIGNAL_NAME, O_RDWR, 0666);
    if(shm_signal_fd == -1)  
        errExit("shm_open");
    char *buffer_signal = mmap(0, MAX_NUMBER_LEN, PROT_WRITE, MAP_SHARED, shm_signal_fd, 0);
    if(buffer_signal == MAP_FAILED)
        errExit("mmap");
    
    /*// open loss data shm
    int shm_loss_fd = shm_open(SHM_LOSS_NAME, O_RDWR, 0666);
    if(shm_loss_fd == -1)  
        errExit("shm_open");
    SharedData *buffer_loss = mmap(0, sizeof(SharedData), PROT_READ, MAP_SHARED, shm_loss_fd, 0); // read it
    if(buffer_loss == MAP_FAILED)
        errExit("mmap");
      */  
    
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
    
    
    int i = 0;
    for (i = 0; i < M; i++) { // start to send msgs and signals
        usleep(R * 1000);  // usleep accept micro second, so it need to * 1000
        
        // produce msg
        snprintf(buffer[i % B], 80 - 1, "%s%d", HEAD_MSG, i);
        printf("[Producer] send '%s'\n", buffer[i % B]);

        // send signal to consumer
        //printf("[Producer] Sending signal to process group %d\n", -group_pid);
        
        /*  use shared memory to send the msg index because sigqueue can only send signal to process one by one.
            I want to use killpg to send signal to all processes in this group which is forked from the program process
        */
        snprintf(buffer_signal, MAX_NUMBER_LEN - 1, "%d", i);
        if (killpg(group_pid, SIGUSR1) == -1) {
            perror("[Producer] Failed to send signal using killpg");
        }
        printf("[Producer] send signal to %d\n", group_pid);
    }
    // last time
    usleep(R * 1000);  // usleep accept micro second, so it need to * 1000
        
    // produce msg
    snprintf(buffer[i % B], 80 - 1, "%s%d", HEAD_MSG, -1);
    printf("[Producer] send last msg '%s'\n", buffer[i % B]);

    // send signal to consumer
    snprintf(buffer_signal, MAX_NUMBER_LEN - 1, "%d", 0);
    if (killpg(group_pid, SIGUSR1) == -1) {
        perror("[Producer] Failed to send signal using killpg");
    }
    printf("[Producer] send last signal to %d\n", group_pid);
    
    
    
    close(shm_fd); // File descriptor is no longer needed after mmap
    if (munmap(buffer, B * 80) == -1)
        errExit("munmap");
    close(shm_signal_fd); // File descriptor is no longer needed after mmap
    if (munmap(buffer_signal, MAX_NUMBER_LEN) == -1)
        errExit("munmap");
        
    /*close(shm_loss_fd); // File descriptor is no longer needed after mmap
    if (munmap(buffer_loss, sizeof(SharedData)) == -1)
        errExit("munmap");
       */ 
    printf("[Producer] finshes sending the signal to consumer group.\n");
    exit(EXIT_SUCCESS);
}

