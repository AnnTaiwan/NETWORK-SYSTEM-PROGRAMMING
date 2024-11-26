/* test program */
#include "stack.h"      
#include <pthread.h>
#include <semaphore.h>
#define SEM_ACTION "/sem_prob2"
#define SEM_ACTION2 "/sem2_prob2"
static struct stack s;
sem_t *semAction, *semAction2; // thread1's 
struct action
{
    int pop_or_push; // 0: pop, 1: push
    int value;
};

static struct action a1[5], a2[5]; // each thread has 5 actions, define it in main

void errExit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
// thread1's operation
void* operation(void* arg) 
{
    printf("Hello from thread %d!\n", *(int*)arg);
   
    for(int i = 0; i < 5; i++)
    {
       if (sem_wait(semAction) == -1) // Wait for 
            errExit("sem_wait");
        switch(a1[i].pop_or_push)
        {
            case 0: // pop
                printf("Thread %d pop out %d\n", *(int*)arg, pop(&s));
                break;
            case 1: // push
                printf("Thread %d push %d\n", *(int*)arg, a1[i].value);
                push(&s, a1[i].value);
                break;
            default: 
                printf("Not valid action\n");
        }
        showstack(&s);
        if (sem_post(semAction2) == -1) // Signal the reader
            errExit("sem_post");
    }    
    if (sem_wait(semAction) == -1) // Wait for final call to clean up the semaphore
        errExit("sem_wait");
    // clean up the resource
    if (sem_close(semAction) == -1)
        errExit("sem_close");
    if (sem_close(semAction2) == -1)
        errExit("sem_close");
    if (sem_unlink(SEM_ACTION) == -1)
        errExit("sem_unlink - semAction");
    if (sem_unlink(SEM_ACTION2) == -1)
        errExit("sem_unlink - semAction2");
    printf("END of thread %d\n", *(int*)arg);
    return NULL;
}
// thread2's operation
void* operation2(void* arg) 
{
    printf("Hello from thread %d!\n", *(int*)arg);
    /* Create and initialize semaphores */
    sem_t *semAction = sem_open(SEM_ACTION, 0); 
    if (semAction == SEM_FAILED)
        errExit("sem_open");
    sem_t *semAction2 = sem_open(SEM_ACTION2, 0); 
    if (semAction2 == SEM_FAILED)
        errExit("sem_open");
    for(int i = 0; i < 5; i++)
    {
        
        if (sem_wait(semAction2) == -1) // Wait for 
            errExit("sem_wait");
        switch(a2[i].pop_or_push)
        {
            case 0: // pop
                printf("Thread %d pop out %d\n", *(int*)arg, pop(&s));
                break;
            case 1:
                printf("Thread %d push %d\n", *(int*)arg, a2[i].value);
                push(&s, a2[i].value);
                break;
            default: 
                printf("Not valid action\n");
        }
        showstack(&s);
        if (sem_post(semAction) == -1) // Signal the thread 1
            errExit("sem_post");
    }    
    
            
    printf("END of thread %d\n", *(int*)arg);
    return NULL;
}




int main(int argc, char**argv)
{
    srand(time(NULL));
    // test data
    // 1 : push, 0 : pop
    a1[0].pop_or_push = 1;
    a1[1].pop_or_push = 1;
    a1[2].pop_or_push = 0;
    a1[3].pop_or_push = 0;
    a1[4].pop_or_push = 1;
    
    a2[0].pop_or_push = 1;
    a2[1].pop_or_push = 1;
    a2[2].pop_or_push = 1;
    a2[3].pop_or_push = 0;
    a2[4].pop_or_push = 0;
    for(int i = 0; i < 5; i++)
    {
        a1[i].value = random() % 100;
        a2[i].value = random() % 100;
    }
     /* Create and initialize semaphores */
     // open them before creating thread
    semAction = sem_open(SEM_ACTION, O_CREAT, OBJ_PERMS, 1); // Initially let thread1 first take action
    if (semAction == SEM_FAILED)
        errExit("sem_open");
    semAction2 = sem_open(SEM_ACTION2, O_CREAT, OBJ_PERMS, 0); // let thread2 wait
    if (semAction2 == SEM_FAILED)
        errExit("sem_open");
    
    
    pthread_t thread1, thread2;  
    int thread1_id = 1, thread2_id = 2;
    
    if (pthread_create(&thread1, NULL, operation, (void*)&thread1_id)) {
        fprintf(stderr, "Error creating thread 1\n");
        return 1;
    }

    if (pthread_create(&thread2, NULL, operation2, (void*)&thread2_id)) {
        fprintf(stderr, "Error creating thread 2\n");
        return 1;
    }

    if (pthread_join(thread1, NULL)) {
        fprintf(stderr, "Error joining thread 1\n");
        return 1;
    }

    if (pthread_join(thread2, NULL)) {
        fprintf(stderr, "Error joining thread 2\n");
        return 1;
    }
    return 0;
}
