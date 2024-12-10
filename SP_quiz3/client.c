#include "message.h"
int main(int argc, char *argv[]) 
{
    if (argc != 2) 
    {
        fprintf(stderr, "[Consumer] Usage: %s [client_id]\n", argv[0]);
        return 1;
    }
    
    int client_id = atoi(argv[1]);
    
    int namesize = sizeof(struct sockaddr_un);
    int sd;
    struct sockaddr_un srvr;
    Message msg;
    static int first_time = 1;
    if(first_time)
    {
        first_time = 0;
        /* Create client's socket sd */
        if( (sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
            perror("clnt1.c:main:socket");
            exit(1);
        }

        /* Fill in server's address and connect to server */
        srvr.sun_family = AF_UNIX;
        strcpy(srvr.sun_path, RESOURCE);

        if( connect(sd, (struct sockaddr*)&srvr, namesize) == -1 ) {
            perror("clnt1.c:main:connect");
            exit(1);
        }
    }
    int src, dest;
    while(read(sd, &msg, sizeof(msg)))
    {
        if(msg.src_id == 0) // send from the router
        { // send msg to node
            src = atoi(msg.buffer);  // Get the sequence number of signal
            //dest = atoi(msg.buffer[strlen(msg.buffer) + 1]);
            //printf("SRC: %d, Dest: %d\n", src, dest);
            msg.src_id = src;
            if(dest == 0)
            {
                //for(int i = 1; i <= MAX_NUMBER_CLIENT; i++)
                //{
                    msg.dest_id = 0;
                    snprintf(msg.buffer, BUF_SIZE - 1, "%d send to all", src);
                //}
            }
            else
            {
                msg.dest_id = dest;
                snprintf(msg.buffer, BUF_SIZE - 1, "%d send to %d", src, dest);
            }
            write(sd, &msg, sizeof(msg)); // send to router
        }
        else // receive msg from router
        {
            printf("Node %d receives \"%s\"\n", client_id, msg.buffer);
        }
    }
    
    /* Communicate with server */
    close(sd);
    exit(EXIT_SUCCESS);
}

