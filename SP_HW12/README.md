## SP_HW12
### Description of this HW
* Target is same as **SP_HW11** : Let the client request a word, and the server has to find the dinfition in order to write it down in the shared memory.
    * But in this time, Using shared memory and semaphore (System V version) to implement it.
* Use two semaphore to maintain the **sychronization**:
    * Semaphore 0: Controls client access to shared memory (ensuring only one client accesses it at a time).
    * Semaphore 1: Controls server synchronization (when the server can process client requests).
### Implementation Flow
Server-Side (Initialization)  
1. Call `shmget()` to create shared memory for one Dictrec structure  
2. Call `shmat()` to attach shared memory to the server process's address space  
3. Call `semget()` to create two semaphores for synchronization  
4. Initialize Semaphore 0 to 1 to control client access to shared memory  
5. Initialize Semaphore 1 to 0 to control server synchronization  

Client Process  
1. Decrement Semaphore 0 by 1 to lock access to shared memory  
2. Write the word to shared memory (this is the client's request)  
3. Increment Semaphore 1 by 2 to alert the server that the request is ready  
4. Wait for Semaphore 1 to reach 0 (this indicates that the server has processed the request)  
5. Read the definition from shared memory (this is the server's response)  
6. Increment Semaphore 0 by 1 to release access to shared memory, allowing other clients to use it  

Server Process (Infinite Loop)  
1. Decrement Semaphore 1 by 1 to detect that a client request is pending  
2. Decrement Semaphore 1 by 1 again to indicate that the server is processing the request  
3. Read the word from shared memory (the client's request)  
4. Look up the definition of the word (this could be from a file or a database)  
5. Write the definition back to shared memory (the client's response)  
6. Wait for the next client request by looping back to Step 1  
