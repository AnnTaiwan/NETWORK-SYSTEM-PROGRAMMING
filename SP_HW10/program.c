/* 
    This is program.c, the top rpogram to execute producer and consumer program.
    usage: 
        $ ./program [M data number] [R transmit rate] [N number of consumers] [B buffer size]
    It will create an shared memory with B bytes, N consumers processes, and 1 producer process.
    Producer will send M data with R rate to consumer.
    Consumer will check the received message to examine the loss rate.
*/ 
#include "program.h"
int main(int argc, char *argv[]) 
{
    if (argc != 5) 
    {
        fprintf(stderr, "Usage: %s [M data number] [R transmit rate (ms)] [N number of consumers] [B buffer size]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int M = atoi(argv[1]);  // data number
    int R = atoi(argv[2]);  // transmit rate
    int N = atoi(argv[3]);  // number of consumers
    int B = atoi(argv[4]);  // buffer size
    // for arguments in execl
    char str_M[MAX_NUMBER_LEN], str_R[MAX_NUMBER_LEN], str_N[MAX_NUMBER_LEN], str_B[MAX_NUMBER_LEN];
    char str_group_leader[MAX_NUMBER_LEN];
    snprintf(str_M, sizeof(str_M), "%d", M);
    snprintf(str_R, sizeof(str_R), "%d", R);
    snprintf(str_N, sizeof(str_N), "%d", N);
    snprintf(str_B, sizeof(str_B), "%d", B);

    if (N > MAX_CONSUMERS) 
    {
        fprintf(stderr, "Error: Maximum consumers supported is %d\n", MAX_CONSUMERS);
        exit(EXIT_FAILURE);
    }

    // create shared memory using Posix shared memory
    // shm_fd : POSIX shared memory file descriptor
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (shm_fd == -1) 
    {
        if (errno == EEXIST) 
        { // Shared memory already exists, unlink and recreate
            printf("Shared memory already exists. Deleting it...\n");
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
    char (*buffer)[80] = mmap(NULL, B * 80, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0); // buffer[0] points to an array who is 50 bytes, and so does buffer[1]
    if (buffer == MAP_FAILED)
        errExit("mmap");
  
    
    
    pid_t group_leader = getpid();  // Main process will be the group leader
    
    snprintf(str_group_leader, sizeof(str_group_leader), "%d", group_leader);
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
            setpgid(0, group_leader); // turn all consumers into a group whose groupid is main processs's pid
            char consumer_id[10];
            sprintf(consumer_id, "%d", i);
            execl("./consumer", "consumer", consumer_id, str_M, str_B, NULL); // execute Consumer process
            errExit("Consumer execution failed");
        }
        else if (consumers[i] > 0)  // not sure wwhether parent or child enter first, so setgpid twice after fork() immediately
        {
            setpgid(consumers[i], group_leader);  // Set the PGID of each consumer to the leader's PID
        }
    }
    // send a signal to producer to inform it the consumers ready
    kill(producer_pid, SIGUSR1);
    printf("[Program] Finish creating consumer processes.\n");
    // wait for Producer and Consumers to end
    waitpid(producer_pid, NULL, 0);
    for (int i = 0; i < N; i++) {
        waitpid(consumers[i], NULL, 0);
    }
    close(shm_fd); // File descriptor is no longer needed after mmap
    // release resources
    if (munmap(buffer, B * 80) == -1)
        errExit("munmap");
    if(shm_unlink(SHM_NAME) == -1)
        errExit("shm_unlink");
    printf("[Program] release resources.\n");
    return 0;
}

