#include "fifo_seqnum.h"
#include <stdarg.h>
#include <fcntl.h>
#define BUFF_SIZE 1024
static char clientFifo[CLIENT_FIFO_NAME_LEN];

static void             /* Invoked on exit to delete client FIFO */
removeFifo(void)
{
    unlink(clientFifo); // call the unlink function to remove the specified file
}
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
int getNumber(int number, int min_value, int max_value, const char *name) {
    // Check if the value is less than the minimum required
    if (number < min_value) {
        fprintf(stderr, "Error: %s must be greater than %d\n", name, min_value - 1);
        exit(EXIT_FAILURE);
    }
    if (number > max_value) {
        fprintf(stderr, "Error: %s must be smaller than %d\n", name, max_value + 1);
        exit(EXIT_FAILURE);
    }
    return (int) number;
}
int main(int argc, char *argv[])
{
    int serverFd, clientFd;
    struct request req;
    char buf[BUFF_SIZE];
    int situation;
    // Program introduction and expected result
    printf("Here is going to test on the operations of nonblocking opens and nonblocking I/O on FIFOs.\nExplain each operation below:\n");
    printf("1: nonblock open\n\tIn client, client doesn't open the clientfifo for reading. So, server will detect the error when server try to open this clientfifo for writing response to client. Server will show the ENXIO error.\n");
    printf("2: nonblock read\n\tIn nonblock read mode, when no data is present and the write end is open, it will see the error EAGIN.\n\tHence, system will print the EAGIN in client's interface when client try to get the response.\n");
    printf("3: nonblock write\n\tIn nonblock write mode, if no sufficient space is available to immediately write n bytes, it will see the error EAGIN.\n\tHence, system will print the EAGIN in server's interface when server try to send the response.\n");
    printf("Choose one situation to test by enter number 1~3 (1: nonblock open, 2: nonblock read, 3: nonblock write)\n");
    printf("(Enter here): ");
    scanf("%d", &situation); // user choose the case
    
    /* Create our FIFO (before sending request, to avoid a race) */
    umask(0);                   /* So we get the permissions we want */
    snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
            (long) getpid());
    if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1
                && errno != EEXIST)
        errExit("mkfifo %s", clientFifo);

    if (atexit(removeFifo) != 0)
        errExit("atexit");

    /* Construct request message, open server FIFO, and send message */
    req.pid = getpid();
    req.seqLen = getNumber(situation, 1, 3, "situation number"); // add situation number in the request
    printf("* Open the server fifo.\n");
    serverFd = open(SERVER_FIFO, O_WRONLY);
    if (serverFd == -1)
        errExit("open %s", SERVER_FIFO);
    // send the request
    printf("* Send the request to server.\n");
    if (write(serverFd, &req, sizeof(struct request)) !=
            sizeof(struct request))
        errExit("Can't write to server");

    // start to test on the operation of nonblocking open and nonblocking I/O on FIFOs
    switch(situation)
    {
        case 1: // test nonblock open, client don't open the client fifo
            sleep(1);
            printf("* Don't open the client fifo and read from it.\n");
            removeFifo();
            exit(EXIT_SUCCESS);
        case 2: // test nonblock read, server's write end is open, but write nothing, client's read end will get error EAGIN.
            clientFd = open(clientFifo, O_RDONLY);
            if (clientFd == -1)
                errExit("open %s", clientFifo);
            // change the type of fd
            int flags = fcntl(clientFd, F_GETFL);
            flags |= O_NONBLOCK;
            fcntl(clientFd, F_SETFL, flags);
            if(read(clientFd, &buf, sizeof(buf)) == -1 && errno == EAGAIN)
                fprintf(stderr, "Nonblocking read: Read side detects EAGAIN.\n");
            if (close(clientFd) == -1)
                fprintf(stderr, "close\n");
            removeFifo();
            break;
        case 3: // test nonblock write, server can't write n bytes immediately, server will see the EAGAIN error.
            /* Open our FIFO, read and display response */
            clientFd = open(clientFifo, O_RDONLY);
            if (clientFd == -1)
                errExit("open %s", clientFifo);

            if (read(clientFd, &buf, sizeof(buf)) != sizeof(buf))
                fprintf(stderr, "Can't read response from server");
            // Read data slowly (optional)
            ssize_t bytesRead;
            while (1) 
            {
                bytesRead = read(clientFd, buf, sizeof(buf));
                if (bytesRead == -1) 
                {
                    perror("read");
                    exit(EXIT_FAILURE);
                } 
                else if (bytesRead == 0) 
                {
                    printf("End of file reached. Server closed the FIFO.\n");
                    break;
                } 
                else 
                {
                    printf("Read %zd bytes from the FIFO.\n", bytesRead);
                    sleep(1); // Simulate slow reading to keep the FIFO buffer full
                }
            }
            if (close(clientFd) == -1)
                fprintf(stderr, "close\n");
	    removeFifo();
	    break;
    }
    exit(EXIT_SUCCESS);
}
