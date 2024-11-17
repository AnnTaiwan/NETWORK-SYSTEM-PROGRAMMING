#include "my_svmsg_file.h"
#include <stdarg.h>
static int seqNum = 0; // global seqNum start from 0
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
static void             /* SIGCHLD handler */
grimReaper(int sig)
{
    int savedErrno;

    savedErrno = errno;                 /* waitpid() might change 'errno' */
    while (waitpid(-1, NULL, WNOHANG) > 0)
        continue;
    errno = savedErrno;
}

static void             /* Executed in child process: serve a single client */
serveRequest(const struct requestMsg *req)
{
    struct responseMsg resp;
    // send seqNum data 
    sprintf(resp.seqNum, "%d", seqNum);
    seqNum += atoi(req->seqLen);           /* Update our sequence number */
    resp.mtype = RESP_MT_DATA;
    msgsnd(req->clientId, &resp, strlen(resp.seqNum) + 1, 0);
    
    /* Send a message of type RESP_MT_END to signify end-of-file */

    resp.mtype = RESP_MT_END;
    msgsnd(req->clientId, &resp, 0, 0);         /* Zero-length mtext */
    printf("* Send the response to client whose clientId is %d.\n", req->clientId);
}

int main(int argc, char *argv[])
{
    struct requestMsg req;
    ssize_t msgLen;
    int serverId;
    struct sigaction sa;

    /* Create server message queue */

    serverId = msgget(SERVER_KEY, IPC_CREAT | S_IRUSR | S_IWUSR | S_IWGRP);
    if (serverId == -1) {
        if (errno == EEXIST) {
            printf("Message queue already exists. Cleaning up...\n");
            serverId = msgget(SERVER_KEY, 0); // get the existed id
            if (serverId != -1) {
                msgctl(serverId, IPC_RMID, NULL); // delete this existed id
                printf("Removed existing message queue.\n");
            }
            // create again
            serverId = msgget(SERVER_KEY, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IWGRP);
        }
    }
    if (serverId == -1)
        errExit("msgget");

    /* Establish SIGCHLD handler to reap terminated children */

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grimReaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
        errExit("sigaction");

    /* Read requests, handle each in a separate child process */

    for (;;) {
        printf("* Wait for request from client...\n");
        msgLen = msgrcv(serverId, &req, REQ_MSG_SIZE, 0, 0);
        if (msgLen == -1) {
            if (errno == EINTR)         /* Interrupted by SIGCHLD handler? */
                continue;               /* ... then restart msgrcv() */
            fprintf(stderr, "msgrcv");           /* Some other error */
            break;                      /* ... so terminate loop */
        }
        printf("* Get the request from client whose clientId is %d.\n", req.clientId);
        serveRequest(&req);
    }

    /* If msgrcv() fails, remove server MQ and exit */

    if (msgctl(serverId, IPC_RMID, NULL) == -1)
        errExit("msgctl");
    exit(EXIT_SUCCESS);
}
