/* posix_msq_queue_file_client.c

   Send a message to the server posix_msq_queue_file_server.c requesting the
   contents of the file named on the command line, and then receive the
   file contents via a series of messages sent back by the server. Display
   the total number of bytes and messages received. The server and client
   communicate using POSUX message queues.
*/
#include "posix_msq_queue_file.h"
#include <stdarg.h>
static char client_queue_name[64]; // specify the client queue name
static mqd_t client_mqd;
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
static void
removeQueue(void)
{
    if(mq_close(client_mqd) == -1)
        errExit("mq_close");
    if(mq_unlink(client_queue_name) == -1)
        errExit("mq_unlink");
}

int
main(int argc, char *argv[])
{
    struct requestMsg req;
    struct responseMsg resp;
    int numMsgs;
    ssize_t msgLen, totBytes;
    unsigned int prio;
    struct mq_attr attr_client; // mq's attributes
    
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
    {
        fprintf(stderr, "%s pathname\n", argv[0]);
        exit(EXIT_FAILURE);    
    }   
    if (strlen(argv[1]) > sizeof(req.pathname) - 1)
    {
        fprintf(stderr, "pathname too long (max: %ld bytes)\n", (long) sizeof(req.pathname) - 1);
        exit(EXIT_FAILURE);    
    }

    /* Get server's queue identifier; create queue for response */
    mqd_t server_mqd = mq_open(SERVER_KEY, O_WRONLY);
    if (server_mqd == -1)
        errExit("server_mq_open - server message queue");
    
    memset(&attr_client, 0, sizeof(struct mq_attr));
    attr_client.mq_msgsize = sizeof(struct responseMsg);
    attr_client.mq_maxmsg = 10;
    
    snprintf(client_queue_name, sizeof(client_queue_name), "/queue_%d", getpid());
    client_mqd = mq_open(client_queue_name, O_CREAT | O_RDONLY, 0666, &attr_client);
    if (client_mqd == -1)
        errExit("client_mq_open - client message queue");

    if (atexit(removeQueue) != 0)
        errExit("atexit");
    
    /* Send message asking for file named in argv[1] */
    req.client_pid = getpid();
    strncpy(req.pathname, argv[1], sizeof(req.pathname) - 1);
    req.pathname[sizeof(req.pathname) - 1] = '\0';
                                        /* Ensure string is terminated */
    if (mq_send(server_mqd, (char *)&req, sizeof(req), 0) == -1)
        errExit("mq_send");
    /* Get first response, which may be failure notification */
    msgLen = mq_receive(client_mqd, (char *)&resp, sizeof(resp), &prio);
    if (msgLen == -1)
        errExit("mq_receive");
    if (resp.mtype == RESP_MT_FAILURE) {
        printf("%s\n", resp.data);      /* Display msg from server */
        exit(EXIT_FAILURE);
    }

    /* File was opened successfully by server; process messages
       (including the one already received) containing file data */

    totBytes = strlen(resp.data);                  /* Count first message */
    for (numMsgs = 1; resp.mtype == RESP_MT_DATA; numMsgs++) {
        msgLen = mq_receive(client_mqd, (char *)&resp, sizeof(resp), &prio);
        if (msgLen == -1)
            errExit("mq_receive");
        if(resp.mtype == RESP_MT_DATA)
            totBytes += strlen(resp.data); // accumlate the file's size
    }

    printf("Received %ld bytes (%d messages)\n", (long) totBytes, numMsgs);

    exit(EXIT_SUCCESS);
}
