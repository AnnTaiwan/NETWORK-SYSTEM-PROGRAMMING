#include "program.h"
// Global variable to store the received signal sequence number
int received_signal_seq = -1; // signal index
char *buffer_signal; // point to signal index
// handler for SIGUSR1
void signal_handler(int signo) 
{
    if (signo == SIGUSR1) 
    {
        int received_seq = atoi(buffer_signal);  // Get the sequence number of signal
        
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
    int msg_num; // index in the msg
    int head_len = strlen(HEAD_MSG);
    
    //printf("[Consumer%d]'s gpid is %d\n",consumer_id, (int)getpgid(getpid()));
    // open already created shared memory
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if(shm_fd == -1)  
        errExit("shm_open");
    char (*buffer)[80] = mmap(0, B * 80, PROT_READ, MAP_SHARED, shm_fd, 0);
    if(buffer == MAP_FAILED)
        errExit("mmap");
        
     // open signal data shm
    int shm_signal_fd = shm_open(SHM_SIGNAL_NAME, O_RDWR, 0666);
    if(shm_signal_fd == -1)  
        errExit("shm_open");
    buffer_signal = mmap(0, MAX_NUMBER_LEN, PROT_READ, MAP_SHARED, shm_signal_fd, 0);
    if(buffer_signal == MAP_FAILED)
        errExit("mmap");
        
    // open loss data shm
    int shm_loss_fd = shm_open(SHM_LOSS_NAME, O_RDWR, 0666);
    if(shm_loss_fd == -1)  
        errExit("shm_open");
    SharedData *buffer_loss = mmap(0, sizeof(SharedData), PROT_WRITE, MAP_SHARED, shm_loss_fd, 0); // write it
    if(buffer_loss == MAP_FAILED)
        errExit("mmap");   
        
    // set the signal action for SIGUSR1
    struct sigaction action;
    action.sa_handler = signal_handler; 
    sigemptyset(&action.sa_mask);         
    action.sa_flags = 0; 
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
    char msg[80];
    for (int i = 0; i < M; i++) 
    {
        // wait for producer's signal
        if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
            errExit("sigsuspend");
        // read the msg in shm
        strncpy(msg, buffer[(received_signal_seq % B)], 80);
        msg_num = atoi(msg + head_len);
        if(msg_num == -1) // received final msg early
        {
            printf("[Consumer%d] At idx %d, receive '-1' msg early.\n", consumer_id, i);
            break;
        }
        else 
        {
            // Lock the mutex before modifying the shared variable
            pthread_mutex_lock(&buffer_loss->mutex);
            // Critical section
            if (received_signal_seq == msg_num)  // Check if the received sequence number matches the message number
            {
                buffer_loss->shared_var += 1;
            }
            else
            {
                printf("ERROR: [Consumer%d] idx = %d, msg_num = %d, received_signal_seq = %d\n", 
                        consumer_id, i, msg_num, received_signal_seq);
            }
            /*
            printf("[Consumer%d] idx = %d, msg_num = %d, received_signal_seq = %d\n", consumer_id, i, msg_num, received_signal_seq);
            */
            // Unlock the mutex after modification
            pthread_mutex_unlock(&buffer_loss->mutex);
        }
        // printf("[Consumer%d] receives msg: %s\n", consumer_id, msg);
        //printf("[Consumer%d] msg_num = %d, received_signal_seq = %d\n", consumer_id, msg_num, received_signal_seq);
        
    }
    
    close(shm_fd); // File descriptor is no longer needed after mmap
    if (munmap(buffer, B * 80) == -1)
        errExit("munmap");
    close(shm_signal_fd); // File descriptor is no longer needed after mmap
    if (munmap(buffer_signal, MAX_NUMBER_LEN) == -1)
        errExit("munmap");
    close(shm_loss_fd); // File descriptor is no longer needed after mmap
    if (munmap(buffer_loss, sizeof(SharedData)) == -1)
        errExit("munmap");
    printf("[Consumer%d] exit(EXIT_SUCCESS).\n", consumer_id);
    exit(EXIT_SUCCESS);
}

