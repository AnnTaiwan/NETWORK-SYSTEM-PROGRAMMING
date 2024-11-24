/* posix_msq_queue_file.h

   Header file for posix_msq_queue_file_server.c and posix_msq_queue_file_client.c.
*/
#include <sys/types.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <stddef.h>                     /* For definition of offsetof() */
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#define SERVER_KEY "/myserver"           /* Key for server's message queue */

struct requestMsg {                     /* Requests (client to server) */
    int client_pid;
    char pathname[PATH_MAX];            /* File to be returned */
};

#define RESP_MSG_SIZE 2048

struct responseMsg {                    /* Responses (server to client) */
    long mtype;                         /* One of RESP_MT_* values below */
    char data[RESP_MSG_SIZE];           /* File content / response message */
};

/* Types for response messages sent from server to client */

#define RESP_MT_FAILURE 1               /* File couldn't be opened */
#define RESP_MT_DATA    2               /* Message contains file data */
#define RESP_MT_END     3               /* File data complete */
