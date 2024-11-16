/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2024.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 44-8 */

/* misbehaving_client.c

   * Normal client: 
     A simple client that uses a well-known FIFO to request a (trivial)
     "sequence number service". This client creates its own FIFO (using a
     convention agreed upon by client and server) which is used to receive a reply
     from the server. The client then sends a request to the server consisting of
     its PID and the length of the sequence it wishes to be allocated. The client
     then reads the server's response and displays it on stdout.
     
   * Misbehaving client:
      Won't open its fifo before sending request
      
   See fifo_seqnum.h for the format of request and response messages.

   The server is in fifo_seqnum_server.c.
*/
#include "fifo_seqnum.h"
#include <stdarg.h>
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
int getInt(const char *arg, int min_value, const char *name) {
    char *endptr;
    long val = strtol(arg, &endptr, 10);
    // strtol is used to convert the argument to a long integer. endptr points to the first invalid character in arg if conversion stops.
    
    // Check if strtol encountered a non-numeric character or overflowed
    if (errno != 0 || *endptr != '\0' || endptr == arg) {
        fprintf(stderr, "Error: Invalid integer value for %s\n", name);
        exit(EXIT_FAILURE);
    }

    // Check if the value is less than the minimum required
    if (val < min_value) {
        fprintf(stderr, "Error: %s must be greater than %d\n", name, min_value - 1);
        exit(EXIT_FAILURE);
    }

    return (int) val;
}
int main(int argc, char *argv[])
{
    // Don't declare client and resp because this client will be abandoned
    int serverFd;
    struct request req;
    //struct response resp;

    if (argc > 1 && strcmp(argv[1], "--help") == 0)
        errExit("%s [seq-len]\n", argv[0]);

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
    req.seqLen = (argc > 1) ? getInt(argv[1], 1, "seq-len") : 1; // argument number should large than 0

    serverFd = open(SERVER_FIFO, O_WRONLY);
    if (serverFd == -1)
        errExit("open %s", SERVER_FIFO);
    // send the request
    if (write(serverFd, &req, sizeof(struct request)) !=
            sizeof(struct request))
        fprintf(stderr, "Can't write to server");

    /* Didn't Open our FIFO, so no need to read and display response */
    sleep(1);
    removeFifo();
    
    exit(EXIT_SUCCESS);
}
