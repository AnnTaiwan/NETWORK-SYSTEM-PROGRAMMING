/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2024.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 44-7 */

/* fifo_seqnum_server.c

   An example of a server using a FIFO to handle client requests.
   The "service" provided is the allocation of unique sequential
   numbers. Each client submits a request consisting of its PID, and
   the length of the sequence it is to be allocated by the server.
   The PID is used by both the server and the client to construct
   the name of the FIFO used by the client for receiving responses.

   The server reads each client request, and uses the client's FIFO
   to send back the starting value of the sequence allocated to that
   client. The server then increments its counter of used numbers
   by the length specified in the client request.

   See fifo_seqnum.h for the format of request and response messages.

   The client is in fifo_seqnum_client.c.
*/
#include <signal.h>
#include <stdarg.h>
#include "fifo_seqnum.h"
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
    struct response resp;
    int seqNum = 0;                     /* This is our "service" */

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
        printf("Wait for request from client...\n");
        if (read(serverFd, &req, sizeof(struct request))
                != sizeof(struct request)) {
            fprintf(stderr, "Error reading request; discarding\n");
            continue;                   /* Either partial read or error */
        }
        printf("Get the request from pid %d client.\n", req.pid);
        /* Open client FIFO (previously created by client) */
    
        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
                (long) req.pid);
                
        int retries = 5;  // maximum retry number
        // try to nonblock to prevent from encounter misbehaving client who doesn't open its fifo
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
                fprintf(stderr, "Error: Client FIFO '%s' not open on the other end (ENXIO)\n", clientFifo);
                continue;

            } 
            else /* Open failed, give up on client */
            {
                fprintf(stderr, "Error: open %s\n", clientFifo); // just print the error message, and skip this request to deal with next
                continue;
            }
        }
        
        /* Send response and close FIFO */

        resp.seqNum = seqNum;
        if (write(clientFd, &resp, sizeof(struct response))
                != sizeof(struct response))
            fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);
        if (close(clientFd) == -1)
            fprintf(stderr, "close\n");

        seqNum += req.seqLen;           /* Update our sequence number */
    }
    return 0;
}
