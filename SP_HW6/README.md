## SP_HW6  
1. compile the program  
    `$ make`  
2. exercise44_1.c turn input string text into uppercase string.  
    `$ ./exercise44_1`   
3. Implement `popen` and `pclose` in popen_pclose.c  
exercise44_2.c implement a simple shell, and it can **execute the command that user inputs.**  
Inside, it will call `my_popen` and `my_pclose`.  
    `$ ./exercise44_2`  
    myshell -> ls -al  
    total 108  
    drwxrwxr-x  2 ann ann  4096 十一  4 20:51 .  
    drwxrwxr-x 10 ann ann  4096 十一  3 17:17 ..  
    -rwxrwxr-x  1 ann ann 20440 十一  4 20:51 exercise44_1  
    -rw-rw-r--  1 ann ann  2696 十一  3 20:50 exercise44_1.c  
    -rw-rw-r--  1 ann ann 11304 十一  4 20:51 exercise44_1.o  
    -rwxrwxr-x  1 ann ann 22984 十一  4 20:51 exercise44_2  
    -rw-rw-r--  1 ann ann  2318 十一  4 20:46 exercise44_2.c  
    -rw-rw-r--  1 ann ann  9464 十一  4 20:51 exercise44_2.o  
    -rw-rw-r--  1 ann ann   657 十一  4 20:08 makefile  
    -rw-rw-r--  1 ann ann  3488 十一  4 20:43 popen_pclose.c  
    -rw-rw-r--  1 ann ann   489 十一  4 20:05 popen_pclose.h  
    -rw-rw-r--  1 ann ann 11576 十一  4 20:51 popen_pclose.o  
    -rw-rw-r--  1 ann ann     0 十一  4 20:47 README.md  
	    my_pclose() status == 0  
	    Child exited, status=0  
     myshell -> exit # to quit the program   
