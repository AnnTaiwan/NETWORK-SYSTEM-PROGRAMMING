## SP_HW8  
1. compile the program  
    `$ make clean all`  
    
2. Exercises 44-6  
Based on the same code, Listing 44-6, 44-7, and 44-8, I only extend the server code with `O_NONBLOCK` flag when open the clientfifo for writing.  
And, try to examine this open for at most **five** times, in order not to arise `ENXIO` error in normal client.  
In `misbehaving_client.c`, **remove the `open` and `read`** to act as a client who doesn't open its fifo. So, it will print the error msg in server side.  
* usage:
Open one terminal input:  
    `$ ./fifo_seqnum_server_nonblock`  
    wait for request from client...  
    Get the request from pid 54084 client.  
    Wait for request from client...  
    Get the request from pid 54085 client.  
    Wait for request from client...  
    Get the request from pid 54086 client.  
    The other clientFifo end isn't open yet, last retries number : 5  
    The other clientFifo end isn't open yet, last retries number : 4  
    The other clientFifo end isn't open yet, last retries number : 3  
    The other clientFifo end isn't open yet, last retries number : 2  
    The other clientFifo end isn't open yet, last retries number : 1  
    Error: Client FIFO '/tmp/seqnum_cl.54086' not open on the other end (ENXIO)  
    Wait for request from client...  
Open another terminal input:  
    `$ ./fifo_seqnum_client 6`  
    or  
    `$ ./misbehaving_client 6`  


