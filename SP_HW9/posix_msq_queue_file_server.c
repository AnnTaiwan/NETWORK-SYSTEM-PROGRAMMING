#include "posix_msq_queue_file.h"
#include <stdarg.h>
static void             /* SIGCHLD handler */
grimReaper(int sig)
{
    int savedErrno;

    savedErrno = errno;                 /* waitpid() might change 'errno' */
    while (waitpid(-1, NULL, WNOHANG) > 0)
        continue;
    errno = savedErrno;
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

static void             /* Executed in child process: serve a single client */
serveRequest(const struct requestMsg *req)
{
    int fd;
    ssize_t numRead;
    struct responseMsg resp;
    
    char client_queue_name[64];
    snprintf(client_queue_name, sizeof(client_queue_name), "/queue_%d", req->client_pid);
    mqd_t client_mqd = mq_open(client_queue_name, O_WRONLY);
    if (client_mqd == -1)
        errExit("client_mq_open - client message queue");
        
    fd = open(req->pathname, O_RDONLY);
    if (fd == -1) {                     /* Open failed: send error text */
        resp.mtype = RESP_MT_FAILURE;
        snprintf(resp.data, sizeof(resp.data), "%s", "Couldn't open");
        mq_send(client_mqd, (char *)&resp, sizeof(resp), 0);
        exit(EXIT_FAILURE);             /* and terminate */
    }

    /* Transmit file contents in messages with type RESP_MT_DATA. We don't
       diagnose read() and msgsnd() errors since we can't notify client. */
    resp.mtype = RESP_MT_DATA;
    while ((numRead = read(fd, resp.data, RESP_MSG_SIZE - 1)) > 0)
    {
        resp.data[numRead] = '\0';
        if (mq_send(client_mqd, (char*)&resp, sizeof(resp), 0) == -1)
            break;
        memset(resp.data, 0, sizeof(resp.data));
    }
    /* Send a message of type RESP_MT_END to signify end-of-file */
    resp.mtype = RESP_MT_END;
    mq_send(client_mqd, (char*)&resp, sizeof(resp), 0);         /* Zero-length mtext */
    
    if(mq_close(client_mqd) == -1)
        errExit("mq_close");
}

int
main(int argc, char *argv[])
{
    struct requestMsg req;
    //printf("%d, %d\n", sizeof(struct requestMsg), sizeof(struct responseMsg));
    pid_t pid;
    ssize_t msgLen;
    struct sigaction sa;
    struct mq_attr attr;
    unsigned int prio;
    /* Create server message queue */
    memset(&attr, 0, sizeof(struct mq_attr));
    attr.mq_msgsize = sizeof(struct requestMsg);
    attr.mq_maxmsg = 10;
    mqd_t server_mqd = mq_open(SERVER_KEY, O_CREAT | O_EXCL | O_RDONLY, 0666, &attr);
    if (server_mqd == (mqd_t)-1) 
    {
        // if server msg queue already exist, delete it
        if (errno == EEXIST) 
        {
            printf("Message queue already exists. Deleting it...\n");
            if (mq_unlink(SERVER_KEY) == -1)
                errExit("mq_unlink");

            // create it again
            server_mqd = mq_open(SERVER_KEY, O_CREAT | O_EXCL | O_RDONLY, 0666, &attr);
            if (server_mqd == (mqd_t)-1) 
                errExit("mq_open");
            printf("Message queue recreated successfully.\n");
        } 
        else 
        {
            // other error
            errExit("mq_open");
        }
    } 
    else 
    {
        printf("Message queue created successfully.\n");
    }
    
    /* Establish SIGCHLD handler to reap terminated children */

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grimReaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
        errExit("sigaction");

    /* Read requests, handle each in a separate child process */

    for (;;) {
        printf("* Wait for request from client...\n");
        msgLen = mq_receive(server_mqd, (char *)&req, sizeof(req), &prio);
        if (msgLen == -1) {
            if (errno == EINTR)         /* Interrupted by SIGCHLD handler? */
                continue;               /* ... then restart msgrcv() */
            fprintf(stderr, "mq_receive");           /* Some other error */
            break;                      /* ... so terminate loop */
        }

        pid = fork();                   /* Create child process */
        if (pid == -1) {
            fprintf(stderr, "fork");
            break;
        }

        if (pid == 0) {                 /* Child handles request */
            serveRequest(&req);
            exit(EXIT_SUCCESS);
        }

        /* Parent loops to receive next client request */
    }

    /* If mq_receive() or fork() fails, close and remove server MQ and exit */
    if (mq_close(server_mqd) == -1)
        errExit("mq_close");
    if (mq_unlink(SERVER_KEY) == -1)
        errExit("mq_unlink");
    exit(EXIT_SUCCESS);
}
