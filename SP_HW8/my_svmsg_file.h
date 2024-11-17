/* 
    Usage: Do the sequence-number client-server application of Section 44.8 to use System V message queues.
    Adjust svmsg_file.h: change structure of request and response
*/
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stddef.h>                     /* For definition of offsetof() */
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#define SERVER_KEY 0x1aaaaaa1           /* Key for server's message queue */
#define BUFF_SIZE 1024
struct requestMsg {                     /* Requests (client to server) */
    long mtype;                         /* Unused */
    int  clientId;                      /* ID of client's message queue */
    char seqLen[BUFF_SIZE];             /* Length of desired sequence */
};
/* REQ_MSG_SIZE computes size of 'mtext' part of 'requestMsg' structure.
   We use offsetof() to handle the possibility that there are padding
   bytes between the 'clientId' and 'pathname' fields. */

#define REQ_MSG_SIZE (offsetof(struct requestMsg, seqLen) - \
                      offsetof(struct requestMsg, clientId) + BUFF_SIZE)

#define RESP_MSG_SIZE BUFF_SIZE
	    
struct responseMsg {                    /* Responses (server to client) */
    long mtype;                         /* One of RESP_MT_* values below */
    char seqNum[RESP_MSG_SIZE];                         /* Start of sequence */
};

/* Types for response messages sent from server to client */

#define RESP_MT_FAILURE 1               /* File couldn't be opened */
#define RESP_MT_DATA    2               /* Message contains file data */
#define RESP_MT_END     3               /* File data complete */
