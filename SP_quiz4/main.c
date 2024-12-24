#include "stack.h"
int main()
{
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) 
    {
        errExit("shm_open");
    }
    printf("Below are all the actions, the action of each process may not be printed near the stack actions.\n");
    printf("The action of each process will be printed earlier.([Process X] ...)\n");
    if (ftruncate(shm_fd, sizeof(struct stack)) == -1)// each data is 80 bytes
        errExit("ftruncate");
    struct stack *s = mmap(NULL, sizeof(struct stack), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0); 
    if (s == MAP_FAILED)
        errExit("mmap");


    // Initialize the mutex (first process to run this code should initialize)
    pthread_mutexattr_t mutex_attr;
    pthread_condattr_t cond_attr;
  
    if (pthread_condattr_init(&cond_attr) != 0)
        errExit("pthread_condattr_init");
    if (pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED) != 0)
        errExit("pthread_condattr_setpshared");

    if (pthread_mutexattr_init(&mutex_attr) != 0)
        errExit("pthread_mutexattr_init");
    if (pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED) != 0)
        errExit("pthread_mutexattr_setpshared");

    if (pthread_cond_init(&s->conditionVar, &cond_attr) != 0)
        errExit("pthread_cond_init not_full");
    if (pthread_mutex_init(&s->mutex, &mutex_attr) != 0)
        errExit("pthread_mutex_init");
    // Initialize shared variable
    s->count = 0;
    // create Consumer Processes
    pid_t childs[NUM_CHILD_PROCESSES];
    for (int i = 0; i < NUM_CHILD_PROCESSES; i++) {
        childs[i] = fork();
        if (childs[i] == 0) {
            char child_id[10];
            sprintf(child_id, "%d", i);
            execl("./child", "child", child_id, NULL); // execute Consumer process
            errExit("child execution failed");
        }
    }
    for (int i = 0; i < NUM_CHILD_PROCESSES; i++) {
        waitpid(childs[i], NULL, 0);
    }
    
    
    // Clean up resources
    if (pthread_mutex_destroy(&s->mutex) != 0)
        errExit("pthread_mutex_destroy");
    if (pthread_cond_destroy(&s->conditionVar) != 0)
        errExit("pthread_cond_destroy");
        
        
    close(shm_fd); // File descriptor is no longer needed after mmap
    if (munmap(s, sizeof(struct stack)) == -1)
        errExit("munmap");
    if (shm_unlink(SHM_NAME) == -1)
        errExit("shm_unlink");
    printf("[Main] release resources.\n");
    return 0;

}
