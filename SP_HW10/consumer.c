#include "program.h"
// Global variable to store the received signal sequence number
int received_signal_seq = -1;
// handler for SIGUSR1
// Signal handler for SIGUSR1
void signal_handler(int signo, siginfo_t *info, void *context) 
{
    if (signo == SIGUSR1) 
    {
        int received_seq = info->si_value.sival_int;  // Get the sequence number of signal
        
        // Store received_seq globally or pass it to the main process in some other way
        // For simplicity, we use a global variable here
        // Set this value in the consumer's main loop to compare
        extern int received_signal_seq;
        received_signal_seq = received_seq;
    }
}

int main(int argc, char *argv[]) 
{
    if (argc != 4) 
    {
        fprintf(stderr, "[Consumer] Usage: %s [consumer_id] [M] [B]\n", argv[0]);
        return 1;
    }

    int consumer_id = atoi(argv[1]);
    int M = atoi(argv[2]);
    int B = atoi(argv[3]);
    int msg_num;
    int head_len = strlen(HEAD_MSG);
    // open already created shared memory
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if(shm_fd == -1)  
        errExit("shm_open");
    char (*buffer)[80] = mmap(0, B * 80, PROT_READ, MAP_SHARED, shm_fd, 0);
    if(buffer == MAP_FAILED)
        errExit("mmap");
    // set the signal action for SIGUSR1
    struct sigaction action;
    action.sa_sigaction = signal_handler; 
    sigemptyset(&action.sa_mask);         
    action.sa_flags = SA_SIGINFO;         // due to carrying data, so flag should be SA_SIGINFO. Then, handler can access the signal data

    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        errExit("sigaction failed");
    }

    
    
    // block SIGUSR1 signal
    sigset_t blockedMask, emptyMask;
    sigemptyset(&blockedMask);
    sigaddset(&blockedMask, SIGUSR1);
    if (sigprocmask(SIG_SETMASK, &blockedMask, NULL) == -1)
        errExit("sigprocmask");
    
    sigemptyset(&emptyMask);
    printf("[Consumer%d] waits for signal...\n", consumer_id);
    int count = 0;
    for (int i = 0; i < M; i++) 
    {
        // wait for producer's signal
        if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
            errExit("sigsuspend");
        // read the msg in shm
        msg_num = atoi(buffer[(i % B) + head_len]);
        
        // Check if the received sequence number matches the message number
        if (received_signal_seq == msg_num) 
        {
            count++;
        } 
    }
    float loss_rate = 1 - (float)count / (float)M;
    printf("[Consumer%d] loss rate = %f\n", consumer_id, loss_rate);
    close(shm_fd); // File descriptor is no longer needed after mmap
    if (munmap(buffer, B * 80) == -1)
        errExit("munmap");
    return 0;
}
