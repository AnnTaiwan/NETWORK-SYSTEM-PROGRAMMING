/* 
    This is program.c, the top rpogram to execute producer and consumer program.
    usage: 
        $ ./program [M data number] [R transmit rate] [N number of consumers] [B buffer size]
    It will create an shared memory with B bytes, N consumers processes, and 1 producer process.
    Producer will send M data with R rate to consumer.
    Consumer will check the received message to examine the loss rate.
*/ 
#include "program.h"
void sigusr1_handler(int signo) {
    printf("\t[*Program] Received SIGUSR1 signal.\n");
}
int main(int argc, char *argv[]) 
{
    if (argc != 5) 
    {
        fprintf(stderr, "Usage: %s [M data number] [R transmit rate (ms) (please input integer)] [N number of consumers] [B buffer size]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int M = atoi(argv[1]);  // data number
    int R = atoi(argv[2]);  // transmit rate
    int N = atoi(argv[3]);  // number of consumers
    int B = atoi(argv[4]);  // buffer size
    // for arguments in execl
    char str_M[MAX_NUMBER_LEN], str_R[MAX_NUMBER_LEN], str_N[MAX_NUMBER_LEN], str_B[MAX_NUMBER_LEN];
    char str_group_leader[MAX_NUMBER_LEN];
    snprintf(str_M, sizeof(str_M) - 1, "%d", M);
    snprintf(str_R, sizeof(str_R) - 1, "%d", R);
    snprintf(str_N, sizeof(str_N) - 1, "%d", N);
    snprintf(str_B, sizeof(str_B) - 1, "%d", B);

    if (N > MAX_CONSUMERS) 
    {
        fprintf(stderr, "Error: Maximum consumers supported is %d\n", MAX_CONSUMERS);
        exit(EXIT_FAILURE);
    }

    
    // set handler for SIGUSR1, in order not to let SIGUSR1 terminate this process
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = sigusr1_handler; 
    sigemptyset(&sa.sa_mask);         
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        errExit("sigaction failed");
    }
    
    // to block the SIGUSR1 signal
    sigset_t blockedMask, originalmask;
    sigemptyset(&blockedMask);
    sigaddset(&blockedMask, SIGUSR1);
    // in order to block all the previous SIGUSR1 signal, and use sigsuspend to wait for new SIGUSR1 signal 
    if (sigprocmask(SIG_SETMASK, &blockedMask, &originalmask) == -1) 
        errExit("sigprocmask");
    
    // create shared memory using Posix shared memory
    // shm_fd : POSIX shared memory file descriptor
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (shm_fd == -1) 
    {
        if (errno == EEXIST) 
        { // Shared memory already exists, unlink and recreate
            printf("\t<%s> Shared memory already exists. Deleting it...\n", SHM_NAME);
            if (shm_unlink(SHM_NAME) == -1)
                errExit("shm_unlink");

            shm_fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0666);
            if (shm_fd == -1)
                errExit("shm_open");
        } 
        else 
        {
            errExit("shm_open");
        }
    }
    if (ftruncate(shm_fd, B * 80) == -1)// each data is 80 bytes
        errExit("ftruncate");
    char (*buffer)[80] = mmap(NULL, B * 80, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0); // buffer[0] points to an array who is 80 bytes, and so does buffer[1].
    if (buffer == MAP_FAILED)
        errExit("mmap");
    // create shm for storing signal data
    int shm_signal_fd = shm_open(SHM_SIGNAL_NAME, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (shm_signal_fd == -1) 
    {
        if (errno == EEXIST) 
        { // Shared memory already exists, unlink and recreate
            printf("\t<%s> Shared memory already exists. Deleting it...\n", SHM_SIGNAL_NAME);
            if (shm_unlink(SHM_SIGNAL_NAME) == -1)
                errExit("shm_unlink");

            shm_signal_fd = shm_open(SHM_SIGNAL_NAME, O_CREAT | O_EXCL | O_RDWR, 0666);
            if (shm_signal_fd == -1)
                errExit("shm_open");
        } 
        else 
        {
            errExit("shm_open");
        }
    }
    if (ftruncate(shm_signal_fd, MAX_NUMBER_LEN) == -1)// each data is 80 bytes
        errExit("ftruncate");
    char *buffer_signal = mmap(NULL, MAX_NUMBER_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, shm_signal_fd, 0); 
    if (buffer_signal == MAP_FAILED)
        errExit("mmap");
    
    // create shm for storing loss data
    int shm_loss_fd = shm_open(SHM_LOSS_NAME, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (shm_loss_fd == -1) 
    {
        if (errno == EEXIST) 
        { // Shared memory already exists, unlink and recreate
            printf("\t<%s> Shared memory already exists. Deleting it...\n", SHM_LOSS_NAME);
            if (shm_unlink(SHM_LOSS_NAME) == -1)
                errExit("shm_unlink");

            shm_loss_fd = shm_open(SHM_LOSS_NAME, O_CREAT | O_EXCL | O_RDWR, 0666);
            if (shm_loss_fd == -1)
                errExit("shm_open");
        } 
        else 
        {
            errExit("shm_open");
        }
    }
    if (ftruncate(shm_loss_fd, sizeof(SharedData)) == -1)// each data is 80 bytes
        errExit("ftruncate");
    SharedData *buffer_loss = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shm_loss_fd, 0); 
    if (buffer_loss == MAP_FAILED)
        errExit("mmap");
    
    // Initialize the mutex (first process to run this code should initialize)
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED); // Make mutex process-shared
    pthread_mutex_init(&buffer_loss->mutex, &attr);
    pthread_mutexattr_destroy(&attr);
    // Initialize shared variable
    buffer_loss->shared_var = 0;
    
    // Simulate access to shared variable
    printf("\t[*Program] Initial shared variable: %d\n", buffer_loss->shared_var);
    
    
    
    pid_t group_leader = getpgid(getpid());  // all processes are in the same group
    snprintf(str_group_leader, sizeof(str_group_leader) - 1, "%d", group_leader);
    // create Producer Process
    pid_t producer_pid = fork();
    if (producer_pid == 0) {
        execl("./producer", "producer", str_M, str_R, str_B, str_group_leader, NULL); // execute Producer process
        errExit("Producer execution failed");
    }
    // create Consumer Processes
    pid_t consumers[MAX_CONSUMERS];
    for (int i = 0; i < N; i++) {
        consumers[i] = fork();
        if (consumers[i] == 0) {
            char consumer_id[10];
            sprintf(consumer_id, "%d", i);
            execl("./consumer", "consumer", consumer_id, str_M, str_B, NULL); // execute Consumer process
            errExit("Consumer execution failed");
        }
    }
    // send a signal to producer to inform it the consumers ready
    int sec = 5;
    printf("\t[*Program] 'sleep(%d)' let the producer process...\n", sec);
    sleep(sec);
    if (kill(producer_pid, SIGUSR1) == -1) // send signal to producer
    {
        perror("Failed to send SIGUSR1");
        exit(EXIT_FAILURE);
    }
    printf("\t[*Program] Finish creating consumer processes.\n");
    // wait for Producer and Consumers to end
    printf("\t[*Program] wait producer to finish, and producer_pid : %d ...\n", producer_pid);
    waitpid(producer_pid, NULL, 0);
    printf("\t[*Program] wait %d consumers to finish...\n", N);
    for (int i = 0; i < N; i++) {
        waitpid(consumers[i], NULL, 0);
    }
    printf("\t[*Program] all producer and consumers finished !\n");
    
    // start to calculate the loss rate
    // Lock the mutex before modifying the shared variable
    pthread_mutex_lock(&buffer_loss->mutex);
    // Critical section
    // read the share number
    int rec_num = buffer_loss->shared_var;
    // Unlock the mutex after modification
    pthread_mutex_unlock(&buffer_loss->mutex);
    // print the result
    printf("-------------------------------------------------------------\n");
    printf("M = %d, R = %d, N = %d, B = %d\n", M, R, N, B);
    printf("Total messages: %d\n", M * N);
    printf("Sum of received messages by all consumers:\n%d\n", rec_num);
    printf("Loss rate: %f\n", 1 - (float)rec_num / (float)(M * N));
    printf("-------------------------------------------------------------\n");
    close(shm_fd); // File descriptor is no longer needed after mmap
    // release resources
    if (munmap(buffer, B * 80) == -1)
        errExit("munmap");
    if(shm_unlink(SHM_NAME) == -1)
        errExit("shm_unlink");
        
    close(shm_signal_fd); // File descriptor is no longer needed after mmap
    // release resources
    if (munmap(buffer_signal, MAX_NUMBER_LEN) == -1)
        errExit("munmap");
    if(shm_unlink(SHM_SIGNAL_NAME) == -1)
        errExit("shm_unlink");
    
    close(shm_loss_fd); // File descriptor is no longer needed after mmap
    // release resources
    if (munmap(buffer_loss, sizeof(SharedData)) == -1)
        errExit("munmap");
    if(shm_unlink(SHM_LOSS_NAME) == -1)
        errExit("shm_unlink");
        
    printf("\t[*Program] release resources.\n");
    return 0;
}

