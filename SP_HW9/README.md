## SP_HW9  
### 1. compile the program  
    `$ make clean all`  
### 2. 49-1 in Section 49.13 Exercise:  implement `cp`
    `$ ./exercise_49_1 <src_file> <dest_file>`  
### 3. 52-3 in Section 52.11 Exercise: Use POSIX message queue to rewrite the client-server program in Section 46.8
* In one terminal:
    `$ ./posix_msq_queue_file_server`  
* Another terminal
    `$ ./posix_msq_queue_file_client <file>`  
### 4. 54-1 in Section 54.7 Exercise: Use POSIX shared memory object to rewrite the Rewrite the programs in Listing 48-2 (svshm_xfr_writer.c) and Listing 48-3 (svshm_xfr_reader.c) 
* In one terminal:
    `$ ./posix_shm_writer < file_name`  
* Another terminal
    `$ ./posix_shm_reader`  
    Below, we will see the information in that file.  
