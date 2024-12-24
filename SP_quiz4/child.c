#include "stack.h"

// Fisher-Yates shuffle function
void shuffle(int *array, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

int main(int argc, char **argv)
{ 
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s <child pid>\n", argv[0]);
        exit(1);
    }
    int mypid = atoi(argv[1]);
    
    struct action a1[NUM_ACTION];
    srand(getpid()); // Seed random number generator
    
    // Step 1: Create an array with equal number of push (0) and pop (1)
    // half push and half pop
    int operations[NUM_ACTION];
    for (int i = 0; i < NUM_ACTION / 2; i++) {
        operations[i] = 1; 
        operations[i + NUM_ACTION / 2] = 0; 
    }
    
    // Step 2: Shuffle the operations array
    //shuffle(operations, NUM_ACTION);
    // step 3: set the actions
    for(int i = 0; i < NUM_ACTION; i++)
    {
        a1[i].pop_or_push = operations[i];
        a1[i].value = (char)((random() % 26) + 65);
    }
    
    // open the shared memory
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if(shm_fd == -1)  
        errExit("shm_open");
    struct stack *s = mmap(NULL, sizeof(struct stack), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0); 
    if (s == MAP_FAILED)
        errExit("mmap");
    
    // start to do the actions
    for(int i = 0; i < NUM_ACTION; i++)
    {
        switch(a1[i].pop_or_push)
        {
            case 0: // pop
                char temp = pop(s);
                printf("[Process %d] action %d/%d pop out %c.\n", mypid, i+1 , NUM_ACTION, temp);
                //showstack(s);
                break;
            case 1: // push
                printf("[Process %d] action %d/%d push %c\n", mypid, i+1 , NUM_ACTION, a1[i].value);
                push(s, a1[i].value);
                //showstack(s);
                break;
            default: 
                printf("Not valid action\n");
        }
       // printf("--------------------------------------------\n");
       // showstack(s);
       // printf("--------------------------------------------\n");
    }
    close(shm_fd); // File descriptor is no longer needed after mmap
    if (munmap(s, sizeof(struct stack)) == -1)
        errExit("munmap");
    printf("[Process %d] exit(EXIT_SUCCESS).\n", mypid);
    exit(EXIT_SUCCESS);
}

