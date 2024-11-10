## SP_HW7  
1. compile the program  
    `$ make`  
2. `exercise22_3.c` turn input string text into uppercase string.  
    `$ ./exercise22_3`   
3. Implement the System V functions `sigset()`, `sighold()`, `sigrelse()`, `sigignore()`, and `sigpause()` using the POSIX signal API. in `system_v.c`  
`exercise22_4.c` can **test all five function by following the directions on the screen.**  
Inside, it will call `sigset()`, `sighold()`, `sigrelse()`, `sigignore()`, and `sigpause()`.  
    `$ ./exercise22_4` // see expected output below  
    Setting up custom signal handler for SIGINT  
    Blocking SIGINT signal  
    Press Ctrl+C to send SIGINT, but it will be blocked...  
    ^CUnblocking SIGINT signal  
    RECEIVED SIGNAL 2  
    Press Ctrl+C to send SIGINT again, now it should be received...  
    ^CRECEIVED SIGNAL 2  
    Setting up to ignore SIGTERM signal  
    Sending SIGTERM signal by `kill` this process, it should be ignored...  
    Waiting for SIGUSR1 signal using my_sigpause  
    User defined signal 1  

