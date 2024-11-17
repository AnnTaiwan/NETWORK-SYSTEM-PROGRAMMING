#include "my_svmsg_file.h"
#include <stdarg.h>
static int clientId;
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
static void
removeQueue(void)
{
    if (msgctl(clientId, IPC_RMID, NULL) == -1)
        errExit("msgctl");
}

int main(int argc, char *argv[])
{
    struct requestMsg req;
    struct responseMsg resp;
    int serverId, numMsgs;
    ssize_t msgLen;

    if (argc > 1 && strcmp(argv[1], "--help") == 0)
        errExit("%s [seq-len]\n", argv[0]);
    if (argc > 1)
        getInt(argv[1], 1, "seq-len"); // test if argv[1] is valid number
        
    /* Get server's queue identifier; create queue for response */
    printf("* Get the server's message queue.\n");
    serverId = msgget(SERVER_KEY, S_IWUSR);
    if (serverId == -1)
        errExit("msgget - server message queue");
    printf("* Get the client's message queue.\n");
    clientId = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR | S_IWGRP);
    if (clientId == -1)
        errExit("msgget - client message queue");

    if (atexit(removeQueue) != 0)
        errExit("atexit");

    /* Send message asking for file named in argv[1] */

    req.mtype = 1;                      /* Any type will do */
    req.clientId = clientId;
    strcpy(req.seqLen, (argc > 1) ? argv[1] : "1");
    printf("* Send the request to server.\n");
    if (msgsnd(serverId, &req, REQ_MSG_SIZE, 0) == -1)
        errExit("msgsnd");

    /* Get first response, which may be failure notification */
    msgLen = msgrcv(clientId, &resp, RESP_MSG_SIZE, 0, 0);
    if (msgLen == -1)
        errExit("msgrcv");

    /* File was opened successfully by server; process messages
       (including the one already received) containing file data */
    
    for (numMsgs = 1; resp.mtype == RESP_MT_DATA; numMsgs++) {
        msgLen = msgrcv(clientId, &resp, RESP_MSG_SIZE, 0, 0);
        if (msgLen == -1)
            errExit("msgrcv");
        printf("%s\n", resp.seqNum);
    }
    exit(EXIT_SUCCESS);
}
