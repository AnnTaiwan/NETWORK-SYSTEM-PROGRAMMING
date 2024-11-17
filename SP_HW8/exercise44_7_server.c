#include <signal.h>
#include <stdarg.h>
#include "fifo_seqnum.h"
#define BUFF_SIZE 1024
void errExit(const char *format, ...) { // accept varied arguments
    va_list args;

    // print error message
    fprintf(stderr, "Error: ");
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    if (errno != 0) { 
        fprintf(stderr, ": ");
        perror("");
    } else {
        fprintf(stderr, "\n");
    }

    exit(EXIT_FAILURE);
}
int main(int argc, char *argv[])
{
    int serverFd, dummyFd, clientFd;
    char clientFifo[CLIENT_FIFO_NAME_LEN];
    struct request req;
    char buf[BUFF_SIZE]; // used in clientfifo's I/O
    int retries = 5;  // maximum retry number when opening fifo
    for(int i = 0; i < BUFF_SIZE; i++) // initialize the buf
        buf[i] = 'a';
    /* Create well-known FIFO, and open it for reading */
    umask(0);                           /* So we get the permissions we want */
    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1
            && errno != EEXIST)
        errExit("mkfifo %s", SERVER_FIFO);
    serverFd = open(SERVER_FIFO, O_RDONLY);
    if (serverFd == -1)
        errExit("open %s", SERVER_FIFO);

    /* Open an extra write descriptor, so that we never see EOF */

    dummyFd = open(SERVER_FIFO, O_WRONLY);
    if (dummyFd == -1)
        errExit("open %s", SERVER_FIFO);

    /* Let's find out about broken client pipe via failed write() */

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)    errExit("signal");

    for (;;) {                          /* Read requests and send responses */
        printf("* Wait for request from client...\n");
        if (read(serverFd, &req, sizeof(struct request))
                != sizeof(struct request)) {
            fprintf(stderr, "Error reading request; discarding\n");
            continue;                   /* Either partial read or error */
        }
        printf("* Get the request from pid %d client.\n", req.pid);
        /* Open client FIFO (previously created by client) */
        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) req.pid);
        switch(req.seqLen) // situation number from client
        {
            case 1: // test nonblock open, client don't open the client fifo
                // try to nonblock to prevent from encounter misbehaving client who doesn't open its fifo
                retries = 5;
                while ((clientFd = open(clientFifo, O_WRONLY | O_NONBLOCK)) == -1 && errno == ENXIO && retries > 0) {
                    printf("The other clientFifo end isn't open yet, last retries number : %d\n", retries);
                    retries--;
                    usleep(100000);  // wait 100 ms
                }
                /*
                    if open for writting and set O_NONBLOCK flag and other end of FIFO closed, it will fail(ENXIO)
                */
                if (clientFd == -1) 
                {
                    if (errno == ENXIO) 
                    {
                        // print error message
                        fprintf(stderr, "Nonblocking open: Client FIFO '%s' is not open on the other end (ENXIO)\n", clientFifo);
                    } 
                    else /* Open failed, give up on client */
                    {
                        fprintf(stderr, "Error: open %s\n", clientFifo); 
                        // just print the error message, and skip this request to deal with next
                    }
                    continue;
                }
                break;
            case 2: // test nonblock read, server's write end is open, but write nothing, client's read end will get error EAGIN.
                clientFd = open(clientFifo, O_WRONLY);
                if (clientFd == -1) 
                {
                    fprintf(stderr, "Error: open %s\n", clientFifo); // just print the error message, and skip this request to deal
                    continue;
                }
                // Do not write to the FIFO
                printf("* Do not write to the client FIFO.\n");
                // Keep the FIFO open for a while to allow the client to attempt a read
                sleep(5);

                // Close the client FIFO
                if (close(clientFd) == -1) 
                {
                    fprintf(stderr, "close\n");
                }
                break;
            case 3: // test nonblock write, server can't write n bytes immediately, server will see the EAGAIN error.
                // Open the FIFO in write-only nonblocking mode
                // make sure both side can open the fifo, prevent client from blocking
                // try to nonblock to prevent from encounter misbehaving client who doesn't open its fifo
                retries = 5;
                while ((clientFd = open(clientFifo, O_WRONLY | O_NONBLOCK)) == -1 && errno == ENXIO && retries > 0) {
                    printf("The other clientFifo end isn't open yet, last retries number : %d\n", retries);
                    retries--;
                    usleep(100000);  // wait 100 ms
                }
                if (clientFd == -1)
                {
                    fprintf(stderr, "Error: open %s\n", clientFifo); // just print the error message, and skip this request to deal
                    continue;
                }
                // Fill the FIFO buffer by writing repeatedly
                ssize_t bytesWritten;
                while (1) 
                {
                    bytesWritten = write(clientFd, buf, sizeof(buf));
                    if (bytesWritten == -1) 
                    {
                        if (errno == EAGAIN) 
                        {
                            printf("Nonblocking write: FIFO buffer is full. errno = EAGAIN\n");
                            break;
                        } 
                        else 
                        {
                            perror("write");
                            exit(EXIT_FAILURE);
                        }
                    } 
                    else 
                    {
                        printf("Wrote %zd bytes to the FIFO.\n", bytesWritten);
                    }
                }
                if (close(clientFd) == -1)
                    fprintf(stderr, "close\n");
                break;
        }
    }
    return 0;
}
